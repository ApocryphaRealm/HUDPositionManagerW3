"""Write the mod's string tables: dist\\Mods\\modHUDPositionManager\\content\\<lang>.w3strings, one per language file the game
ships (content\\content0\\<lang>.w3strings), then read every file back.

    python tools/gen_strings.py [--check]

The settings page's labels (Options > Mods > HUD Position Manager, also a page in Apocrypha Menu Framework). Our eleven
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
OUT = os.path.join(REPO, "dist", "Mods", "modHUDPositionManager", "content")

# Our string ids: idspace 7715 (2110000000 + 7715 * 1000 + n), next to Witcher Gear Overhaul's 7714.
ID_BASE = 2117715000

KEYS = ["panel_sqm_menu", "option_sqm_zoom", "option_sqm_top", "option_sqm_right", "option_sqm_quest_gap",
        "option_sqm_player_left", "option_sqm_player_bottom"]
_NAME = "HUD Position Manager"
TEXT = {
    "en": [_NAME, "Minimap zoom (%)", "Minimap: distance from the top", "Minimap: distance from the right",
           "Gap above the quest list", "Player widget: distance from the left", "Player widget: distance from the bottom"],
    "de": [_NAME, "Minikarten-Zoom (%)", "Minikarte: Abstand von oben", "Minikarte: Abstand von rechts",
           "Abstand zur Questliste", "Spieleranzeige: Abstand von links", "Spieleranzeige: Abstand von unten"],
    "fr": [_NAME, "Zoom de la mini-carte (%)", "Mini-carte : distance du haut", "Mini-carte : distance de la droite",
           "Écart avec la liste des quêtes", "Widget du joueur : distance de la gauche",
           "Widget du joueur : distance du bas"],
    "es": [_NAME, "Zoom del minimapa (%)", "Minimapa: distancia desde arriba", "Minimapa: distancia desde la derecha",
           "Separación de la lista de misiones", "Indicador del jugador: distancia desde la izquierda",
           "Indicador del jugador: distancia desde abajo"],
    "it": [_NAME, "Zoom della minimappa (%)", "Minimappa: distanza dall'alto", "Minimappa: distanza da destra",
           "Spazio sopra l'elenco delle missioni", "Indicatore del giocatore: distanza da sinistra",
           "Indicatore del giocatore: distanza dal basso"],
    "pl": [_NAME, "Przybliżenie minimapy (%)", "Minimapa: odległość od góry", "Minimapa: odległość od prawej",
           "Odstęp nad listą zadań", "Wskaźnik gracza: odległość od lewej", "Wskaźnik gracza: odległość od dołu"],
    "cz": [_NAME, "Přiblížení minimapy (%)", "Minimapa: vzdálenost od horního okraje",
           "Minimapa: vzdálenost od pravého okraje", "Mezera nad seznamem úkolů",
           "Ukazatel hráče: vzdálenost od levého okraje", "Ukazatel hráče: vzdálenost od dolního okraje"],
    "ru": [_NAME, "Масштаб мини-карты (%)", "Мини-карта: отступ сверху", "Мини-карта: отступ справа",
           "Отступ над списком заданий", "Панель игрока: отступ слева", "Панель игрока: отступ снизу"],
    "jp": [_NAME, "ミニマップの拡大率 (%)", "ミニマップ: 上端からの距離", "ミニマップ: 右端からの距離",
           "クエスト一覧との間隔", "プレイヤー表示: 左端からの距離", "プレイヤー表示: 下端からの距離"],
    "kr": [_NAME, "미니맵 확대 (%)", "미니맵: 위쪽 여백", "미니맵: 오른쪽 여백", "퀘스트 목록과의 간격",
           "플레이어 표시: 왼쪽 여백", "플레이어 표시: 아래쪽 여백"],
    "cn": [_NAME, "小地图缩放 (%)", "小地图：距顶部距离", "小地图：距右侧距离", "与任务列表的间距",
           "玩家状态栏：距左侧距离", "玩家状态栏：距底部距离"],
    "zh": [_NAME, "小地圖縮放 (%)", "小地圖：距頂部距離", "小地圖：距右側距離", "與任務列表的間距",
           "玩家狀態列：距左側距離", "玩家狀態列：距底部距離"],
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
