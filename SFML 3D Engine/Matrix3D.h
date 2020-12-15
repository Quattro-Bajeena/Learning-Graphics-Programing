#pragma once
#include "Vector3D.h"

namespace tge {
class Matrix3D
{
	float fields[4][4] = {};

public:
	float* operator[](const int x);
	const float* operator[](const int x) const;

};


const Vector3D operator*(const Matrix3D& m, Vector3D v);
const Matrix3D operator*(const Matrix3D& m1, const Matrix3D& m2);

const Matrix3D MatQuickInverse(const Matrix3D& m);
const Matrix3D IdentityMatrix();
const Matrix3D MatRotX(float angleRad);
const Matrix3D MatRotY(float angleRad);
const Matrix3D MatRotZ(float angleRad);
const Matrix3D MatTrans(float x, float y, float z);
Matrix3D MatProject(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
Matrix3D MatLookAt(const Vector3D& pos, const Vector3D& target, const Vector3D up);
}


