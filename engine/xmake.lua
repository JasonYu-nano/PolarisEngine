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

local current_mode = ""
if is_mode("debug") then
    current_mode = "debug"
    add_defines("DEBUG")
end
if is_mode("releasedbg") then
    current_mode = "development"
    add_defines("DEVELOPMENT")
end
if is_mode("release") then
    current_mode = "shipping"
    add_defines("SHIPPING")
end


local cfg_debug = false
local cfg_vs_runtime = "MD"
-- for the windows platform (msvc)
if is_plat("windows") then
    add_defines("WIN32")
    add_defines("_WIN32")
    if is_mode("debug") then
        --add_cxflags("-Gs", "-RTC1")
        set_runtimes("MDd")
        cfg_vs_runtime = "MDd"
        cfg_debug = true
    else
        set_runtimes("MD")
        cfg_vs_runtime = "MD"
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

option("with_test")
    set_default(true)
    set_showmenu(true)
option_end()

option("with_benchmark")
    set_default(false)
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
    set_targetdir("$(buildir)/bin/$(plat)/$(arch)/"..current_mode.."/")
else
    set_targetdir("$(buildir)/lib/$(plat)/$(arch)"..current_mode.."/")
end

-- 3rd
local conan_remote = "conan-center"
-- add_requires("conan::fmt/7.1.3", {alias = "fmt", debug = cfg_debug, vs_runtime = cfg_vs_runtime})
add_requires("conan::spdlog/1.8.2", {alias = "spdlog", debug = cfg_debug, vs_runtime = cfg_vs_runtime, configs = { options = "spdlog:wchar_support=True\nspdlog:shared=True", remote = conan_remote}})
add_requires("conan::glfw/3.3.4", {alias = "glfw", debug = cfg_debug, vs_runtime = cfg_vs_runtime, configs = { options = "glfw:shared=True", remote = conan_remote}})
--add_requires("conan::mimalloc/2.0.2", {alias = "mimalloc", debug = cfg_debug, vs_runtime = cfg_vs_runtime, configs = { options = "mimalloc:shared=True" }})
if has_config("with_test") then
    add_requires("conan::gtest/cci.20210126", {alias = "gtest", debug = cfg_debug, vs_runtime = cfg_vs_runtime, configs = { options = "gtest:shared=False", remote = conan_remote}})
end

if has_config("with_benchmark") then
    --add_requires("conan::benchmark/1.6.0", {alias = "benchmark", debug = cfg_debug, vs_runtime = cfg_vs_runtime, configs = { options = "benchmark:shared=False", remote = conan_remote}})
end

-- include project sources
add_includedirs("source")
includes("source")

if has_config("with_test") then
    includes("test")
end

if has_config("with_benchmark") then
    includes("benchmark")
end