#include "animtool.h"
#include "animation/anim.h"
#include "misc/Utils.h"

#include <fbxsdk.h>
#include <string>

void DebugNode(FbxNode* node, int& depth);

AnimBlock* BuildAnimBlock(const char* filename)
{
	FbxManager* fbxManager;
	FbxImporter* fbxImporter;
	FbxScene* scene;

	fbxManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ioSettings);

	AnimBlock* animBlock = new AnimBlock();

	fbxImporter = FbxImporter::Create(fbxManager, "");
	fbxImporter->Initialize(filename, -1, fbxManager->GetIOSettings());

	fbxsdk::FbxScene* scene = FbxScene::Create(fbxManager, "sceneRoot");
	if (scene == nullptr) return animBlock;

	if (fbxImporter->Import(scene) == false)
	{
		auto result = fbxImporter->GetStatus();
		printf("Unexpected Import error: %s\n", result.GetErrorString());
		return animBlock;
	}

	// We're only interested in the top level anim stack (currently).
	FbxAnimStack* animStack = scene->GetCurrentAnimationStack();

	fbxsdk::FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == nullptr) return nullptr;

	int rootNodeChildCount = rootNode->GetChildCount();

	printf("Nodes off the root node ***************************\n");
	for (int index = 0; index < rootNodeChildCount; index++)
	{
		int depth = 0;
		FbxNode* node = rootNode->GetChild(index);
		DebugNode(node, depth);
	}

	fbxManager->Destroy();
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

	// walk the children, printing them
	int childCount = node->GetChildCount();
	for (int index = 0; index < childCount; index++)
	{
		FbxNode* child = node->GetChild(index);
		DebugNode(child, depth);
	}

	depth--;
}
AnimBlock* BuildAnimBlock(fbxsdk::FbxImporter* importer, fbxsdk::FbxScene* scene)
{
	AnimBlock* block = new AnimBlock();

	// We're only interested in the top level anim stack (currently).
	FbxAnimStack* animStack = scene->GetCurrentAnimationStack();

	if (animStack == nullptr) return block;

	unsigned __int32 animLayerCount = SizeT2UInt32(animStack->GetMemberCount<FbxAnimLayer>());

	for (unsigned __int32 animLayerIndex = 0; animLayerIndex < animLayerCount; animLayerIndex++)
	{
		FbxAnimLayer* layer = animStack->GetSrcObject<FbxAnimLayer>(animLayerIndex);

		if (layer == nullptr) continue;

		int memberCount = layer->GetMemberCount();
		for (int memberIndex = 0; memberIndex < memberCount; memberIndex++)
		{
			auto memberNode = layer->GetMember(memberIndex);
			int srcPropertyCount = layer->GetSrcPropertyCount();
			for (int propertyIndex = 0; propertyIndex < srcPropertyCount; propertyIndex++)
			{
				auto property = layer->GetSrcProperty(propertyIndex);
			}
			int dstPropertyCount = layer->GetDstPropertyCount();
			for (int dstPropIndex = 0; dstPropIndex < dstPropertyCount; dstPropIndex++)
			{
				auto property = layer->GetDstProperty(dstPropIndex);
			}
			int dstObjCount = layer->GetDstObjectCount();

			int srcObjCount = layer->GetSrcObjectCount();
			for (int srcObjIndex = 0; srcObjIndex < srcObjCount; srcObjIndex++)
			{
				auto object = layer->GetSrcObject(srcObjIndex);
			}
		}

		auto animCurveCount = layer->GetSrcObjectCount<FbxAnimCurveNode>();
		for (int animCurveIndex = 0; animCurveIndex < animCurveCount; animCurveIndex++)
		{
			FbxAnimCurveNode* curveNode = layer->GetSrcObject<FbxAnimCurveNode>(animCurveIndex);
			auto channelCount = curveNode->GetChannelsCount();
			for (unsigned int channelIndex = 0; channelIndex < channelCount; channelIndex++)
			{
				FbxAnimCurve* curve = curveNode->GetCurve(channelIndex);
				if (curve == nullptr) continue;

				const char* name = curve->GetName();
			}
		}
	}

	return block;
}
