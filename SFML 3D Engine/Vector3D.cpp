#include "stdafx.h"
#include "Vector3D.h"

namespace tge {

}

tge::Vector3D::Vector3D(){}

tge::Vector3D::Vector3D(float x, float y, float z, float w )
	:x(x), y(y), z(z), w(w)
{}

float tge::Vector3D::Dot(const Vector3D& vec) const {
	return x * vec.x + y * vec.y + z * vec.z;
}

tge::Vector3D tge::Vector3D::Cross(const Vector3D& v) {
	float x = y * v.z - z * v.y;
	float y = z * v.x - x * v.z;
	float z = x * v.y - y * v.x;
	return Vector3D(x, y, z, w);
}

float tge::Vector3D::Length() const {
	return sqrtf(x * x + y * y + z * z);
}

tge::Vector3D tge::Vector3D::Normalized() const {
	float l = Length();
	return Vector3D(x / l, y / l, z / l, w);
}

void tge::Vector3D::Normalize() {
	float l = Length();
	x /= l; y /= l; z /= l;
}

void tge::Vector3D::ScaleByW() {
	x /= w;
	y /= w;
	z /= w;
	w = 1;
}




const tge::Vector3D tge::operator+(const Vector3D& vec1, const Vector3D& vec2) {
	return Vector3D(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z, vec1.w + vec2.w);
}

tge::Vector3D& tge::operator+=(Vector3D& vec1, const Vector3D& vec2)
{
	vec1.x += vec2.x;
	vec1.y += vec2.y;
	vec1.z += vec2.z;
	return vec1;
}


const tge::Vector3D tge::operator-(const Vector3D& vec1, const Vector3D& vec2) {
	return Vector3D(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z, vec1.w - vec2.w);
}

tge::Vector3D& tge::operator-=(Vector3D& vec1, const Vector3D& vec2)
{
	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
	vec1.z -= vec2.z;
	return vec1;
}

const tge::Vector3D tge::operator*(float scalar, const Vector3D& vector) {
	return Vector3D(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w);
}

const tge::Vector3D tge::operator*(const Vector3D& vector, float scalar) {
	return Vector3D(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w);
}

tge::Vector3D& tge::operator*=(Vector3D& vector, float scalar)
{
	vector.x *= scalar;
	vector.y *= scalar;
	vector.z *= scalar;
	return vector;
}

const tge::Vector3D tge::operator/(const Vector3D& vector, float scalar) {
	return Vector3D(vector.x / scalar, vector.y / scalar, vector.z / scalar, vector.w);
}

tge::Vector3D tge::operator-(Vector3D vector)
{
	vector.x *= -1;
	vector.y *= -1;
	vector.z *= -1;
	return vector;
}

float tge::DotProduct(const Vector3D& v1, const Vector3D& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

tge::Vector3D tge::CrossProduct(const Vector3D& v1, const Vector3D& v2) {
	float x = v1.y * v2.z - v1.z * v2.y;
	float y = v1.z * v2.x - v1.x * v2.z;
	float z = v1.x * v2.y - v1.y * v2.x;

	return Vector3D(x, y, z);
}



tge::Vector3D tge::Normalize(const Vector3D& v) {
	return v.Normalized();
}

// https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
tge::Vector3D tge::PlaneIntersect(const Vector3D& plane_point, const Vector3D& plane_normal,
	const Vector3D& lineStart, const Vector3D& lineEnd,
	float& t) {

	//plane_p - point on the plane
	//plane_n - normal to the plane
	//lineStart, End - starting and ending point of the line

	//d = (
	//punkt przeciecia = punk na lini + kierunek lini * d

	//t - znormalizowana wartosc (0,1) oznaczajaca procent lini w ktorym nastopilo przeciecie z plaszczyna
	// np. t = 0.5, linia przecina plaszczyne w polowei swojeje dlugosci 


	Vector3D lineDirection = lineEnd - lineStart;
	Vector3D planeNormal = plane_normal.Normalized();

	//Jak na wikipedii
	t = DotProduct(plane_point - lineStart, planeNormal) / DotProduct(lineDirection, planeNormal);

	Vector3D lineToIntersect = lineDirection * t;
	return lineStart + lineToIntersect;
	//Returns point where line intersects with a plane
}


