#pragma once

#include <vector>

// Forward Declarations for the FBX sdk
namespace fbxsdk
{
	class FbxManager;
	class FbxImporter;
	class FbxScene;
	class FbxNode;
};

class AnimBlock;

class ImporterBase
{
public:
	ImporterBase();
	~ImporterBase();

	AnimBlock* Import(const char* filename);

private:
	fbxsdk::FbxManager* m_FBXManager;
	fbxsdk::FbxImporter* m_FBXImporter;
	fbxsdk::FbxScene* m_Scene;
};