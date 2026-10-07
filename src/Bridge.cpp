#include "Bridge.h"

#include "AMF.h"
#include "Settings.h"

namespace bridge
{
	namespace
	{
		constexpr const char* kGroup = "HUDPositionManager";

		std::mutex                 g_lock;
		std::vector<ElementStatus> g_status(elements::Count());
		bool                       g_reported = false;
		std::string                g_lastSent;
		std::string                g_lastReport;

		std::string Read(const char* a_var)
		{
			char buf[8192]{};
			return AMF::GetGameVar(kGroup, a_var, buf, sizeof(buf)) ? std::string(buf) : std::string();
		}

		// "<key>:<found>:<l>:<t>:<r>:<b>;..." (percent of the screen) from HUDPositionManager.ws, which takes it from the movies
		// The anchors left the page on 2026-10-07 (the owner: "Get rid of the anchors ... now that we have control over its
		// position through sliders, we don't need to lock it to a position"). An anchor still on in the settings (a file or
		// preset from before) is converted once the HUD has measured it: its slider values become the ones that keep the
		// element where the anchor put it, and the anchor goes off - nothing moves on the screen. The player widget's move
		// then passes to its parts (Settings.cpp MigratePlayer).
		void ConvertAnchors(const std::unordered_map<std::string, std::pair<float, float>>& a_anchors)
		{
			if (a_anchors.empty()) return;
			const auto           v = settings::Snapshot();
			std::vector<std::size_t> todo;
			for (std::size_t i = 0; i < elements::Count() && i < v.elements.size(); ++i) {
				if (v.elements[i].place && a_anchors.contains(elements::All()[i].key)) todo.push_back(i);
			}
			if (todo.empty()) return;
			settings::Update([&](settings::Values& s) {
				// the effects' anchor sits above the player widget: when the widget's anchor goes first, the widget goes back to
				// the game's place (its parts keep their spot through their own sliders), so an effects anchor still on takes
				// the same move, and stays where it is until its own turn comes
				const int player = elements::IndexOf("Player"), buffs = elements::IndexOf("Buffs");
				if (player >= 0 && buffs >= 0 && std::ranges::find(todo, static_cast<std::size_t>(player)) != todo.end() &&
					std::ranges::find(todo, static_cast<std::size_t>(buffs)) == todo.end() && s.elements[static_cast<std::size_t>(buffs)].place) {
					const auto& [px, py] = a_anchors.at("Player");
					s.elements[static_cast<std::size_t>(buffs)].x += px - s.elements[static_cast<std::size_t>(player)].x;
					s.elements[static_cast<std::size_t>(buffs)].y += py - s.elements[static_cast<std::size_t>(player)].y;
				}
				for (const auto i : todo) {
					const auto& [x, y] = a_anchors.at(elements::All()[i].key);
					logger::info("bridge: {} anchor off - its sliders now {:.2f}, {:.2f} (were {:.2f}, {:.2f}), the same place on the screen",
						elements::All()[i].key, x, y, s.elements[i].x, s.elements[i].y);
					s.elements[i].x = x;
					s.elements[i].y = y;
					s.elements[i].place = false;
				}
			});
		}

		void ParseReport(const std::string& a_s)
		{
			std::vector<ElementStatus>                                st(elements::Count());
			std::unordered_map<std::string, std::pair<float, float>> anchors;
			std::size_t                p = 0;
			while (p < a_s.size()) {
				auto e = a_s.find(';', p);
				if (e == std::string::npos) e = a_s.size();
				const std::string part = a_s.substr(p, e - p);
				p = e + 1;
				std::vector<std::string> f;
				for (std::size_t q = 0; q <= part.size();) {
					const auto c = part.find(':', q);
					f.push_back(part.substr(q, (c == std::string::npos ? part.size() : c) - q));
					if (c == std::string::npos) break;
					q = c + 1;
				}
				if (f.size() < 2) continue;
				if (f[0] == "Anchor") {   // "Anchor:<key>:<x>:<y>" - the sliders that replace an anchor still on
					if (f.size() >= 4) anchors[f[1]] = { static_cast<float>(std::atof(f[2].c_str())), static_cast<float>(std::atof(f[3].c_str())) };
					continue;
				}
				const int i = elements::IndexOf(f[0]);
				if (i < 0) continue;
				auto& s = st[static_cast<std::size_t>(i)];
				s.found = f[1] == "1";
				if (f.size() >= 6) {
					s.left = std::atof(f[2].c_str());
					s.top = std::atof(f[3].c_str());
					s.right = std::atof(f[4].c_str());
					s.bottom = std::atof(f[5].c_str());
				}
			}
			{
				std::scoped_lock l(g_lock);
				g_status = std::move(st);
				g_reported = true;
			}
			ConvertAnchors(anchors);
		}

