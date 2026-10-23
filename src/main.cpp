// HUD Position Manager for The Witcher 3: Wild Hunt - Remastered (patch 5.0) - the settings page on the Apocrypha Menu
// Framework, organised as the Skyrim and Oblivion Remastered versions are (the owner, 2026-10-07). An .asi in
// bin\x64_dx12, loaded by the same ASI loader as the framework; it starts from DllMain on a thread of its own. The HUD
// itself is changed by the mod's HUD movies (Mods\modHUDPositionManager), which the layout reaches through the game's
// script (bridge.cpp).
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "AMF.h"
#include "Bridge.h"
#include "Diagnose.h"
#include "Page.h"
#include "Paths.h"
#include "Settings.h"

#include <spdlog/sinks/basic_file_sink.h>

namespace
{
	void StartLog()
	{
		try {
			const auto path = paths::DataFolder() / L"HUDPositionManager.log";
			auto       log = spdlog::basic_logger_mt("HUDPositionManager", path.string(), true);
			log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
			spdlog::set_default_logger(std::move(log));
		} catch (...) {
		}
	}

	bool TryRegister()
	{
		if (!AMF::IsInstalled()) return false;
		page::Register();
		return true;
	}

	void Run()
	{
		StartLog();
		settings::Load();
		const int  lvl = std::clamp(settings::Snapshot().logLevel, 0, 6);
		const auto level = static_cast<spdlog::level::level_enum>(lvl);
		logger::set_level(level, level);
		logger::info("HUD Position Manager {} loaded (The Witcher 3) from {} - log level {} ({} and above are written); set uLogLevel=0 "
		             "in HUDPositionManager.ini to capture everything",
			HPM_VERSION, paths::ModuleFolder().string(), lvl, spdlog::level::to_string_view(level));
		diagnose::Run();   // the Mods folder, mods.settings and other mods' HUD movies - the links the page cannot see
		bool registered = false;
		for (int i = 0; i < 1200 && !registered; ++i) {   // the framework is usually loaded first (its name sorts first)
			registered = TryRegister();
			if (!registered) std::this_thread::sleep_for(500ms);
		}
		if (!registered) {
			logger::error("the Apocrypha Menu Framework never loaded - no page (is ApocryphaMenuFramework.asi in {}?)", paths::ModuleFolder().string());
			return;
		}
		bridge::Start();
		spdlog::default_logger_raw()->flush();
	}
}

BOOL APIENTRY DllMain(HMODULE a_module, DWORD a_reason, LPVOID)
{
	if (a_reason == DLL_PROCESS_ATTACH) {
		::DisableThreadLibraryCalls(a_module);
		std::thread(&Run).detach();
	}
	return TRUE;
}
