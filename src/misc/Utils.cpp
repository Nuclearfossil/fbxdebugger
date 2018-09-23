
#include "Utils.h"

#include <fstream>
#include <streambuf>
#include <io.h>

#include <windows.h>
#include <filesystem>

const std::string empty("");

std::string ResourcePath(std::string fileName)
{
	WCHAR executablePathUnicode[1024] = { '\0' };
	char executablePath[1024] = { '\0' };
	DWORD charsCopied = GetModuleFileName(nullptr, executablePathUnicode, 1024);
	if (charsCopied > 0 && charsCopied < 1024)
	{
		size_t length;
		wcstombs_s(&length, executablePath, 1024, executablePathUnicode, 1024);

		std::experimental::filesystem::path filePath(executablePath);

		return std::string(filePath.parent_path().generic_string()) + "\\resources\\" + fileName;
	}
	else
		return empty;
}

bool OpenFile(char* filename)
{
	bool result = false;
	const int BUFSIZE = 1024;
	WCHAR buffer[BUFSIZE] = { 0 };
	OPENFILENAME ofns = { 0 };
	ofns.lStructSize = sizeof(ofns);
	ofns.lpstrFile = buffer;
	ofns.nMaxFile = BUFSIZE;
	ofns.lpstrTitle = TEXT("Select Input FBX file");
	ofns.lpstrFilter = TEXT("FBX Files (*.fbx)\0*.fbx\0");
	if (GetOpenFileName(&ofns) == TRUE)
	{
		size_t i;
		wcstombs_s(&i, filename, (size_t)1024, ofns.lpstrFile, (size_t)1024);
		result = true;
	}

	return result;
}

const char* NormalizePath(const char* filename)
{
	static char tempFile[4096];

	// copy the filename one character at a time
	for (size_t index = 0; filename[index] != 0; index++)
	{
		char value = filename[index];
		tempFile[index] = value == '/' ? '\\' : value;
	}
	return tempFile;
}

// Simple utility for checking to see if a file exists
bool FileExists(const char* filename)
{
	bool result = false;

	if (_access(filename, 0) == 0)
		result = true;

	return result;
}


unsigned __int32 SizeT2Int32(size_t value)
{
	if (value > MAXUINT32) return MAXUINT32;

	return static_cast<unsigned __int32>(value);
}