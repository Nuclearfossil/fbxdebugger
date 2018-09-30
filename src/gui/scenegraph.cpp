#include "scenegraph.h"

#include <atomic>

std::atomic<unsigned int> IdIncrementor;

SceneNode::SceneNode() 
{
	mId = IdIncrementor++;
	mName = std::to_string(mId);
}

void SceneNode::SetPosition(float x, float y, float z)
{
	mTranslation.x = x;
	mTranslation.y = y;
	mTranslation.z = z;
}

glm::vec3 SceneNode::GetPosition()
{
	return mTranslation;
}

void SceneNode::SetRotation(float yaw, float pitch, float roll)
{
	mEuler.x = pitch;
	mEuler.y = yaw;
	mEuler.z = roll;
}

glm::vec3 SceneNode::GetRotation()
{
	return mEuler;
}


SceneNode::~SceneNode() {}

SceneGraph::SceneGraph() {}
SceneGraph::~SceneGraph() {}

