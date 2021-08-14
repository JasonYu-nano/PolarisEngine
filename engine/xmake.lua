-- project
set_project("PolarisEngine")

-- set xmake minimum version
set_xmakever("2.5.6")

-- set project version
set_version("0.0.1", {build = "%Y%m%d%H%M"})

-- set warning all as error
set_warnings("all")

add_cxflags("-wd4251", "-fno-strict-aliasing", "-Wno-error=nullability-completeness")

-- set language: cxx20
set_languages("cxx20")

-- add build modes
add_rules("mode.debug", "mode.releasedbg", "mode.release")
if is_mode("debug") then
    add_defines("DEBUG")
end
if is_mode("releasedbg") then
    add_defines("DEVELOPMENT")
end
if is_mode("release") then
    add_defines("SHIPPING")
end


local cfg_debug = false
local cfg_vs_runtime = "MT"
-- for the windows platform (msvc)
if is_plat("windows") then
    add_defines("WIN32")
    add_defines("_WIN32")
    if is_mode("debug") then
        --add_cxflags("-Gs", "-RTC1")
        set_runtimes("MTd")
        cfg_vs_runtime = "MTd"
        cfg_debug = true
    else
        set_runtimes("MT")
        cfg_vs_runtime = "MT"
    end
end

set_allowedarchs("windows|x64")
set_defaultmode("debug")

--option
option("shared")
    set_default(true)
    set_showmenu(true)
option_end()

option("unicode")
    set_default(true)
    set_showmenu(true)
option_end()

if has_config("shared") then
    add_defines("SHARED_LIB")
end

if has_config("unicode") then
    add_defines("UNICODE")
end


-- output
if has_config("shared") then
    set_targetdir("$(buildir)/bin/$(plat)/$(arch)")
else
    set_targetdir("$(buildir)/lib/$(plat)/$(arch)")
end

-- 3rd
add_requires("conan::fmt/7.1.2", {alias = "fmt", debug = cfg_debug})
add_requires("conan::spdlog/1.8.2", {alias = "spdlog", debug = cfg_debug, configs = { options = "spdlog:wchar_support=True" }})


-- include project sources
add_includedirs("source")
add_includedirs("source/core")
includes("source")