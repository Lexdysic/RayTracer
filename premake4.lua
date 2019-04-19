solution "RayTracer"
    language "C++"
    flags { "StaticRuntime", "Unicode" }
    
    location "."
    debugdir "."
    includedirs {
        "../Ferrite/Code/",
        "./Code/"
    }
    defines { "_UNICODE" }
    
    -- WindowsSDK
    configuration { "vs2017"}
        windowstargetplatformversion(string.gsub(os.getenv("WindowsSDKVersion") or "10.0.16299.0", "\\", ""))
    configuration {}
    
    
    -- CONFIGURATIONS ------------------------
    configurations { "Debug", "Release" }
    platforms { "x32", "x64" }
    
    configuration "Debug"
        defines { "BUILD_DEBUG", "_DEBUG" }
        objdir "./Build/Obj/Debug/"
        flags { "Symbols" }
        targetsuffix ".Debug"

    configuration "Release"
        defines { "BUILD_RELEASE" }
        objdir "./Build/Obj/Release/"
        flags { "Symbols", "Optimize" }



    -- GAME ----------------------------------
    project "RayTracer"
        kind "ConsoleApp"
        -- flags { "WinMain" }
        
        location "./Build/Projects/"
        targetdir "./Bin/"
        
        files {
            "./Code/**.h",
            "./Code/**.cpp",
            "./Code/**.inl",
        }
        vpaths {
            ["*"] = "./Code/**"
        }
        
        links {
            "Ferrite"
        }
    
    
    
    -- ENGINE -------------------------------

    include "../Ferrite"