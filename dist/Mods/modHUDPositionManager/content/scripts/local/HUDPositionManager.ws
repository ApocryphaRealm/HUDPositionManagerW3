// =====================================================================================================================
// HUD Position Manager (The Witcher 3: Wild Hunt - Remastered 5.0) - the settings bridge. Formerly Square Minimap
// (renamed by the owner, 2026-10-07: "you may as well just rename it HUD Position Manager at this point").
//
// The minimap movie (hud_minimap2, rebuilt by tools/build_flash.py) does the square, the frame and the placement of the
// minimap, the quest list under it and the player widget in the bottom left. This script hands it the settings page's
// values (Options > Mods > HUD Position Manager, HUD units of a 1920 x 1080 screen):
//   Top, Right      the square's distance from the screen's top and right edges
//   QuestGap        the gap between the square and the quest list under it
//   PlayerLeft, PlayerBottom   the player widget's distance from the left and bottom edges (8 / 8 since 2026-10-07)
//   ItemsLeft, ItemsTop        the quick items' (potions, consumables) distance from the left and top edges
//   DialogueRight, DialogueGap the dialogue choices moved to the right, their longest line DialogueGap from the edge
//                              (handed to the dialogue movie, hud_dialog, whenever the game sets the choices);
//                              DialogueY moves them up (-) or down (+)
//   <Widget>X, <Widget>Y       every other HUD widget, left (-) / right (+) and up (-) / down (+) from where the game
//                              puts it (the buffs: from their place above the player widget) - the owner, 2026-10-07:
//                              "make sure that each of the HUD widgets have sliders so you can change their positions"
//   SignX, SignY, SkullX, SkullY   the player widget's sign badge and toxicity skull, apart from their bars
// and applies Zoom (a percentage of the game's own minimap zoom). Read when the module starts and twice a second after,
// and passed on only when one changed. Annotations only: no game script replaced. Log channel 'HUDPositionManager'.
// =====================================================================================================================

@addField(CR4HudModuleMinimap2)
var sqmFxSettings : CScriptedFlashFunction;

@addField(CR4HudModuleMinimap2)
var sqmFxPlayer : CScriptedFlashFunction;

@addField(CR4HudModuleMinimap2)
var sqmFxItems : CScriptedFlashFunction;

@addField(CR4HudModuleMinimap2)
var sqmFxOffsets : CScriptedFlashFunction;

// An offset slider's value: "" and "-1" (a row never set) read as 0, the game's place.
function SQM_Offset(cfg : CInGameConfigWrapper, id : name) : float
{
	return SQM_Setting(cfg, id, 0);
}

