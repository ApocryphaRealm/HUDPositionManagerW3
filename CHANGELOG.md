# Changelog

Every version, beside the code it describes (rule 61).

## 1.0.0 - 2026-10-07 - reopened (unreleased) - HUD Position Manager

Renamed from Square Minimap (the owner, 2026-10-07: "you may as well just rename it HUD Position Manager at this
point"): mod folder modHUDPositionManager, script HUDPositionManager.ws, menu HUDPositionManager.xml (group
HUDPositionManager - settings start from their defaults), log channel HUDPositionManager.
- Minimap zoom (50-300 %, a factor on the game's own outdoor / interior / boat zoom), set live - in place of Better
  Minimap, which the owner turned off ("build on top of square minimap a zoom function so you can set its zoom and
  position in game"). After a zoom change the movie asks for the map tiles of the new zoom at once (before, part of the
  square stayed dark until Geralt walked).
- The player widget (medallion, vitality, stamina, toxicity, adrenaline) in the screen's bottom left, its buffs just
  above it; distance from the left / bottom on the settings page (default 20 / 20).
- The north mark now rides just INSIDE the square's frame at every angle (it stood half outside, and the screen's top
  edge cut it off).
- Position sliders reach across the screen (top 0-1000, right / left 0-1800, bottom 0-1000), one unit per nudge.
- Checked in game 2026-10-07 on the Nexus 12404 save: widget bottom left, zoom 50 / 201 / 50 live with no dark area,
  north mark inside at three camera angles.
