workspace "keynes2"
    platforms {
--        "x64",
        "x32"
    }

    warnings "Default"

    configurations {
        "Debug",
        "Release",
        "Distribute",
        "Profile"
    }

    staticruntime "on"
    language "C++"
    cppdialect "C++17"
    largeaddressaware "on"
    location "build"

    ROOT = _WORKING_DIR

    filter "platforms:*32"
        architecture "x86"
    filter "platforms:*64"
        architecture "x64"

    filter {}

    includedirs {
        "share",
        "extern/include",
        "extern/include/d3d8",
        "extern/include/python",
        "extern/include/tracy"
    }

    libdirs {
        "extern/lib",
        "extern/lib/%{cfg.system}/%{cfg.platform}",
        "extern/lib/%{cfg.system}/%{cfg.platform}/%{cfg.runtime}"
    }

    local os_ver = os.getversion()
    local hostname = string.format("__HOSTNAME__=\" %d.%d.%d (%s)\"", os_ver.majorversion, os_ver.minorversion, os_ver.revision, os_ver.description)
    local target_os = string.format("__TARGET_OS__=\"%s\"", os.target())
    local working_dir = string.format("__WORKING_DIR__=\"%s\"", _WORKING_DIR)
    local premake_ver = string.format("__PREMAKE_VERSION__=\"%s\"", _PREMAKE_VERSION)

    defines {
        "__BUILD_TYPE__=\"%{cfg.buildcfg}\"",
        "__TOOLSET__=\"%{cfg.toolset}\"",
        "__ARCHITECTURE__=\"%{cfg.architecture}\"",
        hostname,
        target_os,
        working_dir,
        premake_ver,
        "FMT_HEADER_ONLY",
    }

    filter "configurations:Debug"
        defines {
            "_DEBUG",
        }

        runtime "Debug"
        symbols "Full"

    filter "configurations:Release"
        defines {
            "NDEBUG",
            "ENABLE_FILES_CHECK",
        }

        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Profile"
        defines {
            "NDEBUG",
            "FBE_LOGGING",
            "TRACY_ENABLE"
        }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Distribute"
        defines {
            "NDEBUG",
            "ENABLE_FILES_CHECK",
            "DISTRIBUTE",
        }

        runtime "Release"
        optimize "On"
        symbols "On"

    filter "action:vs*"
        systemversion "latest"
        atl ("Static")

        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "_HAS_STD_BYTE=0",
        }

        disablewarnings {
            "4242",
            "4307",
            "4996",
            "4244",
            "4267",
        }

        characterset "MBCS"

        flags {
            "MultiProcessorCompile",
            "NoBufferSecurityCheck",
            "NoRuntimeChecks",
            "RelativeLinks",
        }

    filter { "platforms:*32", "action:vs*"}
        defines { "_USE_32BIT_TIME_T" }
    filter {}

    if os.istarget("windows") then
        group "client"
            defines {
                "IS_CLIENT",
            }

            project "EffectLib"
                kind "StaticLib"
                removeplatforms { "x64" }
                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "EterBase"
                kind "StaticLib"
                removeplatforms { "x64" }
                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "EterPack"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.hpp",
                    "%{prj.group}/%{prj.name}/**.cpp",
                    "%{prj.group}/%{prj.name}/**.asm",
                    "%{prj.group}/%{prj.name}/**.inc"
                }

            project "EterGrnLib"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "EterImageLib"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "EterLib"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "EterLocale"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "EterPythonLib"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "GameLib"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "MilesLib"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "Stdafx.h"
                pchsource "%{prj.group}/%{prj.name}/Stdafx.cpp"

            project "PRTerrainLib"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "ScriptLib"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "SpeedTreeLib"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "SphereLib"
                kind "StaticLib"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

            project "UserInterface"
                kind "WindowedApp"
                targetdir (ROOT .. "/bin")
                targetname "keynes2"
                targetsuffix "_%{cfg.architecture}_%{cfg.buildcfg}"
                removeplatforms { "x64" }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp",
                    "%{prj.group}/%{prj.name}/**.rc",
                    "%{prj.group}/%{prj.name}/**.ico"
                }

                pchheader "StdAfx.h"
                pchsource "%{prj.group}/%{prj.name}/StdAfx.cpp"

                ignoredefaultlibraries { "libci" }

                links {
                    "EffectLib",
                    "EterBase",
                    "EterPack",
                    "EterGrnLib",
                    "EterImageLib",
                    "EterLib",
                    "EterLocale",
                    "EterPythonLib",
                    "GameLib",
                    "MilesLib",
                    "PRTerrainLib",
                    "ScriptLib",
                    "SpeedTreeLib",
                    "SphereLib",
                    "Crypt32"
                }

                linkoptions {
                    "/ignore:4098",
                    "/ignore:4099",
                }

                configuration {
                    "Debug",
                    "vs*"
                }
                    postbuildcommands {
                        "{COPYFILE} %{cfg.buildtarget.abspath} ..\\..\\client\\%{cfg.targetname}%{cfg.buildtarget.extension}"
                    }

                configuration {
                    "Release or Distribute or Profile",
                    "vs*"
                }
                    linkoptions {
                        "/SAFESEH:NO"
                    }

                    flags {
                        "LinkTimeOptimization",
                        "NoIncrementalLink",
                        "NoImplicitLink",
                    }

                configuration {
                    "Release",
                    "vs*"
                }
                    postbuildcommands {
                        "{COPYFILE} %{cfg.buildtarget.abspath} ..\\..\\client\\%{cfg.targetname}%{cfg.buildtarget.extension}"
                    }

                configuration {
                    "Distribute",
                    "vs*"
                }
                    postbuildcommands {
                        "{COPYFILE} %{cfg.buildtarget.abspath} ..\\..\\client\\%{cfg.targetname}%{cfg.buildtarget.extension}"
                    }
                    linkoptions {
                        "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
                    }
    end

    if not os.istarget("windows") then
        group "server"
            defines {
                "IS_SERVER",
            }

            project "libthecore"
                kind "StaticLib"

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                filter "action:not vs*"
                    disablewarnings {
                        "invalid-source-encoding",
                    }
                    pchheader "%{prj.group}/%{prj.name}/stdafx.h"
                filter "action:vs*"
                    pchheader "stdafx.h"
                    pchsource "%{prj.group}/%{prj.name}/stdafx.cpp"

            project "libsql"
                kind "StaticLib"

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                filter "action:not vs*"
                    pchheader "%{prj.group}/%{prj.name}/stdafx.h"
                filter "action:vs*"
                    pchheader "stdafx.h"
                    pchsource "%{prj.group}/%{prj.name}/stdafx.cpp"

            project "libpoly"
                kind "StaticLib"

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                filter "action:not vs*"
                    pchheader "%{prj.group}/%{prj.name}/stdafx.h"
                filter "action:vs*"
                    pchheader "stdafx.h"
                    pchsource "%{prj.group}/%{prj.name}/stdafx.cpp"

            project "liblua"
                kind "StaticLib"
                language "C"
                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.c",
                }

            project "libgame"
                kind "StaticLib"

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                filter "action:not vs*"
                    pchheader "%{prj.group}/%{prj.name}/stdafx.h"
                filter "action:vs*"
                    pchheader "stdafx.h"
                    pchsource "%{prj.group}/%{prj.name}/stdafx.cpp"

            project "quest"
                kind "ConsoleApp"
                targetdir (ROOT .. "/bin")
                targetname "qc"
                targetsuffix "_%{cfg.architecture}_%{cfg.buildcfg}"

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cc"
                }

                links {
                    "liblua",
                }

            project "db"
                kind "ConsoleApp"
                targetdir (ROOT .. "/bin")
                targetsuffix "_%{cfg.architecture}_%{cfg.buildcfg}"

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                links {
                    "libgame",
                    "libpoly",
                    "libsql",
                    "libthecore",
                }

                filter "action:not vs*"
                    pchheader "%{prj.group}/%{prj.name}/stdafx.h"
                filter "action:vs*"
                    pchheader "stdafx.h"
                    pchsource "%{prj.group}/%{prj.name}/stdafx.cpp"

                filter "system:bsd"
                    links { "pthread" }

                filter "system:windows"
                    links {
                        "ws2_32",
                        "shlwapi",
                        "mariadbclient"
                    }
                    postbuildcommands {
                        "{COPYFILE} %{cfg.buildtarget.abspath} %{cfg.buildtarget.directory}\\%{prj.name}%{cfg.buildtarget.extension}"
                    }
                filter {}

                links {
                    "mariadbclient"
                }

            project "game"
                kind "ConsoleApp"
                targetdir (ROOT .. "/bin")
                targetsuffix "_%{cfg.architecture}_%{cfg.buildcfg}"

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.hpp",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                links {
                    "libgame",
                    "libpoly",
                    "libsql",
                    "libthecore",
                    "liblua",
                }

                filter "configurations:Release"
                    defines {
                        "ENABLE_FILES_CHECK",
                    }

                filter "action:not vs*"
                    pchheader "%{prj.group}/%{prj.name}/stdafx.h"

                filter "action:vs*"
                    pchheader "stdafx.h"
                    pchsource "%{prj.group}/%{prj.name}/stdafx.cpp"

                filter "system:bsd"
                    links {
                        "pthread",
                        "IL",
                        "png16",
                        "jasper",
                        "jpeg",
                        "jbig",
                        "tiff",
                        "lzma",
                        "md",
                        "z",
                        "sodium",
                        "cryptopp",
                    }

                    disablewarnings {
                        "invalid-source-encoding",
                    }

                filter "system:windows"
                    links {
                        "ws2_32",
                        "shlwapi",
                        "DevIL",
                        "cryptopp-static",
                        "libsodium"
                    }

                    linkoptions {
                        "/ignore:4098",
                        "/ignore:4099"
                    }

                    postbuildcommands {
                        "{COPYFILE} %{cfg.buildtarget.abspath} %{cfg.buildtarget.directory}\\%{prj.name}%{cfg.buildtarget.extension}"
                    }

                filter {}
                links {
                    "mariadbclient",
                    "lzo2",
                }
    end

    group "tools"
        if os.istarget("windows") then
            project "FoxFSArchiver"
                kind "ConsoleApp"
                targetdir (ROOT .. "/bin")
                targetsuffix "_%{cfg.architecture}_%{cfg.buildcfg}"
                removeplatforms { "x64" }

                includedirs {
                    "extern/tools/include"
                }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                defines {
                    "TBB_SUPPRESS_DEPRECATED_MESSAGES"
                }

                links {
                    "EterPack",
                    "lzo2",
                    "liblz4_static",
                    "cryptopp-static"
                }

                linkoptions {
                    "/ignore:4098",
                    "/ignore:4099"
                }
 
                postbuildcommands {
                    "{COPYFILE} %{cfg.buildtarget.abspath} ..\\..\\tools\\FoxFSArchiver\\bin\\FoxFSArchiver.exe"
                }

            project "Mysql2Proto"
                kind "ConsoleApp"
                targetdir (ROOT .. "/bin")
                targetsuffix "_%{cfg.architecture}_%{cfg.buildcfg}"
                removeplatforms { "x64" }

                includedirs {
                    "extern/tools/include"
                }

                libdirs {
                    "extern/tools/lib/%{cfg.system}/%{cfg.platform}"
                }

                files {
                    "%{prj.group}/%{prj.name}/**.h",
                    "%{prj.group}/%{prj.name}/**.cpp"
                }

                links {
                    "EterBase",
                    "lzo2",
                }

                linkoptions {
                    "/ignore:4098",
                    "/ignore:4099"
                }

                postbuildcommands {
                    "{COPYFILE} %{cfg.buildtarget.abspath} ..\\..\\tools\\Mysql2Proto\\Mysql2Proto.exe"
                }
        end
