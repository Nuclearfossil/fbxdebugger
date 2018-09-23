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

void SetWindowHints();

bool InitGfxSubsystem();
void ShutdownGFX();

bool CreateMainWindow(volatile AppState* state, nlohmann::json& gSettings);
bool WindowShouldClose();
void PollEvents();

KeyState GetKeystate();

void SetMouseInfo(MouseInfo* info);

void InitRenderState();

void UpdateFramebufferSize(int &width, int &height);

void Swap();

void ClearBackground();


void UIInit();
void UINewFrame();

void UIShutdown();

void SetupWindow(GLFWwindow* window);

void LoadTriangle(Program* program, GLuint& vao, GLuint& vbo);
void LoadShaders(Program** program, const char* vertexShaderFile, const char* fragmentShaderFile);
