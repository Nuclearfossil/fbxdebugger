#pragma once

#include <vector>
#include <string>
#include <memory>

#include "glm/mat4x4.hpp"

class SceneNode;

typedef std::shared_ptr<SceneNode> SceneNodeSharedPtr;

class SceneNode
{
public:
	SceneNode();
	~SceneNode();

	void SetPosition(float x, float y, float z);
	glm::vec3 GetPosition();

	void SetRotation(float yaw, float pitch, float roll);
	glm::vec3 GetRotation();

private:
	unsigned int mId;
	std::string mName;
	glm::mat4 mTransform;
	glm::vec3 mTranslation;
	glm::vec3 mEuler;
	glm::vec3 mScale;

	std::vector<SceneNodeSharedPtr> mChildren;
};

class SceneGraph
{
public:
	SceneGraph();
	~SceneGraph();


private:
	std::vector<SceneNodeSharedPtr> mGeometry;
	std::vector<SceneNodeSharedPtr> mAnimations;

};