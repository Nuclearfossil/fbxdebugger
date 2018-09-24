#pragma once

#define GLEW_STATIC
#include <GL\glew.h>

#include "misc\json.h"
#include "misc\Utils.h"

// Forward Declarations
class Program;

typedef struct
{
	bool ShouldExit;
	bool OpenFile;
} AppState;

GLFWwindow* GetWindow();

void SetWindowHints();

bool InitGfxSubsystem();
void ShutdownGFX();

bool CreateMainWindow(volatile AppState* state, nlohmann::json& gSettings);
bool WindowShouldClose();
void PollEvents();

KeyState GetKeystate(ImGuiIO& io);

void InitRenderState();

void UpdateFramebufferSize(int &width, int &height);

void Swap();

void Clear(GLFWwindow* window, ImVec4& clearColor);

void UIInit();
void UINewFrame();

void UIShutdown();

void LoadTriangle(Program* program, GLuint& vao, GLuint& vbo);
void LoadShaders(Program** program, const char* vertexShaderFile, const char* fragmentShaderFile);
