solution "RayTracer"
    language "C++"
    flags { "StaticRuntime" }
    
    location "."
    debugdir "."
    includedirs {
        "../Engine/Code/",
        "./Code/"
    }
    defines { "_UNICODE" }
    
    
    
    -- CONFIGURATIONS ------------------------
    configurations { "Debug", "Release" }
    
    configuration "Debug"
        defines { "BUILD_DEBUG", "_DEBUG" }
        objdir "./Build/Obj/Debug/"
        flags { "Symbols" }
        targetsuffix ".Debug"

    configuration "Release"
        defines { "BUILD_RELEASE" }
        objdir "./Build/Obj/Release/"
        flags { "Optimize" }



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
            "Engine"
        }
    
    
    
    -- ENGINE -------------------------------
    project "Engine"
        kind "StaticLib"
        
        location "./Build/Projects/"
        targetdir "./Bin/"
        
        files {
            "../Engine/Code/**.h",
            "../Engine/Code/**.cpp",
            "../Engine/Code/**.inl",
        }
        vpaths {
            ["*"] = { "../Engine/Code/**" },
        }
        excludes {
            "../Engine/Code/Object/**",
            "../Engine/Code/External/**",
            "../Engine/Code/Hash/**",
        }

    -- TODO: need to figure out the error this causes
    -- [string "premake = { }..."]:74: attempt to concatenate local 'fname' (a table value)
    --include { "../Engine/" }