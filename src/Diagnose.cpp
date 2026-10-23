#include "Diagnose.h"

#include "Paths.h"

#include <ShlObj.h>
#include <fstream>

// SPDX-License-Identifier: GPL-3.0-or-later

namespace diagnose
{
	namespace
	{
		constexpr std::wstring_view kOurFolder = L"modHUDPositionManager";

		// the three HUD movies HPM ships in its blob0.bundle (tools/build_flash.py), as the bundle's path table spells them
		constexpr std::array<std::string_view, 3> kMovies{
			"gameplay\\gui_new\\swf\\hud\\hud_minimap2.redswf",
			"gameplay\\gui_new\\swf\\hud\\hud_wolfstatbars.redswf",
			"gameplay\\gui_new\\swf\\hud\\hud_dialog.redswf",
		};
		// the HUD module scripts whose classes HUDPositionManager.ws hooks with annotations; another mod replacing one of
		// these files is normally fine (the annotations apply to its class), so it is logged, not warned about
		constexpr std::array<std::wstring_view, 3> kScripts{ L"hudModuleMinimap2.ws", L"hudModuleWolfHead.ws", L"hudModuleDialog.ws" };

		std::mutex g_lock;
		Result     g_result;

		std::string Narrow(const std::wstring& a_w)
		{
			if (a_w.empty()) return {};
			const int n = ::WideCharToMultiByte(CP_UTF8, 0, a_w.data(), static_cast<int>(a_w.size()), nullptr, 0, nullptr, nullptr);
			std::string s(static_cast<std::size_t>(std::max(n, 0)), '\0');
			if (n > 0) ::WideCharToMultiByte(CP_UTF8, 0, a_w.data(), static_cast<int>(a_w.size()), s.data(), n, nullptr, nullptr);
			return s;
		}

		std::string Lower(std::string a_s)
		{
			std::ranges::transform(a_s, a_s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return a_s;
		}

		bool SameName(const std::wstring& a_a, std::wstring_view a_b)
		{
			return a_a.size() == a_b.size() &&
			       ::CompareStringOrdinal(a_a.data(), static_cast<int>(a_a.size()), a_b.data(), static_cast<int>(a_b.size()), TRUE) == CSTR_EQUAL;
		}

		std::filesystem::path Documents()
		{
			PWSTR raw = nullptr;
			std::filesystem::path p;
			if (SUCCEEDED(::SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &raw)) && raw) p = raw;
			if (raw) ::CoTaskMemFree(raw);
			return p;
		}

		// mods.settings: "[modName]" sections with Enabled=0/1 and Priority=<n>. section (lower case) -> {enabled, priority}
		std::unordered_map<std::string, std::pair<int, int>> ReadModsSettings(const std::filesystem::path& a_file)
		{
			std::unordered_map<std::string, std::pair<int, int>> out;
			std::ifstream f(a_file, std::ios::binary);
			if (!f) return out;
			std::string line, section;
			while (std::getline(f, line)) {
				while (!line.empty() && (line.back() == '\r' || line.back() == ' ' || line.back() == '\t')) line.pop_back();
				if (line.size() >= 3 && static_cast<unsigned char>(line[0]) == 0xEF) line.erase(0, 3);   // a UTF-8 BOM
				if (line.size() >= 2 && line.front() == '[' && line.back() == ']') {
					section = Lower(line.substr(1, line.size() - 2));
					out.try_emplace(section, std::pair{ 1, -1 });
					continue;
				}
				if (section.empty()) continue;
				const auto eq = line.find('=');
				if (eq == std::string::npos) continue;
				const auto key = Lower(line.substr(0, eq));
				const int  val = std::atoi(line.c_str() + eq + 1);
				if (key == "enabled") out[section].first = val;
				else if (key == "priority") out[section].second = val;
			}
			return out;
		}

		// the bundle's path table: "POTATO70", size, dummy, then the table's length at 0x10; the table starts at 0x20
		std::vector<std::string_view> MoviesIn(const std::filesystem::path& a_bundle)
		{
			std::vector<std::string_view> found;
			std::ifstream f(a_bundle, std::ios::binary);
			if (!f) return found;
			char head[0x20]{};
			if (!f.read(head, sizeof(head)) || std::memcmp(head, "POTATO70", 8) != 0) return found;
			std::uint32_t table = 0;
			std::memcpy(&table, head + 0x10, sizeof(table));
			table = std::min<std::uint32_t>(table, 64u << 20);
			std::string buf(table, '\0');
			f.read(buf.data(), static_cast<std::streamsize>(table));
			buf.resize(static_cast<std::size_t>(f.gcount()));
			buf = Lower(std::move(buf));
			for (const auto movie : kMovies) {
				if (buf.find(Lower(std::string(movie))) != std::string::npos) found.push_back(movie);
			}
			return found;
		}

		std::string_view ShortMovie(std::string_view a_movie)
		{
			const auto s = a_movie.rfind('\\');
			return s == std::string_view::npos ? a_movie : a_movie.substr(s + 1);
		}
	}

