#pragma once

#include "Elements.h"

// HUDPositionManager.ini in bin\x64_dx12\HUDPositionManager\ (beside the .asi). The Oblivion Remastered version's file,
// element for element (the owner, 2026-10-07: "organized the way HUD Position Manager is in Skyrim and Oblivion"), plus
// what is The Witcher 3's own: each element's "Place" (the 1.0.0 layout's spots - the minimap in the top right corner,
// the player widget in the bottom left, ...) and the minimap's zoom. Save() rewrites only this mod's keys in place, so
// comments and unknown lines survive. The page draws on AMF's render thread and the bridge runs on its own thread, so
// values are read and written only through Snapshot() / Update().

namespace settings
{
	struct Element
	{
		float       x = 0.0f;              // [<key>] fX - right is positive, a PERCENTAGE of the screen's width
		float       y = 0.0f;              // [<key>] fY - down is positive, a percentage of the screen's height
		float       scale = 1.0f;          // [<key>] fScale - about the element's own centre
		float       stretchX = 1.0f;       // [<key>] fLength - along the element's width, on top of fScale (the bars)
		float       stretchY = 1.0f;       // [<key>] fHeight - the same for its height
		bool        hide = false;          // [<key>] bHide
		int         show = 0;              // [<key>] iShow - 0 always (as the game decides), 1 only in combat, 2 only out of combat
		bool        alwaysVisible = false; // [<key>] bAlwaysVisible - the elements the game fades on its own
		std::string moveWith;              // [<key>] sMoveWith - another element's key, or empty
		bool        place = false;         // [<key>] bPlace - this mod's spot for it (Elements.h placeKey) instead of the game's
		int         fill = 0;              // [<key>] iFill - the bars: 0 the game's own, 1 left, 2 centre, 3 right (phase 3)
		int         grow = 0;              // [<key>] iGrow - the bars: 0 both sides, 1 toward the right, 2 toward the left (phase 3)
		float       zoom = 100.0f;         // [Minimap] fZoom - percent of the game's own zoom, 50..300
	};

	// the Combined widgets tab: any set of elements moves as one on shared sliders, on top of each one's own
	struct Group
	{
		std::vector<std::string> members;   // [Group] sMembers - element keys, comma-separated
		float                    x = 0.0f;  // [Group] fX / fY - percent of the screen, like an element's own
		float                    y = 0.0f;
		bool                     Has(std::string_view a_key) const
		{
			for (const auto& m : members) if (m == a_key) return true;
			return false;
		}
	};

	struct Values
	{
		bool                 enabled = true;        // [General] bEnabled - "Apply my layout"
		bool                 alwaysVisible = false; // [General] bAlwaysVisible - the whole HUD (phase 2)
		bool                 preview = false;       // [General] bPreview - "Show every element" (phase 3)
		bool                 unlocked = false;      // [General] bUnlocked - "Free placement": past the screen's edges
		std::vector<Element> elements = Defaults();
		Group                group;
		int                  logLevel = 2;          // [Log] uLogLevel (rule 14: shipped at info)

		static std::vector<Element> Defaults();
	};

	inline constexpr float kMoveX = 100.0f, kMoveY = 100.0f;   // percent of the screen
	inline constexpr float kScaleMin = 0.25f, kScaleMax = 3.0f;
	inline constexpr float kZoomMin = 50.0f, kZoomMax = 300.0f;

	Element DefaultElement(std::size_t a_index);
	Values  Snapshot();
	void    Update(const std::function<void(Values&)>& a_change);   // clamps, then saves
	void    ResetElement(std::size_t a_index);
	void    ResetAll();                                              // every element's defaults (the switches stay)
	std::uint64_t Generation();                                      // bumps on every change, for the bridge

	void Load();
	bool Save();

	// Presets: whole layouts as INI files in bin\x64_dx12\HUDPositionManager\presets\<name>.ini (the element sections, the
	// [Group] section and a [Preset] header: sName, sAuthor, sNote). Any preset can be deleted (memory presets-can-be-deleted).
	struct PresetInfo
	{
		std::filesystem::path path;
		std::string           name, author, note;
	};
	std::filesystem::path   PresetsFolder();
	std::vector<PresetInfo> ListPresets();
	bool                    LoadPreset(const std::filesystem::path& a_path);
	std::filesystem::path   SavePreset(std::string a_name, const std::string& a_author, const std::string& a_note);
	bool                    UpdatePreset(const std::filesystem::path& a_path);
	bool                    DeletePreset(const std::filesystem::path& a_path);
	std::filesystem::path   IniPath();

	// The whole layout as the one string the game's script hands to the HUD movies (bridge.cpp writes it into the hidden
	// setting HUDPositionManager.Layout). Format: "1|<enabled>|<unlocked>|<alwaysVisible>|<preview>|<groupX>|<groupY>" then
	// one "|<key>,<x>,<y>,<scale>,<length>,<height>,<hide>,<show>,<always>,<place>,<moveWith>,<groupMember>,<zoom>,<fill>,<grow>"
	// per element. x / y are percent of the screen (the movies scale them by the visible screen).
	std::string LayoutString(const Values& a_v);
}
