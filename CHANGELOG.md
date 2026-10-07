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
