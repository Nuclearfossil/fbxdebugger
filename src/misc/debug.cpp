#include "debug.h"

#include "imgui.h"
#include "Utils.h"
#include "widgets/CurveEditor.h"
#include "widgets/lumix.h"

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

			DisplayCurveData(node, layer);

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

		DisplayCurveData(node, layer);

		ImGui::TreePop();
	}

}

void DisplayCurve(const char* label, FbxAnimCurve* curve)
{
	FbxString output(label);
	output.Append("##", 2);
	output.Append(label, strlen(label) + 1);
	output.Append(curve->GetName(), strlen(curve->GetName()) + 1);
	unsigned __int32 keyCount = SizeT2Int32(curve->KeyGetCount());

	if (keyCount > 1)
	{
		ImVec2* points = new ImVec2[keyCount];
		ImGui::Text("Keycount: %i", keyCount);
		for (unsigned _int32 index = 0; index < keyCount; index++)
		{
			float x = static_cast<float>(curve->KeyGetTime(index).GetSecondDouble());
			float y = curve->KeyGetValue(index);

			points[index].x = x;
			points[index].y = y;
		}

		int newCount;
		int selectedPoint;

		int flags = (int)ImGui::CurveEditorFlags::NO_TANGENTS | (int)ImGui::CurveEditorFlags::SHOW_GRID;

		ImGui::BeginChild(output.Buffer(), ImVec2(600, 600), true);
		// if (ImGui::CurveEditor(output.Buffer(), (float*)points, keyCount, ImVec2(600, 600), flags, &newCount, &selectedPoint))
		// {
		// }

		ImGui::Curve(label, ImVec2(500, 400), keyCount, points);

		ImGui::EndChild();

		delete[] points;
	}
	else
	{
		if (ImGui::TreeNode(output.Buffer()))
		{
			ImGui::Indent();
			ImGui::Text("Keys: %d", keyCount);
			ImGui::Unindent();
			ImGui::TreePop();
		}
	}
}


void DisplayCurveData(fbxsdk::FbxNode * node, fbxsdk::FbxAnimLayer * layer)
{
	FbxAnimCurve* animCurves[9];

	animCurves[0] = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
	animCurves[1] = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
	animCurves[2] = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);
	animCurves[3] = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
	animCurves[4] = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
	animCurves[5] = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);
	animCurves[6] = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
	animCurves[7] = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
	animCurves[8] = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);

	if (animCurves[0] != nullptr
		|| animCurves[1] != nullptr
		|| animCurves[2] != nullptr
		|| animCurves[3] != nullptr
		|| animCurves[4] != nullptr
		|| animCurves[5] != nullptr
		|| animCurves[6] != nullptr
		|| animCurves[7] != nullptr
		|| animCurves[8] != nullptr
		)
	{
		std::stringstream label;
		label << "Animation Curves##" << node->GetUniqueID() << layer->GetUniqueID();
		if (ImGui::TreeNode(label.str().c_str()))
		{
			for (size_t index = 0; index <= 8; index++)
			{
				if (animCurves[index] != nullptr)
				{
					if (ImGui::TreeNode(TrackLayerNames[index][0]))
					{
						DisplayCurve(TrackLayerNames[index][1], animCurves[index]);
						ImGui::TreePop();
					}
				}
			}

			ImGui::TreePop();
		}
	}
}