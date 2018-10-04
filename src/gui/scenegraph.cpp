#include "fbxcontainer/Node.h"
#include "fbxcontainer/Model.h"
#include "fbxcontainer/scenecontainer.h"

#include "scenegraph.h"

#include <atomic>

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

void VisualSceneNode::Setup(FbxNode* fbxNode)
{
	Name(fbxNode->GetNameOnly());
	auto position = fbxNode->GeometricTranslation.Get();
	auto rotation = fbxNode->GeometricRotation.Get();
	auto scale = fbxNode->GeometricScaling.Get();
	SetPosition(position[0], position[1], position[2]);
	SetRotation(rotation[0], rotation[1], rotation[2]);
	SetScale(scale[0], scale[1], scale[2]);

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
				ComponentSharedPtr component(new MeshComponent());
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

void VisualSceneGraph::Reset()
{
	mSceneGraph.clear();
	mAnimations.clear();
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
