#include "animtool.h"
#include "animation/anim.h"
#include "misc/Utils.h"

#include <fbxsdk.h>
#include <string>

static std::string debugText("I'm Empty Dammit!");

Anim* BuildAnimBlock(fbxsdk::FbxImporter* importer, fbxsdk::FbxScene* scene)
{
	debugText.clear();
	Anim* block = new Anim();

	// We're only interested in the top level anim stack (currently).
	FbxAnimStack* animStack = scene->GetCurrentAnimationStack();

	if (animStack == nullptr) return block;

	unsigned __int32 animLayerCount = SizeT2UInt32(animStack->GetMemberCount<FbxAnimLayer>());

	for (unsigned __int32 animLayerIndex = 0; animLayerIndex < animLayerCount; animLayerIndex++)
	{
		FbxAnimLayer* layer = animStack->GetSrcObject<FbxAnimLayer>(animLayerIndex);
		debugText += "Layer: ";
		debugText += layer->GetName();
		debugText += "\n";

		if (layer == nullptr) continue;

		int memberCount = layer->GetMemberCount();
		for (size_t memberIndex = 0; memberIndex < memberCount; memberIndex++)
		{
			auto memberNode = layer->GetMember(memberIndex);
			debugText += "Member: ";
			debugText += memberNode->GetNameWithNameSpacePrefix().Buffer();
			debugText += "\n";
			int srcPropertyCount = layer->GetSrcPropertyCount();
			for (size_t propertyIndex = 0; propertyIndex < srcPropertyCount; propertyIndex++)
			{
				auto property = layer->GetSrcProperty(propertyIndex);
				debugText += "  Src Prop: ";
				debugText += property.GetName();
				debugText += "\n";
			}
			int dstPropertyCount = layer->GetDstPropertyCount();
			for (size_t dstPropIndex = 0; dstPropIndex < dstPropertyCount; dstPropIndex++)
			{
				auto property = layer->GetDstProperty(dstPropIndex);
				debugText += "  Dst Prop: ";
				debugText += property.GetName();
				debugText += "\n";
			}
			int dstObjCount = layer->GetDstObjectCount();

			int srcObjCount = layer->GetSrcObjectCount();
			for (size_t srcObjIndex = 0; srcObjIndex < srcObjCount; srcObjIndex++)
			{
				auto object = layer->GetSrcObject(srcObjIndex);
				debugText += "  src Obj: ";
				debugText += object->GetName();
				debugText += "\n";
			}
		}

		auto animCurveCount = layer->GetSrcObjectCount<FbxAnimCurveNode>();
		for (size_t animCurveIndex = 0; animCurveIndex < animCurveCount; animCurveIndex++)
		{
			FbxAnimCurveNode* curveNode = layer->GetSrcObject<FbxAnimCurveNode>(animCurveIndex);
			debugText += "  CurveNode: ";
			debugText += curveNode->GetName();
			debugText += "\n";
			auto channelCount = curveNode->GetChannelsCount();
			for (size_t channelIndex = 0; channelIndex < channelCount; channelIndex++)
			{
				FbxAnimCurve* curve = curveNode->GetCurve(channelIndex);
				if (curve == nullptr) continue;

				const char* name = curve->GetName();
				debugText += "    Curve: ";
				debugText += name;
				debugText += "\n";
			}
		}
 	}

	return block;
}

const char* GetDebugText()
{
	return debugText.c_str();
}
