#include "SceneContainer.h"
//#include "debugscene.h"
#include "misc/Utils.h"
//#include "graphics/Program.h"

#include "Camera.h"

#include <fbxsdk.h>
#include <iostream>

SceneContainer::SceneContainer()
{
	mCanRender = false;
	mFBXManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(mFBXManager, IOSROOT);
	mFBXManager->SetIOSettings(ioSettings);

	mFBXImporter = FbxImporter::Create(mFBXManager, "");
	mScene = nullptr;
	mActiveProgram = nullptr;
}

SceneContainer::~SceneContainer()
{
	mCanRender = false;
	mScene->Destroy();
	mFBXImporter->Destroy();
	mFBXManager->Destroy();

	mFBXImporter = nullptr;
	mFBXManager = nullptr;
}

void SceneContainer::LoadFile(const char* filename)
{
	mSceneModels.clear();
	mTakes.clear();
	mHierarchy.clear();
	mFBXTakeList.clear();
	mFBXAnimStackList.clear();

	mFBXImporter->Initialize(filename, -1, mFBXManager->GetIOSettings());
	mScene = FbxScene::Create(mFBXManager, "sceneRoot");
	mFBXImporter->Import(mScene);

	FbxGeometryConverter fbxGeometryConverter(mFBXManager);
	fbxGeometryConverter.Triangulate(mScene, true);

	// mFBXTakeList = BuildTakeList(mFBXImporter);
}

bool SceneContainer::Process()
{
	FbxNode* rootNode = mScene->GetRootNode();
	if (rootNode == nullptr) return false;

	int rootNodeCount = rootNode->GetChildCount();

	for (int index = 0; index < rootNodeCount; index++)
	{
		NodeSharedPtr model(new Node());
		if (BuildModels(rootNode->GetChild(index), model))
			mSceneModels.push_back(model);
	}

	BuildAnimation();

	return true;
}

void SceneContainer::BuildHierarchy()
{
	mHierarchy.clear();
}

void SceneContainer::BuildMeshFromAttribute(ModelSharedPtr model, FbxNodeAttribute* attr)
{
	FbxMesh* mesh = (FbxMesh*)attr;

	// Little test of sanity
	if (mesh == nullptr) return;

	int polygonVertexCount = mesh->GetPolygonVertexCount();
	FbxVector4* controlPoints = mesh->GetControlPoints();
	int controlPointCount = mesh->GetControlPointsCount();

	for (int index = 0; index < controlPointCount; index++)
	{
		glm::vec4 vertex =
		{
			controlPoints[index][0],
			controlPoints[index][1],
			controlPoints[index][2],
			1.0
		};
		model->Vertices.push_back(vertex);
	}

	FbxLayerElementNormal* pLayerNormals = mesh->GetLayer(0)->GetNormals();
	if (pLayerNormals != NULL)
	{
		FbxLayerElement::EMappingMode normalMappingMode = pLayerNormals->GetMappingMode();
		FbxLayerElement::EReferenceMode normalReferenceMode = pLayerNormals->GetReferenceMode();
		if (normalMappingMode == FbxLayerElement::eByControlPoint)
		{
			for (int index = 0; index < controlPointCount; index++)
			{
				FbxVector4 meshNormal = pLayerNormals->GetDirectArray().GetAt(index);
				glm::vec4 normal =
				{
					meshNormal[0],
					meshNormal[1],
					meshNormal[2],
					meshNormal[3]
				};
				model->Normals.push_back(normal);
			}
		}
		else if (normalMappingMode == FbxLayerElement::eByPolygon)
		{
			assert(false);
		}
		else if (normalMappingMode == FbxLayerElement::eByPolygonVertex)
		{
			for (int index = 0; index < controlPointCount; index++)
			{
				FbxVector4 meshNormal;
				switch (pLayerNormals->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					meshNormal = pLayerNormals->GetDirectArray().GetAt(index);
					break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int id = pLayerNormals->GetIndexArray().GetAt(index);
					meshNormal = pLayerNormals->GetDirectArray().GetAt(id);
				}
				break;
				default:
					assert(false);
				}

				glm::vec4 normal =
				{
					meshNormal[0],
					meshNormal[1],
					meshNormal[2],
					meshNormal[3]
				};
				model->Normals.push_back(normal);
			}
		}
	}

	int polygons = mesh->GetPolygonCount();
	for (int index = 0; index < polygons; index++)
	{
		int triCount = mesh->GetPolygonSize(index);
		if (triCount != 3)
		{
			std::cout << "invalid polygon size! index " << index << "polygon Size: " << triCount << std::endl;
			continue;
		}

		glm::ivec3 indices =
		{
			mesh->GetPolygonVertex(index, 0),
			mesh->GetPolygonVertex(index, 1),
			mesh->GetPolygonVertex(index, 2)
		};

		model->TriangleIndices.push_back(indices);
	}
}

