"""Write the settings page's eleven translation files (rule 66), the way Item Explorer W3 does them:
dist\\bin\\x64_dx12\\AMF\\Translations\\HUDPositionManager_<language>.txt - UTF-16LE with a BOM, CRLF, one line per key,
"$HPM_<Key><TAB>text" ("\\n" in a text for a new line), keys sorted. They go in the FRAMEWORK's translation folder because
AMF builds its font atlas from every <Mod>_<language>.txt there (src\\Strings.cpp reads them; src\\Strings.h).

    python tools/gen_translations.py           write the eleven files, then check them
    python tools/gen_translations.py --check   only check: fails (non-zero exit) when

  - a TR("HPM_<Key>", "<English>") in src\\*.cpp has no row in TABLE, or a row lacks a language or has an empty text;
  - a row's English is not the source's English any more (the translations need a look);
  - a row is for a key the source no longer has;
  - a text's "%" count differs from its English (placeholders such as %d and %%);
  - a file on disk is missing or differs from what the table writes.

English comes from the source (the TR() calls); TABLE holds it too, so a changed English is caught. Game terms follow
each language's Witcher 3 localisation where it has one (Stamina / Adrenaline / Toxicity, Signs, quests), and the
element names match tools\\gen_strings.py (the game's Options > Mods page). "HUD", "Apocrypha Menu Framework" and the
preset name "ApocryphaRealm" stay untranslated. Chinese is Simplified (Item Explorer's "chinese").
"""
import argparse
import glob
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(REPO, "src")
OUT = os.path.join(REPO, "dist", "bin", "x64_dx12", "AMF", "Translations")
PREFIX = "HUDPositionManager_"

# Item Explorer W3's file names; the row order below follows LANGS (English first)
LANGS = ["english", "german", "french", "spanish", "italian", "polish", "czech", "russian", "japanese", "korean", "chinese"]

TR = re.compile(r'TR\(\s*"(HPM_\w+)"\s*,\s*"((?:[^"\\]|\\.)*)"\s*\)')

# key -> {language: text}
TABLE = {}


def row(key, *texts):
    assert len(texts) == len(LANGS), key
    assert key not in TABLE, key
    TABLE[key] = dict(zip(LANGS, texts))


# ------------------------------------------------------------------------------------------------ the Presets tab
row("HPM_TabPresets", "Presets",
    "Presets", "Préréglages", "Preajustes", "Preset", "Presety", "Předvolby", "Пресеты",
    "プリセット", "프리셋", "预设")
row("HPM_PresetsIntro",
    "A preset is a whole layout: every element's position, size and visibility. Load one made by someone else, or save your own to switch between.",
    "Ein Preset ist ein ganzes Layout: Position, Größe und Sichtbarkeit jedes Elements. Lade eines, das jemand anderes gemacht hat, oder speichere deine eigenen, um zwischen ihnen zu wechseln.",
    "Un préréglage est une disposition complète : la position, la taille et la visibilité de chaque élément. Chargez-en un créé par quelqu'un d'autre, ou enregistrez les vôtres pour passer de l'un à l'autre.",
    "Un preajuste es una disposición completa: la posición, el tamaño y la visibilidad de cada elemento. Carga uno hecho por otra persona o guarda los tuyos para cambiar entre ellos.",
    "Un preset è un layout completo: posizione, dimensione e visibilità di ogni elemento. Caricane uno creato da qualcun altro, o salva i tuoi per passare dall'uno all'altro.",
    "Preset to cały układ: pozycja, rozmiar i widoczność każdego elementu. Wczytaj preset innego autora albo zapisz własne, aby się między nimi przełączać.",
    "Předvolba je celé rozložení: pozice, velikost a viditelnost každého prvku. Načtěte předvolbu od někoho jiného, nebo si uložte vlastní a přepínejte mezi nimi.",
    "Пресет - это целая раскладка: положение, размер и видимость каждого элемента. Загрузите чужой пресет или сохраните свои, чтобы переключаться между ними.",
    "プリセットはレイアウト全体です: 各要素の位置、サイズ、表示。ほかの人が作ったものを読み込むか、自分のものを保存して切り替えられます。",
    "프리셋은 레이아웃 전체입니다: 각 요소의 위치, 크기, 표시 여부. 다른 사람이 만든 프리셋을 불러오거나, 직접 저장해 바꿔 가며 쓰세요.",
    "预设是一整套布局：每个元素的位置、大小和可见性。可以加载别人制作的预设，也可以保存自己的预设以便切换。")
row("HPM_PresetsNone",
    "No presets yet. Save your layout below, or put a preset file from another author in the folder above.",
    "Noch keine Presets. Speichere unten dein Layout oder lege die Preset-Datei eines anderen Autors in den Ordner oben.",
    "Aucun préréglage pour l'instant. Enregistrez votre disposition ci-dessous, ou placez le fichier de préréglage d'un autre auteur dans le dossier ci-dessus.",
    "Aún no hay preajustes. Guarda tu disposición abajo o pon en la carpeta de arriba un archivo de preajuste de otro autor.",
    "Ancora nessun preset. Salva il tuo layout qui sotto, o metti nella cartella qui sopra il file di preset di un altro autore.",
    "Brak presetów. Zapisz swój układ poniżej albo umieść plik presetu innego autora w folderze powyżej.",
    "Zatím žádné předvolby. Uložte své rozložení níže, nebo vložte soubor předvolby od jiného autora do složky výše.",
    "Пресетов пока нет. Сохраните свою раскладку ниже или положите файл пресета другого автора в папку выше.",
    "プリセットはまだありません。下で今のレイアウトを保存するか、ほかの作者のプリセットファイルを上のフォルダーに置いてください。",
    "아직 프리셋이 없습니다. 아래에서 레이아웃을 저장하거나, 다른 제작자의 프리셋 파일을 위 폴더에 넣으세요.",
    "还没有预设。在下方保存你的布局，或把其他作者的预设文件放进上面的文件夹。")
row("HPM_Preset", "Preset",
    "Preset", "Préréglage", "Preajuste", "Preset", "Preset", "Předvolba", "Пресет",
    "プリセット", "프리셋", "预设")
row("HPM_PresetLoad", "Load this preset",
    "Dieses Preset laden", "Charger ce préréglage", "Cargar este preajuste", "Carica questo preset",
    "Wczytaj ten preset", "Načíst tuto předvolbu", "Загрузить этот пресет",
    "このプリセットを読み込む", "이 프리셋 불러오기", "加载此预设")
row("HPM_PresetLoaded", "Loaded",
    "Geladen", "Chargé", "Cargado", "Caricato", "Wczytano", "Načteno", "Загружен",
    "読み込みました", "불러옴", "已加载")
row("HPM_PresetFailed", "Could not read",
    "Konnte nicht gelesen werden", "Lecture impossible", "No se pudo leer", "Impossibile leggere",
    "Nie udało się odczytać", "Nelze načíst", "Не удалось прочитать",
    "読み込めませんでした", "읽을 수 없음", "无法读取")
row("HPM_PresetDelete", "Delete",
    "Löschen", "Supprimer", "Borrar", "Elimina", "Usuń", "Smazat", "Удалить",
    "削除", "삭제", "删除")
