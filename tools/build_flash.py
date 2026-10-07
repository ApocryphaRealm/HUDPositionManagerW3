"""Build HUD Position Manager's changes into the game's own HUD movies.

    python tools/build_flash.py [--redkit "<The Witcher 3 REDkit folder>"] [--ffdec "<ffdec-cli.exe>"] [--movie-only]

Movies (MOVIES below), each built the same way - the pipeline of Simple Loadout System W3's tools/build_flash.py:
1. takes CD PROJEKT RED's source movie from REDkit's depot (r4data\\gameplay\\gui_new\\swf\\hud\\<movie>.swf);
2. decompiles only its module class with FFDec and inserts our code file plus the hook lines, each at an anchor that
   must be found exactly once (else the build stops);
3. recompiles that class into the movie (FFDec -importScript);
4. REDkit's wcc_lite: swfimport (movie -> .redswf) for every movie, then ONE pack (-> blob0.bundle) and metadatastore,
   into dist\\Mods\\modHUDPositionManager\\content - this mod's own bundle.
   hud_minimap2      the square minimap, its zoom, the quest list under it, the player widget's place
   hud_wolfstatbars  the player widget's straight stamina / adrenaline / toxicity bars (2026-10-07)

Only our own code is kept in the repository; CD PROJEKT RED's movies and their decompiled classes are read from the
player's REDkit install at build time and never committed. wcc_lite runs with -agreetoterms (the owner accepted the
Witcher III Modding Tool rules on 2026-10-06; dist\\README.txt starts with the line they ask for).

Paths: --redkit / W3_REDKIT, else Steam's library folders ("The Witcher 3 REDkit"); --ffdec / FFDEC, else
%USERPROFILE%\\tools\\ffdec\\ffdec-cli.exe, else FFDec's own install folder. Work files go to build\\flash (ignored by git).
"""
import argparse
import os
import re
import shutil
import subprocess
import sys
import winreg

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD = os.path.join(REPO, "build", "flash")
CONTENT = os.path.join(REPO, "dist", "Mods", "modHUDPositionManager", "content")
SWF_DIR = os.path.join("gameplay", "gui_new", "swf", "hud")
DEPOT_OUT = "sqm_build"   # a folder of ours inside REDkit's workspace, removed after the build

MINIMAP_IMPORTS = ("import flash.display.DisplayObject;\n   import flash.display.DisplayObjectContainer;\n"
                   "   import flash.display.GradientType;\n   import flash.display.Shape;\n"
                   "   import flash.display.Sprite;\n   import flash.events.Event;\n   import flash.geom.Matrix;\n"
                   "   import flash.geom.Rectangle;\n   import flash.text.TextField;\n   ")
WOLF_IMPORTS = ("import flash.display.DisplayObject;\n   import flash.display.GradientType;\n"
                "   import flash.display.Shape;\n   import flash.display.Sprite;\n   import flash.geom.Matrix;\n"
                "   import flash.geom.Rectangle;\n   ")