	void Run()
	{
		Result r;
		r.ran = true;
		std::error_code ec;
		const auto root = paths::GameRoot();
		const auto mods = root / L"Mods";
		const auto ours = mods / kOurFolder;
		r.modFolder = std::filesystem::is_directory(ours, ec);
		const bool bundle = std::filesystem::is_regular_file(ours / L"content" / L"blob0.bundle", ec);
		const bool script = std::filesystem::is_regular_file(ours / L"content" / L"scripts" / L"local" / L"HUDPositionManager.ws", ec);
		r.files = bundle && script;
		if (!r.modFolder) {
			logger::warn("check: {} is not there - the HUD movies and the script are missing, so the page works but the HUD never "
			             "changes. Install the mod's Mods folder into the game folder (beside bin and content)",
				Narrow(ours.wstring()));
		} else if (!r.files) {
			logger::warn("check: {} is incomplete - content\\blob0.bundle {}, content\\scripts\\local\\HUDPositionManager.ws {}. Reinstall the mod",
				Narrow(ours.wstring()), bundle ? "present" : "MISSING", script ? "present" : "MISSING");
		} else {
			logger::info("check: Mods\\modHUDPositionManager is installed (HUD movies and script present) in {}", Narrow(mods.wstring()));
		}

		// mods.settings (Documents\The Witcher 3; under Mod Organizer 2 the profile's own copy, through the virtual folder)
		const auto docs = Documents();
		const auto settingsFile = docs / L"The Witcher 3" / L"mods.settings";
		std::unordered_map<std::string, std::pair<int, int>> list;
		if (docs.empty() || !std::filesystem::is_regular_file(settingsFile, ec)) {
			logger::info("check: no mods.settings at {} - the game loads every Mods folder in name order", Narrow(settingsFile.wstring()));
		} else {
			list = ReadModsSettings(settingsFile);
			const auto it = list.find(Lower(Narrow(std::wstring(kOurFolder))));
			if (it == list.end()) {
				r.listed = 0;
				logger::info("check: mods.settings ({}) has no [modHUDPositionManager] line - if the HUD does not change, add "
				             "[modHUDPositionManager] Enabled=1 and a Priority (under Mod Organizer 2 the game does not add it by itself)",
					Narrow(settingsFile.wstring()));
			} else {
				r.listed = it->second.first != 0 ? 1 : 2;
				if (r.listed == 2) {
					logger::warn("check: mods.settings ({}) has [modHUDPositionManager] Enabled=0 - the game does not load the mod's HUD "
					             "movies or script. Set Enabled=1 (or enable it in your mod manager)",
						Narrow(settingsFile.wstring()));
				} else {
					logger::info("check: mods.settings lists [modHUDPositionManager] Enabled=1 Priority={}", it->second.second);
				}
			}
		}
		auto priority = [&](const std::wstring& a_name) {
			const auto it = list.find(Lower(Narrow(a_name)));
			if (it == list.end()) return std::string("not listed");
			return std::format("Priority={}{}", it->second.second, it->second.first == 0 ? ", Enabled=0" : "");
		};

		// every other Mods folder: our HUD movies in its bundles, our hooked HUD module scripts among its scripts
		std::size_t scanned = 0;
		if (std::filesystem::is_directory(mods, ec)) {
			for (const auto& d : std::filesystem::directory_iterator(mods, std::filesystem::directory_options::skip_permission_denied, ec)) {
				if (!d.is_directory(ec)) continue;
				const auto name = d.path().filename().wstring();
				if (SameName(name, kOurFolder)) continue;
				++scanned;
				const auto content = d.path() / L"content";
				std::vector<std::string> movies;
				for (const auto& f : std::filesystem::directory_iterator(content, std::filesystem::directory_options::skip_permission_denied, ec)) {
					if (!f.is_regular_file(ec) || Lower(Narrow(f.path().extension().wstring())) != ".bundle") continue;
					for (const auto m : MoviesIn(f.path())) {
						const std::string s(ShortMovie(m));
						if (std::ranges::find(movies, s) == movies.end()) movies.push_back(s);
					}
				}
				if (!movies.empty()) {
					std::string joined;
					for (const auto& m : movies) joined += (joined.empty() ? "" : ", ") + m;
					logger::warn("check: Mods\\{} ({}) also replaces {} - the game uses only one mod's copy of a file, and where its "
					             "copy is used the layout cannot reach that part of the HUD. Keep one of the two, or give HUD Position "
					             "Manager the higher priority (ours: {})",
						Narrow(name), priority(name), joined, priority(std::wstring(kOurFolder)));
					r.movieConflicts.push_back(Narrow(name));
				}
				const auto modules = content / L"scripts" / L"game" / L"gui" / L"hud" / L"modules";
				for (const auto s : kScripts) {
					if (std::filesystem::is_regular_file(modules / s, ec)) {
						logger::info("check: Mods\\{} replaces the game's script {} - HUD Position Manager hooks that class with "
						             "annotations, which normally works with a replaced file; if the HUD does not change, try without it",
							Narrow(name), Narrow(std::wstring(s)));
					}
				}
			}
		}
		logger::info("check: {} other Mods folder(s) looked at, {} replacing one of HUD Position Manager's HUD movies", scanned,
			r.movieConflicts.size());
		logger::flush();
		std::scoped_lock l(g_lock);
		g_result = std::move(r);
	}

	Result Get()
	{
		std::scoped_lock l(g_lock);
		return g_result;
	}

	bool Blocking()
	{
		const auto r = Get();
		return r.ran && (!r.modFolder || !r.files || r.listed == 2 || !r.movieConflicts.empty());
	}

	std::string Summary()
	{
		const auto  r = Get();
		std::string s;
		for (const auto& m : r.movieConflicts) s += (s.empty() ? "" : ", ") + m;
		return s;
	}
}
