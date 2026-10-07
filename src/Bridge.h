#pragma once

// The page's line into the game. A native mod cannot call WitcherScript, so the layout travels as one string in the hidden
// game setting HUDPositionManager.Layout (written through AMF_SetGameVar); HUDPositionManager.ws reads it and hands it to
// the HUD movies. The other way, the HUD's report - which elements exist and where their drawn boxes are - comes back in
// HUDPositionManager.Report (AMF_WatchGameVar / AMF_GetGameVar). Both declared in HUDPositionManager.xml (hideAlways).

namespace bridge
{
	struct ElementStatus
	{
		bool   found = false;
		double left = 0, top = 0, right = 0, bottom = 0;   // the drawn box, in percent of the screen
	};

	void Start();                                   // its own thread: sends the layout when it changes, reads the report
	bool HudReported();                             // the HUD has reported at least once (a game is loaded)
	std::vector<ElementStatus> Statuses();          // per element (elements::All() order)
	std::string LastSent();                         // for the driving tool
}