row("HPM_PresetDeleteSure", "Press again to delete",
    "Zum Löschen erneut drücken", "Appuyez à nouveau pour supprimer", "Pulsa de nuevo para borrar",
    "Premi di nuovo per eliminare", "Naciśnij ponownie, aby usunąć", "Stiskněte znovu pro smazání",
    "Нажмите ещё раз, чтобы удалить", "もう一度押すと削除", "한 번 더 누르면 삭제", "再按一次以删除")
row("HPM_PresetDeleted", "Deleted",
    "Gelöscht", "Supprimé", "Borrado", "Eliminato", "Usunięto", "Smazáno", "Удалён",
    "削除しました", "삭제됨", "已删除")
row("HPM_PresetDeleteFailed", "Could not delete",
    "Konnte nicht gelöscht werden", "Suppression impossible", "No se pudo borrar", "Impossibile eliminare",
    "Nie udało się usunąć", "Nelze smazat", "Не удалось удалить",
    "削除できませんでした", "삭제할 수 없음", "无法删除")
row("HPM_PresetSaveGroup", "Save my layout",
    "Mein Layout speichern", "Enregistrer ma disposition", "Guardar mi disposición", "Salva il mio layout",
    "Zapisz mój układ", "Uložit mé rozložení", "Сохранить мою раскладку",
    "レイアウトを保存", "내 레이아웃 저장", "保存我的布局")
row("HPM_PresetNew", "New preset",
    "Neues Preset", "Nouveau préréglage", "Nuevo preajuste", "Nuovo preset", "Nowy preset", "Nová předvolba",
    "Новый пресет", "新しいプリセット", "새 프리셋", "新预设")
row("HPM_PresetSaveTo", "Save to",
    "Speichern in", "Enregistrer dans", "Guardar en", "Salva in", "Zapisz do", "Uložit do", "Сохранить в",
    "保存先", "저장 위치", "保存到")
row("HPM_PresetName", "Name",
    "Name", "Nom", "Nombre", "Nome", "Nazwa", "Název", "Название",
    "名前", "이름", "名称")
row("HPM_PresetNameHint", "With a controller, leave the name as it is: each save gets its own number.",
    "Mit einem Controller lass den Namen, wie er ist: Jede Speicherung bekommt ihre eigene Nummer.",
    "Avec une manette, laissez le nom tel quel : chaque enregistrement reçoit son propre numéro.",
    "Con un mando, deja el nombre como está: cada guardado recibe su propio número.",
    "Con un controller, lascia il nome così com'è: ogni salvataggio riceve il suo numero.",
    "Na kontrolerze zostaw nazwę bez zmian: każdy zapis dostaje własny numer.",
    "S ovladačem nechte název, jak je: každé uložení dostane vlastní číslo.",
    "С геймпадом оставьте название как есть: каждое сохранение получит свой номер.",
    "コントローラーでは名前はそのままにしてください: 保存ごとに番号が付きます。",
    "컨트롤러로는 이름을 그대로 두세요: 저장할 때마다 번호가 붙습니다.",
    "使用手柄时请保留名称不变：每次保存都会有自己的编号。")
row("HPM_PresetSave", "Save as a preset",
    "Als Preset speichern", "Enregistrer comme préréglage", "Guardar como preajuste", "Salva come preset",
    "Zapisz jako preset", "Uložit jako předvolbu", "Сохранить как пресет",
    "プリセットとして保存", "프리셋으로 저장", "保存为预设")
row("HPM_PresetSaveFailed", "The preset could not be written.",
    "Das Preset konnte nicht geschrieben werden.", "Le préréglage n'a pas pu être écrit.",
    "No se pudo escribir el preajuste.", "Impossibile scrivere il preset.", "Nie udało się zapisać presetu.",
    "Předvolbu nelze zapsat.", "Не удалось записать пресет.",
    "プリセットを書き込めませんでした。", "프리셋을 기록할 수 없습니다.", "无法写入预设。")
row("HPM_PresetSaved", "Saved",
    "Gespeichert", "Enregistré", "Guardado", "Salvato", "Zapisano", "Uloženo", "Сохранён",
    "保存しました", "저장됨", "已保存")
row("HPM_PresetUpdate", "Update this preset with my layout",
    "Dieses Preset mit meinem Layout aktualisieren", "Mettre à jour ce préréglage avec ma disposition",
    "Actualizar este preajuste con mi disposición", "Aggiorna questo preset con il mio layout",
    "Zaktualizuj ten preset moim układem", "Aktualizovat tuto předvolbu mým rozložením",
    "Обновить этот пресет моей раскладкой",
    "このプリセットを今のレイアウトで更新", "이 프리셋을 내 레이아웃으로 갱신", "用我的布局更新此预设")

# ------------------------------------------------------------------------------------------------ the Layout tab
row("HPM_TabLayout", "Layout",
    "Layout", "Disposition", "Disposición", "Layout", "Układ", "Rozložení", "Раскладка",
    "レイアウト", "레이아웃", "布局")
row("HPM_Intro",
    "Move, resize or hide each part of the HUD. Changes show in the HUD at once and are saved automatically.",
    "Verschiebe, skaliere oder verstecke jeden Teil des HUD. Änderungen erscheinen sofort im HUD und werden automatisch gespeichert.",
    "Déplacez, redimensionnez ou masquez chaque partie du HUD. Les changements s'affichent aussitôt dans le HUD et sont enregistrés automatiquement.",
    "Mueve, cambia de tamaño u oculta cada parte del HUD. Los cambios se ven en el HUD al instante y se guardan automáticamente.",
    "Sposta, ridimensiona o nascondi ogni parte dell'HUD. Le modifiche si vedono subito nell'HUD e vengono salvate automaticamente.",
    "Przesuwaj, zmieniaj rozmiar lub ukrywaj każdą część HUD. Zmiany widać w HUD od razu i są zapisywane automatycznie.",
    "Posuňte, změňte velikost nebo skryjte každou část HUD. Změny se v HUD projeví hned a ukládají se automaticky.",
    "Перемещайте, меняйте размер или скрывайте любую часть HUD. Изменения сразу видны в HUD и сохраняются автоматически.",
    "HUDの各部分を移動、サイズ変更、非表示にできます。変更はすぐにHUDに反映され、自動的に保存されます。",
    "HUD의 각 부분을 옮기거나 크기를 바꾸거나 숨기세요. 변경 사항은 HUD에 바로 보이고 자동으로 저장됩니다.",
    "移动、缩放或隐藏 HUD 的每个部分。更改会立即在 HUD 中显示并自动保存。")
row("HPM_NoBridge",
    "This Apocrypha Menu Framework cannot pass settings to the game (1.0.2 or later is needed).",
    "Dieses Apocrypha Menu Framework kann keine Einstellungen an das Spiel übergeben (1.0.2 oder neuer wird benötigt).",
    "Cet Apocrypha Menu Framework ne peut pas transmettre les réglages au jeu (la version 1.0.2 ou plus récente est requise).",
    "Este Apocrypha Menu Framework no puede pasar los ajustes al juego (se necesita la 1.0.2 o posterior).",
    "Questo Apocrypha Menu Framework non può passare le impostazioni al gioco (serve la 1.0.2 o successiva).",
    "Ten Apocrypha Menu Framework nie może przekazać ustawień do gry (potrzebna wersja 1.0.2 lub nowsza).",
    "Tento Apocrypha Menu Framework nemůže předat nastavení hře (je potřeba verze 1.0.2 nebo novější).",
    "Этот Apocrypha Menu Framework не может передавать настройки в игру (нужна версия 1.0.2 или новее).",
    "このApocrypha Menu Frameworkは設定をゲームに渡せません (1.0.2以降が必要です)。",
    "이 Apocrypha Menu Framework는 설정을 게임에 전달할 수 없습니다 (1.0.2 이상 필요).",
    "此 Apocrypha Menu Framework 无法将设置传给游戏（需要 1.0.2 或更高版本）。")