void SceneContainer::BindModelAttributes(ModelSharedPtr model, FbxNode* node)
{
	int nodeAttribCount = node->GetNodeAttributeCount();
	model->AttributeCount = nodeAttribCount;

	for (int index = 0; index < nodeAttribCount; index++)
	{
		FbxNodeAttribute* attr = node->GetNodeAttributeByIndex(index);
		switch (attr->GetAttributeType())
		{
		case FbxNodeAttribute::EType::eMesh:
			BuildMeshFromAttribute(model, attr);
			break;
		default:
			model->AttributeNames.push_back("Unknown -- ");
			break;
		}
	}
}

// This whole method is incredibly ugly - I need to refactor how it works.
bool SceneContainer::BuildModels(FbxNode* node, NodeSharedPtr& nodeSharedPtr)
{
	bool retval = true;
	nodeSharedPtr->FbxNodeRef = node;
	nodeSharedPtr->Name = node->GetNameOnly();

	int attributeCount = node->GetNodeAttributeCount();
	if (attributeCount == 0)
	{
		int childCount = node->GetChildCount();
		for (int index = 0; index < childCount; index++)
		{
			NodeSharedPtr childModel(new Model());
			BuildModels(node->GetChild(index), childModel);

			nodeSharedPtr->Children.push_back(childModel);
		}

		return retval;
	}

	for (int index = 0; index < attributeCount; index++)
	{
		FbxNodeAttribute* attrib = node->GetNodeAttributeByIndex(index);

		if (attrib == nullptr) return false;

		FbxNodeAttribute::EType attribType = node->GetNodeAttribute()->GetAttributeType();

		if (attribType == FbxNodeAttribute::eMesh)
		{
			ModelSharedPtr model = std::dynamic_pointer_cast<Model>(nodeSharedPtr);

			if (model == nullptr)
			{
				nodeSharedPtr = ModelSharedPtr(new Model());
				nodeSharedPtr->FbxNodeRef = node;
				nodeSharedPtr->FbxNodeAttr = attrib;
				nodeSharedPtr->Name = node->GetNameOnly();
				model = std::dynamic_pointer_cast<Model>(nodeSharedPtr);
			}

			// Do other stuff with the model
			BindModelAttributes(model, node);

			// and look for children
			int childCount = node->GetChildCount();
			for (int index = 0; index < childCount; index++)
			{
				ModelSharedPtr childModel(new Model());
				BuildModels(node->GetChild(index), std::dynamic_pointer_cast<Node>(childModel));

				nodeSharedPtr->Children.push_back(childModel);
			}

			retval = true;
		}
		else if (attribType == FbxNodeAttribute::eCamera)
		{
			CameraSharedPtr camera(new Camera());
			camera->FbxNodeRef = node;
			camera->Name = node->GetNameOnly(); // we're overriding the model that's passed in
			nodeSharedPtr = camera;

			// and look for children
			int childCount = node->GetChildCount();
			for (int index = 0; index < childCount; index++)
			{
				NodeSharedPtr childModel(new Model());
				BuildModels(node->GetChild(index), childModel);

				camera->Children.push_back(childModel);
			}

			retval = true;
		}
		else
		{
			// Let's stuff in a placeholder name for the entity type
			switch (attribType)
			{
			case fbxsdk::FbxNodeAttribute::eUnknown:
				nodeSharedPtr->NodeType = "Unknown";
				break;
			case fbxsdk::FbxNodeAttribute::eMarker:
				nodeSharedPtr->NodeType = "Marker";
				break;
			case fbxsdk::FbxNodeAttribute::eSkeleton:
				nodeSharedPtr->NodeType = "Skeleton";
				break;
			case fbxsdk::FbxNodeAttribute::eNurbs:
				nodeSharedPtr->NodeType = "Nurbs";
				break;
			case fbxsdk::FbxNodeAttribute::ePatch:
				nodeSharedPtr->NodeType = "Patch";
				break;
			case fbxsdk::FbxNodeAttribute::eCameraStereo:
				nodeSharedPtr->NodeType = "CameraStereo";
				break;
			case fbxsdk::FbxNodeAttribute::eCameraSwitcher:
				nodeSharedPtr->NodeType = "CameraSwitcher";
				break;
			case fbxsdk::FbxNodeAttribute::eLight:
				nodeSharedPtr->NodeType = "Light";
				break;
			case fbxsdk::FbxNodeAttribute::eOpticalReference:
				nodeSharedPtr->NodeType = "OpticalReference";
				break;
			case fbxsdk::FbxNodeAttribute::eOpticalMarker:
				nodeSharedPtr->NodeType = "OpticalMarker";
				break;
			case fbxsdk::FbxNodeAttribute::eNurbsCurve:
				nodeSharedPtr->NodeType = "NurbsCurve";
				break;
			case fbxsdk::FbxNodeAttribute::eTrimNurbsSurface:
				nodeSharedPtr->NodeType = "TrimNurbsSurface";
				break;
			case fbxsdk::FbxNodeAttribute::eBoundary:
				nodeSharedPtr->NodeType = "Boundary";
				break;
			case fbxsdk::FbxNodeAttribute::eNurbsSurface:
				nodeSharedPtr->NodeType = "NurbsSurface";
				break;
			case fbxsdk::FbxNodeAttribute::eShape:
				nodeSharedPtr->NodeType = "Shape";
				break;
			case fbxsdk::FbxNodeAttribute::eLODGroup:
				nodeSharedPtr->NodeType = "LODGroup";
				break;
			case fbxsdk::FbxNodeAttribute::eSubDiv:
				nodeSharedPtr->NodeType = "SubDiv";
				break;
			case fbxsdk::FbxNodeAttribute::eCachedEffect:
				nodeSharedPtr->NodeType = "CachedEffect";
				break;
			case fbxsdk::FbxNodeAttribute::eLine:
				nodeSharedPtr->NodeType = "Line";
				break;
			default:
				break;
			}
			// This isn't great, because we're not building the transform hierarchy
			int childCount = node->GetChildCount();
			for (int index = 0; index < childCount; index++)
			{
				NodeSharedPtr childModel(new Node());
				BuildModels(node->GetChild(index), childModel);
				nodeSharedPtr->Children.push_back(childModel);
			}
			retval = childCount > 0;
		}
	}

	return retval;
}

