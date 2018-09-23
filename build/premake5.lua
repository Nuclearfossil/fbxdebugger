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

    files {
        imguiPath .. "*.h", imguiPath .. "*.cpp",
        srcPath .. "**.h", srcPath .. "**.cpp"
    }

    includedirs {
        imguiPath,
        srcPath,
        glfwPath .. "include",
        glewPath .. "include",
        glmPath
    }

    libdirs { 
        glfwPath .. "lib/static/v140/x64",
        glewPath .. "lib/v110/x64/Release/static"
 }

    filter "configurations:Debug"
        architecture "x86_64"
        links {"d3d11.lib", "d3dcompiler.lib", "dxgi.lib", "opengl32.lib", "glfw3.lib", "glew.lib"}
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release"
        architecture "x86_64"
        links {"d3d11.lib", "d3dcompiler.lib", "dxgi.lib", "opengl32.lib", "glfw3.lib", "glew.lib"}
        defines {"NDEBUG"}
        optimize "On"