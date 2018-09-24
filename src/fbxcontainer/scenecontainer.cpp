#include "SceneContainer.h"
//#include "debugscene.h"
#include "misc/Utils.h"
//#include "graphics/Program.h"

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
		NodeSharedPtr model(new Model());
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

bool SceneContainer::BuildModels(FbxNode* node, NodeSharedPtr modelSharedPtr)
{
	bool retval = false;
	modelSharedPtr->FbxNodeRef = node;

	int attributeCount = node->GetNodeAttributeCount();

	for (int index = 0; index < attributeCount; index++)
	{
		FbxNodeAttribute* attrib = node->GetNodeAttributeByIndex(index);

		if (attrib == nullptr) return false;

		FbxNodeAttribute::EType attribType = node->GetNodeAttribute()->GetAttributeType();

		if (attribType == FbxNodeAttribute::eMesh)
		{
			ModelSharedPtr model = std::dynamic_pointer_cast<Model>(modelSharedPtr);
			model->Name = node->GetNameOnly();
			// Do other stuff with the model
			BindModelAttributes(model, node);

			// and look for children
			int childCount = node->GetChildCount();
			for (int index = 0; index < childCount; index++)
			{
				NodeSharedPtr childModel(new Model());
				BuildModels(node->GetChild(index), childModel);

				model->Children.push_back(childModel);
			}

			retval = true;
		}
		else
		{
			ModelSharedPtr model = std::dynamic_pointer_cast<Model>(modelSharedPtr);
			model->Name = node->GetNameOnly();
			// This isn't great, because we're not building the transform hierarchy
			int childCount = node->GetChildCount();
			for (int index = 0; index < childCount; index++)
			{
				NodeSharedPtr childModel(new Model());
				BuildModels(node->GetChild(index), childModel);
				model->Children.push_back(childModel);
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
		ModelSharedPtr model = std::dynamic_pointer_cast<Model>(mSceneModels[index]);
		if (model != nullptr)
		{
			model->BuildRenderables();
		}
	}

	mCanRender = true;
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
