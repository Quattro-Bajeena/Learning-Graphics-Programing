#include "stdafx.h"
#include "Camera.h"

namespace tge {
;

tge::Camera::Camera()
	:fov(90), aspectRatio(16/9), near(0.1), far(1000)
{

}

tge::Camera::Camera(float fov, float aspect_ratio, float near, float far)
	:fov(fov), aspectRatio(aspect_ratio), near(near), far(far)
{

}

Matrix3D Camera::GetProjMatrix() const
{
	return MatProject(fov, aspectRatio, near, far);
}

Vector3D Camera::LookDir() const
{
	return MatRotZ(transform.rotation.z) * MatRotX(transform.rotation.x) * MatRotY(transform.rotation.y) * Vector3D(0, 0, 1);
}


}