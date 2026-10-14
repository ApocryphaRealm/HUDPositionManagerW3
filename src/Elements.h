#pragma once

// ============================================================================================================
// The HUD elements of The Witcher 3 (5.0) this page places - the same shape as the Oblivion Remastered and Skyrim
// versions' element tables (the owner, 2026-10-07: "I want HUD Position Manager completely reformatted and organized
// the way HUD Position Manager is in Skyrim and Oblivion"; plan: 4. plans\square-minimap-witcher3\PLAN-2-hpm-parity.md).
//
// An element is either one of the game's HUD MODULES (its moduleName in the HUD movies - the minimap movie's code finds
// it in the HUD's display tree and moves it) or a PART of one that this mod draws or owns (the player widget's bars and
// badges, in hud_wolfstatbars; the dialogue choice list, in hud_dialog). The element's key is what the layout string,
// the INI section and the TR key use; `movie` says which of our movies applies it.
// ============================================================================================================

namespace elements
{
	enum class Movie { kMinimap, kWolf, kDialog };

	struct Element
	{
		const char* key;            // INI section, TR key suffix (HPM_El_<key>), layout string key
		const char* english;        // the page's tab
		const char* module;         // the game's moduleName (a whole module), or nullptr for a part of one
		Movie       movie = Movie::kMinimap;
		bool        stretch = false;  // Length and Height beside Size (the bars)
		bool        fades = false;    // the game fades it on its own: "Always visible" applies
		bool        bar = false;      // one of the player widget's bars: Fill from / Grows toward (phase 3)
		// The 1.0.0 layout's anchor for it (a corner, under the minimap, ...). Gone from the page since 2026-10-07 (the owner:
		// "Get rid of the anchors ... now that we have control over its position through sliders, we don't need to lock it
		// to a position"): every element starts where the game puts it and its sliders move it. An anchor still on in a
		// settings file or preset keeps working only until the HUD reports how far it moved the element; the bridge then
		// adds that to the element's sliders and switches the anchor off (Bridge.cpp ConvertAnchors), so nothing jumps.
		const char* placeKey = nullptr;
		const char* placeEnglish = nullptr;
		bool        placeDefault = false;   // on by default: only the dialogue choices, as in the default layout (Settings.cpp)
		// not on the page: the player widget as a whole, which only carries its parts' shared start place (the owner,
		// 2026-10-07: the player widget is broken up into its parts, not one widget that joins the Combined widgets)
		bool        internal = false;
	};

