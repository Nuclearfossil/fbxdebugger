-- premake5.lua

workspace "FbxDebugger"
    configurations { "Debug", "Release"}
    location "sln"

project "Viewer"
    location "sln/viewer"
    kind "ConsoleApp"
    language "C++"
    system "windows"
    targetdir "targets/%{cfg.buildcfg}"

    local srcPath = "../src/"
    local imguiPath = "../extern/imgui/"
    local glfwPath = "../extern/glfw/"
    local glewPath = "../extern/glew/"
    local glmPath = "../extern/glm/"
    local fbxPath = "../extern/FBX/"
    local shaderPath = "../resources/"
    local buildPath = "../build/"

    files {
        imguiPath .. "*.h", imguiPath .. "*.cpp",
        srcPath .. "**.h", srcPath .. "**.cpp",
        shaderPath .. "**.hlsl",
        buildPath .. "*.lua",
        "../README.md"
    }

    includedirs {
        imguiPath,
        srcPath,
        glfwPath .. "include",
        glewPath .. "include",
        glmPath,
        fbxPath .. "include"
    }

    filter "configurations:Debug"
        architecture "x86_64"
        links {"d3d11.lib", "d3dcompiler.lib", "dxgi.lib", "opengl32.lib", "glfw3.lib", "glew32sd.lib", "libfbxsdk-md.lib"}
        defines {"DEBUG"}
        symbols "On"
        libdirs {
            glfwPath .. "lib/static/v140/x64",
            glewPath .. "lib/Debug/x64",
            fbxPath .. "lib/vs2015/x64/Debug"
        }
        postbuildcommands { "xcopy $(SolutionDir)..\\..\\resources $(TargetDir)resources\\* /s /e /y" }
        debugdir "$(TargetDir)"

    filter "configurations:Release"
        architecture "x86_64"
        links {"d3d11.lib", "d3dcompiler.lib", "dxgi.lib", "opengl32.lib", "glfw3.lib", "glew32s.lib", "libfbxsdk-md.lib"}
        defines {"NDEBUG"}
        optimize "On"
        libdirs {
            glfwPath .. "lib/static/v140/x64",
            glewPath .. "lib/Release/x64",
            fbxPath .. "lib/vs2015/x64/Release"
        }
        postbuildcommands { "xcopy $(SolutionDir)..\\..\\resources $(TargetDir)resources\\* /s /e /y" }
        debugdir "$(TargetDir)"