row("HPM_Enabled", "Apply my layout",
    "Mein Layout anwenden", "Appliquer ma disposition", "Aplicar mi disposición", "Applica il mio layout",
    "Zastosuj mój układ", "Použít mé rozložení", "Применить мою раскладку",
    "レイアウトを適用", "내 레이아웃 적용", "应用我的布局")
row("HPM_EnabledHint", "Off: every element goes back to where the game puts it.",
    "Aus: Jedes Element kehrt dorthin zurück, wo das Spiel es platziert.",
    "Désactivé : chaque élément retourne là où le jeu le place.",
    "Desactivado: cada elemento vuelve a donde lo pone el juego.",
    "Disattivo: ogni elemento torna dove lo mette il gioco.",
    "Wyłączone: każdy element wraca tam, gdzie umieszcza go gra.",
    "Vypnuto: každý prvek se vrátí tam, kam ho dává hra.",
    "Выкл.: каждый элемент возвращается туда, где его ставит игра.",
    "オフ: すべての要素がゲームの置く位置に戻ります。",
    "꺼짐: 모든 요소가 게임이 두는 위치로 돌아갑니다.",
    "关闭：所有元素回到游戏默认的位置。")
row("HPM_Unlocked", "Free placement",
    "Freie Platzierung", "Placement libre", "Colocación libre", "Posizionamento libero",
    "Swobodne rozmieszczanie", "Volné umístění", "Свободное размещение",
    "自由配置", "자유 배치", "自由放置")
row("HPM_UnlockedHint",
    "On: the move sliders go all the way, past the edges of the screen. Off: an element stops where it meets the edge of the screen.",
    "An: Die Verschiebe-Regler gehen bis ganz ans Ende, über die Bildschirmränder hinaus. Aus: Ein Element stoppt, wo es den Bildschirmrand erreicht.",
    "Activé : les curseurs de déplacement vont jusqu'au bout, au-delà des bords de l'écran. Désactivé : un élément s'arrête quand il touche le bord de l'écran.",
    "Activado: los controles de movimiento llegan hasta el final, más allá de los bordes de la pantalla. Desactivado: un elemento se detiene al tocar el borde de la pantalla.",
    "Attivo: i cursori di spostamento arrivano fino in fondo, oltre i bordi dello schermo. Disattivo: un elemento si ferma quando tocca il bordo dello schermo.",
    "Włączone: suwaki przesuwania sięgają do końca, poza krawędzie ekranu. Wyłączone: element zatrzymuje się przy krawędzi ekranu.",
    "Zapnuto: posuvníky posunu jdou až na konec, za okraje obrazovky. Vypnuto: prvek se zastaví na okraji obrazovky.",
    "Вкл.: ползунки сдвига идут до конца, за края экрана. Выкл.: элемент останавливается у края экрана.",
    "オン: 移動スライダーが画面の端を越えて最後まで動きます。オフ: 要素は画面の端で止まります。",
    "켜짐: 이동 슬라이더가 화면 가장자리를 넘어 끝까지 움직입니다. 꺼짐: 요소가 화면 가장자리에서 멈춥니다.",
    "开启：移动滑块可一直拉到底，超出屏幕边缘。关闭：元素碰到屏幕边缘时停止。")
row("HPM_GroupVisibility", "HUD visibility",
    "HUD-Sichtbarkeit", "Visibilité du HUD", "Visibilidad del HUD", "Visibilità dell'HUD", "Widoczność HUD",
    "Viditelnost HUD", "Видимость HUD", "HUDの表示", "HUD 표시", "HUD 可见性")
row("HPM_AlwaysAll", "Always visible",
    "Immer sichtbar", "Toujours visible", "Siempre visible", "Sempre visibile", "Zawsze widoczny",
    "Vždy viditelné", "Всегда видно", "常に表示", "항상 표시", "始终可见")
row("HPM_AlwaysAllOnHint",
    "The player widget and the bars stay shown while you play. Menus, dialogue and loading screens still hide the HUD.",
    "Die Spieleranzeige und die Leisten bleiben beim Spielen sichtbar. Menüs, Dialoge und Ladebildschirme blenden das HUD weiterhin aus.",
    "Le widget du joueur et les barres restent affichés pendant que vous jouez. Les menus, les dialogues et les écrans de chargement masquent toujours le HUD.",
    "El indicador del jugador y las barras siguen visibles mientras juegas. Los menús, los diálogos y las pantallas de carga siguen ocultando el HUD.",
    "L'indicatore del giocatore e le barre restano visibili mentre giochi. Menu, dialoghi e schermate di caricamento nascondono comunque l'HUD.",
    "Wskaźnik gracza i paski pozostają widoczne podczas gry. Menu, dialogi i ekrany wczytywania nadal ukrywają HUD.",
    "Ukazatel hráče a pruhy zůstávají během hraní zobrazené. Nabídky, rozhovory a načítací obrazovky HUD stále skrývají.",
    "Панель игрока и полосы остаются на экране во время игры. Меню, диалоги и экраны загрузки по-прежнему скрывают HUD.",
    "プレイ中、プレイヤー表示とバーは表示されたままです。メニュー、会話、ロード画面では引き続きHUDが隠れます。",
    "플레이 중 플레이어 표시와 바가 계속 보입니다. 메뉴, 대화, 로딩 화면에서는 여전히 HUD가 숨겨집니다.",
    "游戏中玩家状态栏和各条保持显示。菜单、对话和加载画面仍会隐藏 HUD。")
row("HPM_AlwaysAllOffHint", "Off: the game decides. The player widget fades out when it is full.",
    "Aus: Das Spiel entscheidet. Die Spieleranzeige blendet sich aus, wenn sie voll ist.",
    "Désactivé : le jeu décide. Le widget du joueur disparaît quand il est plein.",
    "Desactivado: decide el juego. El indicador del jugador se desvanece cuando está lleno.",
    "Disattivo: decide il gioco. L'indicatore del giocatore svanisce quando è pieno.",
    "Wyłączone: decyduje gra. Wskaźnik gracza wygasa, gdy jest pełny.",
    "Vypnuto: rozhoduje hra. Ukazatel hráče zmizí, když je plný.",
    "Выкл.: решает игра. Панель игрока исчезает, когда она заполнена.",
    "オフ: ゲームに任せます。プレイヤー表示は満タンになるとフェードアウトします。",
    "꺼짐: 게임이 결정합니다. 플레이어 표시는 가득 차면 사라집니다.",
    "关闭：由游戏决定。玩家状态栏满值时会淡出。")
row("HPM_ResetAll", "Reset every element",
    "Alle Elemente zurücksetzen", "Réinitialiser tous les éléments", "Restablecer todos los elementos",
    "Ripristina tutti gli elementi", "Zresetuj wszystkie elementy", "Obnovit všechny prvky", "Сбросить все элементы",
    "すべての要素をリセット", "모든 요소 초기화", "重置所有元素")

