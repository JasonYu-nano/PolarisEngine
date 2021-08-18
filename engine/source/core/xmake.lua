target("Core")
    if has_config("shared") then
        set_kind("shared")
        add_defines("CORE_EXPORT")
    else
        set_kind("static")
    end
    set_group("Engine")
    set_pcxxheader("precompiled_core.hpp")
    add_files("**.cpp")
    add_headerfiles("**.hpp")
    add_headerfiles("**.inl")
    add_packages("spdlog", {public = true})
    add_includedirs("$(projectdir)/source/core", {public = true})
target_end()