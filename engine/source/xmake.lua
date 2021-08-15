if is_plat("windows") then
    local engine_dir = os.curdir()
    engine_dir = string.gsub(engine_dir, "\\", "\\\\")
    if has_config("unicode") then
        add_defines(string.format("ENGINE_ROOT_PATH=L\"%s\"", engine_dir))
    else
        add_defines(string.format("ENGINE_ROOT_PATH=\"%s\"", engine_dir))
    end
else
    add_defines("ENGINE_ROOT_PATH=\"$(curdir)\"")
end

includes("app")
includes("core")
includes("render")