# every anchor must occur exactly once in FFDec's decompiled class: (anchor, text inserted, "before" | "after")
MOVIES = [
    {
        "swf": "hud_minimap2",
        "class": "red.game.witcher3.hud.modules.HudModuleMinimap2",
        "ours": os.path.join("flash", "sqm_module.as.txt"),
        "hooks": [
            ("import flash.display.MovieClip;\n", MINIMAP_IMPORTS, "before"),
            # after the game's own setup: the children exist and the round masks are set
            ("         this.UpdateVisibility();\n         registerDataBinding(\"hud.minimap.paths.add\",this.handleAddPath);\n",
             "         this.sqmInit();\n", "after"),
            # the square masks take the round ones' place whenever the game switches masking on or off
            ("      public function EnableMask(param1:Boolean) : *\n      {\n",
             "         if(this.sqmEnableMask(param1))\n         {\n            return;\n         }\n", "after"),
        ],
    },
    {
        "swf": "hud_wolfstatbars",
        "class": "red.game.witcher3.hud.modules.HudModuleWolfHead",
        "ours": os.path.join("flash", "hpm_wolf.as.txt"),
        "hooks": [
            ("import flash.display.MovieClip;\n", WOLF_IMPORTS, "before"),
            # the bars exist before the game sends its first values (OnConfigUI)
            ("         dispatchEvent(new GameEvent(GameEvent.CALL,\"OnConfigUI\"));\n", "         this.hpmBarsInit();\n",
             "before"),
            ("         this.mcStaminaBar.value = param1 * 100;\n", "         this.hpmSet(0,param1 * 100);\n", "after"),
            ("      public function UpdateFocusPointsBar(param1:Number) : void\n      {\n",
             "         this.hpmSet(1,param1 * 100 / 3);\n", "after"),
            ("            this.mcToxicityBar.value = param1 * 100;\n", "            this.hpmSet(2,param1 * 100);\n",
             "after"),
            ("         this.isCiriMainPlayer = param1;\n", "         this.hpmCiri(param1);\n", "after"),
        ],
    },
]


def steam_libraries():
    try:
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\Valve\Steam") as k:
            steam = winreg.QueryValueEx(k, "SteamPath")[0]
        vdf = open(os.path.join(steam, "steamapps", "libraryfolders.vdf"), encoding="utf-8").read()
        return [p.replace("\\\\", "\\") for p in re.findall(r'"path"\s+"([^"]+)"', vdf)]
    except OSError:
        return []


def swf_rel(name):
    return os.path.join("r4data", SWF_DIR, name + ".swf")


def find_redkit(arg):
    for cand in [arg, os.environ.get("W3_REDKIT")] + [os.path.join(l, "steamapps", "common", "The Witcher 3 REDkit") for l in steam_libraries()]:
        if cand and all(os.path.isfile(os.path.join(cand, swf_rel(m["swf"]))) for m in MOVIES):
            return cand
    sys.exit("REDkit not found: pass --redkit or set W3_REDKIT")


def find_ffdec(arg):
    for cand in [arg, os.environ.get("FFDEC"), os.path.join(os.environ.get("USERPROFILE", ""), "tools", "ffdec", "ffdec-cli.exe"),
                 os.path.join(os.environ.get("ProgramFiles(x86)", ""), "FFDec", "ffdec-cli.exe")]:
        if cand and os.path.isfile(cand):
            return cand
    sys.exit("FFDec not found: pass --ffdec or set FFDEC")


def run(cmd, cwd=None, timeout=900, ok_text=None):
    print("  >", " ".join(os.path.basename(c) if i == 0 else c for i, c in enumerate(cmd)))
    p = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, errors="replace", timeout=timeout)
    out = p.stdout + p.stderr
    if p.returncode != 0 or (ok_text and ok_text not in out):
        print(out[-4000:])
        sys.exit(f"failed: {os.path.basename(cmd[0])} {cmd[1]}")
    return out


def wcc(redkit, args, timeout=900):
    exe = os.path.join(redkit, "bin", "x64_RedKit", "wcc_lite.exe")
    out = run([exe] + args + ["-agreetoterms"], cwd=os.path.dirname(exe), timeout=timeout)
    errs = [l for l in out.splitlines() if "[Error][WCC]" in l]
    if errs:
        print("\n".join(errs))
        sys.exit(f"wcc_lite {args[0]} reported errors")
    return out


def patch(text, movie):
    ours = open(os.path.join(REPO, movie["ours"]), encoding="utf-8").read()
    text = text.replace("\r\n", "\n")
    for anchor, add, where in movie["hooks"]:
        n = text.count(anchor)
        if n != 1:
            sys.exit(f"{movie['swf']}: anchor found {n} times (the game's class changed?): {anchor[:60]!r}")
        text = text.replace(anchor, add + anchor if where == "before" else anchor + add)
    end = text.rstrip().rfind("}")          # the package's brace
    end = text.rstrip()[:end].rstrip().rfind("}")   # the class's brace
    return text[:end] + ours + "\n" + text[end:]


