# FBX Debugger project

just a sandbox for trying some stuff out.

## Migration from ImGui 1.55 to 1.6 (WIP)

So, a lot of things I ran into that isn't necessarily documented.

Things to note:

- This project uses GLEW and OpenGL.
  - There were a number of significant changes from my old renderer to the one provided with the latest ImGui.
- This project also uses the FBX sdk - this is outside of the scope of this section.

### OpenGL and GLFW

Previously, I had my own GLFW-passthrough to handle (and trap) Mouse and Keyboard events and then pass them through to the ImGui handlers. 

For example, in my previous codebase, I would register the following GLFW callback:

``` C++
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		gAppState->ShouldExit = true;
	}

	int wasCursorKey = -1;
	switch (key)
	{
	case GLFW_KEY_W:
		wasCursorKey = KEY_W;
		break;
	case GLFW_KEY_S:
		wasCursorKey = KEY_S;
		break;
	case GLFW_KEY_A:
		wasCursorKey = KEY_A;
		break;
	case GLFW_KEY_D:
		wasCursorKey = KEY_D;
		break;
	default:
		break;
	}

	if (wasCursorKey != -1)
	{
		gCurrentKeyState.key[wasCursorKey] = true;
		gCurrentKeyState.action[wasCursorKey] = action;		
	}
	gCurrentKeyState.mods = mods;

	// Pass calls on through to ImGui
	ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
}
```

Whereas now we have the inverse, where we get mouse and keyboard events from `ImGui::GetIO` and the GLFW ImGui Implementation (`imgui_impl_glfw.h/.cpp).

``` C++
	// Don't update if the UI has the UI.
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard || io.WantCaptureMouse)
	{
		return;
	}

	UpdateMouse(io);
```

and `UpdateMouse` would look like this:

``` C++
void UpdateMouse(ImGuiIO& io)
{
	gMouseInfo.lmb = ImGui::IsMouseDown(0); // 0 - lmb  1 - mmb  2 - rmb

	gMouseInfo.deltaX = io.MouseDelta.x;
	gMouseInfo.deltaY = io.MouseDelta.y;

	gLastMousePosition = io.MousePos;
}
```

This does not mean that we stop invoking `glfwDestroyWindow`/`glfwTerminate` ourselves, but it does mean we need to invoke other methods.

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

### lumix.h

The current curve editor was pulled from the Lumix engine (https://github.com/nem0/LumixEngine). There used to be a fair number of other functions in there, 
but as I'm not currently using them, I'd rather not maintain them.

## Notes

This project used the FBX sdk - due to file size, you'll need to grab the FBX sdk yourself and put it into extern/fbx.
I've placed that library here: https://drive.google.com/file/d/1e3wGSMajOVD42VWAPAWmcXdrEfkdcaKo/view?usp=sharing

Note that I've already got the includes here - you really only need the lib folder.