#pragma once
#include "Vector3D.h"
#include "Matrix3D.h"
namespace tge {


class Transform
{
private:

public:
	Vector3D position;
	Vector3D rotation;
	

public:
	Transform();

	void Translate(Vector3D translation);
	void Rotate(Vector3D rotation);

	void SetPosition(Vector3D position);
	void SetRotation(Vector3D rotation);

	Vector3D GetPosition() const;
	Vector3D GetRotation() const;

	Matrix3D TransformMatrix() const;

};

}


