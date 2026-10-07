# Changelog

Every version, beside the code it describes (rule 61).

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
