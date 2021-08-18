target("Render")
    if has_config("shared") then
        set_kind("shared")
        add_defines("RENDER_EXPORT")
    else
        set_kind("static")
    end
    set_group("Engine")
    add_files("**.cpp")
    add_headerfiles("**.hpp")
    add_deps("Core")
    add_packages("glfw", {public = true})
    add_includedirs("$(projectdir)/source/render", {public = true})

    local vulkan_path = os.getenv("VULKAN_SDK")
    add_includedirs(vulkan_path .. "/Include", {public = true})
    add_linkdirs(vulkan_path .. "/Lib", {public = true})
    add_links("vulkan-1", {public = true})
target_end()