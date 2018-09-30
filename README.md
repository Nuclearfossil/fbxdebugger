# FBX Debugger project

just a sandbox for trying some stuff out.

## Migration from ImGui 1.55 to 1.5

So, a lot of things I ran into that isn't necessarily documented.

Things to note:

- This project uses GLEW and OpenGL.
  - There were a number of significant changes from my old renderer to the one provided with the latest ImGui.
- This project also uses the FBX sdk - this is outside of the scope of this section.

### OpenGL and GLEW

Previously, I had my own GLEW-passthrough to handle (and trap) ImGui events so that we don't have keyboard/mouse events
pass through to keep from moving the camera when the mouse is clicked in the ImGui window. Now we have the inverse, where
we get mouse and keyboard events from the GLFW ImGui Implementation (`imgui_impl_glfw.h/.cpp).

This does not mean that we still don't invoke `glfwDestroyWindow`/`glfwTerminate` ourselves, but it does mean we need to invoke
other methods.

Multiple function signature changes, for example (but not limited to):

- `ImGui_ImplGlfwGL3_NewFrame()`/`ImGui_ImplGlfw_NewFrame()`
- `ImGui_ImplGlfwGL3_Shutdown`/`ImGui_ImplGlfw_Shutdown`

### What feels like undocumented changes

I've seen a number of methods that have been deprecated and doesn't feel like it's been properly documented. I've made a number of assumtpions, but that is exactly what they are: assumptions.

- `ImGuiContext` - `FocusedWindow` feels like it's been changed to `NavWindow`
- `ImGuiContext` - `OsImePosRequest` feels like it's been changed to `PlatformImePos`
- `ImGuiWindow` - `WindowRectClipped` feels like it's been changed to `OuterRectClipped`
- On of the widgets I'm porting used `IsItemHovered` but it doesn't feel like there is an appropriate alternative, so I've had to pull in the replacement.

You  can see the last point illustrated in lumix.h:

``` C++
    // if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsItemHovered())
    // This also does not work
    // if (ImGui::GetIO().MouseWheel != 0 && IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
    // And neither does this
    // if (ImGui::GetIO().MouseWheel != 0 && IsItemHovered())
```

## Notes

This project used the FBX sdk - due to file size, you'll need to grab the FBX sdk yourself and put it into extern/fbx.
I've placed that library here: https://drive.google.com/file/d/1e3wGSMajOVD42VWAPAWmcXdrEfkdcaKo/view?usp=sharing

Note that I've already got the includes here - you really only need the lib folder.