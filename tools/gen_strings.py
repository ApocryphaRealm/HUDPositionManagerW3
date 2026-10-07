"""Write the mod's string tables: dist\\Mods\\modSquareMinimap\\content\\<lang>.w3strings, one per language file the game
ships (content\\content0\\<lang>.w3strings), then read every file back.

    python tools/gen_strings.py [--check]

The settings page's labels (Options > Mods > Square Minimap, also a page in Apocrypha Menu Framework). Our eleven
languages (rule 66) are written out; the game's other language files get a near language or English. --check only
compares the files on disk with the table and fails on a difference. Menu label keys are hashed "panel_<group>" /
"option_<var>" (logic library: Witcher 3 mod-menu labels are hashed keys).
"""
import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import w3strings  # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(REPO, "dist", "Mods", "modSquareMinimap", "content")

# Our string ids: idspace 7715 (2110000000 + 7715 * 1000 + n), next to Witcher Gear Overhaul's 7714.
ID_BASE = 2117715000

KEYS = ["panel_sqm_menu", "option_sqm_top", "option_sqm_right", "option_sqm_quest_gap"]
_NAME = "Square Minimap"
TEXT = {
    "en": [_NAME, "Distance from the top", "Distance from the right", "Gap above the quest list"],
    "de": [_NAME, "Abstand von oben", "Abstand von rechts", "Abstand zur Questliste"],
    "fr": [_NAME, "Distance du haut", "Distance de la droite", "Écart avec la liste des quêtes"],
    "es": [_NAME, "Distancia desde arriba", "Distancia desde la derecha", "Separación de la lista de misiones"],
    "it": [_NAME, "Distanza dall'alto", "Distanza da destra", "Spazio sopra l'elenco delle missioni"],
    "pl": [_NAME, "Odległość od góry", "Odległość od prawej", "Odstęp nad listą zadań"],
    "cz": [_NAME, "Vzdálenost od horního okraje", "Vzdálenost od pravého okraje", "Mezera nad seznamem úkolů"],
    "ru": [_NAME, "Отступ сверху", "Отступ справа", "Отступ над списком заданий"],
    "jp": [_NAME, "上端からの距離", "右端からの距離", "クエスト一覧との間隔"],
    "kr": [_NAME, "위쪽 여백", "오른쪽 여백", "퀘스트 목록과의 간격"],
    "cn": [_NAME, "距顶部距离", "距右侧距离", "与任务列表的间距"],
    "zh": [_NAME, "距頂部距離", "距右側距離", "與任務列表的間距"],
}
NEAR = {"esmx": "es", "br": "es", "ua": "ru"}


def rows_for(lang):
    texts = TEXT.get(lang) or TEXT.get(NEAR.get(lang, "")) or TEXT["en"]
    assert len(texts) == len(KEYS), lang
    return [(ID_BASE + i + 1, key, texts[i]) for i, key in enumerate(KEYS)]


def game_languages():
    content0 = os.path.join(os.environ.get("W3_GAME", r"D:\Steam Library\steamapps\common\The Witcher 3"), "content", "content0")
    if os.path.isdir(content0):
        langs = sorted(f[:-10] for f in os.listdir(content0) if f.endswith(".w3strings"))
        if langs:
            return langs
    return ["ar", "br", "cn", "cz", "de", "en", "es", "esmx", "fr", "hu", "it", "jp", "kr", "pl", "ru", "tr", "ua", "zh"]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true")
    a = ap.parse_args()
    os.makedirs(OUT, exist_ok=True)
    problems = []
    for lang in game_languages():
        path = os.path.join(OUT, f"{lang}.w3strings")
        rows = rows_for(lang)
        if not a.check:
            w3strings.write(path, rows)
        got = w3strings.read(path) if os.path.isfile(path) else {}
        for _, key, text in rows:
            if got.get(w3strings.key_hash(key)) != text:
                problems.append(f"{lang}.w3strings: {key} missing or stale")
    if problems:
        print("\n".join(problems))
        sys.exit(1)
    print(f"ok    {len(game_languages())} language files, {len(KEYS)} strings each")


if __name__ == "__main__":
    main()
