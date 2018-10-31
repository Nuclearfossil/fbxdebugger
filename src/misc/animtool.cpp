#include "animtool.h"
#include "animation/anim.h"
#include "misc/Utils.h"

#include <fbxsdk.h>
#include <string>

void DebugNode(FbxNode* node, FbxAnimLayer* animLayer, int& depth);
void BuildAnimFromNode(FbxNode* node, Anim* anim, FbxAnimLayer* layer);
bool AddAnimBlock(FbxNode* node, FbxAnimLayer* animLayer, Anim* anim);

Anim* BuildAnim(const char* filename)
{
	FbxManager* fbxManager = nullptr;
	FbxImporter* fbxImporter = nullptr;
	FbxScene* scene = nullptr;
	FbxAnimStack* defaultAnimStack = nullptr;

	fbxManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ioSettings);

	Anim* anim = new Anim();

	fbxImporter = FbxImporter::Create(fbxManager, "");
	fbxImporter->Initialize(filename, -1, fbxManager->GetIOSettings());

	scene = FbxScene::Create(fbxManager, "sceneRoot");
	if (scene == nullptr) return anim;

	if (fbxImporter->Import(scene) == false)
	{
		auto result = fbxImporter->GetStatus();
		printf("Unexpected Import error: %s\n", result.GetErrorString());
		return anim;
	}

	// We're only interested in the top level anim stack (currently).
	defaultAnimStack = scene->GetCurrentAnimationStack();
	FbxAnimLayer* layer = defaultAnimStack->GetSrcObject<FbxAnimLayer>(0);

	fbxsdk::FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == nullptr) return nullptr;

	int rootNodeChildCount = rootNode->GetChildCount();

	printf("Nodes off the root node ***************************\n");
	for (int index = 0; index < rootNodeChildCount; index++)
	{
		int depth = 0;
		FbxNode* node = rootNode->GetChild(index);
		DebugNode(node, layer, depth);
		BuildAnimFromNode(node, anim, layer);
	}

	fbxManager->Destroy();
	return anim;
}

void PrintIndent(int depth)
{
	for (int index = 0; index < depth; index++)
	{
		printf("    ");
	}
}

void DebugNode(FbxNode* node, FbxAnimLayer* animLayer, int& depth)
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
		foundOne = true;
	}
	if (node->GetNull() != nullptr)
	{
		printf("[NULL]");
		foundOne = true;
	}
	if (node->GetMarker() != nullptr)
	{
		printf("[Marker]");
		foundOne = true;
	}
	if (node->GetGeometry() != nullptr)
	{
		printf("[Geo]");
		foundOne = true;
	}
	if (node->GetMesh() != nullptr)
	{
		printf("[Mesh]");
		foundOne = true;
	}
	if (node->GetNurbs() != nullptr)
	{
		printf("[Nurbs]");
		foundOne = true;
	}
	if (node->GetNurbsSurface() != nullptr)
	{
		printf("[NurbSurf]");
		foundOne = true;
	}
	if (node->GetPatch() != nullptr)
	{
		printf("[Patch]");
		foundOne = true;
	}
	if (node->GetLight() != nullptr)
	{
		printf("[Light]");
		foundOne = true;
	}
	if (node->GetLine() != nullptr)
	{
		printf("[Line]");
		foundOne = true;
	}
	printf("\n");

	// What do we have for animation?
	FbxAnimCurve* animCurves[9];

	animCurves[0] = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	animCurves[1] = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	animCurves[2] = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	animCurves[3] = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	animCurves[4] = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	animCurves[5] = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	animCurves[6] = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	animCurves[7] = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	animCurves[8] = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	if (animCurves[0] != nullptr)
	{
		printf("Translation X keycount: %d\n", animCurves[0]->KeyGetCount());
	}
	if (animCurves[1] != nullptr)
	{
		printf("Translation Y keycount: %d\n", animCurves[1]->KeyGetCount());
	}
	if (animCurves[2] != nullptr)
	{
		printf("Translation Z keycount: %d\n", animCurves[2]->KeyGetCount());
	}

	if (animCurves[3] != nullptr)
	{
		printf("Rotation X keycount: %d\n", animCurves[3]->KeyGetCount());
	}
	if (animCurves[4] != nullptr)
	{
		printf("Rotation Y keycount: %d\n", animCurves[4]->KeyGetCount());
	}
	if (animCurves[5] != nullptr)
	{
		printf("Rotation Z keycount: %d\n", animCurves[5]->KeyGetCount());
	}

	if (animCurves[6] != nullptr)
	{
		printf("Scale X keycount: %d\n", animCurves[6]->KeyGetCount());
	}
	if (animCurves[7] != nullptr)
	{
		printf("Scale Y keycount: %d\n", animCurves[7]->KeyGetCount());
	}
	if (animCurves[8] != nullptr)
	{
		printf("Scale Z keycount: %d\n", animCurves[8]->KeyGetCount());
	}

	// walk the children, printing them
	int childCount = node->GetChildCount();
	for (int index = 0; index < childCount; index++)
	{
		FbxNode* child = node->GetChild(index);
		DebugNode(child, animLayer, depth);
	}

	depth--;
}
void BuildAnimFromNode(FbxNode* node, Anim* anim, FbxAnimLayer* layer)
{
	AddAnimBlock(node, layer, anim);

	// walk the children, printing them
	int childCount = node->GetChildCount();
	for (int index = 0; index < childCount; index++)
	{
		FbxNode* child = node->GetChild(index);
		BuildAnimFromNode(child, anim, layer);
	}
}

bool AddAnimBlock(FbxNode* node, FbxAnimLayer* animLayer, Anim* anim)
{
	// are there any anim curves on this node?
	FbxAnimCurve* animCurves[9];

	animCurves[0] = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	animCurves[1] = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	animCurves[2] = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	animCurves[3] = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	animCurves[4] = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	animCurves[5] = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	animCurves[6] = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	animCurves[7] = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	animCurves[8] = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	if (animCurves[0] == nullptr &&
		animCurves[1] == nullptr &&
		animCurves[2] == nullptr &&
		animCurves[3] == nullptr &&
		animCurves[4] == nullptr &&
		animCurves[5] == nullptr &&
		animCurves[6] == nullptr &&
		animCurves[7] == nullptr &&
		animCurves[8] == nullptr)
	{
		return false;
	}
	AnimBlock& block = anim->AddAnimBlock(node->GetNameWithoutNameSpacePrefix().Buffer());

	for (int index = 0; index <= 8; index++)
	{
		if (animCurves[index] != nullptr)
		{
			int keyCount = animCurves[index]->KeyGetCount();
			AnimCurve* curve = block.AddCurve((AnimCurveEnum)index);
			curve->ReserveKeys(keyCount);

			for (int keyIndex = 0; keyIndex < keyCount; keyIndex++)
			{
				FbxAnimCurveKey curveKey = animCurves[index]->KeyGet(keyIndex);
				curve->AddKey(curveKey.GetTime().GetSecondDouble(), curveKey.GetValue());
			}
		}
	}

	return true;
}