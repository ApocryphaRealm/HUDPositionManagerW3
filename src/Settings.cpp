#include "Settings.h"

#include "Paths.h"

#include <fstream>
#include <sstream>

// Carried from the Oblivion Remastered version's Settings.cpp (the same file format, the same in-place save), with The
// Witcher 3's elements, each one's Place and the minimap's zoom.

namespace settings
{
	namespace
	{
		Values                     g_values;
		std::mutex                 g_saveLock;
		std::mutex                 g_valuesLock;   // the page's thread writes, the bridge's thread reads
		std::atomic<std::uint64_t> g_generation{ 1 };

		std::string_view Trim(std::string_view a_s)
		{
			while (!a_s.empty() && (a_s.front() == ' ' || a_s.front() == '\t')) a_s.remove_prefix(1);
			while (!a_s.empty() && (a_s.back() == ' ' || a_s.back() == '\t' || a_s.back() == '\r')) a_s.remove_suffix(1);
			return a_s;
		}

		std::string Lower(std::string a_s)
		{
			std::ranges::transform(a_s, a_s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return a_s;
		}

		std::vector<std::pair<std::string, std::string>> Rows(const Values& a_v)
		{
			std::vector<std::pair<std::string, std::string>> out{
				{ "General.bEnabled", a_v.enabled ? "1" : "0" },
				{ "General.bAlwaysVisible", a_v.alwaysVisible ? "1" : "0" },
				{ "General.bPreview", a_v.preview ? "1" : "0" },
				{ "General.bUnlocked", a_v.unlocked ? "1" : "0" },
				{ "General.bImported", a_v.imported ? "1" : "0" },
			};
			const auto& all = elements::All();
			for (std::size_t i = 0; i < all.size() && i < a_v.elements.size(); ++i) {
				const std::string s = all[i].key;
				const auto&       e = a_v.elements[i];
				out.emplace_back(s + ".fX", std::format("{:.2f}", e.x));
				out.emplace_back(s + ".fY", std::format("{:.2f}", e.y));
				out.emplace_back(s + ".fScale", std::format("{:.2f}", e.scale));
				if (all[i].stretch) {
					out.emplace_back(s + ".fLength", std::format("{:.2f}", e.stretchX));
					out.emplace_back(s + ".fHeight", std::format("{:.2f}", e.stretchY));
				}
				out.emplace_back(s + ".bHide", e.hide ? "1" : "0");
				out.emplace_back(s + ".iShow", std::to_string(e.show));
				if (all[i].fades) out.emplace_back(s + ".bAlwaysVisible", e.alwaysVisible ? "1" : "0");
				out.emplace_back(s + ".sMoveWith", e.moveWith);
				if (all[i].placeKey) out.emplace_back(s + ".bPlace", e.place ? "1" : "0");
				if (all[i].bar) {
					out.emplace_back(s + ".iFill", std::to_string(e.fill));
					out.emplace_back(s + ".iGrow", std::to_string(e.grow));
				}
				if (s == "Minimap") out.emplace_back(s + ".fZoom", std::format("{:.0f}", e.zoom));
			}
			std::string members;
			for (const auto& m : a_v.group.members) members += (members.empty() ? "" : ",") + m;
			out.emplace_back("Group.sMembers", members);
			out.emplace_back("Group.fX", std::format("{:.2f}", a_v.group.x));
			out.emplace_back("Group.fY", std::format("{:.2f}", a_v.group.y));
			out.emplace_back("Log.uLogLevel", std::to_string(a_v.logLevel));
			return out;
		}

		void Clamp(Values& a_v)
		{
			a_v.elements.resize(elements::Count());
			for (std::size_t i = 0; i < a_v.elements.size(); ++i) {
				const auto& el = elements::All()[i];
				auto&       e = a_v.elements[i];
				e.x = std::clamp(e.x, -kMoveX, kMoveX);
				e.y = std::clamp(e.y, -kMoveY, kMoveY);
				e.scale = std::clamp(e.scale, kScaleMin, kScaleMax);
				e.stretchX = el.stretch ? std::clamp(e.stretchX, kScaleMin, kScaleMax) : 1.0f;
				e.stretchY = el.stretch ? std::clamp(e.stretchY, kScaleMin, kScaleMax) : 1.0f;
				e.show = std::clamp(e.show, 0, 2);
				if (!el.fades) e.alwaysVisible = false;
				if (!el.placeKey) e.place = false;
				if (!e.moveWith.empty() && (elements::IndexOf(e.moveWith) < 0 || e.moveWith == el.key)) e.moveWith.clear();
				e.fill = el.bar ? std::clamp(e.fill, 0, 3) : 0;
				e.grow = el.bar ? std::clamp(e.grow, 0, 2) : 0;
				e.zoom = std::clamp(std::round(e.zoom), kZoomMin, kZoomMax);
			}
			std::vector<std::string> members;
			for (const auto& m : a_v.group.members) {
				if (elements::IndexOf(m) >= 0 && std::ranges::find(members, m) == members.end()) members.push_back(m);
			}
			a_v.group.members = std::move(members);
			a_v.group.x = std::clamp(a_v.group.x, -kMoveX, kMoveX);
			a_v.group.y = std::clamp(a_v.group.y, -kMoveY, kMoveY);
			a_v.logLevel = std::clamp(a_v.logLevel, 0, 6);
		}

		using Entries = std::unordered_map<std::string, std::string>;   // "section.key" (lower case) -> value

		bool ReadIni(const std::filesystem::path& a_path, Entries& a_out)
		{
			std::ifstream file(a_path);
			if (!file.is_open()) return false;
			std::string line, section;
			while (std::getline(file, line)) {
				const auto t = Trim(line);
				if (t.empty() || t.front() == ';' || t.front() == '#') continue;
				if (t.front() == '[' && t.back() == ']') {
					section = Lower(std::string(Trim(t.substr(1, t.size() - 2))));
					continue;
				}
				const auto eq = t.find('=');
				if (eq == std::string_view::npos) continue;
				a_out[section + "." + Lower(std::string(Trim(t.substr(0, eq))))] = std::string(Trim(t.substr(eq + 1)));
			}
			return true;
		}

		const std::string* Get(const Entries& a_e, const std::string& a_key)
		{
			const auto it = a_e.find(Lower(a_key));
			return it != a_e.end() ? &it->second : nullptr;
		}

		bool  Flag(const std::string& a_v) { return a_v != "0" && Lower(a_v) != "false"; }
		float Num(const std::string& a_v) { return static_cast<float>(std::atof(a_v.c_str())); }

		// the layout keys of a settings or preset file into a_v; how many elements differ from the defaults
		int ReadLayout(const Entries& a_e, Values& a_v)
		{
			if (const auto* s = Get(a_e, "General.bAlwaysVisible")) a_v.alwaysVisible = Flag(*s);
			if (const auto* s = Get(a_e, "General.bPreview")) a_v.preview = Flag(*s);
			const auto& all = elements::All();
			a_v.elements.resize(all.size());
			int changed = 0;
			for (std::size_t i = 0; i < all.size(); ++i) {
				const std::string k = all[i].key;
				auto&             e = a_v.elements[i];
				if (const auto* s = Get(a_e, k + ".fX")) e.x = Num(*s);
				if (const auto* s = Get(a_e, k + ".fY")) e.y = Num(*s);
				if (const auto* s = Get(a_e, k + ".fScale")) e.scale = Num(*s);
				if (const auto* s = Get(a_e, k + ".fLength")) e.stretchX = Num(*s);
				if (const auto* s = Get(a_e, k + ".fHeight")) e.stretchY = Num(*s);
				if (const auto* s = Get(a_e, k + ".bHide")) e.hide = Flag(*s);
				if (const auto* s = Get(a_e, k + ".iShow")) e.show = std::atoi(s->c_str());
				if (const auto* s = Get(a_e, k + ".bAlwaysVisible")) e.alwaysVisible = Flag(*s);
				if (const auto* s = Get(a_e, k + ".sMoveWith")) e.moveWith = *s;
				if (const auto* s = Get(a_e, k + ".bPlace")) e.place = Flag(*s);
				if (const auto* s = Get(a_e, k + ".iFill")) e.fill = std::atoi(s->c_str());
				if (const auto* s = Get(a_e, k + ".iGrow")) e.grow = std::atoi(s->c_str());
				if (const auto* s = Get(a_e, k + ".fZoom")) e.zoom = Num(*s);
				const auto d = DefaultElement(i);
				changed += (e.x != d.x || e.y != d.y || e.scale != 1.0f || e.stretchX != 1.0f || e.stretchY != 1.0f || e.hide || e.show != 0 || e.place != d.place) ? 1 : 0;
			}
			if (const auto* s = Get(a_e, "Group.sMembers")) {
				a_v.group.members.clear();
				for (std::size_t p = 0; p <= s->size();) {
					const auto c = s->find(',', p);
					const auto part = std::string(Trim(std::string_view(*s).substr(p, (c == std::string::npos ? s->size() : c) - p)));
					if (!part.empty()) a_v.group.members.push_back(part);
					if (c == std::string::npos) break;
					p = c + 1;
				}
			}
			if (const auto* s = Get(a_e, "Group.fX")) a_v.group.x = Num(*s);
			if (const auto* s = Get(a_e, "Group.fY")) a_v.group.y = Num(*s);
			return changed;
		}

		bool WritePreset(const std::filesystem::path& a_path, const std::string& a_name, const std::string& a_author, const std::string& a_note)
		{
			Values        snapshot = Snapshot();
			std::ofstream out(a_path, std::ios::binary | std::ios::trunc);
			if (!out.is_open()) {
				logger::error("preset: {} could not be written", a_path.string());
				return false;
			}
			out << "; HUD Position Manager (The Witcher 3) preset - a whole layout. Load it from the Presets tab; every element\r\n"
			    << "; the file leaves out goes back to its default. fX / fY are a percentage of the screen.\r\n"
			    << "[Preset]\r\nsName=" << a_name << "\r\nsAuthor=" << a_author << "\r\nsNote=" << a_note << "\r\n\r\n"
			    << "[General]\r\nbAlwaysVisible=" << (snapshot.alwaysVisible ? 1 : 0) << "\r\n";
			std::string section;
			for (const auto& [key, value] : Rows(snapshot)) {
				const auto        dot = key.find('.');
				const std::string sec = key.substr(0, dot);
				if (sec == "General" || sec == "Log") continue;
				if (sec != section) {
					out << "\r\n[" << sec << "]\r\n";
					section = sec;
				}
				out << key.substr(dot + 1) << "=" << value << "\r\n";
			}
			return true;
		}
	}

