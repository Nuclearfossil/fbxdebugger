#include "importer\importer.h"
#include "animation\anim.h"

#include <fbxsdk.h>

// forward decls for internal functions
void DebugNode(FbxNode* node, int& depth);

ImporterBase::ImporterBase() :
	m_FBXManager(nullptr),
	m_FBXImporter(nullptr)
{}

ImporterBase::~ImporterBase()
{
	m_FBXImporter->Destroy();
	m_FBXManager->Destroy();

	m_FBXImporter = nullptr;
	m_FBXManager = nullptr;
}

//
// Where we perform the actual import.
// We also assume at this point that the filename has been validated before calling Import.
AnimBlock* ImporterBase::Import(const char* filename)
{
	m_FBXManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(m_FBXManager, IOSROOT);
	m_FBXManager->SetIOSettings(ioSettings);

	AnimBlock* animBlock = new AnimBlock();

	m_FBXImporter = FbxImporter::Create(m_FBXManager, "");
	m_FBXImporter->Initialize(filename, -1, m_FBXManager->GetIOSettings());

	fbxsdk::FbxScene* scene = FbxScene::Create(m_FBXManager, "sceneRoot");
	if (scene == nullptr) return animBlock;

	if (m_FBXImporter->Import(scene) == false)
	{
		auto result = m_FBXImporter->GetStatus();
		printf("Unexpected Import error: %s\n", result.GetErrorString());
		return animBlock;
	}


	fbxsdk::FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == nullptr) return nullptr;

	int rootNodeChildCount = rootNode->GetChildCount();

	printf("Nodes off the root node ***************************\n");
	for (int index = 0; index < rootNodeChildCount; index++)
	{
		int depth=0;
		FbxNode* node = rootNode->GetChild(index);
		DebugNode(node, depth);
	}

	return animBlock;
}

void PrintIndent(int depth)
{
	for (int index = 0; index < depth; index++)
	{
		printf("\t");
	}
}

void DebugNode(FbxNode* node, int& depth)
{
	if (node == nullptr) return;

	depth++;

	// print the current node info
	PrintIndent(depth);
	printf("Node Name: %s ", node->GetName());

	// Let's do a bunch of casts to see if there's anything we can cast it to
	bool foundOne = false;
	if (node->GetSkeleton() != nullptr)
	{
		printf("[Skeleton]");
		foundOne=true;
	}
	if (node->GetNull() != nullptr)
	{
		printf("[NULL]");
		foundOne=true;
	}
	if (node->GetMarker() != nullptr)
	{
		printf("[Marker]");
		foundOne=true;
	}
	if (node->GetGeometry() != nullptr)
	{
		printf("[Geo]");
		foundOne=true;
	}
	if (node->GetMesh() != nullptr)
	{
		printf("[Mesh]");
		foundOne=true;
	}
	if (node->GetNurbs() != nullptr)
	{
		printf("[Nurbs]");
		foundOne=true;
	}
	if (node->GetNurbsSurface() != nullptr)
	{
		printf("[NurbSurf]");
		foundOne=true;
	}
	if (node->GetPatch() != nullptr)
	{
		printf("[Patch]");
		foundOne=true;
	}
	if (node->GetLight() != nullptr)
	{
		printf("[Light]");
		foundOne=true;
	}
	if (node->GetLine() != nullptr)
	{
		printf("[Line]");
		foundOne=true;
	}
	printf("\n");

	// walk the children, printing them
	int childCount = node->GetChildCount();
	for (int index = 0; index < childCount; index++)
	{
		FbxNode* child = node->GetChild(index);
		DebugNode(child, depth);
	}

	depth--;
}