#pragma once

#include "Node.h"

class Camera;
typedef std::shared_ptr<Camera> CameraSharedPtr;

class Camera : public Node
{
public:
	Camera()
	{
		NodeType = "Camera";
	}

	~Camera() {}
};