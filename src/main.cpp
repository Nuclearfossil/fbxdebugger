// FBX Debugger

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <stdio.h>

// Using GLEW and GLFW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "graphics/Graphics.h"
#include "graphics/Grid.h"

#include "fbxcontainer/scenecontainer.h"

#include "misc/imgui_Samples.h"
#include "basiccamera/FreeLookCamera.h"

#include "misc/debug.h"

#include <thread>
#include <fstream>

basicCamera::FreeLookCamera gCamera;
Grid gGrid;

MouseInfo gMouseInfo;

volatile AppState gAppState;
volatile bool gRegenAssets = false;

SceneContainer* gContainer = nullptr;

static NodeSharedPtr sSelectedNode = nullptr;
const double turnFactorRadsPerSecond = glm::radians(45.0);

void InitApp();

// UI functions
void BeginUI();
void BuildGUI(volatile AppState& state);
void BuildMenu(volatile AppState& state);
void RenderUI(GLFWwindow* window);

void RenderScene(int width, int height);

void DisplaySceneInfo();
void DisplayMeshInfo(volatile AppState& state);
void DisplaySubModel(NodeSharedPtr submodel);

void DisplayAnimationInfo();

void ClearBackground();

void Update(double deltaSeconds);

void ProcessingThread();

Program* gProgram = nullptr;
Program* gWireframe3DProgram = nullptr;
Program* gModelProgram = nullptr;

nlohmann::json gSettings;

int main(int, char**)
{
	if (FileExists("settings.json"))
	{
		std::ifstream settings("settings.json");
		settings >> gSettings;
	}
	else
	{
		gSettings["window"]["width"] = 1500;
		gSettings["window"]["height"] = 960;
	}

	InitApp();

	// Initialize our processing thread
	std::thread processor(ProcessingThread);

    // Setup window
    // glfwSetErrorCallback(glfw_error_callback);
    // if (!glfwInit())
    //     return 1;
	if (!InitGfxSubsystem())
		return -1;

    // GL 3.0 + GLSL 150
	// OLD:
    // const char* glsl_version = "#version 150";
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
	SetWindowHints();

	// OLD:
	// Create window with graphics context
    // GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    // if (window == NULL)
    //     return 1;
    // glfwMakeContextCurrent(window);
    // glfwSwapInterval(1); // Enable vsync
	// 
    // // Initialize OpenGL loader
    // bool err = glewInit() != GLEW_OK;
	// 
    // if (err)
    // {
    //     fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    //     return 1;
    // }

	CreateMainWindow(&gAppState, gSettings);
	
	GetWindow();
	
	InitRenderState();

	gCamera.setPosition(glm::vec3(10.0f, 10.0f, 10.0f));

	LoadShaders(&gProgram, "vertex-shader.txt", "fragment-shader.txt");
	LoadShaders(&gWireframe3DProgram, "grid-vshader.txt", "grid-fshader.txt");
	LoadShaders(&gModelProgram, "model-vshader.txt", "model-fshader.txt");

	gGrid.Initialize(50.0f, 50, 5);

    // Setup Dear ImGui binding
	// OLD:
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// OLD:
    // ImGui_ImplGlfw_InitForOpenGL(window, true);
    // ImGui_ImplOpenGL3_Init(glsl_version);
	UIInit();

    // Setup style
	// OLD:
    //ImGui::StyleColorsDark();
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

	glfwSetTime(0.0);

	double lastFrameTime = glfwGetTime();

	GLFWwindow* window = GetWindow();
    // Main loop
    while (!glfwWindowShouldClose(window) && !gAppState.ShouldExit)
    {
		int width, height;

		double currentTime = glfwGetTime();

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

		Update(currentTime - lastFrameTime);
		lastFrameTime = currentTime;

		UpdateFramebufferSize(width, height);

		// OLD
        // Start the Dear ImGui frame
        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();
		BeginUI();

		BuildGUI(gAppState);

		ShowSampleWindows();

		Clear(window, clear_color);

		RenderScene(width, height);

		RenderUI(window);

		Swap();

		// do we need to regenerate assets?
		if (gRegenAssets)
		{
			gContainer->BuildRenderable();
			gRegenAssets = false;
		}

    }

	// Shut down processing thread
	if (processor.joinable())
	{
		processor.detach();
	}

    // Cleanup
	// OLD:
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();
	// 
    // glfwDestroyWindow(window);
    // glfwTerminate();

	ShutdownGFX();

    return 0;
}

