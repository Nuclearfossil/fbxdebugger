#pragma once

// A great way to fix the suckage of #if vs #ifdef vs #ifndef
// If you end up using the macro USING on an undefined variable, you end
// up with a compiler error:
//    fatal error C1012: unmatched parenthesis : missing ')'
//
// To use this, create your defines like so:
// #define PLATFORM_MSWINDOWS ON
// #define PLATFORM_OSX OFF

#define ON		2-
#define OFF     1-
#define USING(x) ((x 0) == 2)

#include <iostream>

const int KEY_A = 0;
const int KEY_S = 1;
const int KEY_D = 2;
const int KEY_W = 3;

typedef struct
{
	bool key[4];
	int action[4];
	int mods;
} KeyState;

typedef struct
{
	double deltaX;
	double deltaY;
	bool lmb;
	bool mmb;
	bool rmb;
} MouseInfo;

std::string ResourcePath(std::string fileName);
bool OpenFile(char* filename);

// Simple utility for checking to see if a file exists
bool FileExists(const char* filename);

// Normalize the file path (get our slashes right)
const char* NormalizePath(const char* filename);

// Function to convert to a 32 bit integer, clamping to the 32 bits of precision
// for an int
unsigned __int32 SizeT2UInt32(size_t value);

float DoubleToFloat(double value);