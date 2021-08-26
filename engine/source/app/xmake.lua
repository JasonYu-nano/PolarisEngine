target("App")
    if has_config("shared") then
        set_kind("shared")
        add_defines("APP_EXPORT")
    else
        set_kind("static")
    end
    set_group("Engine")
    add_files("**.cpp")
    add_headerfiles("**.hpp")
    add_deps("Core")
    add_packages("glfw", {public = true})
    add_includedirs("$(projectdir)/source/app", {public = true})
target_end()