# ------------------------------------------------------------------------------------------------ the HUD elements
# (the same names as tools\gen_strings.py's Options > Mods labels)
row("HPM_El_Minimap", "Minimap",
    "Minikarte", "Mini-carte", "Minimapa", "Minimappa", "Minimapa", "Minimapa", "Мини-карта",
    "ミニマップ", "미니맵", "小地图")
row("HPM_El_Quests", "Quest list",
    "Questliste", "Liste des quêtes", "Lista de misiones", "Elenco delle missioni", "Lista zadań", "Seznam úkolů",
    "Список заданий", "クエスト一覧", "퀘스트 목록", "任务列表")
row("HPM_El_Medallion", "Medallion",
    "Medaillon", "Médaillon", "Medallón", "Medaglione", "Medalion", "Medailon", "Медальон", "メダリオン", "메달리온", "徽章")
row("HPM_El_Health", "Health bar",
    "Lebensleiste", "Barre de vie", "Barra de vida", "Barra della salute", "Pasek zdrowia", "Ukazatel zdraví",
    "Полоса здоровья", "体力バー", "체력 바", "生命值条")
row("HPM_El_Experience", "Experience bar",
    "Erfahrungsleiste", "Barre d'expérience", "Barra de experiencia", "Barra dell'esperienza", "Pasek doświadczenia",
    "Ukazatel zkušeností", "Полоса опыта", "経験値バー", "경험치 바", "经验条")
row("HPM_AnchorConverting", "Being moved over to its sliders - it keeps its place on the screen.",
    "Wird auf die Regler übertragen - es behält seinen Platz auf dem Bildschirm.",
    "En cours de transfert vers ses curseurs - il garde sa place à l'écran.",
    "Se está pasando a sus controles deslizantes: conserva su sitio en la pantalla.",
    "In corso di passaggio ai suoi cursori: mantiene il suo posto sullo schermo.",
    "Przenoszenie na suwaki - zachowuje swoje miejsce na ekranie.",
    "Převádí se na posuvníky - zachová si své místo na obrazovce.",
    "Переносится на ползунки - остаётся на своём месте на экране.",
    "スライダーへ移行中です。画面上の位置はそのままです。",
    "슬라이더로 옮기는 중입니다. 화면의 위치는 그대로입니다.",
    "正在转为由滑块控制——屏幕上的位置保持不变。")
row("HPM_WolfPartHint",
    "One part of the player widget, on its own: it moves, sizes and hides by itself. Pick it on the Combined widgets tab to move it together with other parts or widgets.",
    "Ein Teil der Spieleranzeige, für sich allein: Er wird einzeln verschoben, skaliert und ausgeblendet. Wähle ihn im Tab Kombinierte Anzeigen, um ihn zusammen mit anderen Teilen oder Anzeigen zu bewegen.",
    "Une partie du widget du joueur, indépendante : elle se déplace, se redimensionne et se masque seule. Choisissez-la dans l'onglet Widgets combinés pour la déplacer avec d'autres parties ou widgets.",
    "Una parte del indicador del jugador, por separado: se mueve, cambia de tamaño y se oculta sola. Elígela en la pestaña Indicadores combinados para moverla junto con otras partes o indicadores.",
    "Una parte dell'indicatore del giocatore, a sé stante: si sposta, si ridimensiona e si nasconde da sola. Sceglila nella scheda Indicatori combinati per spostarla insieme ad altre parti o indicatori.",
    "Jedna część wskaźnika gracza, osobno: przesuwa się, zmienia rozmiar i ukrywa samodzielnie. Wybierz ją na karcie Połączone wskaźniki, aby przesuwać ją razem z innymi częściami lub wskaźnikami.",
    "Jedna část ukazatele hráče, samostatně: posouvá se, mění velikost a skrývá se sama. Vyberte ji na kartě Spojené ukazatele, aby se posouvala spolu s dalšími částmi nebo ukazateli.",
    "Отдельная часть панели игрока: двигается, меняет размер и скрывается сама по себе. Выберите её на вкладке «Объединённые виджеты», чтобы двигать вместе с другими частями или виджетами.",
    "プレイヤー表示の一部を単独で扱います: 個別に移動、サイズ変更、非表示ができます。ほかの部分やウィジェットと一緒に動かすには、結合ウィジェットのタブで選んでください。",
    "플레이어 표시의 한 부분을 따로 다룹니다: 혼자 움직이고, 크기가 바뀌고, 숨겨집니다. 다른 부분이나 위젯과 함께 움직이려면 결합된 위젯 탭에서 고르세요.",
    "玩家状态栏的一个独立部分：可单独移动、缩放和隐藏。在“组合组件”标签页中选中它，即可与其他部分或组件一起移动。")
row("HPM_AlwaysPartHint",
    "The game fades the player widget out on its own. On: this part stays shown while you play; the parts left off still fade.",
    "Das Spiel blendet die Spieleranzeige von selbst aus. An: Dieser Teil bleibt beim Spielen sichtbar; die übrigen Teile werden weiterhin ausgeblendet.",
    "Le jeu estompe le widget du joueur de lui-même. Activé : cette partie reste affichée pendant que vous jouez ; les parties laissées désactivées s'estompent toujours.",
    "El juego atenúa el indicador del jugador por su cuenta. Activado: esta parte sigue visible mientras juegas; las partes que lo tienen desactivado siguen atenuándose.",
    "Il gioco dissolve l'indicatore del giocatore da solo. Attivo: questa parte resta visibile mentre giochi; le parti lasciate disattivate si dissolvono comunque.",
    "Gra sama wygasza wskaźnik gracza. Włączone: ta część pozostaje widoczna podczas gry; części z wyłączoną opcją nadal się wygaszają.",
    "Hra ukazatel hráče sama skrývá. Zapnuto: tato část zůstane při hraní vidět; části s vypnutou volbou se dál skrývají.",
    "Игра сама скрывает панель игрока. Вкл.: эта часть остаётся видимой во время игры; части с выключенной настройкой по-прежнему исчезают.",
    "ゲームはプレイヤー表示を自動でフェードアウトします。オン: この部分はプレイ中ずっと表示されます。オフのままの部分はこれまでどおり消えます。",
    "게임은 플레이어 표시를 스스로 흐리게 숨깁니다. 켬: 이 부분은 플레이하는 동안 계속 보입니다. 꺼 둔 부분은 계속 사라집니다.",
    "游戏会自动淡出玩家状态栏。开启：此部分在游戏中始终显示；未开启的部分仍会淡出。")
row("HPM_El_Stamina", "Stamina bar",
    "Ausdauerleiste", "Barre d'endurance", "Barra de aguante", "Barra della stamina", "Pasek wytrzymałości",
    "Ukazatel výdrže", "Полоса выносливости", "スタミナバー", "기력 바", "耐力条")
row("HPM_El_Adrenaline", "Adrenaline bar",
    "Adrenalinleiste", "Barre d'adrénaline", "Barra de adrenalina", "Barra dell'adrenalina", "Pasek adrenaliny",
    "Ukazatel adrenalinu", "Полоса адреналина", "アドレナリンバー", "아드레날린 바", "肾上腺素条")
row("HPM_El_Toxicity", "Toxicity bar",
    "Toxizitätsleiste", "Barre de toxicité", "Barra de toxicidad", "Barra della tossicità", "Pasek toksyczności",
    "Ukazatel toxicity", "Полоса токсичности", "毒性バー", "독성 바", "毒性条")
