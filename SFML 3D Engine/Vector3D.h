#pragma once

namespace tge {


class Vector3D
{
private:


public:
	/*
	-w is a measure of a things groundedness in real spcae, and therfore
	-w is a measrue of thing's trnalateability
	-W i mostly 0 for directions, 1 for positions
	-w is 1 for MAtrix translation
	-thing with w values other than 1 or 0 need to be divided by their w to come back into real space
	-directions are kinda like points in the infinite distance

	*/
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;


	Vector3D();
	Vector3D(float x, float y, float z, float w = 1);

	float Dot(const Vector3D& vec) const;
	Vector3D Cross(const Vector3D& v);
	float Length() const;
	Vector3D Normalized() const;
	void Normalize();
	void ScaleByW();


};


const Vector3D operator+(const Vector3D& vec1, const Vector3D& vec2);
Vector3D& operator+=(Vector3D& vec1, const Vector3D& vec2);
const Vector3D operator-(const Vector3D& vec1, const Vector3D& vec2);
Vector3D& operator-=(Vector3D& vec1, const Vector3D& vec2);
const Vector3D operator*(float scalar, const Vector3D& vector);
const Vector3D operator*(const Vector3D& vector, float scalar);
Vector3D& operator*=(Vector3D& vector, float scalar);

const Vector3D operator/(const Vector3D& vector, float scalar);

Vector3D operator-(Vector3D vector);


float DotProduct(const Vector3D& v1, const Vector3D& v2);
Vector3D CrossProduct(const Vector3D& v1, const Vector3D& v2);
Vector3D Normalize(const Vector3D& v);

// https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
Vector3D PlaneIntersect(const Vector3D& plane_point, const Vector3D& plane_normal,
	const Vector3D& lineStart, const Vector3D& lineEnd,
	float& t);

}