	// The 1.0.0 layout is each element's default: its Place on (the minimap in the top right corner, the player widget in the
	// bottom left, ...) and the small gaps of 1.0.0 as the offset (HUD units of a 1920 x 1080 screen, in percent).
	Element DefaultElement(std::size_t a_index)
	{
		Element e;
		if (a_index >= elements::Count()) return e;
		const auto& el = elements::All()[a_index];
		e.place = el.placeKey && el.placeDefault;
		const std::string_view k = el.key;
		const auto px = [](float a_units) { return a_units / 1920.0f * 100.0f; };
		const auto py = [](float a_units) { return a_units / 1080.0f * 100.0f; };
		if (k == "Minimap") { e.x = -px(4); e.y = py(4); }
		else if (k == "Quests") { e.y = py(8); }
		else if (k == "Player") { e.x = px(8); e.y = -py(8); }
		else if (k == "Buffs") { e.y = -py(6); }
		else if (k == "QuickItems") { e.x = px(8); e.y = py(8); }
		else if (k == "Dialogue") { e.x = -px(40); }
		e.x = std::round(e.x * 100.0f) / 100.0f;   // the INI's two decimals
		e.y = std::round(e.y * 100.0f) / 100.0f;
		return e;
	}

	std::vector<Element> Values::Defaults()
	{
		std::vector<Element> v;
		for (std::size_t i = 0; i < elements::Count(); ++i) v.push_back(DefaultElement(i));
		return v;
	}

