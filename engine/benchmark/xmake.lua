target("BM")
    set_kind("binary")
    set_group("Benchmark")
    add_files("**.cpp")
    add_headerfiles("**.hpp")
    add_deps("Core")

    local bm_path = "$(projectdir)/third_parties/benchmark"
    add_includedirs(bm_path .. "/include")
    if is_mode("debug") then
        add_linkdirs(bm_path .. "/debug")
    else
        add_linkdirs(bm_path .. "/release")
    end
    add_links("benchmark")
    add_links("benchmark_main")
    add_syslinks("Shlwapi")
    add_includedirs("$(projectdir)/benchmark", {public = false})
target_end()