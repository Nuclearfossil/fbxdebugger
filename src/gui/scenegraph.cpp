#include "fbxcontainer/Node.h"
#include "fbxcontainer/Model.h"
#include "fbxcontainer/scenecontainer.h"
#include "misc/Utils.h"

#include "scenegraph.h"

#include <atomic>
#include <iostream>

std::atomic<unsigned int> IdIncrementor;

VisualSceneNode::VisualSceneNode()
{
	mId = IdIncrementor++;
	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	SetScale(0.0f, 0.0f, 0.0f);

	mName = std::to_string(mId);
}

VisualSceneNode::~VisualSceneNode() {}

void VisualSceneNode::Name(const char* value)
{
	mName = value;
}

void VisualSceneNode::Name(const std::string value)
{
	mName = value;
}

const std::string& VisualSceneNode::Name()
{
	return mName;
}

void VisualSceneNode::AddChild(SceneNodeSharedPtr child)
{
	mChildren.push_back(child);
}

void VisualSceneNode::SetPosition(float x, float y, float z)
{
	mTranslation.x = x;
	mTranslation.y = y;
	mTranslation.z = z;
}

glm::vec3 VisualSceneNode::GetPosition()
{
	return mTranslation;
}

void VisualSceneNode::SetRotation(float yaw, float pitch, float roll)
{
	mEuler.x = pitch;
	mEuler.y = yaw;
	mEuler.z = roll;
}

glm::vec3 VisualSceneNode::GetRotation()
{
	return mEuler;
}

void VisualSceneNode::SetScale(float x, float y, float z)
{
	mScale.x = x;
	mScale.y = y;
	mScale.z = z;
}

glm::vec3 VisualSceneNode::GetScale()
{
	return mScale;
}

VisualSceneGraph::VisualSceneGraph() {}
VisualSceneGraph::~VisualSceneGraph() {}

void VisualSceneGraph::Build(FbxImporter* importer, FbxScene* scene)
{
	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == nullptr) return;

	Reset();

	int rootNodeCount = rootNode->GetChildCount();

	for (int index = 0; index < rootNodeCount; index++)
	{
		auto node = rootNode->GetChild(index);

		SceneNodeSharedPtr sceneNode(new VisualSceneNode());
		sceneNode->Setup(node);

		mSceneGraph.push_back(sceneNode);
	}
}

void VisualSceneGraph::BuildRenderables()
{
	for each (auto node in mSceneGraph)
	{
		auto components = node->GetComponents();
		for each (auto component in components)
		{
			if (component->TypeName == "MeshComponent")
			{
				MeshComponentSharedPtr meshComponent = std::dynamic_pointer_cast<MeshComponent>(component);
				meshComponent->mModel->BuildRenderables();
			}
		}
	}
}

void VisualSceneGraph::Reset()
{
	mSceneGraph.clear();
	mAnimations.clear();
}

void BuildMeshFromAttribute(ModelSharedPtr model, FbxNodeAttribute* attr)
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

void VisualSceneNode::Setup(FbxNode* fbxNode)
{
	Name(fbxNode->GetNameOnly());
	auto position = fbxNode->GeometricTranslation.Get();
	auto rotation = fbxNode->GeometricRotation.Get();
	auto scale = fbxNode->GeometricScaling.Get();
	SetPosition(DoubleToFloat(position[0]), DoubleToFloat(position[1]), DoubleToFloat(position[2]));
	SetRotation(DoubleToFloat(rotation[0]), DoubleToFloat(rotation[1]), DoubleToFloat(rotation[2]));
	SetScale(DoubleToFloat(scale[0]), DoubleToFloat(scale[1]), DoubleToFloat(scale[2]));

	// What component to we want to add?
	// we currently support two - Triangulated Mesh and Camera
	// anything else should be considered a null (for now)
	int attributeCount = fbxNode->GetNodeAttributeCount();
	for (int attributeIndex = 0; attributeIndex < attributeCount; attributeIndex++)
	{
		FbxNodeAttribute* attrib = fbxNode->GetNodeAttributeByIndex(attributeIndex);

		if (attrib != nullptr)
		{
			FbxNodeAttribute::EType attribType = fbxNode->GetNodeAttribute()->GetAttributeType();

			switch (attribType)
			{
			case fbxsdk::FbxNodeAttribute::eUnknown:
				break;
			case fbxsdk::FbxNodeAttribute::eNull:
				break;
			case fbxsdk::FbxNodeAttribute::eMarker:
				break;
			case fbxsdk::FbxNodeAttribute::eSkeleton:
			{
				ComponentSharedPtr component(new SkeletonComponent());
				mComponents.push_back(component);
			}
			break;
			case fbxsdk::FbxNodeAttribute::eMesh:
			{
				MeshComponentSharedPtr component(new MeshComponent());
				ModelSharedPtr model(new Model());
				BuildMeshFromAttribute(model, attrib);
				component->mModel = model;
				mComponents.push_back(component);
			}
				break;
			case fbxsdk::FbxNodeAttribute::eNurbs:
				break;
			case fbxsdk::FbxNodeAttribute::ePatch:
				break;
			case fbxsdk::FbxNodeAttribute::eCamera:
			{
				ComponentSharedPtr component(new CameraComponent());
				mComponents.push_back(component);
			}
				break;
			case fbxsdk::FbxNodeAttribute::eCameraStereo:
				break;
			case fbxsdk::FbxNodeAttribute::eCameraSwitcher:
				break;
			case fbxsdk::FbxNodeAttribute::eLight:
				break;
			case fbxsdk::FbxNodeAttribute::eOpticalReference:
				break;
			case fbxsdk::FbxNodeAttribute::eOpticalMarker:
				break;
			case fbxsdk::FbxNodeAttribute::eNurbsCurve:
				break;
			case fbxsdk::FbxNodeAttribute::eTrimNurbsSurface:
				break;
			case fbxsdk::FbxNodeAttribute::eBoundary:
				break;
			case fbxsdk::FbxNodeAttribute::eNurbsSurface:
				break;
			case fbxsdk::FbxNodeAttribute::eShape:
				break;
			case fbxsdk::FbxNodeAttribute::eLODGroup:
				break;
			case fbxsdk::FbxNodeAttribute::eSubDiv:
				break;
			case fbxsdk::FbxNodeAttribute::eCachedEffect:
				break;
			case fbxsdk::FbxNodeAttribute::eLine:
				break;
			default:
				break;
			}
		}
	}

	int childCount = fbxNode->GetChildCount();
	for (int index = 0; index < childCount; index++)
	{
		SceneNodeSharedPtr child(new VisualSceneNode());
		child->Setup(fbxNode->GetChild(index));
		AddChild(child);
	}
}

void MeshComponent::Init()
{

}
bool MeshComponent::Update()
{
	return false;
}

void CameraComponent::Init()
{

}

bool CameraComponent::Update()
{
	return false;
}


void SkeletonComponent::Init()
{

}

bool SkeletonComponent::Update()
{
	return false;
}
