#include "stdafx.h"
#include "Transform.h"


tge::Transform::Transform()
{
}

void tge::Transform::Translate(Vector3D translation)
{
	position = position + translation;
}

void tge::Transform::Rotate(Vector3D rotation)
{
	this->rotation = this->rotation + rotation;
}

void tge::Transform::SetPosition(Vector3D position)
{
	this->position = position;
}

void tge::Transform::SetRotation(Vector3D rotation)
{
	this->rotation = rotation;
}

tge::Vector3D tge::Transform::GetPosition() const
{
	return position;
}

tge::Vector3D tge::Transform::GetRotation() const
{
	return rotation;
}

tge::Matrix3D tge::Transform::TransformMatrix() const
{
	Matrix3D translation = MatTrans(position.x, position.y, position.z);
	Matrix3D rotationX = MatRotX(rotation.x);
	Matrix3D rotationY = MatRotY(rotation.y);
	Matrix3D rotationZ = MatRotZ(rotation.z);

	//std::cout << position.x <<", " << position.y << ", " << position.z << "|" << rotation.x << ", " << rotation.y << ", " << rotation.z << "\n";

	/*
	When we say the associativity of * is left-to-right,
	we mean that the expression a*b*c*d will always evaluate as (((a*b)*c)*d)
	*/
	return translation * rotationX * rotationY * rotationZ;
}
