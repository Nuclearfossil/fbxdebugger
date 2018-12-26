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

#include "gui/scenegraph.h"
#include "gui/ImTimeline.h"

#include "animation/anim.h"
#include "misc/animtool.h"

#include <thread>
#include <fstream>

#include <string>
#include <sstream>

basicCamera::FreeLookCamera gCamera;
Grid gGrid;
Anim* gAnim = nullptr;

MouseInfo gMouseInfo;

volatile AppState gAppState;
volatile bool gRegenAssets = false;

SceneContainer* gContainer = nullptr;
VisualSceneGraph gSceneGraph;

ImVec2 gLastMousePosition;

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
void DisplayChildMesh(SceneNodeSharedPtr node);
void DisplayMeshInfo(volatile AppState& state);
void DisplaySubModel(NodeSharedPtr submodel);

void DisplayAnimationInfo();

void DisplayTestWindow();

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
	if (!InitGfxSubsystem())
		return -1;

	SetWindowHints();

	CreateMainWindow(&gAppState, gSettings);

	GetWindow();

	InitRenderState();

	gCamera.setPosition(glm::vec3(10.0f, 10.0f, 10.0f));

	LoadShaders(&gProgram, "vertex-shader.hlsl", "fragment-shader.hlsl");
	LoadShaders(&gWireframe3DProgram, "grid-vshader.hlsl", "grid-fshader.hlsl");
	LoadShaders(&gModelProgram, "model-vshader.hlsl", "model-fshader.hlsl");

	gGrid.Initialize(50.0f, 50, 5);

	UIInit();

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
			gSceneGraph.Build(gContainer->GetImporter(), gContainer->GetScene());
			gRegenAssets = false;
		}

	}

	// Shut down processing thread
	if (processor.joinable())
	{
		processor.detach();
	}

	// Cleanup
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
				sSelectedNode = nullptr;
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
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void BuildGUI(volatile AppState& state)
{
	BuildMenu(state);

	DisplaySceneInfo();

	DisplayAnimationInfo();

	DisplayTestWindow();
}

void DisplayTestWindow()
{
	ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	ImTimeline::Timeline();
	ImGui::End();
}

static unsigned int sSelectedId = 0;

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

	ImGui::Begin("Visual Scene", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	if (gAppState.OpenFile)
	{
		ImGui::Text("Loading FBX");
	}
	else
	{
		auto children = gSceneGraph.Children();
		for each (auto visualSceneNode in children)
		{
			ImGui::Indent();
			std::stringstream label;
			label << visualSceneNode->Name() << "##" << visualSceneNode->Id();
			if (ImGui::Selectable(label.str().c_str(), sSelectedId == visualSceneNode->Id())) { sSelectedId = visualSceneNode->Id(); }

			if (visualSceneNode->GetComponents().size() > 0)
			{
				auto components = visualSceneNode->GetComponents();
				ImGui::Indent();
				for each (auto component in components)
				{
					ImGui::Text("Component: %s", component->TypeName.c_str());
				}
				ImGui::Unindent();
			}

			DisplayChildMesh(visualSceneNode);
			ImGui::Unindent();
		}
	}
	ImGui::End();
}

void DisplayChildMesh(SceneNodeSharedPtr node)
{
	auto children = node->GetChildren();
	for each (auto child in children)
	{
		ImGui::Indent();
		std::stringstream label;
		label << child->Name() << "##" << child->Id();
		if (ImGui::Selectable(label.str().c_str(), sSelectedId == child->Id())) { sSelectedId = child->Id(); }
		if (child->GetComponents().size() > 0)
		{
			auto components = child->GetComponents();
			ImGui::Indent();
			for each (auto component in components)
			{
				ImGui::Text("Component: %s", component->TypeName.c_str());
			}
			ImGui::Unindent();
		}
		DisplayChildMesh(child);
		ImGui::Unindent();
	}
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
			if (model != nullptr)
			{
				for each (std::string attribName in model->AttributeNames)
				{
					ImGui::Text("Attribute: %s", attribName.c_str());
				}
			}
			DisplaySubModel(var);
		}

		ImGui::EndChild();
	}
}

void DisplaySubModel(NodeSharedPtr submodel)
{
	ImGui::Indent();
	std::stringstream label;
	label << submodel->Name.c_str();
	if (submodel->NodeType.length() > 0)
	{
		label << " (" << submodel->NodeType.c_str() << ")";
	}
	if (ImGui::Selectable(label.str().c_str(), sSelectedNode == submodel))
		sSelectedNode = submodel;

	int childCount = SizeT2UInt32(submodel->Children.size());
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
				const char* animStackName = animStack->GetName();
				if (ImGui::CollapsingHeader(animStackName))
				{
					unsigned __int32 animLayerCount = SizeT2UInt32(animStack->GetMemberCount<FbxAnimLayer>());

					ImGui::Indent();
					for (unsigned __int32 animLayerIndex = 0; animLayerIndex < animLayerCount; animLayerIndex++)
					{
						std::stringstream layerLabel;
						FbxAnimLayer* layer = animStack->GetSrcObject<FbxAnimLayer>(animLayerIndex);
						layerLabel << "Layer: " << layer->GetName() << "##" << animStack->GetUniqueID();
						if (ImGui::TreeNode(layerLabel.str().c_str()))
						{
							for each (auto mesh in gContainer->GetMeshs())
							{
								ProcessAnimLayer(mesh->FbxNodeRef, animStack, layer);
							}

							ImGui::TreePop();
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

				//gSceneGraph.Build(gContainer);
				gSceneGraph.Build(gContainer->GetImporter(), gContainer->GetScene());

				delete gAnim;
				gAnim = BuildAnim(filename);

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