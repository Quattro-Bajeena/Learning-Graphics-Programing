#pragma once
#include "Matrix3D.h"
#include "Vector3D.h"
#include "VectorTex.h"

namespace tge {
class Triangle
{

public:
	Triangle();
	Triangle(Vector3D p1, Vector3D p2, Vector3D p3);
	Triangle(Vector3D p1, Vector3D p2, Vector3D p3, VectorTex uv1, VectorTex uv2, VectorTex uv3);

	Vector3D points[3];
	VectorTex uvCords[3];

	sf::Color color = sf::Color::White;

	void ScalePointsByW();
	void ScaleUvByW();
};


const Triangle operator*(const Matrix3D& mat, Triangle tri);
const Triangle operator*(Triangle tri, float scalar);
const Triangle operator+(Triangle tri, Vector3D vec);
int ClipAgainstPlane(const Vector3D& plane_p, const Vector3D& plane_n,
	Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);

}



