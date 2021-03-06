#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <BasicCamera/Camera.h>
#include <BasicCamera/Fwd.h>

class basicCamera::OrbitCamera : public CameraTransform
{
public:
	OrbitCamera(float distance = 1);
	virtual ~OrbitCamera();

	virtual glm::mat4 getView();

	glm::mat4         getRotation();
	float             getXAngle() const;
	float             getYAngle() const;
	float             getDistance() const;
	glm::vec3         getFocus() const;
	void              setXAngle(float value);
	void              setYAngle(float value);
	void              setAngles(glm::vec2 const& angles);
	void              setDistance(float value);
	void              setFocus(glm::vec3 const& value);
	void              addXPosition(float dx);
	void              addYPosition(float dy);
	void              addXYPosition(glm::vec2 const& value);
	void              addXAngle(float value);
	void              addYAngle(float value);
	void              addAngles(glm::vec2 const& value);
	void              addDistance(float value);

protected:
	float     _xAngle    = 0.f;
	float     _yAngle    = 0.f;
	float     _distance  = 0.f;
	bool      _recompute = true;
	glm::vec3 _position  = glm::vec3(0.f);
	glm::mat4 _view      = glm::mat4(1.f);
	glm::mat4 _rotation  = glm::mat4(1.f);
	void      _compute();
};
