local triple = "aarch64-unknown-none-elf"

local buildflags = {
    "-mcpu=cortex-a57",  -- Utiliser uniquement -mcpu
    "-mtune=cortex-a57",
    "-ftls-model=local-exec",
    "-ffunction-sections",
    "-fdata-sections",
    "-fstack-protector-strong",
    "-fPIC",
    "-D__SWITCH__=1",
    "-D__SWITCH=1",
    "-DLIBNX_NO_DEPRECATION",
    "-D_GNU_SOURCE=1",
    "-D_LIBC",
    "-D_NEWLIB_VERSION=4.3.0",
    "-D__NEWLIB__=4"
}

local sharedlinkflags = {
    "-Wl,-Bdynamic",
    "-fPIC",
    "-Wl,--gc-sections",
    "-Wl,-z,text",
    "-Wl,--build-id=sha1",
    "-Wl,--no-dynamic-linker",
    "-Wl,--as-needed",
    "-Wl,--eh-frame-hdr",
    "-fvisibility=hidden"
}

local executablelinkflags = {
    "-Wl,-Bsymbolic",
    "-fPIE",
    "-Wl,-pie",
    "-Wl,--gc-sections",
    "-Wl,-z,text",
    "-Wl,--build-id=sha1",
    "-Wl,--no-dynamic-linker",
    "-Wl,--as-needed",
    "-Wl,--eh-frame-hdr",
    "-fvisibility=hidden",
}

toolchain("devkita64")
    add_defines("__SWITCH__")

    set_kind("standalone")

    local DEVKITPRO = os.getenv("DEVKITPRO")
    if not DEVKITPRO then
        DEVKITPRO = "/opt/devkitpro"
        return
    end

    set_toolset("cc", "aarch64-none-elf-gcc")
    set_toolset("cxx", "aarch64-none-elf-g++")
    set_toolset("ld", "aarch64-none-elf-g++")
    set_toolset("sh", "aarch64-none-elf-g++")
    set_toolset("ar", "aarch64-none-elf-ar")
    set_toolset("strip", "aarch64-none-elf-strip")
    set_toolset("objcopy", "aarch64-none-elf-objcopy")
    set_toolset("ranlib", "aarch64-none-elf-ranlib")
    set_toolset("as", "aarch64-none-elf-gcc")

    add_cxflags(buildflags)
    add_asflags(buildflags)
    add_ldflags(executablelinkflags)
    add_shflags(sharedlinkflags)

    on_check("check")

    on_load(function(toolchain)
        toolchain:add("defines", "SWITCH", "__SWITCH__",  "HAVE_LIBNX")
        toolchain:add("arch", "-mcpu=cortex-a57", "-mtune=cortex-a57", "-mtp=soft", "-fPIE")

        toolchain:add("cflags", "-g", "-Wall", "-O2", "-ffunction-sections", "-fdata-sections", {force = true})
        toolchain:add("cxflags", "-mcpu=cortex-a57", "-mtune=cortex-a57", "-mtp=soft", "-fPIE", {force = true})
        toolchain:add("cxxflags", "-frtti", "-fexceptions", {force = true})
        
        toolchain:add("asflags", "-g", "-mcpu=cortex-a57", "-mtune=cortex-a57", "-mtp=soft", "-fPIE", {force = true})
        toolchain:add("ldflags", "-specs=" .. path.join(DEVKITPRO, "/libnx/switch.specs"), "-g", "-W", "-fPIC","$(notdir $*.map)", {force = true})

        toolchain:add("syslinks", "gcc", "c", "m")
    end)