void InitApp()
{
	gAppState.OpenFile = false;
	gAppState.ShouldExit = false;
}

void PrepRender(GLFWwindow* window, ImVec4& clearColor)
{
	int display_w, display_h;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderUI(GLFWwindow* window)
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void BuildMenu(volatile AppState& state)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
				// sSelectedNode = nullptr;
				// sSelectedCurve = nullptr;
				state.OpenFile = true;
			}

			if (ImGui::MenuItem("Quit", "Alt-F4"))
				state.ShouldExit = true;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void BeginUI()
{
	//OLD: ImGui_ImplGlfwGL3_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void BuildGUI(volatile AppState& state)
{
	BuildMenu(state);

	DisplaySceneInfo();
 
	DisplayAnimationInfo();
}

void DisplaySceneInfo()
{
	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::SetWindowSize(ImVec2(600, 400), ImGuiSetCond_FirstUseEver);

	if (gAppState.OpenFile)
	{
		ImGui::Text("Loading FBX");
	}
	else
	{
		DisplayMeshInfo(gAppState);
	}

	ImGui::End();
}

void DisplayMeshInfo(volatile AppState& state)
{
	// Display contents in a scrolling region
	if (gContainer != nullptr)
	{
		ImGui::BeginChild("Scrolling");

		// Display debug on meshes
		int index = 0;
		auto models = gContainer->GetMeshs();
		for each (NodeSharedPtr var in models)
		{
			ModelSharedPtr model = std::dynamic_pointer_cast<Model>(var);
			for each (std::string attribName in model->AttributeNames)
			{
				ImGui::Text("Attribute: %s", attribName.c_str());
			}

			DisplaySubModel(model);
		}

		ImGui::EndChild();
	}
}

void DisplaySubModel(NodeSharedPtr submodel)
{
	ImGui::Indent();
	if (ImGui::Selectable(submodel->Name.c_str(), sSelectedNode == submodel))
		sSelectedNode = submodel;

	int childCount = SizeT2Int32(submodel->Children.size());
	for (int index = 0; index < childCount; index++)
	{
		DisplaySubModel(submodel->Children[index]);
	}

	ImGui::Unindent();
}

void DisplayAnimationInfo()
{
	ImGui::Begin("Animation", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(ImVec2(600, 400), ImGuiSetCond_FirstUseEver);

	if (gAppState.OpenFile)
	{
		ImGui::Text("Loading FBX");
	}
	else
	{
		if (gContainer != nullptr)
		{
			ImGui::BeginChild("Scrolling");

			for each (auto take in gContainer->GetTakeList())
			{
				int layers = take.mLayerInfoList.Size();
				for (int index = 0; index < layers; index++)
				{
					ImGui::Text(take.mLayerInfoList[index]->mName.Buffer());
				}
			}

			for each (auto animStack in gContainer->GetAnimStack())
			{
				if (ImGui::CollapsingHeader(animStack->GetName()))
				{
					unsigned __int32 animLayerCount = SizeT2Int32(animStack->GetMemberCount<FbxAnimLayer>());

					ImGui::Indent();
					for (unsigned __int32 animLayerIndex = 0; animLayerIndex < animLayerCount; animLayerIndex++)
					{
						for each (auto mesh in gContainer->GetMeshs())
						{
							ProcessAnimLayer(mesh->FbxNodeRef, animStack, animLayerIndex);
						}
					}
					ImGui::Unindent();
				}
			}

			ImGui::EndChild();
		}
	}

	ImGui::End();
}

void ProcessingThread()
{
	while (gAppState.ShouldExit == false)
	{
		if (gAppState.OpenFile)
		{
			if (gContainer != nullptr)
			{
				delete gContainer;
				gContainer = nullptr;
			}

			// open the file
			char filename[1024];
			if (OpenFile(filename))
			{
				gContainer = new SceneContainer();
				gContainer->LoadFile(filename);
				gContainer->Process();
				
				gRegenAssets = true;
				gAppState.OpenFile = false;
			}
		}
	}
}

void RenderScene(int width, int height)
{
	// Build out matrices
	glm::mat4 model(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 10000.0f);

	// glm::mat4 camera = glm::lookAt(glm::vec3(575.0f, 575.0f, 575.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 view = gCamera.getView();

	glm::mat4 mvp = projection * view * model;

	glViewport(0, 0, width, height);
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Begin Test");

	glUseProgram(gWireframe3DProgram->object());
	GLuint mvpMatrixID = gWireframe3DProgram->uniform("mvpmatrix");
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, glm::value_ptr(mvp));

	gGrid.Draw();
	glPopDebugGroup();

	if (gContainer != nullptr)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Scene Data Render");
		glUseProgram(gModelProgram->object());
		GLuint mvpMatrixID = gModelProgram->uniform("mvpmatrix");
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, glm::value_ptr(mvp));
		gContainer->RenderAll(sSelectedNode, gModelProgram);
		glUseProgram(0);
		glPopDebugGroup();
	}

	glUseProgram(0);
}

