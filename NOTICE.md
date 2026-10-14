# HUD Position Manager (The Witcher 3) - copyright and licence

Copyright (C) 2026 ApocryphaRealm

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program (`LICENSE`). If not, see
<https://www.gnu.org/licenses/>.

SPDX-License-Identifier: GPL-3.0-or-later

## What is ours, and what is not

Ours: the settings page's source (`src/`, `include/`), the ActionScript inserted into the HUD movies (`flash/`), the
WitcherScript (`dist/Mods/modHUDPositionManager/content/scripts/local/HUDPositionManager.ws`), the settings XML, the
string tables and translations and their generators, and the build tools (`tools/`). `include/AMF.h` is the Apocrypha
Menu Framework's API header (same author, MIT).

Not ours: the third-party libraries linked into HUDPositionManager.asi (`THIRD_PARTY_NOTICES.md`), and the game's HUD
movies (`hud_minimap2`, `hud_wolfstatbars`, `hud_dialog`), which are CD PROJEKT RED's. They are never in this
repository: `tools/build_flash.py` reads them from the builder's own REDkit install, inserts our code and rebuilds them
with REDkit's `wcc_lite` (the Witcher III Modding Tool; the README carries the line its rules ask for). The built
`blob0.bundle` ships in the mod's download only.