row("HPM_El_Magic", "Magic bar",
    "Magieleiste", "Barre de magie", "Barra de magia", "Barra della magia", "Pasek magii",
    "Ukazatel magie", "Полоса магии", "魔力バー", "마력 바", "魔力条")
row("HPM_El_Sign", "Sign badge",
    "Zeichen-Abzeichen", "Badge du Signe", "Insignia de la señal", "Distintivo del segno", "Odznaka znaku",
    "Odznak znamení", "Значок знака", "印のバッジ", "표식 배지", "法印徽章")
row("HPM_El_Skull", "Toxicity skull",
    "Toxizitätsschädel", "Crâne de toxicité", "Calavera de toxicidad", "Teschio della tossicità",
    "Czaszka toksyczności", "Lebka toxicity", "Череп токсичности", "毒性のドクロ", "독성 해골", "毒性骷髅")
row("HPM_El_Buffs", "Effects (food, potions)",
    "Effekte (Essen, Tränke)", "Effets (nourriture, potions)", "Efectos (comida, pociones)",
    "Effetti (cibo, pozioni)", "Efekty (jedzenie, mikstury)", "Efekty (jídlo, lektvary)",
    "Эффекты (еда, эликсиры)", "効果 (食料、薬)", "효과 (음식, 물약)", "效果（食物、药水）")
row("HPM_El_QuickItems", "Quick items",
    "Schnellzugriff", "Objets rapides", "Objetos rápidos", "Oggetti rapidi", "Szybkie przedmioty",
    "Rychlé předměty", "Быстрые предметы", "クイックアイテム", "빠른 아이템", "快捷物品")
row("HPM_El_Dialogue", "Dialogue choices",
    "Dialogoptionen", "Choix de dialogue", "Opciones de diálogo", "Scelte di dialogo", "Opcje dialogowe",
    "Možnosti rozhovoru", "Варианты диалога", "会話の選択肢", "대화 선택지", "对话选项")
row("HPM_El_Subtitles", "Subtitles",
    "Untertitel", "Sous-titres", "Subtítulos", "Sottotitoli", "Napisy", "Titulky", "Субтитры",
    "字幕", "자막", "字幕")
row("HPM_El_Messages", "Messages",
    "Meldungen", "Messages", "Mensajes", "Messaggi", "Komunikaty", "Zprávy", "Сообщения",
    "メッセージ", "메시지", "消息")
row("HPM_El_Hints", "Button hints",
    "Tastenhinweise", "Aide des touches", "Indicaciones de botones", "Suggerimenti dei tasti",
    "Podpowiedzi przycisków", "Nápověda tlačítek", "Подсказки кнопок", "ボタンのヒント", "버튼 안내", "按键提示")
row("HPM_El_Area", "Area name",
    "Gebietsname", "Nom de la zone", "Nombre de la zona", "Nome dell'area", "Nazwa obszaru", "Název oblasti",
    "Название области", "地域名", "지역 이름", "区域名称")
row("HPM_El_Journal", "Quest and journal updates",
    "Quest- und Tagebuchmeldungen", "Mises à jour des quêtes et du journal", "Avisos de misiones y del diario",
    "Aggiornamenti di missioni e diario", "Aktualizacje zadań i dziennika", "Aktualizace úkolů a deníku",
    "Обновления заданий и журнала", "クエストと日誌の更新", "퀘스트 및 일지 갱신", "任务与日志更新")
row("HPM_El_LootFeed", "Items received",
    "Erhaltene Gegenstände", "Objets reçus", "Objetos recibidos", "Oggetti ricevuti", "Otrzymane przedmioty",
    "Získané předměty", "Полученные предметы", "入手したアイテム", "획득한 아이템", "获得的物品")
row("HPM_El_Enemy", "Enemy health",
    "Gegner-Lebensleiste", "Vie de l'ennemi", "Vida del enemigo", "Salute del nemico", "Zdrowie wroga",
    "Zdraví nepřítele", "Здоровье врага", "敵の体力", "적 체력", "敌人生命值")
row("HPM_El_Boss", "Boss health",
    "Boss-Lebensleiste", "Vie du boss", "Vida del jefe", "Salute del boss", "Zdrowie bossa", "Zdraví bosse",
    "Здоровье босса", "ボスの体力", "보스 체력", "首领生命值")
row("HPM_El_Companion", "Companion health",
    "Begleiter-Lebensleiste", "Vie du compagnon", "Vida del acompañante", "Salute del compagno",
    "Zdrowie towarzysza", "Zdraví společníka", "Здоровье спутника", "仲間の体力", "동료 체력", "同伴生命值")
row("HPM_El_Breath", "Breath bar",
    "Atemleiste", "Barre d'oxygène", "Barra de aliento", "Barra del respiro", "Pasek oddechu", "Ukazatel dechu",
    "Полоса дыхания", "呼吸バー", "호흡 바", "呼吸条")
row("HPM_El_HorseStamina", "Horse stamina",
    "Pferdeausdauer", "Endurance du cheval", "Aguante del caballo", "Stamina del cavallo", "Wytrzymałość konia",
    "Výdrž koně", "Выносливость коня", "馬のスタミナ", "말 기력", "马匹耐力")
row("HPM_El_HorseFear", "Horse fear",
    "Pferdeangst", "Peur du cheval", "Miedo del caballo", "Paura del cavallo", "Strach konia", "Strach koně",
    "Страх коня", "馬の恐怖", "말 공포", "马匹恐惧")
row("HPM_El_Boat", "Boat health",
    "Bootszustand", "État du bateau", "Estado del bote", "Stato della barca", "Stan łodzi", "Stav lodi",
    "Прочность лодки", "ボートの耐久", "배 내구도", "船只耐久")
row("HPM_El_Damaged", "Damaged gear",
    "Beschädigte Ausrüstung", "Équipement abîmé", "Equipo dañado", "Equipaggiamento danneggiato",
    "Uszkodzony ekwipunek", "Poškozená výstroj", "Повреждённое снаряжение", "損傷した装備", "손상된 장비", "损坏的装备")
row("HPM_El_TimeLeft", "Time limit",
    "Zeitlimit", "Temps limite", "Tiempo límite", "Tempo limite", "Limit czasu", "Časový limit", "Лимит времени",
    "制限時間", "제한 시간", "时限")
row("HPM_El_TimeLapse", "Passing time text",
    "Zeitsprung-Text", "Texte du temps qui passe", "Texto del paso del tiempo", "Testo del tempo che passa",
    "Tekst upływu czasu", "Text plynutí času", "Текст течения времени", "時間経過の表示", "시간 경과 문구", "时间流逝文字")

# ------------------------------------------------------------------------------------------------ each element's Place

# ------------------------------------------------------------------------------------------------ an element's tab
row("HPM_NoHud", "The HUD has not reported yet - load a game to see this element.",
    "Das HUD hat sich noch nicht gemeldet - lade einen Spielstand, um dieses Element zu sehen.",
    "Le HUD ne s'est pas encore signalé - chargez une partie pour voir cet élément.",
    "El HUD aún no ha informado - carga una partida para ver este elemento.",
    "L'HUD non ha ancora risposto - carica una partita per vedere questo elemento.",
    "HUD jeszcze się nie zgłosił - wczytaj grę, aby zobaczyć ten element.",
    "HUD se ještě neohlásil - načtěte hru, abyste tento prvek viděli.",
    "HUD ещё не ответил - загрузите игру, чтобы увидеть этот элемент.",
    "HUDからまだ応答がありません - この要素を見るにはゲームをロードしてください。",
    "HUD가 아직 응답하지 않았습니다 - 이 요소를 보려면 게임을 불러오세요.",
    "HUD 尚未回报 - 载入游戏即可看到此元素。")
