-- HUD Position Manager for The Witcher 3: Wild Hunt - Remastered (patch 5.0, DX12): the settings page on the Apocrypha
-- Menu Framework (HUDPositionManager.asi, beside the framework in bin\x64_dx12), organised as the Skyrim and Oblivion
-- Remastered versions are (the owner, 2026-10-07; plan 4. plans/square-minimap-witcher3/PLAN-2-hpm-parity.md). The HUD
-- itself is changed by the mod's HUD movies (tools/build_flash.py -> dist/Mods/modHUDPositionManager).
-- rule 45: no build-machine paths in any compiled object (/d1trimfile wrapped in a table, logic library 7598).
add_cxflags({"/d1trimfile:$(projectdir)"}, {force = true, expand = false})
add_shflags("/PDBALTPATH:%_PDB%", {force = true})

set_project("HUDPositionManager")
set_version("1.0.0")
set_license("GPL-3.0-or-later")
set_languages("c++23")
set_warnings("allextra")
set_arch("x64")
set_runtimes("MT")

add_rules("mode.debug", "mode.releasedbg")

add_requires("spdlog", {configs = {runtimes = "MT"}})
add_requireconfs("spdlog", {configs = {header_only = false, fmt_external = false, std_format = false}})

-- Dear ImGui 1.90.8 docking, the framework's own build: AMF::UseFrameworkImGui() checks the version and struct sizes,
-- so this must stay the same source and the same defines (copied from Item Explorer W3, which copies AMF W3's).
target("imgui")
    set_kind("static")
    set_warnings("none")
    add_files("extern/imgui/imgui.cpp", "extern/imgui/imgui_draw.cpp", "extern/imgui/imgui_tables.cpp",
              "extern/imgui/imgui_widgets.cpp")
    add_includedirs("extern/imgui", {public = true})

target("HUDPositionManager")
    set_kind("shared")
    set_extension(".asi")
    add_deps("imgui")
    add_packages("spdlog")
    add_syslinks("user32", "shell32", "ole32")
    on_load(function (target)
        target:add("defines", "HPM_VERSION=\"" .. (target:version() or "0.0.0") .. "\"")
    end)
    add_defines("NOMINMAX")
    add_files("src/**.cpp")
    add_headerfiles("src/**.h", "include/**.h")
    add_includedirs("include", "src")
    set_pcxxheader("src/pch.h")
