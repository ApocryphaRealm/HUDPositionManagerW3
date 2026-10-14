Witcher III Modding Tool, developed and released by CD PROJEKT RED

HUD Position Manager (The Witcher 3: Wild Hunt - Remastered)
Version 0.0.0

Move, resize or hide every part of the HUD from a settings page in the Apocrypha Menu Framework. Out of the box nothing
moves: every element starts where the game puts it, at the game's own size, and stays there until you move it.


WHAT IT DOES
- 30 HUD elements, each on its own tab: the minimap, the quest list, every part of the player widget (medallion, health
  bar, experience bar, stamina, adrenaline and toxicity bars, sign badge, toxicity skull), the effects row (food,
  potions, oils), quick items, dialogue choices, subtitles, messages, button hints, area name, quest and journal
  updates, items received, enemy and boss health, companion health, breath bar, horse stamina and fear, boat health,
  damaged gear, time limit and the passing time text.
- Move left / right and up / down, from the game's own place. Size, and Length / Height for the bars.
- Hide, or show only in combat / only out of combat. Always visible for the elements the game fades on its own.
- Move with: an element follows another one. Combined widgets: any set of widgets moves as one on shared sliders.
- The minimap: square, framed with the game's own ring art drawn square, with a zoom slider (50 to 300 percent of the
  game's own zoom, set live). Stamina, adrenaline and toxicity are straight bars under the health bar.
- Presets: save whole layouts, load someone else's, update or delete them. Files in
  bin\x64_dx12\HUDPositionManager\presets\.
- Reset this element / Reset every element take you back to the game's own layout. "Apply my layout" off shows the
  game's layout without losing yours. "Free placement" lets the sliders go past the screen's edges.
- Changes show in the HUD at once and are saved by themselves.


SETTINGS
Open the Apocrypha Menu Framework and pick HUD Position Manager. Tabs: Presets, Layout (one tab per element) and
Combined widgets. Settings are kept in bin\x64_dx12\HUDPositionManager\HUDPositionManager.ini; delete it to start again
from the game's own layout.


INSTALL
Install the Apocrypha Menu Framework (1.0.2 or later) first, as its page describes. Then copy the Mods and bin folders
into the game folder, or install with a mod manager (Mod Organizer 2, Vortex). No script merging is needed.
It replaces three of the game's HUD movies (hud_minimap2, hud_wolfstatbars and hud_dialog): another mod that replaces
one of those same movies will not combine with it.

The page's text follows the game's language (English, Japanese, Korean, Chinese, Russian, German, French, Spanish,
Italian, Polish and Czech).


DEBUGGING
For a bug report, send bin\x64_dx12\HUDPositionManager\HUDPositionManager.log (it is written fresh at every start).


REQUIREMENTS
The Witcher 3: Wild Hunt - Remastered (patch 5.0 or later, DirectX 12).
Apocrypha Menu Framework for The Witcher 3, 1.0.2 or later.


LICENCE
GPL-3.0-or-later (LICENSE, NOTICE.md; the linked libraries in THIRD_PARTY_NOTICES.md). The HUD movies are
CD PROJEKT RED's, changed by this mod and rebuilt with the Witcher III Modding Tool (REDkit).