row("HPM_NotFound", "Not in your HUD right now: it may appear later. Its settings are kept.",
    "Gerade nicht in deinem HUD: Es kann später erscheinen. Seine Einstellungen bleiben erhalten.",
    "Absent de votre HUD pour le moment : il peut apparaître plus tard. Ses réglages sont conservés.",
    "Ahora no está en tu HUD: puede aparecer más tarde. Sus ajustes se conservan.",
    "Al momento non è nel tuo HUD: potrebbe comparire più tardi. Le sue impostazioni vengono mantenute.",
    "Nie ma go teraz w twoim HUD: może pojawić się później. Jego ustawienia zostają zachowane.",
    "Teď není ve vašem HUD: může se objevit později. Jeho nastavení zůstává zachováno.",
    "Сейчас его нет в вашем HUD: он может появиться позже. Его настройки сохраняются.",
    "今はHUDにありません: 後で現れることがあります。設定は保持されます。",
    "지금은 HUD에 없습니다: 나중에 나타날 수 있습니다. 설정은 유지됩니다.",
    "目前不在你的 HUD 中：它可能稍后出现。其设置会保留。")
row("HPM_Found", "In your HUD. Changes show at once.",
    "In deinem HUD. Änderungen sind sofort zu sehen.", "Présent dans votre HUD. Les changements s'affichent aussitôt.",
    "En tu HUD. Los cambios se ven al instante.", "Nel tuo HUD. Le modifiche si vedono subito.",
    "W twoim HUD. Zmiany widać od razu.", "Ve vašem HUD. Změny se projeví hned.", "В вашем HUD. Изменения видны сразу.",
    "HUDにあります。変更はすぐに反映されます。", "HUD에 있습니다. 변경 사항이 바로 보입니다.", "在你的 HUD 中。更改立即生效。")
row("HPM_MoveX", "Move left / right",
    "Nach links / rechts", "Déplacer gauche / droite", "Mover izquierda / derecha", "Sposta sinistra / destra",
    "Przesuń w lewo / w prawo", "Posun vlevo / vpravo", "Сдвиг влево / вправо", "左右に移動", "좌우 이동", "左右移动")
row("HPM_MoveY", "Move up / down",
    "Nach oben / unten", "Déplacer haut / bas", "Mover arriba / abajo", "Sposta su / giù",
    "Przesuń w górę / w dół", "Posun nahoru / dolů", "Сдвиг вверх / вниз", "上下に移動", "상하 이동", "上下移动")
row("HPM_Size", "Size",
    "Größe", "Taille", "Tamaño", "Dimensione", "Rozmiar", "Velikost", "Размер", "サイズ", "크기", "大小")
row("HPM_Length", "Length",
    "Länge", "Longueur", "Longitud", "Lunghezza", "Długość", "Délka", "Длина", "長さ", "길이", "长度")
row("HPM_Height", "Height",
    "Höhe", "Hauteur", "Altura", "Altezza", "Wysokość", "Výška", "Высота", "高さ", "높이", "高度")
row("HPM_StretchHint", "Length and Height stretch the bar on one side each, on top of Size.",
    "Länge und Höhe strecken die Leiste jeweils in eine Richtung, zusätzlich zur Größe.",
    "Longueur et Hauteur étirent chacune la barre d'un côté, en plus de la Taille.",
    "Longitud y Altura estiran la barra cada una por un lado, además del Tamaño.",
    "Lunghezza e Altezza allungano la barra ciascuna su un lato, in aggiunta alla Dimensione.",
    "Długość i Wysokość rozciągają pasek, każda w jedną stronę, dodatkowo do Rozmiaru.",
    "Délka a Výška natahují pruh každá na jednu stranu, navíc k Velikosti.",
    "Длина и Высота растягивают полосу каждая в одну сторону, в дополнение к Размеру.",
    "長さと高さは、サイズに加えてバーをそれぞれ一方向に伸ばします。",
    "길이와 높이는 크기에 더해 바를 각각 한쪽으로 늘립니다.",
    "长度和高度在大小之外，分别向一侧拉伸该条。")
row("HPM_Zoom", "Zoom",
    "Zoom", "Zoom", "Zoom", "Zoom", "Przybliżenie", "Přiblížení", "Масштаб", "ズーム", "확대", "缩放")
row("HPM_ZoomHint",
    "Percent of the game's own zoom, which changes by itself outdoors, indoors and on a boat. Over 100: closer.",
    "Prozent des spieleigenen Zooms, der sich draußen, drinnen und auf einem Boot von selbst ändert. Über 100: näher.",
    "Pourcentage du zoom du jeu, qui change de lui-même en extérieur, en intérieur et en bateau. Au-delà de 100 : plus près.",
    "Porcentaje del zoom propio del juego, que cambia solo en exteriores, en interiores y en barco. Más de 100: más cerca.",
    "Percentuale dello zoom del gioco, che cambia da solo all'aperto, al chiuso e in barca. Oltre 100: più vicino.",
    "Procent własnego przybliżenia gry, które samo zmienia się na zewnątrz, we wnętrzach i na łodzi. Powyżej 100: bliżej.",
    "Procento vlastního přiblížení hry, které se samo mění venku, uvnitř a na lodi. Nad 100: blíž.",
    "Процент от собственного масштаба игры, который сам меняется на улице, в помещениях и в лодке. Больше 100: ближе.",
    "ゲーム本来のズームに対する割合です。ゲームのズームは屋外、屋内、ボートの上で自動的に変わります。100を超えると近くなります。",
    "게임 자체 확대의 백분율입니다. 게임의 확대는 야외, 실내, 배 위에서 저절로 바뀝니다. 100 초과: 더 가깝게.",
    "游戏自身缩放的百分比，游戏缩放会在室外、室内和乘船时自动变化。超过 100：更近。")
row("HPM_SquareMap", "Square minimap",
    "Quadratische Minikarte", "Mini-carte carrée", "Minimapa cuadrado", "Minimappa quadrata", "Kwadratowa minimapa",
    "Čtvercová minimapa", "Квадратная мини-карта", "四角いミニマップ", "사각형 미니맵", "方形小地图")