		// 1.0.0 kept its values on Options > Mods (HUD units of a 1920 x 1080 screen); taken over once into the INI
		bool Import()
		{
			const char* vars[] = { "Top", "Right", "QuestGap", "Zoom", "PlayerLeft", "PlayerBottom", "ItemsLeft", "ItemsTop", "DialogueRight",
				"DialogueGap", "DialogueY" };
			std::unordered_map<std::string, std::string> old;
			for (const char* v : vars) {
				const auto s = Read(v);
				if (!s.empty() && s != "-1") old[v] = s;
			}
			if (old.empty()) return false;   // nothing read yet (the framework reads a watched value from the next frame on)
			const auto num = [&](const char* a_k, float a_def) { const auto it = old.find(a_k); return it == old.end() ? a_def : static_cast<float>(std::atof(it->second.c_str())); };
			const auto px = [](float u) { return std::round(u / 1920.0f * 10000.0f) / 100.0f; };
			const auto py = [](float u) { return std::round(u / 1080.0f * 10000.0f) / 100.0f; };
			settings::Update([&](settings::Values& v) {
				auto set = [&](const char* a_key, auto&& a_fn) { if (const int i = elements::IndexOf(a_key); i >= 0) a_fn(v.elements[static_cast<std::size_t>(i)]); };
				set("Minimap", [&](settings::Element& e) { e.x = -px(num("Right", 4)); e.y = py(num("Top", 4)); e.zoom = num("Zoom", 100); });
				set("Quests", [&](settings::Element& e) { e.y = py(num("QuestGap", 8)); });
				set("Player", [&](settings::Element& e) { e.x = px(num("PlayerLeft", 8)); e.y = -py(num("PlayerBottom", 8)); });
				set("QuickItems", [&](settings::Element& e) { e.x = px(num("ItemsLeft", 8)); e.y = py(num("ItemsTop", 8)); });
				set("Dialogue", [&](settings::Element& e) {
					const auto r = old.find("DialogueRight");
					e.place = r == old.end() || (r->second != "false" && r->second != "0");
					e.x = -px(num("DialogueGap", 40));
					e.y = py(num("DialogueY", 0));
				});
				v.imported = true;
			});
			logger::info("bridge: the 1.0.0 settings (Options > Mods) taken over - {} value(s)", old.size());
			return true;
		}

		void Loop()
		{
			for (int tries = 0; !AMF::HasGameVars() && tries < 600; ++tries) std::this_thread::sleep_for(500ms);
			if (!AMF::HasGameVars()) {
				logger::warn("bridge: this Apocrypha Menu Framework has no game-setting calls (1.0.2+ needed) - the layout cannot reach the HUD");
				return;
			}
			AMF::WatchGameVar(kGroup, "Report");
			const bool needImport = !settings::Snapshot().imported;
			if (needImport) {
				for (const char* v : { "Top", "Right", "QuestGap", "Zoom", "PlayerLeft", "PlayerBottom", "ItemsLeft", "ItemsTop", "DialogueRight", "DialogueGap", "DialogueY" }) {
					AMF::WatchGameVar(kGroup, v);
				}
			}
			std::uint64_t sentGen = 0;
			int           importTries = 0;
			for (;;) {
				if (!settings::Snapshot().imported && importTries < 240) {   // up to two minutes for the game's settings to be read
					++importTries;
					if (!Import() && importTries == 240) {
						logger::info("bridge: no 1.0.0 settings found - the defaults stand");
						settings::Update([](settings::Values& v) { v.imported = true; });
					}
				}
				const auto gen = settings::Generation();
				if (gen != sentGen) {
					const auto s = settings::LayoutString(settings::Snapshot());
					if (s != g_lastSent && AMF::SetGameVar(kGroup, "Layout", s.c_str())) {
						std::scoped_lock l(g_lock);
						g_lastSent = s;
						logger::debug("bridge: layout sent ({} characters)", s.size());
					}
					sentGen = gen;
				}
				const auto rep = Read("Report");
				if (!rep.empty() && rep != g_lastReport) {
					g_lastReport = rep;
					ParseReport(rep);
				}
				std::this_thread::sleep_for(250ms);
			}
		}
	}

	void Start()
	{
		std::thread(Loop).detach();
	}

	bool HudReported()
	{
		std::scoped_lock l(g_lock);
		return g_reported;
	}

	std::vector<ElementStatus> Statuses()
	{
		std::scoped_lock l(g_lock);
		return g_status;
	}

	std::string LastSent()
	{
		std::scoped_lock l(g_lock);
		return g_lastSent;
	}
}