- Then the player widget's art (the owner, 2026-10-07): stamina, adrenaline and toxicity as straight bars under the
  health bar, in that order - drawn bars (dark track, two-tone fill, thin border, pointed end) as thick as the game's
  own toxicity bar, each a little shorter than the one above (lengths from the game's own three bars); adrenaline
  marked at its thirds; the game's stamina arc, adrenaline points and toxicity arc faded out. The skull badge stays
  toxicity's (at its bar's left end, as in the game), the sign badge beside it. A second movie, hud_wolfstatbars, is
  now built into the same bundle. Checked in game (the potion's toxicity fills the green bar). Reopened: untested
  again until the owner has seen it.
- Then three more (the owner, 2026-10-07), built, not yet seen in game:
  - **Dialogue choices on the right** ("so they don't block the view of the person you're talking to"): a third movie,
    hud_dialog, moves the choice list sideways so its longest line ends 40 HUD units from the screen's right edge; the
    lines stay left aligned with each other and keep their icons and arrow. A switch and the gap on the settings page
    (Dialogue choices on the right / distance from the right); off = the game's own place. A line long enough to wrap
    (540 units) still starts where the game's would.
  - **The player widget all the way into the bottom left** ("a little bit of space in between either side of the screen
    but it needs to be further left"): default distances 8 / 8 (were 20 / 20).
  - **The quick items in the top left** (the potions and consumables, "not the quick wheel"): the game's ItemInfoModule
    placed 8 / 8 from the screen's left and top edges by the box of all its slots and hints, shown or not, so the slots
    do not jump when a hint appears and the hints keep their room under the top edge. Distances on the settings page.
  - Settings rows that were never set ("-1") count as empty, so they take their defaults.
- Then every widget on sliders (the owner, 2026-10-07: "make sure that each of the HUD widgets have sliders so you can
  change their positions, including the widget that pops up whenever you eat food. And move the badges independently
  of their bars. And I can just position everything where I want it myself"), built, not yet seen in game:
  - 17 more widgets, each with a left / right and an up / down slider in HUD units from where the game puts it (0 = the
    game's place; left and up are minus): effects (food, potions - BuffsModule, from its place above the player widget),
    button hints, messages, subtitles, area name, quest and journal updates, items received, enemy and boss health bars,
    companion health bar, breath bar, horse stamina and fear bars, boat health bar, damaged gear icons, time limit bar,
    passing time text. Moved by the minimap movie, which re-reads the game's own position whenever the game re-anchors
    a widget; only widgets with an offset are looked at (once a frame), so the defaults cost nothing.
  - The sign badge and the toxicity skull each on their own left / right and up / down sliders (-300 to 300), apart from
    their bars - the toxicity bar keeps its start.
  - The dialogue choices also move up / down.
  - Not given sliders: the quick wheel, the crosshair, the interaction prompts and the floating one-liners (they follow
    the screen's centre or things in the world), the debug console.
  - A slider set to exactly -1 reads as 0 (the game reports a row never set as "-1").
- Then the whole mod reorganised as the Skyrim and Oblivion versions are (the owner, 2026-10-07: "HUD Position Manager
  completely reformatted and organized the way HUD Position Manager is in Skyrim and Oblivion. And try and copy as much
  of their functionality as you can"; plan 4. plans\square-minimap-witcher3\PLAN-2-hpm-parity.md). Phase 1 and part of
  2, built, not yet seen in game:
  - The settings page moves to the Apocrypha Menu Framework (HUDPositionManager.asi, ported from the Oblivion version):
    Presets (load, save, update, delete with a second press), Layout with a tab per element (27: the minimap, the quest
    list, the player widget and its three bars and two badges, effects, quick items, dialogue choices and 17 more), and
    Combined widgets. Per element: Place (this mod's spot from 1.0.0, on by default where it had one), move left / right
    and up / down in 0.1 % steps of the screen (stopping at the screen's edge unless Free placement is on), Size, Length
    and Height (the bars), Hide, Show (always / only in combat / only out of combat), Always visible (the widgets the
    game fades), Move with, Reset this element, Reset every element; Apply my layout; the minimap's Zoom. Settings and
    presets are files beside the plugin (bin\x64_dx12\HUDPositionManager\).
  - The game's Options > Mods page is gone (hidden): the page sends the whole layout as one string through a hidden
    setting, and HUDPositionManager.ws hands it to the three HUD movies; the HUD reports back which elements are there
    and their boxes. The 1.0.0 values on Options > Mods are taken over once.
  - The minimap movie's code is now one layout engine for every module element; an element left at its defaults with no
    Place is never touched, and only changed elements are looked at each frame.
  - Not yet: Show every element (preview), snapping and collision, the bars' Fill from / Grows toward, the damage
    direction indicator (phase 3); the page's eleven-language files.
- Then the player widget broken up into its parts (the owner, 2026-10-07: "the player widget is a combined widget which
  should be broken up into its individual parts, not as a single widget that can be added to the combined widget"):
  - "Player widget" is no longer an element of the page. Its parts each are: Medallion and Health bar (new), Stamina,
    Adrenaline and Toxicity bars, Sign badge, Toxicity skull - each with its own move, size (Length / Height for the
    bars), Hide, Show in / out of combat, Always visible and Move with, and each can join the Combined widgets or be
    followed by another widget. The widget's start place stays one switch, shown on every part's tab ("Player widget in
    the bottom left corner", shared). The game's own pieces of each part sit in a container of ours, so the game still
    shows and hides what is inside (the level-up mark, the sign text, Ciri's skull).
  - Always visible per part: while a part is set to stay, the widget is not let fade; the parts not set to stay hide
    instead and come back when the game shows the widget.
  - A settings file or preset that moved, sized, hid or grouped the player widget as a whole gives that to every part.
  - The page's move sliders for the parts take their range from each part's box (the player widget's movie reports them).
- Fixes (the owner, 2026-10-07):
  - The HUD-wide "Always visible" could not be switched off: the widgets it held stayed up. Holding called each
    module's own fade-in, so the game counted it as shown and, with nothing new to show, never faded it again. Letting
    go now hands each module back to its display state (the fade-out timer started again, or faded at once when it is
    set to hide or to show on demand).
  - The toxicity skull did not come back after Hide was switched off (its pieces were left see-through; only the sign
    badge's were restored). The parts now hide and show as a whole.
  - The experience (level) bar is a part of its own too, "Experience bar" (the owner, 2026-10-07: it stayed behind
    when the health bar moved - "give it its own widget as well").
  - Eleven languages: 102 keys.

## 1.0.0 - 2026-10-06 - working (finalized, private - not posted)

First version of Square Minimap, for The Witcher 3: Wild Hunt - Remastered 5.0 (plan: 4. plans\square-minimap-witcher3).
- The game's own minimap made square: the three round map masks (open world, city, interior) replaced by squares of the
  same width, the edge shading grown and cut square.
- The ring art squared: the same dark plate (lighter at the bottom) and thin copper line, drawn as a square.
- Flush in the screen's top right corner, re-placed whenever the HUD anchors move the module (resolution, HUD scale).
- The north mark rides the square's edge as the map turns.
- No settings. Replaces the game's hud_minimap2 movie (its own code kept, ours added to it).
- After the owner's first looks the same evening: a settings page (Options > Mods > Square Minimap: distance from the
  top / right, gap above the quest list; defaults 4 / 4 / 8, written once at the main or game menu so the sliders show
  them); the quest list centred exactly under the square by its text, the square moving in from the right as far as
  that text needs; the frame a solid line of one width and colour drawn above the map on all four sides.
- Known: the north mark pokes slightly above the square's top edge.
