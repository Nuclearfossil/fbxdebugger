#include "debug.h"

#include "imgui.h"
#include "Utils.h"

#include <string>
#include <sstream>

const char* TrackLayerNames[][2] =
{
	{"Translation X", "TX"}
	, { "Translation Y", "TY"}
	, { "Translation Z", "TZ"}
	, { "Rotation X", "RX"}
	, { "Rotation Y", "RY"}
	, { "Rotation Z", "RZ"}
	, { "Scale X", "SX"}
	, { "Scale Y", "SY"}
	, { "Scale Z", "SZ"}
};

void ProcessAnimLayer(FbxNode* node, FbxAnimStack* animStack, int animIndex)
{

	std::stringstream layerLabel;
	FbxAnimLayer* layer = animStack->GetSrcObject<FbxAnimLayer>(animIndex);
	layerLabel << "Layer: " << layer->GetName() << "##" << animStack->GetUniqueID() << "##" << animIndex;
	if (ImGui::CollapsingHeader(layerLabel.str().c_str()))
	{
		std::stringstream nodeLabel;
		nodeLabel << "Root Mesh: " << node->GetName() << "##" << animStack->GetName() << "##" << layer->GetName() << animIndex;
		if (ImGui::TreeNode(nodeLabel.str().c_str()))
		{
			unsigned __int32 modelCount = SizeT2Int32(node->GetChildCount());
			if (modelCount > 0)
			{
				for (unsigned _int32 index = 0; index < modelCount; index++)
				{
					FbxNode* child = node->GetChild(index);
					ProcessAnimChildNode(child, animStack, layer);
				}
			}

//			DisplayCurveData(node, layer);

			ImGui::TreePop();
		}
	}
}

void ProcessAnimChildNode(FbxNode* node, FbxAnimStack* animStack, FbxAnimLayer* layer)
{
	std::stringstream nodeLabel;
	nodeLabel << "Mesh: " << node->GetName() << "##" << animStack->GetUniqueID() << "##" << layer->GetName();
	if (ImGui::TreeNode(nodeLabel.str().c_str()))
	{
		unsigned __int32 modelCount = SizeT2Int32(node->GetChildCount());
		if (modelCount > 0)
		{
			for (unsigned _int32 index = 0; index < modelCount; index++)
			{
				FbxNode* child = node->GetChild(index);
				ProcessAnimChildNode(child, animStack, layer);
			}
		}

//		DisplayCurveData(node, layer);

		ImGui::TreePop();
	}

}
