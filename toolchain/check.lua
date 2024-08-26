-- import("core.project.config")
-- import("lib.detect.find_tool")

function main(toolchain)
    if not is_plat("cross") then
        cprint("Use cross platform")
        return
    end

    local paths
    local devkitpro = os.getenv("DEVKITPRO")
    local devkita64 = path.join(devkitpro, "/devkitA64")
    if not os.isdir(devkita64) then 
        return false
    end

    toolchain:config_set("devkita64", devkita64)
    toolchain:config_set("bindir", path.join(devkita64, "bin"))
    return true
end