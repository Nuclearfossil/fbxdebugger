-- premake5.lua

workspace "FbxDebugger"
    configurations { "Debug", "Release"}
    location "sln"
    basedir(basePath)

project "Viewer"
    location "sln/viewer"
    kind "ConsoleApp"
    language "C++"
    system "windows"
    targetdir "targets/%{cfg.buildcfg}"

    local basePath = path.normalize(path.getabsolute("../",os.realpath("./")))    
    local glewPath = path.join(basePath, "extern/glew")
    local srcPath = path.join(basePath, "src/")
    local imguiPath = path.join(basePath, "extern/imgui/")
    local glfwPath = path.join(basePath, "extern/glfw/")
    local glewPath = path.join(basePath, "extern/glew/")
    local glmPath = path.join(basePath, "extern/glm/")
    local fbxPath = path.join(basePath, "extern/FBX/")
    local shaderPath = path.join(basePath, "resources/")
    local buildPath = path.join(basePath, "build/")

    files {
        imguiPath .. "/*.h", imguiPath .. "/*.cpp",
        srcPath .. "/**.h", srcPath .. "/**.cpp",
        shaderPath .. "/**.hlsl",
        buildPath .. "/*.lua",
        "../README.md"
    }

    includedirs {
        imguiPath,
        srcPath,
        glfwPath .. "/include",
        glewPath .. "/include",
        glmPath,
        fbxPath .. "/include"
    }

    filter { "files:**.hlsl" }
        flags "ExcludeFromBuild"
        shadermodel "5.0"

    filter { "files:**.lua" }
        flags "ExcludeFromBuild"

    filter "configurations:Debug"
        architecture "x86_64"
        links {"d3d11.lib", "d3dcompiler.lib", "dxgi.lib", "opengl32.lib", "glfw3.lib", "glew32sd.lib", "libfbxsdk-md.lib"}
        defines {"DEBUG"}
        symbols "On"
        libdirs {
            glfwPath .. "/lib/static/v140/x64",
            glewPath .. "/lib/Debug/x64",
            fbxPath .. "/lib/vs2015/x64/Debug"
        }
        postbuildcommands { "xcopy $(SolutionDir)..\\..\\resources $(TargetDir)resources\\* /s /e /y" }
        debugdir "$(TargetDir)"

    filter "configurations:Release"
        architecture "x86_64"
        links {"d3d11.lib", "d3dcompiler.lib", "dxgi.lib", "opengl32.lib", "glfw3.lib", "glew32s.lib", "libfbxsdk-md.lib"}
        defines {"NDEBUG"}
        optimize "On"
        libdirs {
            glfwPath .. "/lib/static/v140/x64",
            glewPath .. "/lib/Release/x64",
            fbxPath .. "/lib/vs2015/x64/Release"
        }
        postbuildcommands { "xcopy $(SolutionDir)..\\..\\resources $(TargetDir)resources\\* /s /e /y" }
        debugdir "$(TargetDir)"