	Values Snapshot()
	{
		std::scoped_lock l(g_valuesLock);
		return g_values;
	}

	std::uint64_t Generation() { return g_generation.load(); }

	void Update(const std::function<void(Values&)>& a_change)
	{
		{
			std::scoped_lock l(g_valuesLock);
			a_change(g_values);
			Clamp(g_values);
		}
		++g_generation;
		Save();
	}

	void ResetElement(std::size_t a_index)
	{
		{
			std::scoped_lock l(g_valuesLock);
			if (a_index < g_values.elements.size()) g_values.elements[a_index] = DefaultElement(a_index);
		}
		++g_generation;
		Save();
	}

	void ResetAll()
	{
		{
			std::scoped_lock l(g_valuesLock);
			g_values.elements = Values::Defaults();
		}
		++g_generation;
		Save();
	}

	std::filesystem::path PresetsFolder() { return paths::DataFolder() / L"presets"; }
	std::filesystem::path IniPath() { return paths::DataFolder() / L"HUDPositionManager.ini"; }

	void Load()
	{
		Entries entries;
		if (!ReadIni(IniPath(), entries)) {
			logger::info("settings: {} not found - the defaults are in effect (the 1.0.0 layout)", IniPath().string());
			return;
		}
		std::scoped_lock l(g_valuesLock);
		auto& v = g_values;
		if (const auto* s = Get(entries, "General.bEnabled")) v.enabled = Flag(*s);
		if (const auto* s = Get(entries, "General.bUnlocked")) v.unlocked = Flag(*s);
		if (const auto* s = Get(entries, "General.bImported")) v.imported = Flag(*s);
		const int changed = ReadLayout(entries, v);
		if (const auto* s = Get(entries, "Log.uLogLevel")) v.logLevel = std::atoi(s->c_str());
		Clamp(v);
		++g_generation;
		logger::info("settings loaded from {}: layout {}, {} widget(s) moving as one, {} element(s) changed from the defaults, log level {}",
			IniPath().string(), v.enabled ? "on" : "off", v.group.members.size(), changed, v.logLevel);
	}

