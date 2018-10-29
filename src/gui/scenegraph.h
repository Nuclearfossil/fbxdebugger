#pragma once

#include <vector>
#include <string>
#include <memory>

#include "glm/mat4x4.hpp"

// forward Declarations
class VisualSceneNode;
class Component;
class MeshComponent;
class SceneContainer;

namespace fbxsdk
{
	class FbxImporter;
	class FbxScene;
}

// Typedefs for shared pointers
typedef std::shared_ptr<VisualSceneNode> SceneNodeSharedPtr;
typedef std::shared_ptr<Component> ComponentSharedPtr;
typedef std::shared_ptr<MeshComponent> MeshComponentSharedPtr;

class Component
{
public:
	Component() {}
	~Component() {}

	virtual bool Update() = 0;

	std::string TypeName; // Replace with a hash at some point (or enum)
};

class MeshComponent : public Component
{
public:
	MeshComponent()
	{
		TypeName = "MeshComponent";
	}
	~MeshComponent() {}

	void Init();
	virtual bool Update() override;

	ModelSharedPtr mModel;
};

class CameraComponent : public Component
{
public:
	CameraComponent()
	{
		TypeName = "CameraComponent";
	};
	~CameraComponent() {}

	void Init();
	virtual bool Update() override;
};

class SkeletonComponent : public Component
{
public:
	SkeletonComponent()
	{
		TypeName = "SkeletonComponent";
	}
	~SkeletonComponent() {}

	void Init();
	virtual bool Update() override;
};

class VisualSceneNode
{
public:
	VisualSceneNode();
	~VisualSceneNode();

	void Setup(FbxNode* fbxNode);

	void Name(const char* value);
	void Name(const std::string value);
	const std::string& Name();
	unsigned int Id() { return mId; }

	void AddChild(SceneNodeSharedPtr child);
	std::vector<SceneNodeSharedPtr> GetChildren() { return mChildren; }

	std::vector<ComponentSharedPtr> GetComponents() { return mComponents; }

	void SetPosition(float x, float y, float z);
	glm::vec3 GetPosition();

	void SetRotation(float yaw, float pitch, float roll);
	glm::vec3 GetRotation();

	void SetScale(float x, float y, float z);
	glm::vec3 GetScale();

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

	void Build(FbxImporter* importer, FbxScene* scene);
	void BuildRenderables();

	std::vector<SceneNodeSharedPtr> Children() { return mSceneGraph; }

private:
	void Reset();

private:
	std::vector<SceneNodeSharedPtr> mSceneGraph;
	std::vector<SceneNodeSharedPtr> mAnimations;

};