	inline const std::vector<Element>& All()
	{
		static const std::vector<Element> kAll{
			// the minimap: square, flush in the screen's top right corner by default (Square Minimap 1.0.0)
			{ .key = "Minimap", .english = "Minimap", .module = "Minimap2Module", .fades = true, .placeKey = "HPM_PlaceTopRight", .placeEnglish = "Top right corner" },
			// the quest list, centred under the minimap by default
			{ .key = "Quests", .english = "Quest list", .module = "QuestsModule", .fades = true, .placeKey = "HPM_PlaceUnderMinimap", .placeEnglish = "Under the minimap" },
			// the player widget as a whole: never on the page - it only holds where its parts start (bottom left by default,
			// the owner, 2026-10-07; the "Place" switch every part's tab shows), and the minimap movie places it there
			{ .key = "Player", .english = "Player widget", .module = "WolfHeadModule", .placeKey = "HPM_PlaceBottomLeft", .placeEnglish = "Bottom left corner", .internal = true },
			// its parts (the owner, 2026-10-07: "the player widget is a combined widget which should be broken up into its
			// individual parts"), each an element of its own, moved, sized and shown in hud_wolfstatbars
			{ .key = "Medallion", .english = "Medallion", .module = nullptr, .movie = Movie::kWolf, .fades = true },
			{ .key = "Health", .english = "Health bar", .module = nullptr, .movie = Movie::kWolf, .stretch = true, .fades = true },
			// the level (experience) bar (the owner, 2026-10-07: "The level bar doesn't have a widget and it's currently out of
			// sync with the health bar, so I guess give it its own widget as well")
			{ .key = "Experience", .english = "Experience bar", .module = nullptr, .movie = Movie::kWolf, .stretch = true, .fades = true },
			{ .key = "Stamina", .english = "Stamina bar", .module = nullptr, .movie = Movie::kWolf, .stretch = true, .fades = true, .bar = true },
			{ .key = "Adrenaline", .english = "Adrenaline bar", .module = nullptr, .movie = Movie::kWolf, .stretch = true, .fades = true, .bar = true },
			{ .key = "Toxicity", .english = "Toxicity bar", .module = nullptr, .movie = Movie::kWolf, .stretch = true, .fades = true, .bar = true },
			{ .key = "Sign", .english = "Sign badge", .module = nullptr, .movie = Movie::kWolf, .fades = true },
			{ .key = "Skull", .english = "Toxicity skull", .module = nullptr, .movie = Movie::kWolf, .fades = true },
			// the effects (food, potions, oils): just above the player widget by default
			{ .key = "Buffs", .english = "Effects (food, potions)", .module = "BuffsModule", .fades = true, .placeKey = "HPM_PlaceAbovePlayer", .placeEnglish = "Above the player widget" },
			{ .key = "QuickItems", .english = "Quick items", .module = "ItemInfoModule", .fades = true, .placeKey = "HPM_PlaceTopLeft", .placeEnglish = "Top left corner" },
			// the dialogue choice list, in hud_dialog: its longest line ends a small gap from the right edge by default
			{ .key = "Dialogue", .english = "Dialogue choices", .module = nullptr, .movie = Movie::kDialog, .placeKey = "HPM_PlaceRightEdge", .placeEnglish = "At the right edge", .placeDefault = true },
			{ .key = "Subtitles", .english = "Subtitles", .module = "SubtitlesModule", .fades = true },
			{ .key = "Messages", .english = "Messages", .module = "MessageModule", .fades = true },
			{ .key = "Hints", .english = "Button hints", .module = "ControlsFeedbackModule", .fades = true },
			{ .key = "Area", .english = "Area name", .module = "AreaInfoModule", .fades = true },
			{ .key = "Journal", .english = "Quest and journal updates", .module = "JournalUpdateModule", .fades = true },
			{ .key = "LootFeed", .english = "Items received", .module = "LootFeedModule", .fades = true },
			{ .key = "Enemy", .english = "Enemy health", .module = "EnemyFocusModule", .fades = true },
			{ .key = "Boss", .english = "Boss health", .module = "BossFocusModule", .fades = true },
			{ .key = "Companion", .english = "Companion health", .module = "CompanionModule", .fades = true },
			{ .key = "Breath", .english = "Breath bar", .module = "OxygenBarModule", .fades = true },
			{ .key = "HorseStamina", .english = "Horse stamina", .module = "HorseStaminaBarModule", .fades = true },
			{ .key = "HorseFear", .english = "Horse fear", .module = "HorsePanicBarModule", .fades = true },
			{ .key = "Boat", .english = "Boat health", .module = "BoatHealthModule", .fades = true },
			{ .key = "Damaged", .english = "Damaged gear", .module = "DamagedItemsModule", .fades = true },
			{ .key = "TimeLeft", .english = "Time limit", .module = "TimeLeftModule", .fades = true },
			{ .key = "TimeLapse", .english = "Passing time text", .module = "TimeLapseModule", .fades = true },
		};
		return kAll;
	}

	inline std::size_t Count() { return All().size(); }

	// a widget the page lists on its own and that can join the Combined widgets / be followed with "Move with": every whole
	// module and every part of the player widget (the dialogue choices place themselves only)
	inline bool Standalone(const Element& a_el) { return !a_el.internal && (a_el.module || a_el.movie == Movie::kWolf); }
	inline bool WolfPart(const Element& a_el) { return !a_el.module && a_el.movie == Movie::kWolf; }

	inline int IndexOf(std::string_view a_key)
	{
		const auto& all = All();
		for (std::size_t i = 0; i < all.size(); ++i) {
			if (a_key == all[i].key) {
				return static_cast<int>(i);
			}
		}
		return -1;
	}
}
