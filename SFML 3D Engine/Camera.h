#pragma once
#include "Matrix3D.h"
#include "Transform.h"
#include "GameObject.h"

namespace tge {
;

class Camera : public GameObject {

private:


	float fov;
	float aspectRatio;
	float near;
	float far;



public:

	Camera();
	Camera(float fov, float aspect_ratio, float near, float far);

	Matrix3D GetProjMatrix() const;
	Vector3D LookDir() const;

};

}