	bool Save()
	{
		std::scoped_lock         l(g_saveLock);
		const auto               path = IniPath();
		std::vector<std::string> lines;
		bool                     crlf = true;
		{
			std::ifstream in(path, std::ios::binary);
			if (in.is_open()) {
				std::stringstream ss;
				ss << in.rdbuf();
				const std::string all = ss.str();
				crlf = all.find("\r\n") != std::string::npos || all.empty();
				std::string cur;
				for (char c : all) {
					if (c == '\n') {
						if (!cur.empty() && cur.back() == '\r') cur.pop_back();
						lines.push_back(cur);
						cur.clear();
					} else {
						cur.push_back(c);
					}
				}
				if (!cur.empty()) lines.push_back(cur);
			}
		}
		const auto        rows = Rows(Snapshot());
		std::vector<bool> written(rows.size(), false);
		std::string       section;
		for (auto& ln : lines) {
			const auto t = Trim(ln);
			if (!t.empty() && t.front() == '[' && t.back() == ']') {
				section = Lower(std::string(Trim(t.substr(1, t.size() - 2))));
				continue;
			}
			if (t.empty() || t.front() == ';' || t.front() == '#') continue;
			const auto eq = t.find('=');
			if (eq == std::string_view::npos) continue;
			const std::string key = section + "." + Lower(std::string(Trim(t.substr(0, eq))));
			for (std::size_t i = 0; i < rows.size(); ++i) {
				if (Lower(rows[i].first) == key) {
					ln = rows[i].first.substr(rows[i].first.find('.') + 1) + "=" + rows[i].second;
					written[i] = true;
					break;
				}
			}
		}
		for (std::size_t i = 0; i < rows.size(); ++i) {
			if (written[i]) continue;
			const auto        dot = rows[i].first.find('.');
			const std::string want = Lower(rows[i].first.substr(0, dot));
			const std::string text = rows[i].first.substr(dot + 1) + "=" + rows[i].second;
			std::size_t       insertAt = lines.size();
			bool              found = false;
			for (std::size_t j = 0; j < lines.size(); ++j) {
				const auto t = Trim(lines[j]);
				if (!t.empty() && t.front() == '[' && t.back() == ']') {
					if (found) {
						insertAt = j;
						break;
					}
					found = Lower(std::string(Trim(t.substr(1, t.size() - 2)))) == want;
				}
			}
			if (!found) {
				lines.push_back("");
				lines.push_back("[" + rows[i].first.substr(0, dot) + "]");
				insertAt = lines.size();
			} else {
				while (insertAt > 0 && Trim(lines[insertAt - 1]).empty()) --insertAt;
			}
			lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(insertAt), text);
		}
		const auto tmp = path.wstring() + L".tmp";
		{
			std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
			if (!out.is_open()) {
				logger::error("settings: {} could not be written", path.string());
				return false;
			}
			for (const auto& ln : lines) out << ln << (crlf ? "\r\n" : "\n");
		}
		std::error_code ec;
		std::filesystem::rename(tmp, path, ec);   // never a half-written settings file
		if (ec) {
			logger::error("settings: {} could not be replaced ({})", path.string(), ec.message());
			return false;
		}
		logger::debug("settings: saved to {}", path.string());
		return true;
	}