row("HPM_SquareMapHint",
    "On: the minimap is square, framed with the game's ring art drawn square. Off: the game's own round minimap. Reset leaves the shape as it is.",
    "An: Die Minikarte ist quadratisch, gerahmt mit dem Ring des Spiels als Quadrat. Aus: die runde Minikarte des Spiels. Zurücksetzen lässt die Form, wie sie ist.",
    "Activé : la mini-carte est carrée, encadrée par l'anneau du jeu dessiné en carré. Désactivé : la mini-carte ronde du jeu. La réinitialisation ne change pas la forme.",
    "Activado: el minimapa es cuadrado, enmarcado con el anillo del juego dibujado en cuadrado. Desactivado: el minimapa redondo del juego. Restablecer no cambia la forma.",
    "Attivo: la minimappa è quadrata, incorniciata dall'anello del gioco disegnato a quadrato. Disattivo: la minimappa rotonda del gioco. Il ripristino lascia la forma com'è.",
    "Wł.: minimapa jest kwadratowa, w ramce z pierścienia gry narysowanego jako kwadrat. Wył.: okrągła minimapa gry. Reset nie zmienia kształtu.",
    "Zap.: minimapa je čtvercová, v rámu z prstence hry nakresleného jako čtverec. Vyp.: kulatá minimapa hry. Obnovení tvar nemění.",
    "Вкл.: мини-карта квадратная, в рамке из кольца игры, нарисованного квадратом. Выкл.: круглая мини-карта игры. Сброс не меняет форму.",
    "オン：ミニマップは四角く、ゲームのリングを四角く描いた枠で囲まれます。オフ：ゲーム本来の丸いミニマップ。リセットしても形は変わりません。",
    "켜기: 미니맵이 사각형이 되고, 게임의 링을 사각형으로 그린 테두리가 둘러쌉니다. 끄기: 게임 본래의 둥근 미니맵. 초기화해도 모양은 그대로입니다.",
    "开：小地图为方形，以画成方形的游戏圆环为边框。关：游戏原本的圆形小地图。重置不会改变形状。")
row("HPM_Hide", "Hide",
    "Ausblenden", "Masquer", "Ocultar", "Nascondi", "Ukryj", "Skrýt", "Скрыть", "非表示", "숨기기", "隐藏")
row("HPM_Show", "Show",
    "Anzeigen", "Afficher", "Mostrar", "Mostra", "Pokazuj", "Zobrazit", "Показывать", "表示", "표시", "显示")
row("HPM_ShowAlways", "Always",
    "Immer", "Toujours", "Siempre", "Sempre", "Zawsze", "Vždy", "Всегда", "常に", "항상", "始终")
row("HPM_ShowCombat", "Only in combat",
    "Nur im Kampf", "Seulement en combat", "Solo en combate", "Solo in combattimento", "Tylko w walce",
    "Jen v boji", "Только в бою", "戦闘中のみ", "전투 중에만", "仅战斗中")
row("HPM_ShowNoCombat", "Only out of combat",
    "Nur außerhalb des Kampfes", "Seulement hors combat", "Solo fuera de combate", "Solo fuori dal combattimento",
    "Tylko poza walką", "Jen mimo boj", "Только вне боя", "戦闘外のみ", "전투 외에만", "仅非战斗时")
row("HPM_ShowHint",
    "When this shows while you play. Only in combat: hidden while you explore, back as soon as a fight starts. Only out of combat: hidden during fights.",
    "Wann dies beim Spielen zu sehen ist. Nur im Kampf: beim Erkunden ausgeblendet, zurück, sobald ein Kampf beginnt. Nur außerhalb des Kampfes: während Kämpfen ausgeblendet.",
    "Quand cet élément s'affiche en jeu. Seulement en combat : masqué pendant l'exploration, de retour dès qu'un combat commence. Seulement hors combat : masqué pendant les combats.",
    "Cuándo se muestra mientras juegas. Solo en combate: oculto mientras exploras, vuelve en cuanto empieza una pelea. Solo fuera de combate: oculto durante los combates.",
    "Quando appare mentre giochi. Solo in combattimento: nascosto mentre esplori, torna appena inizia uno scontro. Solo fuori dal combattimento: nascosto durante gli scontri.",
    "Kiedy element jest widoczny podczas gry. Tylko w walce: ukryty podczas eksploracji, wraca, gdy tylko zacznie się walka. Tylko poza walką: ukryty w czasie walki.",
    "Kdy se zobrazuje během hraní. Jen v boji: skryto při průzkumu, vrátí se, jakmile začne boj. Jen mimo boj: skryto během bojů.",
    "Когда элемент виден во время игры. Только в бою: скрыт во время исследования, возвращается, как только начинается бой. Только вне боя: скрыт во время боёв.",
    "プレイ中にいつ表示するか。戦闘中のみ: 探索中は隠れ、戦闘が始まるとすぐに戻ります。戦闘外のみ: 戦闘中は隠れます。",
    "플레이 중 언제 표시할지 정합니다. 전투 중에만: 탐험할 때는 숨고, 전투가 시작되면 바로 나타납니다. 전투 외에만: 전투 중에는 숨깁니다.",
    "游戏中何时显示。仅战斗中：探索时隐藏，战斗一开始就重新出现。仅非战斗时：战斗期间隐藏。")
row("HPM_AlwaysOne", "Always visible",
    "Immer sichtbar", "Toujours visible", "Siempre visible", "Sempre visibile", "Zawsze widoczny",
    "Vždy viditelné", "Всегда видно", "常に表示", "항상 표시", "始终可见")
row("HPM_AlwaysOneHint", "The game fades this out on its own. On: it stays shown while you play.",
    "Das Spiel blendet dies von selbst aus. An: Es bleibt beim Spielen sichtbar.",
    "Le jeu fait disparaître cet élément de lui-même. Activé : il reste affiché pendant que vous jouez.",
    "El juego lo desvanece por sí solo. Activado: sigue visible mientras juegas.",
    "Il gioco lo fa sparire da solo. Attivo: resta visibile mentre giochi.",
    "Gra sama go wygasza. Włączone: pozostaje widoczny podczas gry.",
    "Hra ho sama skrývá. Zapnuto: zůstane zobrazený během hraní.",
    "Игра сама скрывает этот элемент. Вкл.: он остаётся на экране во время игры.",
    "ゲームはこれを自動的にフェードアウトさせます。オン: プレイ中は表示されたままです。",
    "게임이 이것을 스스로 사라지게 합니다. 켜짐: 플레이 중 계속 표시됩니다.",
    "游戏会自行将其淡出。开启：游戏中保持显示。")
row("HPM_MoveWith", "Move with",
    "Bewegen mit", "Déplacer avec", "Mover con", "Sposta con", "Przesuwaj razem z", "Posouvat s",
    "Двигать вместе с", "一緒に動かす対象", "함께 이동", "跟随移动")
row("HPM_MoveWithNone", "Nothing - on its own",
    "Nichts - eigenständig", "Rien - indépendant", "Nada - por su cuenta", "Niente - da solo", "Nic - samodzielnie",
    "Nic - samostatně", "Ничего - отдельно", "なし - 単独", "없음 - 단독", "无 - 独立")
row("HPM_InGroupHint", "Also moves with the widgets on the Combined widgets tab.",
    "Bewegt sich auch mit den Anzeigen auf dem Tab Kombinierte Anzeigen.",
    "Se déplace aussi avec les widgets de l'onglet Widgets combinés.",
    "También se mueve con los indicadores de la pestaña Indicadores combinados.",
    "Si sposta anche con gli indicatori della scheda Indicatori combinati.",
    "Przesuwa się też razem ze wskaźnikami z karty Połączone wskaźniki.",
    "Posouvá se také s ukazateli na kartě Spojené ukazatele.",
    "Также двигается вместе с виджетами на вкладке «Объединённые виджеты».",
    "「ウィジェットの連動」タブのウィジェットとも一緒に動きます。",
    "위젯 묶음 탭의 위젯과도 함께 움직입니다.",
    "也会随“组合组件”标签页中的组件一起移动。")
