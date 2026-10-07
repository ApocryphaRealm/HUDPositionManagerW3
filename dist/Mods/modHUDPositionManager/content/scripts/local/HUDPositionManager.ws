// =====================================================================================================================
// HUD Position Manager (The Witcher 3: Wild Hunt - Remastered 5.0) - the settings bridge. Formerly Square Minimap
// (renamed by the owner, 2026-10-07: "you may as well just rename it HUD Position Manager at this point").
//
// The minimap movie (hud_minimap2, rebuilt by tools/build_flash.py) does the square, the frame and the placement of the
// minimap, the quest list under it and the player widget in the bottom left. This script hands it the settings page's
// values (Options > Mods > HUD Position Manager, HUD units of a 1920 x 1080 screen):
//   Top, Right      the square's distance from the screen's top and right edges
//   QuestGap        the gap between the square and the quest list under it
//   PlayerLeft, PlayerBottom   the player widget's distance from the left and bottom edges
// and applies Zoom (a percentage of the game's own minimap zoom). Read when the module starts and twice a second after,
// and passed on only when one changed. Annotations only: no game script replaced. Log channel 'HUDPositionManager'.
// =====================================================================================================================

@addField(CR4HudModuleMinimap2)
var sqmFxSettings : CScriptedFlashFunction;

@addField(CR4HudModuleMinimap2)
var sqmFxPlayer : CScriptedFlashFunction;

@addField(CR4HudModuleMinimap2)
var sqmSettingsTimer : float;

@addField(CR4HudModuleMinimap2)
var sqmLastSettings : string;

function SQM_Setting(cfg : CInGameConfigWrapper, id : name, defaultValue : float) : float
{
	var v : string;

	if (!cfg)
	{
		return defaultValue;
	}
	v = cfg.GetVarValue('HUDPositionManager', id);
	if (v == "")
	{
		return defaultValue;
	}
	return StringToFloat(v, defaultValue);
}

// Zoom (the owner, 2026-10-07, in place of Better Minimap: "build on top of square minimap a zoom function so you can
// set its zoom and position in game"): a percentage of the game's own zoom, which it changes by itself between outdoors
// (1.0), interiors (2.0) and the boat (0.5). 100 = the game's; more = closer, less = more of the world. Applied where the
// game hands the zoom to the minimap movie (SetZoom), so every one of the game's zoom changes keeps the player's factor.
@addField(CR4HudModuleMinimap2)
var sqmZoomFactor : float;

@wrapMethod(CR4HudModuleMinimap2)
function SetZoom(newValue : float, immediately : bool)
{
	if (sqmZoomFactor <= 0)
	{
		sqmZoomFactor = 1.0;
	}
	wrappedMethod(newValue * sqmZoomFactor, immediately);
	SQM_RefreshTiles(newValue * sqmZoomFactor);
}

// The minimap movie streams its map tiles around the player and only asks for new ones when the player moves, so after
// a zoom change part of the square stayed dark (a corner at 50 %, 2026-10-07). The movie's sqmRefreshTiles asks for the
// tiles of the new zoom at the current position. (A position nudge from script, as Better Minimap does, blanked the
// whole map when it ran at start-up, until Geralt walked - 2026-10-07.)
@addField(CR4HudModuleMinimap2)
var sqmFxRefresh : CScriptedFlashFunction;

@addMethod(CR4HudModuleMinimap2)
function SQM_RefreshTiles(zoomValue : float)
{
	if (!sqmFxRefresh)
	{
		sqmFxRefresh = GetModuleFlash().GetMemberFlashFunction("sqmRefreshTiles");
		if (!sqmFxRefresh)
		{
			return;
		}
	}
	sqmFxRefresh.InvokeSelf();
}