// "Name:dx:dy;..." for the minimap movie's sqmSetOffsets
function SQM_OffsetSpec(cfg : CInGameConfigWrapper) : string
{
	var spec : string;

	spec += "BuffsModule:" + FloatToString(SQM_Offset(cfg, 'BuffsX')) + ":" + FloatToString(SQM_Offset(cfg, 'BuffsY')) + ";";
	spec += "ControlsFeedbackModule:" + FloatToString(SQM_Offset(cfg, 'HintsX')) + ":" + FloatToString(SQM_Offset(cfg, 'HintsY')) + ";";
	spec += "MessageModule:" + FloatToString(SQM_Offset(cfg, 'MessagesX')) + ":" + FloatToString(SQM_Offset(cfg, 'MessagesY')) + ";";
	spec += "SubtitlesModule:" + FloatToString(SQM_Offset(cfg, 'SubtitlesX')) + ":" + FloatToString(SQM_Offset(cfg, 'SubtitlesY')) + ";";
	spec += "AreaInfoModule:" + FloatToString(SQM_Offset(cfg, 'AreaX')) + ":" + FloatToString(SQM_Offset(cfg, 'AreaY')) + ";";
	spec += "JournalUpdateModule:" + FloatToString(SQM_Offset(cfg, 'JournalX')) + ":" + FloatToString(SQM_Offset(cfg, 'JournalY')) + ";";
	spec += "LootFeedModule:" + FloatToString(SQM_Offset(cfg, 'LootFeedX')) + ":" + FloatToString(SQM_Offset(cfg, 'LootFeedY')) + ";";
	spec += "EnemyFocusModule:" + FloatToString(SQM_Offset(cfg, 'EnemyX')) + ":" + FloatToString(SQM_Offset(cfg, 'EnemyY')) + ";";
	spec += "BossFocusModule:" + FloatToString(SQM_Offset(cfg, 'BossX')) + ":" + FloatToString(SQM_Offset(cfg, 'BossY')) + ";";
	spec += "CompanionModule:" + FloatToString(SQM_Offset(cfg, 'CompanionX')) + ":" + FloatToString(SQM_Offset(cfg, 'CompanionY')) + ";";
	spec += "OxygenBarModule:" + FloatToString(SQM_Offset(cfg, 'BreathX')) + ":" + FloatToString(SQM_Offset(cfg, 'BreathY')) + ";";
	spec += "HorseStaminaBarModule:" + FloatToString(SQM_Offset(cfg, 'HorseStaminaX')) + ":" + FloatToString(SQM_Offset(cfg, 'HorseStaminaY')) + ";";
	spec += "HorsePanicBarModule:" + FloatToString(SQM_Offset(cfg, 'HorseFearX')) + ":" + FloatToString(SQM_Offset(cfg, 'HorseFearY')) + ";";
	spec += "BoatHealthModule:" + FloatToString(SQM_Offset(cfg, 'BoatX')) + ":" + FloatToString(SQM_Offset(cfg, 'BoatY')) + ";";
	spec += "DamagedItemsModule:" + FloatToString(SQM_Offset(cfg, 'DamagedX')) + ":" + FloatToString(SQM_Offset(cfg, 'DamagedY')) + ";";
	spec += "TimeLeftModule:" + FloatToString(SQM_Offset(cfg, 'TimeLeftX')) + ":" + FloatToString(SQM_Offset(cfg, 'TimeLeftY')) + ";";
	spec += "TimeLapseModule:" + FloatToString(SQM_Offset(cfg, 'TimeLapseX')) + ":" + FloatToString(SQM_Offset(cfg, 'TimeLapseY')) + ";";
	return spec;
}

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
	if (v == "" || v == "-1")
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
	var top, right, gap, zoom, pleft, pbottom, ileft, itop : float;
	var sx, sy, kx, ky : float;
	var sig, spec : string;
	var hud : CR4ScriptedHud;
	var wolf : CR4HudModuleWolfHead;

	if (!sqmFxSettings)
	{
		sqmFxSettings = GetModuleFlash().GetMemberFlashFunction("sqmSetSettings");
		sqmFxPlayer = GetModuleFlash().GetMemberFlashFunction("sqmSetPlayer");
		sqmFxItems = GetModuleFlash().GetMemberFlashFunction("sqmSetItems");
		sqmFxOffsets = GetModuleFlash().GetMemberFlashFunction("sqmSetOffsets");
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
	pleft = SQM_Setting(cfg, 'PlayerLeft', 8);
	pbottom = SQM_Setting(cfg, 'PlayerBottom', 8);
	ileft = SQM_Setting(cfg, 'ItemsLeft', 8);
	itop = SQM_Setting(cfg, 'ItemsTop', 8);
	spec = SQM_OffsetSpec(cfg);
	sx = SQM_Offset(cfg, 'SignX');
	sy = SQM_Offset(cfg, 'SignY');
	kx = SQM_Offset(cfg, 'SkullX');
	ky = SQM_Offset(cfg, 'SkullY');
	sig = FloatToString(top) + "/" + FloatToString(right) + "/" + FloatToString(gap) + "/" + FloatToString(zoom)
		+ "/" + FloatToString(pleft) + "/" + FloatToString(pbottom) + "/" + FloatToString(ileft) + "/" + FloatToString(itop)
		+ "/" + spec + "/" + FloatToString(sx) + "/" + FloatToString(sy) + "/" + FloatToString(kx) + "/" + FloatToString(ky);
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
	if (sqmFxItems)
	{
		sqmFxItems.InvokeSelfTwoArgs(FlashArgNumber(ileft), FlashArgNumber(itop));
	}
	if (sqmFxOffsets)
	{
		sqmFxOffsets.InvokeSelfOneArg(FlashArgString(spec));
	}
	hud = (CR4ScriptedHud)theGame.GetHud();
	if (hud)
	{
		wolf = (CR4HudModuleWolfHead)hud.GetHudModule("WolfHeadModule");
		if (wolf)
		{
			wolf.HPM_SetBadges(sx, sy, kx, ky);
		}
	}
	if (sqmZoomFactor != zoom / 100)
	{
		sqmZoomFactor = zoom / 100;
		SetZoom(m_zoomValue, false);
	}
	LogChannel('HUDPositionManager', "settings: minimap top " + FloatToString(top) + ", right " + FloatToString(right)
		+ ", quest gap " + FloatToString(gap) + ", zoom " + FloatToString(zoom) + "%; player widget left "
		+ FloatToString(pleft) + ", bottom " + FloatToString(pbottom) + "; quick items left " + FloatToString(ileft)
		+ ", top " + FloatToString(itop) + "; badges sign " + FloatToString(sx) + "/" + FloatToString(sy) + ", skull "
		+ FloatToString(kx) + "/" + FloatToString(ky) + "; offsets " + spec);
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
	var v : string;

	v = cfg.GetVarValue('HUDPositionManager', id);
	if (v != "" && v != "-1")
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
	wrote = SQM_Default(cfg, 'PlayerLeft', "8") || wrote;
	wrote = SQM_Default(cfg, 'PlayerBottom', "8") || wrote;
	wrote = SQM_Default(cfg, 'ItemsLeft', "8") || wrote;
	wrote = SQM_Default(cfg, 'ItemsTop', "8") || wrote;
	wrote = SQM_Default(cfg, 'DialogueRight', "true") || wrote;
	wrote = SQM_Default(cfg, 'DialogueGap', "40") || wrote;
	wrote = SQM_Default(cfg, 'DialogueY', "0") || wrote;
	wrote = SQM_Default(cfg, 'SignX', "0") || wrote;
	wrote = SQM_Default(cfg, 'SignY', "0") || wrote;
	wrote = SQM_Default(cfg, 'SkullX', "0") || wrote;
	wrote = SQM_Default(cfg, 'SkullY', "0") || wrote;
	wrote = SQM_Default(cfg, 'BuffsX', "0") || wrote;
	wrote = SQM_Default(cfg, 'BuffsY', "0") || wrote;
	wrote = SQM_Default(cfg, 'HintsX', "0") || wrote;
	wrote = SQM_Default(cfg, 'HintsY', "0") || wrote;
	wrote = SQM_Default(cfg, 'MessagesX', "0") || wrote;
	wrote = SQM_Default(cfg, 'MessagesY', "0") || wrote;
	wrote = SQM_Default(cfg, 'SubtitlesX', "0") || wrote;
	wrote = SQM_Default(cfg, 'SubtitlesY', "0") || wrote;
	wrote = SQM_Default(cfg, 'AreaX', "0") || wrote;
	wrote = SQM_Default(cfg, 'AreaY', "0") || wrote;
	wrote = SQM_Default(cfg, 'JournalX', "0") || wrote;
	wrote = SQM_Default(cfg, 'JournalY', "0") || wrote;
	wrote = SQM_Default(cfg, 'LootFeedX', "0") || wrote;
	wrote = SQM_Default(cfg, 'LootFeedY', "0") || wrote;
	wrote = SQM_Default(cfg, 'EnemyX', "0") || wrote;
	wrote = SQM_Default(cfg, 'EnemyY', "0") || wrote;
	wrote = SQM_Default(cfg, 'BossX', "0") || wrote;
	wrote = SQM_Default(cfg, 'BossY', "0") || wrote;
	wrote = SQM_Default(cfg, 'CompanionX', "0") || wrote;
	wrote = SQM_Default(cfg, 'CompanionY', "0") || wrote;
	wrote = SQM_Default(cfg, 'BreathX', "0") || wrote;
	wrote = SQM_Default(cfg, 'BreathY', "0") || wrote;
	wrote = SQM_Default(cfg, 'HorseStaminaX', "0") || wrote;
	wrote = SQM_Default(cfg, 'HorseStaminaY', "0") || wrote;
	wrote = SQM_Default(cfg, 'HorseFearX', "0") || wrote;
	wrote = SQM_Default(cfg, 'HorseFearY', "0") || wrote;
	wrote = SQM_Default(cfg, 'BoatX', "0") || wrote;
	wrote = SQM_Default(cfg, 'BoatY', "0") || wrote;
	wrote = SQM_Default(cfg, 'DamagedX', "0") || wrote;
	wrote = SQM_Default(cfg, 'DamagedY', "0") || wrote;
	wrote = SQM_Default(cfg, 'TimeLeftX', "0") || wrote;
	wrote = SQM_Default(cfg, 'TimeLeftY', "0") || wrote;
	wrote = SQM_Default(cfg, 'TimeLapseX', "0") || wrote;
	wrote = SQM_Default(cfg, 'TimeLapseY', "0") || wrote;
	if (wrote)
	{
		SaveUserSettings();
		LogChannel('HUDPositionManager', "settings: empty values got their defaults (minimap top 4, right 4, quest gap "
			+ "8, zoom 100%; player widget left 8, bottom 8; quick items left 8, top 8; dialogue choices on the right, "
			+ "gap 40; every widget offset and badge 0)");
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

// The dialogue choices on the right (the owner, 2026-10-07: "move the dialogue selections to the right of the screen so
// they don't block the view of the person you're talking to"). The dialogue movie (hud_dialog, rebuilt by
// tools/build_flash.py) moves the choice list; this hands it the page's switch and gap each time the game sets the
// choices, so a change on the page applies from the next set of choices.
@addField(CR4HudModuleDialog)
var hpmFxDialog : CScriptedFlashFunction;

@addMethod(CR4HudModuleDialog)
function HPM_PushDialog()
{
	var cfg : CInGameConfigWrapper;
	var v : string;
	var on : bool;
	var gap, dy : float;

	if (!hpmFxDialog)
	{
		hpmFxDialog = GetModuleFlash().GetMemberFlashFunction("hpmSetDialog");
		if (!hpmFxDialog)
		{
			return;
		}
	}
	cfg = theGame.GetInGameConfigWrapper();
	on = true;
	if (cfg)
	{
		v = cfg.GetVarValue('HUDPositionManager', 'DialogueRight');
		on = !(v == "false" || v == "0");
	}
	gap = ClampF(SQM_Setting(cfg, 'DialogueGap', 40), 0, 600);
	dy = SQM_Offset(cfg, 'DialogueY');
	hpmFxDialog.InvokeSelfThreeArgs(FlashArgBool(on), FlashArgNumber(gap), FlashArgNumber(dy));
}

@wrapMethod(CR4HudModuleDialog)
function OnConfigUI()
{
	var r : bool;

	r = wrappedMethod();
	HPM_PushDialog();
	return r;
}

@wrapMethod(CR4HudModuleDialog)
function OnDialogChoicesSet(choices : array< SSceneChoice >, alternativeUI : bool)
{
	HPM_PushDialog();
	wrappedMethod(choices, alternativeUI);
}

// The badges, apart from their bars: the player widget's movie (hud_wolfstatbars) moves them.
@addField(CR4HudModuleWolfHead)
var hpmFxBadges : CScriptedFlashFunction;

@addMethod(CR4HudModuleWolfHead)
public function HPM_SetBadges(sx : float, sy : float, kx : float, ky : float)
{
	if (!hpmFxBadges)
	{
		hpmFxBadges = GetModuleFlash().GetMemberFlashFunction("hpmSetBadges");
		if (!hpmFxBadges)
		{
			return;
		}
	}
	hpmFxBadges.InvokeSelfFourArgs(FlashArgNumber(sx), FlashArgNumber(sy), FlashArgNumber(kx), FlashArgNumber(ky));
}