row("HPM_ResetOne", "Reset this element",
    "Dieses Element zurücksetzen", "Réinitialiser cet élément", "Restablecer este elemento",
    "Ripristina questo elemento", "Zresetuj ten element", "Obnovit tento prvek", "Сбросить этот элемент",
    "この要素をリセット", "이 요소 초기화", "重置此元素")

# ------------------------------------------------------------------------------------------------ the Combined widgets tab
row("HPM_TabCombined", "Combined widgets",
    "Kombinierte Anzeigen", "Widgets combinés", "Indicadores combinados", "Indicatori combinati",
    "Połączone wskaźniki", "Spojené ukazatele", "Объединённые виджеты", "ウィジェットの連動", "위젯 묶음", "组合组件")
row("HPM_CombinedIntro",
    "Pick the widgets that should move as one. The sliders below move every picked widget together, on top of the position each has on its own tab.",
    "Wähle die Anzeigen, die sich gemeinsam bewegen sollen. Die Regler unten verschieben alle gewählten Anzeigen zusammen, zusätzlich zur Position, die jede auf ihrem eigenen Tab hat.",
    "Choisissez les widgets qui doivent se déplacer ensemble. Les curseurs ci-dessous déplacent tous les widgets choisis à la fois, en plus de la position de chacun dans son propre onglet.",
    "Elige los indicadores que deben moverse como uno solo. Los controles deslizantes de abajo mueven juntos todos los indicadores elegidos, además de la posición que cada uno tiene en su propia pestaña.",
    "Scegli gli indicatori che devono muoversi insieme. I cursori qui sotto spostano insieme tutti gli indicatori scelti, in aggiunta alla posizione che ognuno ha nella propria scheda.",
    "Wybierz wskaźniki, które mają przesuwać się razem. Suwaki poniżej przesuwają wszystkie wybrane wskaźniki naraz, dodatkowo do pozycji, jaką każdy ma na własnej karcie.",
    "Vyberte ukazatele, které se mají posouvat společně. Posuvníky níže posouvají všechny vybrané ukazatele najednou, navíc k pozici, kterou má každý na své vlastní kartě.",
    "Выберите виджеты, которые должны двигаться вместе. Ползунки ниже сдвигают все выбранные виджеты разом, в дополнение к положению каждого на его собственной вкладке.",
    "一緒に動かすウィジェットを選んでください。下のスライダーは、それぞれのタブでの位置に加えて、選んだすべてのウィジェットをまとめて動かします。",
    "함께 움직일 위젯을 고르세요. 아래 슬라이더는 고른 위젯 모두를 한꺼번에 옮기며, 각 위젯 탭의 위치에 더해집니다.",
    "选择要一起移动的组件。下方滑块会同时移动所有选中的组件，叠加在各自标签页中的位置之上。")
row("HPM_GroupMembers", "Widgets that move as one",
    "Anzeigen, die sich gemeinsam bewegen", "Widgets qui se déplacent ensemble", "Indicadores que se mueven como uno solo",
    "Indicatori che si muovono insieme", "Wskaźniki przesuwane razem", "Ukazatele, které se posouvají společně",
    "Виджеты, которые двигаются вместе", "一緒に動くウィジェット", "함께 움직이는 위젯", "一起移动的组件")
row("HPM_GroupNone", "Pick two or more widgets above to move them together.",
    "Wähle oben zwei oder mehr Anzeigen, um sie gemeinsam zu bewegen.",
    "Choisissez au moins deux widgets ci-dessus pour les déplacer ensemble.",
    "Elige arriba dos o más indicadores para moverlos juntos.",
    "Scegli qui sopra due o più indicatori per spostarli insieme.",
    "Wybierz powyżej co najmniej dwa wskaźniki, aby przesuwać je razem.",
    "Vyberte výše dva nebo více ukazatelů, abyste je posouvali společně.",
    "Выберите выше два или больше виджетов, чтобы двигать их вместе.",
    "上で2つ以上のウィジェットを選ぶと、まとめて動かせます。",
    "위에서 위젯을 두 개 이상 고르면 함께 옮길 수 있습니다.",
    "在上方选择两个或更多组件即可一起移动。")
row("HPM_GroupReset", "Reset the shared position",
    "Gemeinsame Position zurücksetzen", "Réinitialiser la position commune", "Restablecer la posición compartida",
    "Ripristina la posizione condivisa", "Zresetuj wspólną pozycję", "Obnovit společnou pozici",
    "Сбросить общее положение", "共通の位置をリセット", "공통 위치 초기화", "重置共同位置")


# ------------------------------------------------------------------------------------------------ the generator
def source_keys():
    """Every TR("HPM_<Key>", "<English>") in src\\*.cpp -> {key: English}, the English unescaped as the file holds it."""
    found, problems = {}, []
    for path in sorted(glob.glob(os.path.join(SRC, "*.cpp"))):
        with open(path, encoding="utf-8") as f:
            text = f.read()
        for k, v in TR.findall(text):
            v = v.replace('\\"', '"').replace("\\\\", "\\")
            if k in found and found[k] != v:
                problems.append(f"{k} has two English texts in the source: {found[k]!r} / {v!r}")
            found.setdefault(k, v)
    return found, problems


def file_bytes(lang):
    body = "".join(f"${k}\t{TABLE[k][lang].replace(chr(10), chr(92) + 'n')}\r\n" for k in sorted(TABLE))
    return b"\xff\xfe" + body.encode("utf-16-le")


def check(src):
    problems = []
    for k, en in sorted(src.items()):
        if k not in TABLE:
            problems.append(f"{k}: in the source, no row in TABLE")
            continue
        r = TABLE[k]
        if r["english"] != en:
            problems.append(f"{k}: the source's English changed to {en!r} (TABLE has {r['english']!r}) - review the translations")
        for lang in LANGS:
            t = r.get(lang, "")
            if not t.strip():
                problems.append(f"{k}: no {lang} text")
            elif t.count("%") != en.count("%"):
                problems.append(f"{k}: {lang} has a different '%' count from English")
            elif "\t" in t or "\r" in t:
                problems.append(f"{k}: {lang} text holds a tab or carriage return")
    for k in sorted(set(TABLE) - set(src)):
        problems.append(f"{k}: in TABLE, no longer in the source")
    return problems


def check_files():
    problems = []
    for lang in LANGS:
        path = os.path.join(OUT, f"{PREFIX}{lang}.txt")
        if not os.path.isfile(path):
            problems.append(f"{PREFIX}{lang}.txt is missing")
            continue
        with open(path, "rb") as f:
            if f.read() != file_bytes(lang):
                problems.append(f"{PREFIX}{lang}.txt differs from the table - run tools/gen_translations.py")
    return problems


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="only check the table against the source and the files on disk")
    a = ap.parse_args()
    src, problems = source_keys()
    problems += check(src)
    if problems:
        print("\n".join(problems), file=sys.stderr)
        sys.exit(1)
    if not a.check:
        os.makedirs(OUT, exist_ok=True)
        for lang in LANGS:
            with open(os.path.join(OUT, f"{PREFIX}{lang}.txt"), "wb") as f:
                f.write(file_bytes(lang))
    problems = check_files()
    if problems:
        print("\n".join(problems), file=sys.stderr)
        sys.exit(1)
    print(f"ok    {len(LANGS)} languages x {len(TABLE)} keys ({'checked' if a.check else 'written to'} {OUT})")


if __name__ == "__main__":
    main()