ImVec2 gLastMousePosition;

void UpdateMouse(ImGuiIO& io)
{
	gMouseInfo.lmb = ImGui::IsMouseDown(0); // 0 - lmb  1 - mmb  2 - rmb

	gMouseInfo.deltaX = io.MouseDelta.x;
	gMouseInfo.deltaY = io.MouseDelta.y;

	gLastMousePosition = io.MousePos;
}

void Update(double deltaSeconds)
{
	// Don't update if the UI has the UI.
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard || io.WantCaptureMouse)
	{
		return;
	}

	UpdateMouse(io);

	if (!gMouseInfo.lmb) return;

	KeyState keyState = GetKeystate(io);

	gCamera.addXAngle((float)(gMouseInfo.deltaY / turnFactorRadsPerSecond * deltaSeconds));
	gCamera.addYAngle((float)(gMouseInfo.deltaX / turnFactorRadsPerSecond * deltaSeconds));

	if (keyState.action == GLFW_RELEASE)
	{
		gCamera.right(0.0f);
		gCamera.forward(0.0f);

		return;
	}

	float movement = 10.0f;
	if (keyState.mods == 1)
	{
		movement = 100.f;
	}

	movement *= (float)deltaSeconds;

	if (keyState.key[KEY_A])
	{
		gCamera.right((keyState.action[KEY_A] == GLFW_PRESS) || (keyState.action[KEY_A] == GLFW_REPEAT) ? -movement : 0.0f);
	}

	if (keyState.key[KEY_D])
	{
		gCamera.right((keyState.action[KEY_D] == GLFW_PRESS) || (keyState.action[KEY_D] == GLFW_REPEAT) ? movement : 0.0f);
	}

	if (keyState.key[KEY_W])
	{
		gCamera.forward((keyState.action[KEY_W] == GLFW_PRESS) || (keyState.action[KEY_W] == GLFW_REPEAT) ? movement : 0.0f);
	}

	if (keyState.key[KEY_S])
	{
		gCamera.forward((keyState.action[KEY_S] == GLFW_PRESS) || (keyState.action[KEY_S] == GLFW_REPEAT) ? -movement : 0.0f);
	}
}