	std::vector<PresetInfo> ListPresets()
	{
		std::vector<PresetInfo> out;
		std::error_code         ec;
		for (const auto& entry : std::filesystem::directory_iterator(PresetsFolder(), ec)) {
			if (!entry.is_regular_file(ec) || Lower(entry.path().extension().string()) != ".ini") continue;
			PresetInfo p;
			p.path = entry.path();
			p.name = entry.path().stem().string();
			Entries e;
			if (ReadIni(p.path, e)) {
				if (const auto* s = Get(e, "Preset.sName"); s && !s->empty()) p.name = *s;
				if (const auto* s = Get(e, "Preset.sAuthor")) p.author = *s;
				if (const auto* s = Get(e, "Preset.sNote")) p.note = *s;
			}
			out.push_back(std::move(p));
		}
		std::ranges::sort(out, [](const PresetInfo& a, const PresetInfo& b) { return Lower(a.name) < Lower(b.name); });
		return out;
	}

	bool LoadPreset(const std::filesystem::path& a_path)
	{
		Entries e;
		if (!ReadIni(a_path, e)) {
			logger::warn("preset: {} could not be read", a_path.string());
			return false;
		}
		int changed = 0;
		{
			std::scoped_lock l(g_valuesLock);
			g_values.elements = Values::Defaults();   // a preset is the whole layout: what it leaves out is the default
			g_values.group = Group{};
			changed = ReadLayout(e, g_values);
			Clamp(g_values);
		}
		++g_generation;
		Save();
		logger::info("preset: {} loaded - {} element(s) changed from the defaults", a_path.string(), changed);
		return true;
	}

	bool UpdatePreset(const std::filesystem::path& a_path)
	{
		Entries e;
		if (!ReadIni(a_path, e)) {
			logger::warn("preset: {} could not be read - not updated", a_path.string());
			return false;
		}
		const auto* name = Get(e, "Preset.sName");
		const auto* author = Get(e, "Preset.sAuthor");
		const auto* note = Get(e, "Preset.sNote");
		const bool  ok = WritePreset(a_path, name && !name->empty() ? *name : a_path.stem().string(), author ? *author : "", note ? *note : "");
		if (ok) logger::info("preset: {} updated with the current layout", a_path.string());
		return ok;
	}

	std::filesystem::path SavePreset(std::string a_name, const std::string& a_author, const std::string& a_note)
	{
		std::string stem;
		for (const char c : a_name) {
			if (std::strchr("\\/:*?\"<>|", c) == nullptr && static_cast<unsigned char>(c) >= 0x20) stem.push_back(c);
		}
		while (!stem.empty() && (stem.back() == ' ' || stem.back() == '.')) stem.pop_back();
		while (!stem.empty() && stem.front() == ' ') stem.erase(stem.begin());
		if (stem.empty()) stem = "My layout";
		std::error_code ec;
		std::filesystem::create_directories(PresetsFolder(), ec);
		std::filesystem::path path = PresetsFolder() / (stem + ".ini");
		for (int n = 2; std::filesystem::exists(path, ec) && n < 1000; ++n) path = PresetsFolder() / std::format("{} {}.ini", stem, n);
		if (!WritePreset(path, path.stem().string(), a_author, a_note)) return {};
		logger::info("preset: the current layout saved to {}", path.string());
		return path;
	}

	bool DeletePreset(const std::filesystem::path& a_path)
	{
		std::error_code ec;
		const auto      folder = std::filesystem::weakly_canonical(PresetsFolder(), ec);
		const auto      file = std::filesystem::weakly_canonical(a_path, ec);
		if (file.parent_path() != folder) {
			logger::warn("preset: {} is not in the presets folder - not deleted", a_path.string());
			return false;
		}
		return std::filesystem::remove(file, ec) && !ec;
	}

	std::string LayoutString(const Values& a_v)
	{
		const auto  n = [](float a_f) { return std::format("{:.2f}", a_f); };
		std::string s = std::format("1|{}|{}|{}|{}|{}|{}", a_v.enabled ? 1 : 0, a_v.unlocked ? 1 : 0, a_v.alwaysVisible ? 1 : 0,
			a_v.preview ? 1 : 0, n(a_v.group.x), n(a_v.group.y));
		const auto& all = elements::All();
		for (std::size_t i = 0; i < all.size() && i < a_v.elements.size(); ++i) {
			const auto& e = a_v.elements[i];
			s += std::format("|{},{},{},{},{},{},{},{},{},{},{},{},{:.0f},{},{}", all[i].key, n(e.x), n(e.y), n(e.scale), n(e.stretchX), n(e.stretchY),
				e.hide ? 1 : 0, e.show, e.alwaysVisible ? 1 : 0, e.place ? 1 : 0, e.moveWith, a_v.group.Has(all[i].key) ? 1 : 0, e.zoom, e.fill, e.grow);
		}
		return s;
	}
}
