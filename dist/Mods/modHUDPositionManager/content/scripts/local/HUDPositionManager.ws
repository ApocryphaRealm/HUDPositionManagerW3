// =====================================================================================================================
// HUD Position Manager (The Witcher 3: Wild Hunt - Remastered 5.0) - the bridge between the settings page and the HUD.
//
// Since 2026-10-07 the settings live on a page of the Apocrypha Menu Framework (HUDPositionManager.asi), organised as the
// Skyrim and Oblivion versions are (the owner: "HUD Position Manager completely reformatted and organized the way HUD
// Position Manager is in Skyrim and Oblivion"). A native mod cannot call WitcherScript, so the page writes the whole layout
// as one string into the hidden setting HUDPositionManager.Layout (HUDPositionManager.xml, hideAlways). This script:
//   - hands that string to the three HUD movies when it changes (the minimap movie places the game's modules, the player
//     widget's movie its bars and badges, the dialogue movie the choice list), and applies the minimap's zoom;
//   - tells the minimap movie when Geralt is in combat (the elements set to show only in or out of combat);
//   - writes the HUD's report (which elements are in the HUD, and their boxes) into HUDPositionManager.Report for the page.
// Twice a second, from the minimap module's tick. Annotations only: no game script replaced. Log channel 'HUDPositionManager'.
// =====================================================================================================================

@addField(CR4HudModuleMinimap2)
var hpmFxLayout : CScriptedFlashFunction;

@addField(CR4HudModuleMinimap2)
var hpmFxCombat : CScriptedFlashFunction;

@addField(CR4HudModuleMinimap2)
var hpmTimer : float;

@addField(CR4HudModuleMinimap2)
var hpmLastLayout : string;

@addField(CR4HudModuleMinimap2)
var hpmLastReport : string;

@addField(CR4HudModuleMinimap2)
var hpmCombat : bool;

@addField(CR4HudModuleMinimap2)
var hpmCombatSent : bool;

// The layout's minimap zoom (the 13th field of the "Minimap" element; Settings.cpp LayoutString) - 100 when none
function HPM_LayoutZoom(layout : string) : float
{
	var seg : string;
	var i : int;

	if (StrFindFirst(layout, "|Minimap,") < 0)
	{
		return 100;
	}
	seg = StrAfterFirst(layout, "|Minimap,");
	for (i = 0; i < 11; i += 1)
	{
		seg = StrAfterFirst(seg, ",");
	}
	return ClampF(StringToFloat(StrBeforeFirst(seg, ","), 100), 50, 300);
}

// Zoom (the owner, 2026-10-07, in place of Better Minimap): a percentage of the game's own zoom, which it changes by itself
// between outdoors (1.0), interiors (2.0) and the boat (0.5); applied where the game hands the zoom to the movie.
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

// After a zoom change the movie asks for the map tiles of the new zoom at once (else part of the square stays dark until
// Geralt walks - 2026-10-07).
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
function HPM_Update(force : bool)
{
	var cfg : CInGameConfigWrapper;
	var layout, report : string;
	var hud : CR4ScriptedHud;
	var wolf : CR4HudModuleWolfHead;
	var dlg : CR4HudModuleDialog;
	var zoom : float;
	var combat : bool;

	if (!hpmFxLayout)
	{
		hpmFxLayout = GetModuleFlash().GetMemberFlashFunction("hpmSetLayout");
		hpmFxCombat = GetModuleFlash().GetMemberFlashFunction("hpmSetCombat");
		if (!hpmFxLayout)
		{
			return;
		}
	}
	cfg = theGame.GetInGameConfigWrapper();
	if (!cfg)
	{
		return;
	}
	// combat (the movie waits 3 s after a fight before counting it as over)
	combat = thePlayer && thePlayer.IsInCombat();
	hud = (CR4ScriptedHud)theGame.GetHud();
	if (hud)
	{
		wolf = (CR4HudModuleWolfHead)hud.GetHudModule("WolfHeadModule");
	}
	if (force || combat != hpmCombatSent)
	{
		hpmCombatSent = combat;
		if (hpmFxCombat)
		{
			hpmFxCombat.InvokeSelfOneArg(FlashArgBool(combat));
		}
		if (wolf)
		{
			wolf.HPM_SetCombat(combat);
		}
	}
	// the layout, when it changed (or the HUD was rebuilt)
	layout = cfg.GetVarValue('HUDPositionManager', 'Layout');
	if (layout != "" && layout != "-1" && (force || layout != hpmLastLayout))
	{
		hpmLastLayout = layout;
		hpmFxLayout.InvokeSelfOneArg(FlashArgString(layout));
		if (hud)
		{
			if (wolf)
			{
				wolf.HPM_SetLayout(layout);
			}
			dlg = (CR4HudModuleDialog)hud.GetHudModule("DialogModule");
			if (dlg)
			{
				dlg.HPM_SetLayout(layout);
			}
		}
		zoom = HPM_LayoutZoom(layout);
		if (sqmZoomFactor != zoom / 100)
		{
			sqmZoomFactor = zoom / 100;
			SetZoom(m_zoomValue, false);
		}
		LogChannel('HUDPositionManager', "layout from the page applied (" + IntToString(StrLen(layout)) + " characters), minimap zoom "
			+ FloatToString(zoom) + "%");
	}
	// the HUD's report for the page: the minimap movie's (the modules, the dialogue) and the player widget's parts
	report = GetModuleFlash().GetMemberFlashString("sqmReport");
	if (wolf)
	{
		report += wolf.HPM_Report();
	}
	if (report != "" && report != hpmLastReport)
	{
		hpmLastReport = report;
		cfg.SetVarValue('HUDPositionManager', 'Report', report);
	}
}