def build_movie(redkit, ffdec, movie, swfdir):
    name = movie["swf"]
    work = os.path.join(BUILD, name)
    os.makedirs(work)
    src = os.path.join(work, name + ".swf")
    shutil.copyfile(os.path.join(redkit, swf_rel(name)), src)
    cls = movie["class"]
    cls_file = cls.split(".")[-1] + ".as"
    print("1. decompile", cls)
    exp = os.path.join(work, "export")
    run([ffdec, "-selectclass", cls, "-export", "script", exp, src])
    found = [os.path.join(r, f) for r, _, fs in os.walk(exp) for f in fs if f == cls_file]
    if len(found) != 1:
        sys.exit(f"decompiled class not found ({found})")
    patched = patch(open(found[0], encoding="utf-8").read(), movie)
    imp = os.path.join(work, "import")
    rel = os.path.join(*cls.split(".")[:-1], cls_file)
    os.makedirs(os.path.dirname(os.path.join(imp, rel)))
    open(os.path.join(imp, rel), "w", encoding="utf-8", newline="\n").write(patched)
    print("2. recompile into the movie")
    out_swf = os.path.join(swfdir, name + ".swf")
    out = run([ffdec, "-importScript", src, out_swf, imp])
    if not os.path.isfile(out_swf) or re.search(r"(?i)error|exception", out):
        print(out[-4000:])
        sys.exit(f"FFDec did not compile {cls}")
    print(f"ok    {out_swf} ({os.path.getsize(out_swf)} bytes)")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--redkit")
    ap.add_argument("--ffdec")
    ap.add_argument("--movie-only", action="store_true", help="stop after the recompiled .swf files (no wcc_lite)")
    args = ap.parse_args()
    redkit = find_redkit(args.redkit)
    ffdec = find_ffdec(args.ffdec)
    shutil.rmtree(BUILD, ignore_errors=True)
    os.makedirs(BUILD)
    swfdir = os.path.join(BUILD, "swf")
    os.makedirs(swfdir)
    for movie in MOVIES:
        build_movie(redkit, ffdec, movie, swfdir)
    if args.movie_only:
        return

    print("3. wcc_lite swfimport / pack / metadatastore")
    work = os.path.join(redkit, "bin", "workspace", DEPOT_OUT)
    shutil.rmtree(work, ignore_errors=True)
    unc = os.path.join(BUILD, "uncooked", SWF_DIR)
    os.makedirs(unc)
    try:
        wcc(redkit, ["swfimport", f"-fromAbsPath={swfdir}\\", f"-toDepotPath={DEPOT_OUT}\\{SWF_DIR}\\"])
        for movie in MOVIES:
            redswf = os.path.join(work, SWF_DIR, movie["swf"] + ".redswf")
            if not os.path.isfile(redswf):
                sys.exit(f"swfimport wrote no {movie['swf']}.redswf")
            shutil.copyfile(redswf, os.path.join(unc, movie["swf"] + ".redswf"))
    finally:
        shutil.rmtree(work, ignore_errors=True)
    os.makedirs(CONTENT, exist_ok=True)
    for f in ("blob0.bundle", "metadata.store"):
        if os.path.exists(os.path.join(CONTENT, f)):
            os.remove(os.path.join(CONTENT, f))
    wcc(redkit, ["pack", f"-dir={os.path.join(BUILD, 'uncooked')}\\", f"-outdir={CONTENT}\\"])
    wcc(redkit, ["metadatastore", f"-path={CONTENT}\\"])
    for f in ("blob0.bundle", "metadata.store"):
        p = os.path.join(CONTENT, f)
        if not os.path.isfile(p):
            sys.exit(f"{f} was not written")
        print(f"ok    {p} ({os.path.getsize(p)} bytes)")


if __name__ == "__main__":
    main()
