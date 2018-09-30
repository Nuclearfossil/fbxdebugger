#pragma once

#include <vector>
#include <string>
#include <memory>

#include "glm/mat4x4.hpp"

// forward Declarations
class VisualSceneNode;
class Component;
class SceneContainer;

// Typedefs for shared pointers
typedef std::shared_ptr<VisualSceneNode> SceneNodeSharedPtr;
typedef std::shared_ptr<Component> ComponentSharedPtr;

class Component
{
public:
	Component() {}
	~Component() {}

	bool Update() { return false; }
};

class VisualSceneNode
{
public:
	VisualSceneNode();
	~VisualSceneNode();

	void Name(const char* value);
	void Name(const std::string value);
	const std::string& Name();
	unsigned int Id() { return mId; }

	void AddChild(SceneNodeSharedPtr child);
	std::vector<SceneNodeSharedPtr> GetChildren() { return mChildren; }

	void SetPosition(float x, float y, float z);
	glm::vec3 GetPosition();

	void SetRotation(float yaw, float pitch, float roll);
	glm::vec3 GetRotation();

	void SetScale(float x, float y, float z);
	glm::vec3 GetScale();

	NodeSharedPtr Mesh;

private:
	unsigned int mId;
	std::string mName;

	glm::mat4 mTransform;

	glm::vec3 mTranslation;
	glm::vec3 mEuler;
	glm::vec3 mScale;

	std::vector<SceneNodeSharedPtr> mChildren;
	std::vector<ComponentSharedPtr> mComponents;
};

class VisualSceneGraph
{
public:
	VisualSceneGraph();
	~VisualSceneGraph();

	void Build(SceneContainer* container);
	std::vector<SceneNodeSharedPtr> Geometry() { return mGeometry; }

private:
	void Reset();
	void BuildGeometryTree(std::vector<NodeSharedPtr> meshNodes);
	void BuildSceneTreeFromMeshNode(SceneNodeSharedPtr sceneNode, NodeSharedPtr mesh);

private:
	std::vector<SceneNodeSharedPtr> mGeometry;
	std::vector<SceneNodeSharedPtr> mAnimations;

};