@wrapMethod(CR4HudModuleMinimap2)
function OnConfigUI()
{
	var r : bool;

	r = wrappedMethod();
	HPM_Update(true);
	return r;
}

@wrapMethod(CR4HudModuleMinimap2)
function OnTick(timeDelta : float)
{
	var r : bool;

	r = wrappedMethod(timeDelta);
	hpmTimer += timeDelta;
	if (hpmTimer >= 0.5)
	{
		hpmTimer = 0;
		HPM_Update(false);
	}
	return r;
}

// The player widget's movie (hud_wolfstatbars): its bars and badges.
@addField(CR4HudModuleWolfHead)
var hpmFxWolfLayout : CScriptedFlashFunction;

@addMethod(CR4HudModuleWolfHead)
public function HPM_SetLayout(layout : string)
{
	if (!hpmFxWolfLayout)
	{
		hpmFxWolfLayout = GetModuleFlash().GetMemberFlashFunction("hpmSetLayout");
		if (!hpmFxWolfLayout)
		{
			return;
		}
	}
	hpmFxWolfLayout.InvokeSelfOneArg(FlashArgString(layout));
}

// Combat, for the parts set to show only in or only out of combat (the movie waits 3 s after a fight).
@addField(CR4HudModuleWolfHead)
var hpmFxWolfCombat : CScriptedFlashFunction;

@addMethod(CR4HudModuleWolfHead)
public function HPM_SetCombat(combat : bool)
{
	if (!hpmFxWolfCombat)
	{
		hpmFxWolfCombat = GetModuleFlash().GetMemberFlashFunction("hpmSetCombat");
		if (!hpmFxWolfCombat)
		{
			return;
		}
	}
	hpmFxWolfCombat.InvokeSelfOneArg(FlashArgBool(combat));
}

// Each part's box on the screen, for the page's slider ranges ("" when the movie is not ours).
@addField(CR4HudModuleWolfHead)
var hpmFxWolfReport : CScriptedFlashFunction;

@addMethod(CR4HudModuleWolfHead)
public function HPM_Report() : string
{
	if (!hpmFxWolfReport)
	{
		hpmFxWolfReport = GetModuleFlash().GetMemberFlashFunction("hpmMakeReport");
		if (!hpmFxWolfReport)
		{
			return "";
		}
	}
	hpmFxWolfReport.InvokeSelf();
	return GetModuleFlash().GetMemberFlashString("hpmReport");
}

// The dialogue movie (hud_dialog): the choice list. Also handed the layout each time the game sets the choices.
@addField(CR4HudModuleDialog)
var hpmFxDlgLayout : CScriptedFlashFunction;

@addMethod(CR4HudModuleDialog)
public function HPM_SetLayout(layout : string)
{
	if (!hpmFxDlgLayout)
	{
		hpmFxDlgLayout = GetModuleFlash().GetMemberFlashFunction("hpmSetLayout");
		if (!hpmFxDlgLayout)
		{
			return;
		}
	}
	hpmFxDlgLayout.InvokeSelfOneArg(FlashArgString(layout));
}

@wrapMethod(CR4HudModuleDialog)
function OnDialogChoicesSet(choices : array< SSceneChoice >, alternativeUI : bool)
{
	var cfg : CInGameConfigWrapper;
	var layout : string;

	cfg = theGame.GetInGameConfigWrapper();
	if (cfg)
	{
		layout = cfg.GetVarValue('HUDPositionManager', 'Layout');
		if (layout != "" && layout != "-1")
		{
			HPM_SetLayout(layout);
		}
	}
	wrappedMethod(choices, alternativeUI);
}
