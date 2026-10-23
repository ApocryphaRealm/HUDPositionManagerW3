#pragma once

// The start-up check: the page lives in the .asi, but the HUD changes only when the game runs this mod's own HUD movies and
// script (Mods\modHUDPositionManager). A player who sees the page but no change in the HUD needs to know which link is
// missing, so this checks, once at start and from the game folder: our Mods folder and its files, our line in mods.settings,
// and every other Mods folder that ships one of the three HUD movies we replace (hud_minimap2, hud_wolfstatbars, hud_dialog)
// or a script of the HUD modules we hook. It writes what it found to HUDPositionManager.log at info level, and the page
// shows a hint when something would keep the HUD from changing.
//
// Written for the TW3SE report (Banjomir777, Nexus 14337, 2026-10-16): under TW3SE the .asi loads and the HUD answers
// (proven 2026-10-22), so a dead HUD there is one of these links, not the loader.
//
// SPDX-License-Identifier: GPL-3.0-or-later

namespace diagnose
{
	struct Result
	{
		bool                     ran = false;
		bool                     modFolder = false;        // <game>\Mods\modHUDPositionManager exists
		bool                     files = false;            // its blob0.bundle and HUDPositionManager.ws are both there
		int                      listed = -1;              // mods.settings: -1 unread, 0 not listed, 1 enabled, 2 disabled
		std::vector<std::string> movieConflicts;           // other Mods folders shipping one of our three HUD movies
	};

	void   Run();          // once, on the caller's thread; logs everything it finds
	Result Get();
	bool   Blocking();     // something found would keep the HUD from changing (the page shows a hint)
	std::string Summary(); // one line for the page: the mods in the way, or the missing link
}
