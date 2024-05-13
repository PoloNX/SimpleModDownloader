add_repositories("switch-repo https://github.com/PoloNX/switch-repo.git")
add_repositories("zeromake-repo https://github.com/zeromake/xrepo.git")


includes("toolchain/*.lua")

add_defines(
    'BRLS_RESOURCES="romfs:/"',
    "YG_ENABLE_EVENTS"
)

add_rules("mode.debug", "mode.release")

add_requires("borealis", {repo = "switch-repo"}, "libcurl", "libarchive", "bzip2", "zlib", "liblzma", "lz4", "libexpat", "libzstd")

target("SimpleModDownloader")
    set_kind("binary")
    if not is_plat("switch") then 
        return
    end

    set_arch("aarch64")
    add_rules("switch")
    set_toolchains("devkita64")
    set_languages("c++17")
    
    set_values("switch.name", "SimpleModDownloader")
    set_values("switch.author", "PoloNX")
    set_values("switch.version", "2.1.0")
    set_values("switch.romfs", "resources")

    -- SimpleIniParser
    add_files("lib/ini/source/SimpleIniParser/*.cpp")
    add_includedirs("lib/ini/include")
    add_includedirs("lib/ini/include/SimpleIniParser")

    add_files("source/**.cpp")
    add_includedirs("include")
    add_packages("borealis", "libcurl", "libarchive", "bzip2", "zlib", "liblzma", "lz4", "libexpat", "libzstd")
