#include "debug.h"

#include "imgui.h"
#include "Utils.h"
#include "widgets/CurveEditor.h"
#include "widgets/lumix.h"

#include <string>
#include <sstream>

void DisplayCurve(const char* label, FbxAnimCurve* curve);

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

void ProcessCameraAnimations(fbxsdk::FbxCamera * camera, fbxsdk::FbxAnimLayer * layer)
{
	FbxAnimCurve* animCurve = nullptr;

	animCurve = camera->FieldOfView.GetCurve(layer);
	if ((animCurve != nullptr) && ImGui::TreeNode("FOV", "%s : FOV", camera->GetName()))
	{
		DisplayCurve("FOV", animCurve);
		ImGui::TreePop();
	}

	animCurve = camera->FieldOfViewX.GetCurve(layer);
	if ((animCurve != nullptr) && ImGui::TreeNode("FOVX", "%s : FOVX", camera->GetName()))
	{
		DisplayCurve("FOVX", animCurve);
		ImGui::TreePop();
	}

	animCurve = camera->FieldOfViewY.GetCurve(layer);
	if ((animCurve != nullptr) && ImGui::TreeNode("FOVY", "%s : FOVY", camera->GetName()))
	{
		DisplayCurve("FOVY", animCurve);
		ImGui::TreePop();
	}

	animCurve = camera->Position.GetCurve(layer);
	if ((animCurve != nullptr) && ImGui::TreeNode("Position", "%s : Position", camera->GetName()))
	{
		DisplayCurve("Position", animCurve);
		ImGui::TreePop();
	}

	animCurve = camera->FocalLength.GetCurve(layer);
	if ((animCurve != nullptr) && ImGui::TreeNode("FocalLength", "%s : Focal length", camera->GetName()))
	{
		DisplayCurve("FocalLength", animCurve);
		ImGui::TreePop();
	}
}

void ProcessAnimLayer(FbxNode* node, FbxAnimStack* animStack, FbxAnimLayer* layer)
{
	std::stringstream nodeLabel;
	nodeLabel << node->GetName() << "##" << animStack->GetName() << "##" << layer->GetName();
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

void ProcessAnimChildNode(FbxNode* node, FbxAnimStack* animStack, FbxAnimLayer* layer)
{
	std::stringstream nodeLabel;
	nodeLabel << "Node: " << node->GetName() << "##" << animStack->GetUniqueID() << "##" << layer->GetName();
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

		int newCount = 0;
		int selectedPoint = 0;

		int flags = (int)ImGui::CurveEditorFlags::NO_TANGENTS | (int)ImGui::CurveEditorFlags::SHOW_GRID;

		ImGui::BeginChild(output.Buffer(), ImVec2(-1, -1), true);

		if (ImGui::CurveEditor(output.Buffer(), (float*)points, keyCount, ImVec2(450, 400), flags, &newCount, &selectedPoint))
		{
		}

		// ImGui::Curve(label, ImVec2(500, 400), keyCount, points);

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

		FbxNodeAttribute* attrib = node->GetNodeAttribute();

		if (attrib == nullptr) return;

		FbxNodeAttribute::EType attribType = node->GetNodeAttribute()->GetAttributeType();
		if (attribType == FbxNodeAttribute::eCamera)
		{
			FbxCamera* camera = FbxCast<FbxCamera>(attrib);
			ProcessCameraAnimations(camera, layer);
		}

	}
}