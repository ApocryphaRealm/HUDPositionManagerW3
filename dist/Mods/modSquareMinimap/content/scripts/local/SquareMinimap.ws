// =====================================================================================================================
// Square Minimap (The Witcher 3: Wild Hunt - Remastered 5.0) - the settings bridge
//
// The minimap movie (hud_minimap2, rebuilt by tools/build_flash.py) does the square, the frame and the placement. This
// script only hands it the settings page's values (Options > Mods > Square Minimap: Top, Right, QuestGap - the distance
// of the square from the screen's top and right edges and the gap above the quest list, in the HUD's 1920 x 1080
// units). Read when the module starts and twice a second after, and passed on only when one changed. Annotations only:
// no game script replaced. Log channel 'SquareMinimap'.
// =====================================================================================================================

@addField(CR4HudModuleMinimap2)
var sqmFxSettings : CScriptedFlashFunction;

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
	v = cfg.GetVarValue('SquareMinimap', id);
	if (v == "")
	{
		return defaultValue;
	}
	return StringToFloat(v, defaultValue);
}

@addMethod(CR4HudModuleMinimap2)
function SQM_PushSettings(force : bool)
{
	var cfg : CInGameConfigWrapper;
	var top, right, gap : float;
	var sig : string;

	if (!sqmFxSettings)
	{
		sqmFxSettings = GetModuleFlash().GetMemberFlashFunction("sqmSetSettings");
		if (!sqmFxSettings)
		{
			return;
		}
	}
	cfg = theGame.GetInGameConfigWrapper();
	top = SQM_Setting(cfg, 'Top', 20);
	right = SQM_Setting(cfg, 'Right', 20);
	gap = SQM_Setting(cfg, 'QuestGap', 8);
	sig = FloatToString(top) + "/" + FloatToString(right) + "/" + FloatToString(gap);
	if (!force && sig == sqmLastSettings)
	{
		return;
	}
	sqmLastSettings = sig;
	sqmFxSettings.InvokeSelfThreeArgs(FlashArgNumber(top), FlashArgNumber(right), FlashArgNumber(gap));
	LogChannel('SquareMinimap', "settings: top " + FloatToString(top) + ", right " + FloatToString(right)
		+ ", quest gap " + FloatToString(gap));
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
	if (cfg.GetVarValue('SquareMinimap', 'Top') == "") { cfg.SetVarValue('SquareMinimap', 'Top', "20"); wrote = true; }
	if (cfg.GetVarValue('SquareMinimap', 'Right') == "") { cfg.SetVarValue('SquareMinimap', 'Right', "20"); wrote = true; }
	if (cfg.GetVarValue('SquareMinimap', 'QuestGap') == "") { cfg.SetVarValue('SquareMinimap', 'QuestGap', "8"); wrote = true; }
	if (wrote)
	{
		SaveUserSettings();
		LogChannel('SquareMinimap', "settings: empty values got their defaults (top 20, right 20, quest gap 8)");
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
