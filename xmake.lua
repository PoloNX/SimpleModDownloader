add_repositories("zeromake-repo https://github.com/zeromake/xrepo.git")
add_repositories("switch-repo https://github.com/PoloNX/switch-repo.git")

includes("toolchain/*.lua")

add_defines(
    'BRLS_RESOURCES="romfs:/"',
    "YG_ENABLE_EVENTS"
)

local function getVersion(version)
    local versions ={
        ["2024.04.23-alpha"] = "archive/ae9b053ce527faaef5356d1acfd4f7a392604451.tar.gz",
    }
    return versions[tostring(version)]
end

package("borealis")
    set_homepage("https://github.com/PoloNX/borealis")
    set_description("Hardware accelerated, Nintendo Switch inspired UI library for PC, Android, iOS, PSV, PS4 and Nintendo Switch")
    set_license("Apache-2.0")


    set_urls("https://github.com/PoloNX/borealis/$(version)", {
        version = getVersion
    })
    add_versions("2024.04.23-alpha", "f1dde726c122af4a40941ce8e0b27655eda9b0bc6e80d4e9034f5c7978b3e288")

    add_configs("window", {description = "use window lib", default = "nanovg", type = "string"})
    add_configs("driver", {description = "use driver lib", default = "opengl", type = "string"})
    add_configs("winrt", {description = "use winrt api", default = false, type = "boolean"})
    add_deps(
        "yoga =2.0.1",
        "nlohmann_json",
        "fmt",
        "tweeny",
        "stb",
        "tinyxml2"
    )
    add_includedirs("include")
    if is_plat("windows") then
        add_includedirs("include/compat")
        add_syslinks("wlanapi", "iphlpapi", "ws2_32")
    end
    on_load(function (package)
        local window = package:config("window")
        local driver = package:config("driver")
        local winrt = package:config("winrt")
        if window == "glfw" then
            package:add("deps", "glfw")
        elseif window == "sdl" then
            package:add("deps", "sdl2")
        elseif window == "nanovg" then 
            package:add("deps", "nanovg")
        end
        if driver == "opengl" then
            --package:add("deps", "glad =0.1.36")
        elseif driver == "d3d11" then
            package:add("syslinks", "d3d11")
        end
        if winrt then
            package:add("syslinks", "windowsapp")
        end
    end)
    on_install(function (package)
        os.cp(path.join(os.scriptdir(), "port", "xmake.lua"), "xmake.lua")
        local configs = {}
        configs["window"] = package:config("window")
        configs["driver"] = package:config("driver")
        configs["winrt"] = package:config("winrt") and "y" or "n"
        import("package.tools.xmake").install(package, configs)
        os.cp("library/include/*", package:installdir("include").."/")
        os.rm(package:installdir("include/borealis/extern"))
        os.cp("library/include/borealis/extern/libretro-common", package:installdir("include").."/")
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <borealis.hpp>

            static void test() {
                volatile void* i = (void*)&brls::Application::init;
                if (i) {};
            }
        ]]}, {
            configs = {languages = "c++20", defines = { "BRLS_RESOURCES=\".\"" }},
        }))
    end)
package_end()

add_rules("mode.debug", "mode.release")

add_requires("borealis", "libcurl", "libarchive")

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
    add_packages("borealis", "libcurl", "libarchive")
