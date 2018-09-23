//#include "Shader.h"
//
//#include "graphics\Program.h"

#include "misc\Utils.h"
#include "misc\json.h"

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <vector>

#include "Graphics.h"

static void ErrorCallback(int error, const char* description);
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void CharCallback(GLFWwindow* window, unsigned int c);

void UpdateMousePos();

GLFWwindow* gWindow = nullptr;
volatile AppState* gAppState = nullptr;

MouseInfo* gMouseInfo = nullptr;

KeyState gCurrentKeyState;

double mouseX = 0.0;
double mouseY = 0.0;

nlohmann::json* gSettings = nullptr;
const char* glsl_version = "#version 150";

void SetupWindow(GLFWwindow* window)
{
	gWindow = window;
}

bool InitGfxSubsystem()
{
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit())
		return false;

	return true;
}

void InitRenderState()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

KeyState GetKeystate() { return gCurrentKeyState; }

void SetMouseInfo(MouseInfo* info)
{
	gMouseInfo = info;
}

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
	if (gSettings != nullptr)
	{
		(*gSettings)["window"]["width"] = width;
		(*gSettings)["window"]["height"] = height;
	}
}

void BindInternalHandlers()
{
	glfwGetCursorPos(gWindow, &mouseX, &mouseY);

	glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
	glfwSetScrollCallback(gWindow, ScrollCallback);
	glfwSetCharCallback(gWindow, CharCallback);
}

bool CreateMainWindow(volatile AppState* state, nlohmann::json& settings)
{
	gSettings = &settings;
	auto window = (*gSettings)["window"];

	gWindow = glfwCreateWindow(window["width"], window["height"], "FBX Viewer", nullptr, nullptr);
	if (!gWindow)
	{
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(gWindow, WindowSizeCallback);

	glfwMakeContextCurrent(gWindow);
	glfwSetKeyCallback(gWindow, KeyCallback);

	glewExperimental = GL_TRUE;
	glewInit();

	// print out some info about the graphics drivers
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	gAppState = state;
	BindInternalHandlers();
	return true;
}

bool WindowShouldClose()
{
	return glfwWindowShouldClose(gWindow);
}

void PollEvents()
{
	glfwPollEvents();
}

void UpdateFramebufferSize(int &width, int &height)
{
	glfwGetFramebufferSize(gWindow, &width, &height);
}

void ClearBackground()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void Swap()
{
	glfwSwapBuffers(gWindow);

	UpdateMousePos();
}

void ShutdownGFX()
{
	// OLD:
	// glfwDestroyWindow(gWindow);
	// glfwTerminate();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(gWindow);
	glfwTerminate();

	gWindow = nullptr;
}

void SetWindowHints()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
}

void UIInit()
{
	// OLD:
	// ImGui_ImplGlfwGL3_Init(gWindow, false); // We handle UI callbacks elsewhere.

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	ImGui_ImplGlfw_InitForOpenGL(gWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
}

void UINewFrame()
{
	// OLD:
	// ImGui_ImplGlfwGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void UIShutdown()
{
	// OLD:
	//	ImGui_ImplGlfwGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

//void LoadShaders(Program** program, const char* vertexShaderFile, const char* fragmentShaderFile)
//{
//	std::vector<Shader> shaders;
//	shaders.push_back(Shader::shaderFromFile(ResourcePath(vertexShaderFile), GL_VERTEX_SHADER));
//	shaders.push_back(Shader::shaderFromFile(ResourcePath(fragmentShaderFile), GL_FRAGMENT_SHADER));
//
//	*program = new Program(shaders);
//}

static void ErrorCallback(int error, const char* description)
{
	fputs(description, stderr);
}

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
	// OLD:
	// ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	// OLD:
	// ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

static void CharCallback(GLFWwindow* window, unsigned int c)
{
	// OLD:
	// ImGui_ImplGlfwGL3_CharCallback(window, c);
	ImGui_ImplGlfw_CharCallback(window, c);
}

void UpdateMousePos()
{
	if (gMouseInfo == nullptr) return;

	double xpos, ypos;
	glfwGetCursorPos(gWindow, &xpos, &ypos);

	gMouseInfo->deltaX = mouseX - xpos;
	gMouseInfo->deltaY = mouseY - ypos;

	mouseX = xpos;
	mouseY = ypos;
}

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (gMouseInfo == nullptr) return;

	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		gMouseInfo->lmb = (action == GLFW_PRESS);
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		gMouseInfo->mmb = (action == GLFW_PRESS);
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		gMouseInfo->rmb = (action == GLFW_PRESS);
		break;
	default:
		gMouseInfo->lmb = false;
		gMouseInfo->mmb = false;
		gMouseInfo->rmb = false;
		break;
	}

	// OLD:
	// ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}
