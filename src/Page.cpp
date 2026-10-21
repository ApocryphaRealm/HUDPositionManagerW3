// The "HUD Position Manager" page on the Apocrypha Menu Framework (The Witcher 3) - the Oblivion Remastered version's page
// (Presets / Layout with one tab per element / Combined widgets; the owner, 2026-10-07: "completely reformatted and
// organized the way HUD Position Manager is in Skyrim and Oblivion"), with The Witcher 3's own: each element's Place (the
// 1.0.0 spots) and the minimap's zoom. Drawn on AMF's render thread; it changes settings only through settings::Update()
// and reads the HUD's report through bridge::Statuses().
#include "Page.h"
#include "PreciseSlider.h"

#include <imgui.h>

#include "AMF.h"
#include "Bridge.h"
#include "Settings.h"
#include "Strings.h"

namespace page
{
	namespace
	{
		// An on/off switch (rule 32 - never a checkbox): the framework's own design, a red/green track and a white knob.
		bool Switch(const char* a_label, bool* a_v)
		{
			ImGui::PushID(a_label);
			const float  h = ImGui::GetFrameHeight();
			const float  w = h * 2.0f;
			const float  r = h * 0.5f;
			const ImVec2 p = ImGui::GetCursorScreenPos();
			const bool   pressed = ImGui::InvisibleButton("##switch", ImVec2(w, h));
			if (pressed) *a_v = !*a_v;
			const bool  hot = ImGui::IsItemHovered() || ImGui::IsItemFocused();
			const ImU32 track = *a_v ? (hot ? IM_COL32(92, 191, 96, 255) : IM_COL32(76, 175, 80, 255))
			                         : (hot ? IM_COL32(207, 84, 84, 255) : IM_COL32(191, 68, 68, 255));
			auto* dl = ImGui::GetWindowDrawList();
			dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), track, r);
			dl->AddCircleFilled(ImVec2(p.x + r + (*a_v ? w - h : 0.0f), p.y + r), r - 2.0f, IM_COL32(240, 240, 240, 255), 32);
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			ImGui::AlignTextToFramePadding();
			const char* hash = std::strstr(a_label, "##");
			ImGui::TextUnformatted(a_label, hash ? hash : nullptr);
			ImGui::PopID();
			return pressed;
		}

		void Hint(const char* a_text)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			ImGui::TextWrapped("%s", a_text);
			ImGui::PopStyleColor();
		}

		float Width() { return ImGui::GetContentRegionAvail().x * 0.6f; }

		using precise::PercentSlider;
		using precise::ScaleSlider;
		using precise::StepSlider;

		// a slider's range, frozen while it is held (a range that moved with the report made a mouse drag jump)
		struct HeldRange
		{
			bool   x = false, y = false;
			double minX = 0, maxX = 0, minY = 0, maxY = 0;
		};
		std::vector<HeldRange> g_held(elements::Count());
		HeldRange              g_heldGroup;

		// ---------------------------------------------------------------- the Presets tab
		std::vector<settings::PresetInfo> g_presets;
		ULONGLONG                         g_presetsAt = 0;
		int                               g_presetIndex = 0;
		char                              g_presetName[64] = "My layout";
		std::string                       g_presetNotice;
		std::string                       g_deleteArmed;
		int                               g_saveIndex = 0;

		void PresetsTab()
		{
			const ULONGLONG now = GetTickCount64();
			if (now - g_presetsAt >= 1000) {
				g_presetsAt = now;
				g_presets = settings::ListPresets();
			}
			if (g_presetIndex >= static_cast<int>(g_presets.size())) g_presetIndex = g_presets.empty() ? 0 : static_cast<int>(g_presets.size()) - 1;
			ImGui::TextWrapped("%s", TR("HPM_PresetsIntro", "A preset is a whole layout: every element's position, size and visibility. Load one made by someone else, or save your own to switch between."));
			Hint(settings::PresetsFolder().string().c_str());
			ImGui::Spacing();
			if (g_presets.empty()) {
				Hint(TR("HPM_PresetsNone", "No presets yet. Save your layout below, or put a preset file from another author in the folder above."));
			} else {
				std::vector<std::string> labels;
				for (const auto& p : g_presets) labels.push_back(p.author.empty() ? p.name : std::format("{} ({})", p.name, p.author));
				std::vector<const char*> ptrs;
				for (const auto& l : labels) ptrs.push_back(l.c_str());
				ImGui::SetNextItemWidth(Width());
				ImGui::Combo((std::string(TR("HPM_Preset", "Preset")) + "##preset").c_str(), &g_presetIndex, ptrs.data(), static_cast<int>(ptrs.size()));
				const auto& cur = g_presets[static_cast<std::size_t>(g_presetIndex)];
				if (!cur.note.empty()) Hint(cur.note.c_str());
				if (ImGui::Button((std::string(TR("HPM_PresetLoad", "Load this preset")) + "##load").c_str())) {
					const bool ok = settings::LoadPreset(cur.path);
					g_presetNotice = ok ? std::format("{}: {}", TR("HPM_PresetLoaded", "Loaded"), cur.name) : std::format("{}: {}", TR("HPM_PresetFailed", "Could not read"), cur.name);
				}
				ImGui::SameLine();
				const bool armed = g_deleteArmed == cur.path.string();
				if (ImGui::Button((std::string(armed ? TR("HPM_PresetDeleteSure", "Press again to delete") : TR("HPM_PresetDelete", "Delete")) + "##delete").c_str())) {
					if (armed) {
						const bool ok = settings::DeletePreset(cur.path);
						g_presetNotice = ok ? std::format("{}: {}", TR("HPM_PresetDeleted", "Deleted"), cur.name) : std::format("{}: {}", TR("HPM_PresetDeleteFailed", "Could not delete"), cur.name);
						logger::info("page: preset {} {}", cur.path.string(), ok ? "deleted" : "could not be deleted");
						g_deleteArmed.clear();
						g_presetsAt = 0;
					} else {
						g_deleteArmed = cur.path.string();
					}
				}
			}
			ImGui::Spacing();
			ImGui::SeparatorText(TR("HPM_PresetSaveGroup", "Save my layout"));
			{
				std::vector<std::string> labels{ TR("HPM_PresetNew", "New preset") };
				for (const auto& p : g_presets) labels.push_back(p.name);
				std::vector<const char*> ptrs;
				for (const auto& l : labels) ptrs.push_back(l.c_str());
				if (g_saveIndex >= static_cast<int>(labels.size())) g_saveIndex = 0;
				ImGui::SetNextItemWidth(Width());
				ImGui::Combo((std::string(TR("HPM_PresetSaveTo", "Save to")) + "##saveto").c_str(), &g_saveIndex, ptrs.data(), static_cast<int>(ptrs.size()));
			}
			if (g_saveIndex == 0) {
				ImGui::SetNextItemWidth(Width());
				ImGui::InputText((std::string(TR("HPM_PresetName", "Name")) + "##name").c_str(), g_presetName, sizeof(g_presetName));
				Hint(TR("HPM_PresetNameHint", "With a controller, leave the name as it is: each save gets its own number."));
				if (ImGui::Button((std::string(TR("HPM_PresetSave", "Save as a preset")) + "##save").c_str())) {
					const auto saved = settings::SavePreset(g_presetName, "", "");
					g_presetNotice = saved.empty() ? std::string(TR("HPM_PresetSaveFailed", "The preset could not be written.")) : std::format("{}: {}", TR("HPM_PresetSaved", "Saved"), saved.stem().string());
					g_presetsAt = 0;
				}
			} else {
				const auto& target = g_presets[static_cast<std::size_t>(g_saveIndex - 1)];
				if (ImGui::Button((std::string(TR("HPM_PresetUpdate", "Update this preset with my layout")) + "##update").c_str())) {
					const bool ok = settings::UpdatePreset(target.path);
					g_presetNotice = ok ? std::format("{}: {}", TR("HPM_PresetSaved", "Saved"), target.name) : std::string(TR("HPM_PresetSaveFailed", "The preset could not be written."));
					g_presetsAt = 0;
				}
			}
			if (!g_presetNotice.empty()) ImGui::TextWrapped("%s", g_presetNotice.c_str());
		}

		// every element's tab name, written out in full so the translation check reads each key from the source
		const char* ElementName(std::size_t a_i)
		{
			const std::string k = a_i < elements::Count() ? elements::All()[a_i].key : "";
			if (k == "Minimap") return TR("HPM_El_Minimap", "Minimap");
			if (k == "Quests") return TR("HPM_El_Quests", "Quest list");
			if (k == "Medallion") return TR("HPM_El_Medallion", "Medallion");
			if (k == "Health") return TR("HPM_El_Health", "Health bar");
			if (k == "Experience") return TR("HPM_El_Experience", "Experience bar");
			if (k == "Stamina") return TR("HPM_El_Stamina", "Stamina bar");
			if (k == "Adrenaline") return TR("HPM_El_Adrenaline", "Adrenaline bar");
			if (k == "Toxicity") return TR("HPM_El_Toxicity", "Toxicity bar");
			if (k == "Magic") return TR("HPM_El_Magic", "Magic bar");
			if (k == "Sign") return TR("HPM_El_Sign", "Sign badge");
			if (k == "Skull") return TR("HPM_El_Skull", "Toxicity skull");
			if (k == "Buffs") return TR("HPM_El_Buffs", "Effects (food, potions)");
			if (k == "QuickItems") return TR("HPM_El_QuickItems", "Quick items");
			if (k == "Dialogue") return TR("HPM_El_Dialogue", "Dialogue choices");
			if (k == "Subtitles") return TR("HPM_El_Subtitles", "Subtitles");
			if (k == "Messages") return TR("HPM_El_Messages", "Messages");
			if (k == "Hints") return TR("HPM_El_Hints", "Button hints");
			if (k == "Area") return TR("HPM_El_Area", "Area name");
			if (k == "Journal") return TR("HPM_El_Journal", "Quest and journal updates");
			if (k == "LootFeed") return TR("HPM_El_LootFeed", "Items received");
			if (k == "Enemy") return TR("HPM_El_Enemy", "Enemy health");
			if (k == "Boss") return TR("HPM_El_Boss", "Boss health");
			if (k == "Companion") return TR("HPM_El_Companion", "Companion health");
			if (k == "Breath") return TR("HPM_El_Breath", "Breath bar");
			if (k == "HorseStamina") return TR("HPM_El_HorseStamina", "Horse stamina");
			if (k == "HorseFear") return TR("HPM_El_HorseFear", "Horse fear");
			if (k == "Boat") return TR("HPM_El_Boat", "Boat health");
			if (k == "Damaged") return TR("HPM_El_Damaged", "Damaged gear");
			if (k == "TimeLeft") return TR("HPM_El_TimeLeft", "Time limit");
			if (k == "TimeLapse") return TR("HPM_El_TimeLapse", "Passing time text");
			return a_i < elements::Count() ? elements::All()[a_i].english : "";
		}

		// The move sliders' range: as far as the element can go before its drawn box leaves the screen (the box the HUD
		// reported, in percent), or the whole range with Free placement or before the HUD has reported.
		void Range(const bridge::ElementStatus& a_st, const settings::Element& a_e, bool a_unlocked, double& a_minX, double& a_maxX, double& a_minY, double& a_maxY)
		{
			a_minX = -settings::kMoveX;
			a_maxX = settings::kMoveX;
			a_minY = -settings::kMoveY;
			a_maxY = settings::kMoveY;
			if (a_unlocked || !a_st.found || a_st.right <= a_st.left || a_st.bottom <= a_st.top) return;
			a_minX = a_e.x - a_st.left;
			a_maxX = a_e.x + (100.0 - a_st.right);
			a_minY = a_e.y - a_st.top;
			a_maxY = a_e.y + (100.0 - a_st.bottom);
			// never narrower than where the slider is now (a box measured mid-animation must not push it)
			a_minX = std::min(a_minX, static_cast<double>(a_e.x));
			a_maxX = std::max(a_maxX, static_cast<double>(a_e.x));
			a_minY = std::min(a_minY, static_cast<double>(a_e.y));
			a_maxY = std::max(a_maxY, static_cast<double>(a_e.y));
		}

		void ElementTab(std::size_t a_i, const settings::Values& a_v, const std::vector<bridge::ElementStatus>& a_all, bool a_hud)
		{
			const auto&       st = a_all[a_i];
			const auto&       el = elements::All()[a_i];
			auto              e = a_v.elements[a_i];
			const std::string id = std::string("##") + el.key;
			if (!a_hud) {
				Hint(TR("HPM_NoHud", "The HUD has not reported yet - load a game to see this element."));
			} else if (!st.found) {
				ImGui::TextWrapped("%s", TR("HPM_NotFound", "Not in your HUD right now: it may appear later. Its settings are kept."));
			} else {
				Hint(TR("HPM_Found", "In your HUD. Changes show at once."));
			}
			bool changed = false;
			if (elements::WolfPart(el)) {
				Hint(TR("HPM_WolfPartHint", "One part of the player widget, on its own: it moves, sizes and hides by itself. Pick it on the Combined widgets tab to move it together with other parts or widgets."));
			}
			// no anchors (the owner, 2026-10-07: "Get rid of the anchors ... we don't need to lock it to a position"): the
			// sliders move every element from where the game puts it; an anchor left on in older settings is converted by the
			// bridge once the HUD has measured it (Bridge.cpp ConvertAnchors)
			if (e.place) Hint(TR("HPM_AnchorConverting", "Being moved over to its sliders - it keeps its place on the screen."));
			double minX, maxX, minY, maxY;
			Range(st, e, a_v.unlocked, minX, maxX, minY, maxY);
			minX = std::clamp(minX, -100.0, 100.0);
			maxX = std::clamp(maxX, minX, 100.0);
			minY = std::clamp(minY, -100.0, 100.0);
			maxY = std::clamp(maxY, minY, 100.0);
			auto& held = g_held[a_i];
			if (held.x) { minX = held.minX; maxX = held.maxX; } else { held.minX = minX; held.maxX = maxX; }
			if (held.y) { minY = held.minY; maxY = held.maxY; } else { held.minY = minY; held.maxY = maxY; }
			ImGui::SetNextItemWidth(Width());
			changed |= PercentSlider((std::string(TR("HPM_MoveX", "Move left / right")) + id + "x").c_str(), &e.x, minX, maxX);
			held.x = ImGui::IsItemActive();
			ImGui::SetNextItemWidth(Width());
			changed |= PercentSlider((std::string(TR("HPM_MoveY", "Move up / down")) + id + "y").c_str(), &e.y, minY, maxY);
			held.y = ImGui::IsItemActive();
			ImGui::SetNextItemWidth(Width());
			changed |= ScaleSlider((std::string(TR("HPM_Size", "Size")) + id + "s").c_str(), &e.scale, settings::kScaleMin, settings::kScaleMax);
			// the adrenaline bar's look (the owner, 2026-10-20: one Adrenaline bar entry, "just make the settings be in the same
			// page, and it should just be a toggle"): on, the game's own pips; off, the straight bar. A look, not a place:
			// Reset leaves it alone, as with the minimap's shape
			const bool adrenaline = std::string_view(el.key) == "Adrenaline";
			if (adrenaline) {
				bool vanilla = a_v.vanillaAdrenaline;
				if (Switch((std::string(TR("HPM_VanillaLook", "Vanilla look")) + id + "w").c_str(), &vanilla)) {
					settings::Update([vanilla](settings::Values& v) { v.vanillaAdrenaline = vanilla; });
					logger::info("page: adrenaline look {}", vanilla ? "vanilla" : "straight bar");
				}
				Hint(TR("HPM_VanillaLookHint", "On: the game's own adrenaline points. Off: a straight bar under the stamina bar. Both are red. Reset leaves the look as it is."));
			}
			if (el.stretch) {
				ImGui::SetNextItemWidth(Width());
				changed |= ScaleSlider((std::string(TR("HPM_Length", "Length")) + id + "l").c_str(), &e.stretchX, settings::kScaleMin, settings::kScaleMax);
				if (adrenaline) ImGui::SetItemTooltip("%s", TR("HPM_StraightOnly", "Straight-bar look only"));
				ImGui::SetNextItemWidth(Width());
				changed |= ScaleSlider((std::string(TR("HPM_Height", "Height")) + id + "t").c_str(), &e.stretchY, settings::kScaleMin, settings::kScaleMax);
				if (adrenaline) ImGui::SetItemTooltip("%s", TR("HPM_StraightOnly", "Straight-bar look only"));
				Hint(TR("HPM_StretchHint", "Length and Height stretch the bar on one side each, on top of Size."));
				if (adrenaline) Hint(TR("HPM_StraightOnlyHint", "Length and Height: straight-bar look only."));
			}
			if (std::string_view(el.key) == "Minimap") {
				int zoom = static_cast<int>(std::lround(e.zoom));
				ImGui::SetNextItemWidth(Width());
				if (StepSlider((std::string(TR("HPM_Zoom", "Zoom")) + id + "z").c_str(), &zoom, static_cast<int>(settings::kZoomMin), static_cast<int>(settings::kZoomMax), "%d %%")) {
					e.zoom = static_cast<float>(zoom);
					changed = true;
				}
				Hint(TR("HPM_ZoomHint", "Percent of the game's own zoom, which changes by itself outdoors, indoors and on a boat. Over 100: closer."));
				bool square = a_v.squareMinimap;
				if (Switch((std::string(TR("HPM_SquareMap", "Square minimap")) + id + "q").c_str(), &square)) {
					settings::Update([square](settings::Values& v) { v.squareMinimap = square; });
				}
				Hint(TR("HPM_SquareMapHint", "On: the minimap is square, framed with the game's ring art drawn square. Off: the game's own round minimap. Reset leaves the shape as it is."));
			}
			changed |= Switch((std::string(TR("HPM_Hide", "Hide")) + id + "h").c_str(), &e.hide);
			if (!e.hide) {
				const char* shows[3]{ TR("HPM_ShowAlways", "Always"), TR("HPM_ShowCombat", "Only in combat"), TR("HPM_ShowNoCombat", "Only out of combat") };
				ImGui::SetNextItemWidth(Width());
				changed |= ImGui::Combo((std::string(TR("HPM_Show", "Show")) + id + "v").c_str(), &e.show, shows, 3);
				Hint(TR("HPM_ShowHint", "When this shows while you play. Only in combat: hidden while you explore, back as soon as a fight starts. Only out of combat: hidden during fights."));
			}
			if (el.fades) {
				changed |= Switch((std::string(TR("HPM_AlwaysOne", "Always visible")) + id + "a").c_str(), &e.alwaysVisible);
				Hint(elements::WolfPart(el) ? TR("HPM_AlwaysPartHint", "The game fades the player widget out on its own. On: this part stays shown while you play; the parts left off still fade.")
				                            : TR("HPM_AlwaysOneHint", "The game fades this out on its own. On: it stays shown while you play."));
			}
			// move with: nothing, or another widget or part (the dialogue choices place themselves only)
			if (elements::Standalone(el)) {
				std::vector<std::string> labels{ TR("HPM_MoveWithNone", "Nothing - on its own") };
				std::vector<std::string> keys{ "" };
				int                      current = 0;
				for (std::size_t j = 0; j < elements::Count(); ++j) {
					if (j == a_i || !elements::Standalone(elements::All()[j])) continue;
					if (e.moveWith == elements::All()[j].key) current = static_cast<int>(labels.size());
					labels.emplace_back(ElementName(j));
					keys.emplace_back(elements::All()[j].key);
				}
				std::vector<const char*> ptrs;
				for (const auto& l : labels) ptrs.push_back(l.c_str());
				ImGui::SetNextItemWidth(Width());
				if (ImGui::Combo((std::string(TR("HPM_MoveWith", "Move with")) + id + "f").c_str(), &current, ptrs.data(), static_cast<int>(ptrs.size()))) {
					e.moveWith = keys[static_cast<std::size_t>(current)];
					changed = true;
				}
			}
			if (a_v.group.Has(el.key)) Hint(TR("HPM_InGroupHint", "Also moves with the widgets on the Combined widgets tab."));
			if (changed) settings::Update([&](settings::Values& s) { s.elements[a_i] = e; });
			if (ImGui::Button((std::string(TR("HPM_ResetOne", "Reset this element")) + id + "r").c_str())) {
				settings::ResetElement(a_i);
				logger::info("page: {} reset", el.key);
			}
		}

		// ---------------------------------------------------------------- the Combined widgets tab
		void CombinedTab(const settings::Values& a_v)
		{
			ImGui::TextWrapped("%s", TR("HPM_CombinedIntro", "Pick the widgets that should move as one. The sliders below move every picked widget together, on top of the position each has on its own tab."));
			ImGui::SeparatorText(TR("HPM_GroupMembers", "Widgets that move as one"));
			auto                                       group = a_v.group;
			bool                                       changed = false;
			std::vector<std::pair<std::size_t, float>> shift;
			for (std::size_t i = 0; i < elements::Count(); ++i) {
				const auto& el = elements::All()[i];
				if (!elements::Standalone(el)) continue;   // whole widgets and the player widget's parts
				bool on = group.Has(el.key);
				if (Switch((std::string(ElementName(i)) + "##grp" + el.key).c_str(), &on)) {
					if (on) group.members.emplace_back(el.key);
					else std::erase(group.members, std::string(el.key));
					shift.emplace_back(i, on ? 1.0f : -1.0f);
					changed = true;
				}
			}
			ImGui::Spacing();
			if (group.members.size() < 2) {
				Hint(TR("HPM_GroupNone", "Pick two or more widgets above to move them together."));
			} else {
				double minX = -100.0, maxX = 100.0, minY = -100.0, maxY = 100.0;
				auto&  held = g_heldGroup;
				if (held.x) { minX = held.minX; maxX = held.maxX; } else { held.minX = minX; held.maxX = maxX; }
				if (held.y) { minY = held.minY; maxY = held.maxY; } else { held.minY = minY; held.maxY = maxY; }
				ImGui::SetNextItemWidth(Width());
				changed |= PercentSlider((std::string(TR("HPM_MoveX", "Move left / right")) + "##groupx").c_str(), &group.x, minX, maxX);
				held.x = ImGui::IsItemActive();
				ImGui::SetNextItemWidth(Width());
				changed |= PercentSlider((std::string(TR("HPM_MoveY", "Move up / down")) + "##groupy").c_str(), &group.y, minY, maxY);
				held.y = ImGui::IsItemActive();
				if (ImGui::Button((std::string(TR("HPM_GroupReset", "Reset the shared position")) + "##groupreset").c_str())) {
					group.x = settings::DefaultGroup().x;   // the default shared position (none: the game's layout)
					group.y = settings::DefaultGroup().y;
					changed = true;
				}
			}
			if (changed) {
				settings::Update([&](settings::Values& s) {
					s.group = group;
					for (const auto& [i, dir] : shift) {   // joined: its own offset less the shared one, so it stays put; left: plus it
						s.elements[i].x -= dir * group.x;
						s.elements[i].y -= dir * group.y;
					}
				});
			}
		}

		// the bumpers (AMF::DeclareInnerTabs): which tab of each bar is open, and the one the bumpers asked for
		int         g_topTab = 0, g_topRequest = -1;
		int         g_elementTab = 0, g_elementRequest = -1;
		std::string g_elementOpen;
		std::string g_elementOrderId;

		ImGuiTabItemFlags TopFlags(int a_index) { return g_topRequest == a_index ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None; }

		void DeclareTop()
		{
			g_topRequest = -1;
			if (const int r = AMF::DeclareInnerTabs(3, g_topTab); r >= 0 && r != g_topTab) g_topRequest = r;
		}

		void Draw()
		{
			if (!AMF::UseFrameworkImGui()) return;
			strings::Refresh();
			if (!ImGui::BeginTabBar("HpmTop", ImGuiTabBarFlags_None)) return;
			if (ImGui::BeginTabItem((std::string(TR("HPM_TabPresets", "Presets")) + "##tabpresets").c_str(), nullptr, TopFlags(0))) {
				g_topTab = 0;
				PresetsTab();
				ImGui::EndTabItem();
			}
			auto       v = settings::Snapshot();
			const bool hud = bridge::HudReported();
			const auto st = bridge::Statuses();
			if (!ImGui::BeginTabItem((std::string(TR("HPM_TabLayout", "Layout")) + "##tablayout").c_str(), nullptr, TopFlags(1))) {
				if (ImGui::BeginTabItem((std::string(TR("HPM_TabCombined", "Combined widgets")) + "##tabcombined").c_str(), nullptr, TopFlags(2))) {
					g_topTab = 2;
					CombinedTab(v);
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
				DeclareTop();
				return;
			}
			g_topTab = 1;
			ImGui::TextWrapped("%s", TR("HPM_Intro", "Move, resize or hide each part of the HUD. Changes show in the HUD at once and are saved automatically."));
			if (!AMF::HasGameVars()) Hint(TR("HPM_NoBridge", "This Apocrypha Menu Framework cannot pass settings to the game (1.0.2 or later is needed)."));
			if (Switch(TR("HPM_Enabled", "Apply my layout"), &v.enabled)) {
				settings::Update([&](settings::Values& s) { s.enabled = v.enabled; });
				logger::info("page: layout {}", v.enabled ? "on" : "off");
			}
			Hint(TR("HPM_EnabledHint", "Off: every element goes back to where the game puts it."));
			if (Switch(TR("HPM_Unlocked", "Free placement"), &v.unlocked)) {
				settings::Update([&](settings::Values& s) { s.unlocked = v.unlocked; });
			}
			Hint(TR("HPM_UnlockedHint", "On: the move sliders go all the way, past the edges of the screen. Off: an element stops where it meets the edge of the screen."));
			ImGui::SeparatorText(TR("HPM_GroupVisibility", "HUD visibility"));
			if (Switch(TR("HPM_AlwaysAll", "Always visible"), &v.alwaysVisible)) {
				settings::Update([&](settings::Values& s) { s.alwaysVisible = v.alwaysVisible; });
			}
			Hint(v.alwaysVisible ? TR("HPM_AlwaysAllOnHint", "The player widget and the bars stay shown while you play. Menus, dialogue and loading screens still hide the HUD.")
			                     : TR("HPM_AlwaysAllOffHint", "Off: the game decides. The player widget fades out when it is full."));
			ImGui::Spacing();
			// the displayed order: the Combined widgets members first, then the rest
			std::vector<std::size_t> order;
			for (const auto& key : v.group.members) {
				if (const int k = elements::IndexOf(key); k >= 0) order.push_back(static_cast<std::size_t>(k));
			}
			const std::size_t pinned = order.size();
			for (std::size_t i = 0; i < elements::Count(); ++i) {
				if (!elements::All()[i].internal && std::ranges::find(order, i) == order.end()) order.push_back(i);
			}
			std::string orderId;
			for (const auto i : order) orderId += std::to_string(i) + ".";
			const bool reordered = !g_elementOrderId.empty() && orderId != g_elementOrderId;
			g_elementOrderId = orderId;
			std::size_t shown = order.size();
			if (ImGui::BeginTabBar(("HudElements##" + orderId).c_str(), ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_TabListPopupButton)) {
				for (std::size_t d = 0; d < order.size(); ++d) {
					const std::size_t       i = order[d];
					const bool              keep = reordered && g_elementOpen == elements::All()[i].key;
					const ImGuiTabItemFlags f = (g_elementRequest == static_cast<int>(d) || keep) ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None;
					const std::string       label = std::string(d < pinned ? "+ " : "") + ElementName(i) + "##tab" + elements::All()[i].key;
					if (ImGui::BeginTabItem(label.c_str(), nullptr, f)) {
						shown = d;
						g_elementOpen = elements::All()[i].key;
						ElementTab(i, v, st, hud);
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
			if (shown < order.size()) g_elementTab = static_cast<int>(shown);
			g_elementRequest = -1;
			if (const int r = AMF::DeclareInnerTabs(static_cast<int>(order.size()), g_elementTab); r >= 0 && r != g_elementTab) g_elementRequest = r;
			ImGui::Spacing();
			ImGui::Separator();
			if (ImGui::Button(TR("HPM_ResetAll", "Reset every element"))) {
				settings::ResetAll();
				logger::info("page: every element reset");
			}
			ImGui::EndTabItem();
			if (ImGui::BeginTabItem((std::string(TR("HPM_TabCombined", "Combined widgets")) + "##tabcombined").c_str(), nullptr, TopFlags(2))) {
				CombinedTab(v);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}

	void Register()
	{
		if (!AMF::IsInstalled()) {
			logger::info("the Apocrypha Menu Framework is not installed - no settings page");
			return;
		}
		if (AMF::RegisterPage(kModName, "Settings", &Draw)) {
			logger::info("AMF {}: the {} settings page is registered", AMF::Version(), kModName);
		} else {
			logger::warn("AMF refused the {} page", kModName);
		}
	}
}