@addMethod(CR4HudModuleMinimap2)
function SQM_PushSettings(force : bool)
{
	var cfg : CInGameConfigWrapper;
	var top, right, gap, zoom, pleft, pbottom : float;
	var sig : string;

	if (!sqmFxSettings)
	{
		sqmFxSettings = GetModuleFlash().GetMemberFlashFunction("sqmSetSettings");
		sqmFxPlayer = GetModuleFlash().GetMemberFlashFunction("sqmSetPlayer");
		if (!sqmFxSettings)
		{
			return;
		}
	}
	cfg = theGame.GetInGameConfigWrapper();
	top = SQM_Setting(cfg, 'Top', 4);
	right = SQM_Setting(cfg, 'Right', 4);
	gap = SQM_Setting(cfg, 'QuestGap', 8);
	zoom = ClampF(SQM_Setting(cfg, 'Zoom', 100), 50, 300);
	pleft = SQM_Setting(cfg, 'PlayerLeft', 20);
	pbottom = SQM_Setting(cfg, 'PlayerBottom', 20);
	sig = FloatToString(top) + "/" + FloatToString(right) + "/" + FloatToString(gap) + "/" + FloatToString(zoom)
		+ "/" + FloatToString(pleft) + "/" + FloatToString(pbottom);
	if (!force && sig == sqmLastSettings)
	{
		return;
	}
	sqmLastSettings = sig;
	sqmFxSettings.InvokeSelfThreeArgs(FlashArgNumber(top), FlashArgNumber(right), FlashArgNumber(gap));
	if (sqmFxPlayer)
	{
		sqmFxPlayer.InvokeSelfTwoArgs(FlashArgNumber(pleft), FlashArgNumber(pbottom));
	}
	if (sqmZoomFactor != zoom / 100)
	{
		sqmZoomFactor = zoom / 100;
		SetZoom(m_zoomValue, false);
	}
	LogChannel('HUDPositionManager', "settings: minimap top " + FloatToString(top) + ", right " + FloatToString(right)
		+ ", quest gap " + FloatToString(gap) + ", zoom " + FloatToString(zoom) + "%; player widget left "
		+ FloatToString(pleft) + ", bottom " + FloatToString(pbottom));
}

@wrapMethod(CR4HudModuleMinimap2)
function OnConfigUI()
{
	var r : bool;

	r = wrappedMethod();
	SQM_PushSettings(true);
	return r;
}

@wrapMethod(CR4HudModuleMinimap2)
function OnTick(timeDelta : float)
{
	var r : bool;

	r = wrappedMethod(timeDelta);
	sqmSettingsTimer += timeDelta;
	if (sqmSettingsTimer >= 0.5)
	{
		sqmSettingsTimer = 0;
		SQM_PushSettings(false);
	}
	return r;
}

// An empty slider reads "" - the menu would draw it at 0 while the mod uses its default. So each empty value gets its
// default written once per session when the main menu or the game menu opens (never while a save loads: an engine
// setting written at start-up faulted for AMF - logic library), and the page shows what the mod does (rule 16).

@addField(CR4Game)
var sqmDefaultsWritten : bool;

@addMethod(CR4Game)
function SQM_Default(cfg : CInGameConfigWrapper, id : name, value : string) : bool
{
	if (cfg.GetVarValue('HUDPositionManager', id) != "")
	{
		return false;
	}
	cfg.SetVarValue('HUDPositionManager', id, value);
	return true;
}

@addMethod(CR4Game)
public function SQM_EnsureDefaults()
{
	var cfg : CInGameConfigWrapper;
	var wrote : bool;

	if (sqmDefaultsWritten)
	{
		return;
	}
	sqmDefaultsWritten = true;
	cfg = GetInGameConfigWrapper();
	if (!cfg)
	{
		return;
	}
	wrote = SQM_Default(cfg, 'Top', "4") || wrote;
	wrote = SQM_Default(cfg, 'Right', "4") || wrote;
	wrote = SQM_Default(cfg, 'QuestGap', "8") || wrote;
	wrote = SQM_Default(cfg, 'Zoom', "100") || wrote;
	wrote = SQM_Default(cfg, 'PlayerLeft', "20") || wrote;
	wrote = SQM_Default(cfg, 'PlayerBottom', "20") || wrote;
	if (wrote)
	{
		SaveUserSettings();
		LogChannel('HUDPositionManager', "settings: empty values got their defaults (minimap top 4, right 4, quest gap "
			+ "8, zoom 100%; player widget left 20, bottom 20)");
	}
}

@wrapMethod(CR4CommonMainMenuBase)
function OnConfigUI()
{
	theGame.SQM_EnsureDefaults();
	return wrappedMethod();
}

@wrapMethod(CR4IngameMenu)
function OnConfigUI()
{
	theGame.SQM_EnsureDefaults();
	return wrappedMethod();
}
