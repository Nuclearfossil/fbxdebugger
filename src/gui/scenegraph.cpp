#include "fbxcontainer/Node.h"
#include "fbxcontainer/Model.h"
#include "fbxcontainer/scenecontainer.h"

#include "scenegraph.h"

#include <atomic>

std::atomic<unsigned int> IdIncrementor;

VisualSceneNode::VisualSceneNode() 
{
	mId = IdIncrementor++;
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

void VisualSceneGraph::Build(SceneContainer* container)
{
	if (container == nullptr)
	{
		return;
	}

	// Clean up anything lying around
	Reset();

	// Build up the animation list
	BuildGeometryTree(container->GetMeshs());
}

void VisualSceneGraph::Reset()
{
	mGeometry.clear();
	mAnimations.clear();
}

void VisualSceneGraph::BuildGeometryTree(std::vector<NodeSharedPtr> meshNodes)
{
	for each (auto mesh in meshNodes)
	{
		SceneNodeSharedPtr sceneNode(new VisualSceneNode());
		BuildSceneTreeFromMeshNode(sceneNode, mesh);
		mGeometry.push_back(sceneNode);
	}
}

void VisualSceneGraph::BuildSceneTreeFromMeshNode(SceneNodeSharedPtr rootNode, NodeSharedPtr mesh)
{
	// First, populate the sceneNode with the right data from the mesh
	rootNode->Name(mesh->Name);
	rootNode->Mesh = mesh;

	for each (auto child in mesh->Children)
	{
		SceneNodeSharedPtr sceneNode(new VisualSceneNode());
		BuildSceneTreeFromMeshNode(sceneNode, child);
		rootNode->AddChild(sceneNode);
	}
}
