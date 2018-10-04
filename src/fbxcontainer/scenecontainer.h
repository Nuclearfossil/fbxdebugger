#pragma once

#include "Model.h"
#include "Take.h"

#include <vector>
#include <fbxsdk.h>

// Forward Declaration
class Program;

class SceneContainer
{
public:
	SceneContainer();
	~SceneContainer();

	void LoadFile(const char* filename);

	FbxImporter* GetImporter() { return mFBXImporter; }
	FbxScene* GetScene() { return mScene; }

	std::vector<NodeSharedPtr> GetMeshs() { return mSceneModels; }
	std::vector<NodeSharedPtr> GetCameras() { return mCameras; }
	std::vector<NodeSharedPtr> GetHierarchy() { return mHierarchy; }

	std::vector<FbxTakeInfo> GetTakeList() { return mFBXTakeList; }
	std::vector<FbxAnimStack*> GetAnimStack() { return mFBXAnimStackList; }

	bool BuildModels(FbxNode* node, NodeSharedPtr& modelSharedPtr);
	void BuildAnimation();

	bool Process();

	void BuildRenderable();
	void BuildRenderables(NodeSharedPtr node);
	void RenderAll(NodeSharedPtr selected, Program* program);

private:
	void BuildHierarchy();
	void BindModelAttributes(ModelSharedPtr model, FbxNode* node);
	void BuildMeshFromAttribute(ModelSharedPtr model, FbxNodeAttribute* attr);
	void DisplaySubModel(NodeSharedPtr submodel);
	void Render(NodeSharedPtr submodel, NodeSharedPtr selected);

private:
	FbxManager* mFBXManager;
	FbxImporter* mFBXImporter;
	FbxScene* mScene;
	Program* mActiveProgram;
	bool mCanRender;

	std::vector<NodeSharedPtr> mSceneModels;
	std::vector<NodeSharedPtr> mCameras;
	std::vector<TakeSharedPtr> mTakes;
	std::vector<NodeSharedPtr> mHierarchy;

	// These things will need to go away at some point.
	std::vector<FbxTakeInfo> mFBXTakeList;
	std::vector<FbxAnimStack*> mFBXAnimStackList;
};