void SceneContainer::BuildRenderable()
{
	int rootModelCount = SizeT2Int32(mSceneModels.size());

	for (int index = 0; index < rootModelCount; index++)
	{
		NodeSharedPtr nodePtr = mSceneModels[index];
		ModelSharedPtr model = std::dynamic_pointer_cast<Model>(nodePtr);
		if (model != nullptr)
		{
			model->BuildRenderables();
		}
		else
		{
			int nodeChildCount = SizeT2Int32(nodePtr->Children.size());
			for each (auto nodeItem in nodePtr->Children)
			{
				BuildRenderables(nodeItem);
			}
		}
	}

	mCanRender = true;
}

void SceneContainer::BuildRenderables(NodeSharedPtr node)
{
	ModelSharedPtr model = std::dynamic_pointer_cast<Model>(node);
	if (model != nullptr)
	{
		model->BuildRenderables();
	}
	else
	{
		int nodeChildCount = SizeT2Int32(node->Children.size());
		for each (auto nodeItem in node->Children)
		{
			BuildRenderables(nodeItem);
		}
	}

}

void SceneContainer::BuildAnimation()
{
	std::vector<FbxAnimStack*> animStackList;
	int animStackCount = mScene->GetSrcObjectCount<FbxAnimStack>();

	for (int index = 0; index < animStackCount; index++)
	{
		TakeSharedPtr take(new Take());
		take->Name = mScene->GetSrcObject<FbxAnimStack>(index)->GetName();
		mTakes.push_back(take);

		mFBXAnimStackList.push_back(mScene->GetSrcObject<FbxAnimStack>(index));
	}
}

void SceneContainer::RenderAll(NodeSharedPtr selected, Program* program)
{
	if (mCanRender == false) return;
	mActiveProgram = program;

	int rootSceneModelCount = SizeT2Int32(mSceneModels.size());
	for (int index = 0; index < rootSceneModelCount; index++)
	{
		Render(mSceneModels[index], selected);
	}
}

void SceneContainer::Render(NodeSharedPtr submodel, NodeSharedPtr selected)
{
	int childCount = SizeT2Int32(submodel->Children.size());
	for (int index = 0; index < childCount; index++)
	{
		Render(submodel->Children[index], selected);
	}
	submodel->Render(submodel == selected, mActiveProgram);
}
