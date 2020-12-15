#include "stdafx.h"
#include "Matrix3D.h"





float* tge::Matrix3D::operator[](const int x) {
	return fields[x];
}

const float* tge::Matrix3D::operator[](const int x) const {
	return fields[x];
}



const tge::Vector3D tge::operator*(const Matrix3D& m, Vector3D v) {
	Vector3D v1 = v;
	v1.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
	v1.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
	v1.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
	v1.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
	return v1;
}

const tge::Matrix3D tge::operator*(const Matrix3D& m1, const Matrix3D& m2) {
	Matrix3D matrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix[c][r] =
			m1[0][r] * m2[c][0] +
			m1[1][r] * m2[c][1] +
			m1[2][r] * m2[c][2] +
			m1[3][r] * m2[c][3];
	return matrix;

}

const tge::Matrix3D tge::MatQuickInverse(const Matrix3D& m) {

	/*
	You should be able to exploit the fact that the matrix is affine to speed things up over a full inverse.
	Namely, if your matrix looks like this

		A = [ M   b  ]
			[ 0   1  ]

	where A is 4x4, M is 3x3, b is 3x1, and the bottom row is (0,0,0,1), then

		inv(A) = [ inv(M)   -inv(M) * b ]
					[   0            1     ]

	*/
	Matrix3D matrix;
	matrix[0][0] = m[0][0];	matrix[0][1] = m[1][0]; matrix[0][2] = m[2][0]; matrix[0][3] = 0.0f;
	matrix[1][0] = m[0][1]; matrix[1][1] = m[1][1]; matrix[1][2] = m[2][1]; matrix[1][3] = 0.0f;
	matrix[2][0] = m[0][2]; matrix[2][1] = m[1][2]; matrix[2][2] = m[2][2]; matrix[2][3] = 0.0f;
	matrix[3][0] = -(m[3][0] * matrix[0][0] + m[3][1] * matrix[1][0] + m[3][2] * matrix[2][0]);
	matrix[3][1] = -(m[3][0] * matrix[0][1] + m[3][1] * matrix[1][1] + m[3][2] * matrix[2][1]);
	matrix[3][2] = -(m[3][0] * matrix[0][2] + m[3][1] * matrix[1][2] + m[3][2] * matrix[2][2]);
	matrix[3][3] = 1.0f;
	return matrix;
}

const tge::Matrix3D tge::IdentityMatrix() {
	Matrix3D matrix;
	matrix[0][0] = 1.f;
	matrix[1][1] = 1.f;
	matrix[2][2] = 1.f;
	matrix[3][3] = 1.f;
	return matrix;
}

const tge::Matrix3D tge::MatRotX(float angleRad) {
	Matrix3D matrix;
	matrix[0][0] = 1;
	matrix[1][1] = cosf(angleRad);
	matrix[1][2] = sinf(angleRad);
	matrix[2][1] = -sinf(angleRad);
	matrix[2][2] = cosf(angleRad);
	matrix[3][3] = 1.f;
	return matrix;
}

const tge::Matrix3D tge::MatRotY(float angleRad) {
	Matrix3D matrix;
	matrix[0][0] = cosf(angleRad);
	matrix[0][2] = -sinf(angleRad);
	matrix[1][1] = 1;
	matrix[2][0] = sinf(angleRad);
	matrix[2][2] = cosf(angleRad);
	matrix[3][3] = 1.f;
	return matrix;
}

const tge::Matrix3D tge::MatRotZ(float angleRad) {
	Matrix3D matrix;
	matrix[0][0] = cosf(angleRad);
	matrix[0][1] = sinf(angleRad);
	matrix[1][0] = -sinf(angleRad);
	matrix[1][1] = cosf(angleRad);
	matrix[2][2] = 1.f;
	matrix[3][3] = 1.f;
	return matrix;
}

const tge::Matrix3D tge::MatTrans(float x, float y, float z) {
	Matrix3D matrix;
	matrix[0][0] = 1.f;
	matrix[1][1] = 1.f;
	matrix[2][2] = 1.f;
	matrix[3][3] = 1.f;
	matrix[3][0] = x;
	matrix[3][1] = y;
	matrix[3][2] = z;
	return matrix;
}

tge::Matrix3D tge::MatProject(float fFovDegrees, float fAspectRatio, float fNear, float fFar) {

	float fFovRad = 1.f / tanf(fFovDegrees * 0.5f / 180.f * M_PI);

	Matrix3D matrix;
	matrix[0][0] = fAspectRatio * fFovRad;
	matrix[1][1] = fFovRad;
	matrix[2][2] = fFar / (fFar - fNear);
	matrix[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix[2][3] = 1.f;
	matrix[3][3] = 0.f;
	return matrix;
}

tge::Matrix3D tge::MatLookAt(const Vector3D& pos, const Vector3D& target, const Vector3D up) {
	//Calculate new forward direction
	Vector3D newForward = (target - pos).Normalized();


	//Calculate new Up direction
	//I dont get it
	//a -> projection of old up vector on newForward vecotr

	Vector3D a = newForward * DotProduct(up, newForward);
	Vector3D newUp = (up - a).Normalized();



	Vector3D newRight = CrossProduct(newUp, newForward);

	// Construct Dimensioning and Translation Matrix	
	Matrix3D matrix;
	matrix[0][0] = newRight.x;		matrix[0][1] = newRight.y;		matrix[0][2] = newRight.z;		matrix[0][3] = 0.0f;
	matrix[1][0] = newUp.x;			matrix[1][1] = newUp.y;			matrix[1][2] = newUp.z;			matrix[1][3] = 0.0f;
	matrix[2][0] = newForward.x;	matrix[2][1] = newForward.y;	matrix[2][2] = newForward.z;	matrix[2][3] = 0.0f;
	matrix[3][0] = pos.x;			matrix[3][1] = pos.y;			matrix[3][2] = pos.z;			matrix[3][3] = 1.0f;
	return matrix;
}