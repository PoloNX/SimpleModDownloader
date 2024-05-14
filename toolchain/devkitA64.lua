local triple = "aarch64-unknown-none-elf"

local buildflags = {
    "-march=armv8-a+crc+crypto+simd",
    "-mcpu=cortex-a57",
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

    local DEVKITPRO = os.getenv("DEVKITPRO")
    if not DEVKITPRO then
        DEVKITPRO = "/opt/devkitpro"
        return
    end

    add_linkdirs(path.join(DEVKITPRO, "/libnx/lib"), path.join(DEVKITPRO, "/portlibs/switch/lib"))
    add_includedirs(path.join(DEVKITPRO, "/libnx/include"), path.join(DEVKITPRO, "/portlibs/switch/include"))

    add_links(path.join(DEVKITPRO, "/portlibs/switch/lib/libarchive.a"))
    add_links(path.join(DEVKITPRO, "/portlibs/switch/lib/libzstd.a"))
    add_links(path.join(DEVKITPRO, "/portlibs/switch/lib/liblzma.a"))
    add_links(path.join(DEVKITPRO, "/portlibs/switch/lib/liblz4.a"))
    add_links(path.join(DEVKITPRO, "/portlibs/switch/lib/libbz2.a"))
    add_links(path.join(DEVKITPRO, "/portlibs/switch/lib/libz.a"))

    on_load(function(toolchain)
        toolchain:add("defines", "__SWITCH__", "SWITCH", "DEBUG", "DYG_ENABLE_EVENTS", "HAVE_LIBNX", "STBI_NO_THREAD_LOCALS", "BOREALIS_USE_DEKO3D")
        toolchain:add("arch", "-march=armv8-a+crc+crypto", "-mtune=cortex-a57", "-mtp=soft", "-fPIE")

        toolchain:add("cflags", "-g", "-Wall", "-O2", "-ffunction-sections", "-fdata-sections", {force = true})
        toolchain:add("cxflags", "-march=armv8-a+crc+crypto", "-mtune=cortex-a57", "-mtp=soft", "-fPIE", {force = true})
        toolchain:add("cxxflags", "-frtti", "-fexceptions", {force = true})
        
        toolchain:add("asflags", "-g", "-march=armv8-a+crc+crypto", "-mtune=cortex-a57", "-mtp=soft", "-fPIE", {force = true})
        toolchain:add("ldflags", "-specs=${DEVKITPRO}/libnx/switch.specs", "-g", "-W", "-fPIC","$(notdir $*.map)", {force = true})

        toolchain:add("linkdirs", "${DEVKITPRO}/libnx/lib", "${DEVKITPRO}/portlibs/switch/lib")
        toolchain:add("syslinks", "gcc", "c", "m", "nx")
        toolchain:add("links", "nx", "deko3d")
    end)