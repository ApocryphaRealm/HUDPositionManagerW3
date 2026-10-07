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
		// the place this mod gives it instead of the game's (the 1.0.0 layout, kept as each element's "Place" choice):
		// nullptr = only "Where the game puts it"
		const char* placeKey = nullptr;
		const char* placeEnglish = nullptr;
		bool        placeDefault = false;   // that place is the default
	};

	inline const std::vector<Element>& All()
	{
		static const std::vector<Element> kAll{
			// the minimap: square, flush in the screen's top right corner by default (Square Minimap 1.0.0)
			{ .key = "Minimap", .english = "Minimap", .module = "Minimap2Module", .placeKey = "HPM_PlaceTopRight", .placeEnglish = "Top right corner", .placeDefault = true },
			// the quest list, centred under the minimap by default
			{ .key = "Quests", .english = "Quest list", .module = "QuestsModule", .placeKey = "HPM_PlaceUnderMinimap", .placeEnglish = "Under the minimap", .placeDefault = true },
			// the player widget: medallion, vitality and the bars under it; bottom left by default (the owner, 2026-10-07)
			{ .key = "Player", .english = "Player widget", .module = "WolfHeadModule", .fades = true, .placeKey = "HPM_PlaceBottomLeft", .placeEnglish = "Bottom left corner", .placeDefault = true },
			// its parts, drawn by this mod in hud_wolfstatbars (moved inside the widget, on top of the widget's own place)
			{ .key = "Stamina", .english = "Stamina bar", .module = nullptr, .movie = Movie::kWolf, .stretch = true, .bar = true },
			{ .key = "Adrenaline", .english = "Adrenaline bar", .module = nullptr, .movie = Movie::kWolf, .stretch = true, .bar = true },
			{ .key = "Toxicity", .english = "Toxicity bar", .module = nullptr, .movie = Movie::kWolf, .stretch = true, .bar = true },
			{ .key = "Sign", .english = "Sign badge", .module = nullptr, .movie = Movie::kWolf },
			{ .key = "Skull", .english = "Toxicity skull", .module = nullptr, .movie = Movie::kWolf },
			// the effects (food, potions, oils): just above the player widget by default
			{ .key = "Buffs", .english = "Effects (food, potions)", .module = "BuffsModule", .placeKey = "HPM_PlaceAbovePlayer", .placeEnglish = "Above the player widget", .placeDefault = true },
			{ .key = "QuickItems", .english = "Quick items", .module = "ItemInfoModule", .fades = true, .placeKey = "HPM_PlaceTopLeft", .placeEnglish = "Top left corner", .placeDefault = true },
			// the dialogue choice list, in hud_dialog: its longest line ends a small gap from the right edge by default
			{ .key = "Dialogue", .english = "Dialogue choices", .module = nullptr, .movie = Movie::kDialog, .placeKey = "HPM_PlaceRightEdge", .placeEnglish = "At the right edge", .placeDefault = true },
			{ .key = "Subtitles", .english = "Subtitles", .module = "SubtitlesModule" },
			{ .key = "Messages", .english = "Messages", .module = "MessageModule" },
			{ .key = "Hints", .english = "Button hints", .module = "ControlsFeedbackModule" },
			{ .key = "Area", .english = "Area name", .module = "AreaInfoModule" },
			{ .key = "Journal", .english = "Quest and journal updates", .module = "JournalUpdateModule" },
			{ .key = "LootFeed", .english = "Items received", .module = "LootFeedModule" },
			{ .key = "Enemy", .english = "Enemy health", .module = "EnemyFocusModule", .fades = true },
			{ .key = "Boss", .english = "Boss health", .module = "BossFocusModule", .fades = true },
			{ .key = "Companion", .english = "Companion health", .module = "CompanionModule", .fades = true },
			{ .key = "Breath", .english = "Breath bar", .module = "OxygenBarModule", .fades = true },
			{ .key = "HorseStamina", .english = "Horse stamina", .module = "HorseStaminaBarModule", .fades = true },
			{ .key = "HorseFear", .english = "Horse fear", .module = "HorsePanicBarModule", .fades = true },
			{ .key = "Boat", .english = "Boat health", .module = "BoatHealthModule", .fades = true },
			{ .key = "Damaged", .english = "Damaged gear", .module = "DamagedItemsModule" },
			{ .key = "TimeLeft", .english = "Time limit", .module = "TimeLeftModule" },
			{ .key = "TimeLapse", .english = "Passing time text", .module = "TimeLapseModule" },
		};
		return kAll;
	}

	inline std::size_t Count() { return All().size(); }

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
