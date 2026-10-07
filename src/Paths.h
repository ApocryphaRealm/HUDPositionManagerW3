#pragma once

// Where things are. The .asi sits in <game>\bin\x64_dx12 (AMF's loader loads every .asi there); its own files - the INI,
// the log - are kept in bin\x64_dx12\HUDPositionManager\, beside AMF's bin\x64_dx12\AMF\. The game's item definitions and
// names are read from the game folder (read-only).
//
// SPDX-License-Identifier: GPL-3.0-or-later

namespace paths
{
	std::filesystem::path ModuleFolder();   // ...\bin\x64_dx12
	std::filesystem::path DataFolder();     // ...\bin\x64_dx12\HUDPositionManager (created on first use)
	std::filesystem::path GameRoot();       // ...\The Witcher 3 (two folders above witcher3.exe)
	std::filesystem::path TranslationsFolder();   // ...\bin\x64_dx12\AMF\Translations (AMF builds its font from it)
}
