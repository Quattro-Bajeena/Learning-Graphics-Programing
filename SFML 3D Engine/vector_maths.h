#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <fstream>
#include <strstream>


#include "SFML/Graphics.hpp"
#include "SFML/Main.hpp"
#include "SFML/Window.hpp"



class VectorTex {
public:
	float u = 0;
	float v = 0;
	float w = 1;
	
};

class Vector3D {
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
	Vector3D(float x, float y, float z, float w = 1)
		:x(x), y(x), z(z), w(w)
	{}

	float Dot(const Vector3D& vec) const {
		return x * vec.x + y * vec.y + z * vec.z;
	}

	Vector3D Cross(const Vector3D& v) {
		float x = y * v.z - z * v.y;
		float y = z * v.x - x * v.z;
		float z = x * v.y - y * v.x;
		return Vector3D(x, y, z);
	}

	float Length() const {
		return sqrtf(x * x + y*y + z*z);
	}

	Vector3D Normalized() const {
		float l = Length();
		return Vector3D(x / l, y / l, z / l);
	}

	void Normalize() {
		float l = Length();
		x /= l; y /= l; z /= l;
	}

};

const Vector3D operator+(const Vector3D& vec1, const Vector3D& vec2) {
	return Vector3D(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z, vec1.w + vec2.w);
}

const Vector3D operator-(const Vector3D& vec1, const Vector3D& vec2) {
	return Vector3D(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z, vec1.w - vec2.w);
}

const Vector3D operator*(float scalar, const Vector3D& vector) {
	return Vector3D(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

const Vector3D operator*(const Vector3D& vector, float scalar) {
	return Vector3D(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

const Vector3D operator/(const Vector3D& vector, float scalar) {
	return Vector3D(vector.x / scalar, vector.y / scalar, vector.z / scalar);
}

float DotProduct(const Vector3D& v1, const Vector3D& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3D CrossProduct(const Vector3D& v1, const Vector3D& v2) {
	float x = v1.y * v2.z - v1.z * v2.y;
	float y = v1.z * v2.x - v1.x * v2.z;
	float z = v1.x * v2.y - v1.y * v2.x;
	return Vector3D(x, y, z);
}



Vector3D Normalize(const Vector3D& v) {
	return v.Normalized();
}

// https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
Vector3D PlaneIntersect(const Vector3D& plane_point, const Vector3D& plane_normal,
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
	return lineStart +  lineToIntersect;
	//Returns point where line intersects with a plane
}

class Matrix3D {
	float fields[4][4] = {};

public:
	float* operator[](const int x) {
		return fields[x];
	}

	const float* operator[](const int x) const{
		return fields[x];
	}

	

};


const Vector3D operator*(const Matrix3D& m,  const Vector3D& v) {
	Vector3D v1;
	v1.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
	v1.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
	v1.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
	v1.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
	return v1;
}

const Matrix3D operator*(const Matrix3D& m1, const Matrix3D& m2) {
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

const Matrix3D QuickInverse(const Matrix3D& m) {

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

const Matrix3D IdentityMatrix() {
	Matrix3D matrix;
	matrix[0][0] = 1.f;
	matrix[1][1] = 1.f;
	matrix[2][2] = 1.f;
	matrix[3][3] = 1.f;
	return matrix;
}

const Matrix3D RotMatX(float angleRad) {
	Matrix3D matrix;
	matrix[0][0] = 1;
	matrix[1][1] = cosf(angleRad);
	matrix[1][2] = sinf(angleRad);
	matrix[2][1] = -sinf(angleRad);
	matrix[2][2] = cosf(angleRad);
	matrix[3][3] = 1.f;
	return matrix;
}

const Matrix3D RotMatY(float angleRad) {
	Matrix3D matrix;
	matrix[0][0] = cosf(angleRad);
	matrix[0][2] = -sinf(angleRad);
	matrix[1][1] = 1;
	matrix[2][0] = sinf(angleRad);
	matrix[2][2] = cosf(angleRad);
	matrix[3][3] = 1.f;
	return matrix;
}

const Matrix3D RotMatZ(float angleRad) {
	Matrix3D matrix;
	matrix[0][0] = cosf(angleRad);
	matrix[0][1] = sinf(angleRad);
	matrix[1][0] = -sinf(angleRad);
	matrix[1][1] = cosf(angleRad);
	matrix[2][2] = 1.f;
	matrix[3][3] = 1.f;
	return matrix;
}

const Matrix3D TransMat(float x, float y, float z) {
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

Matrix3D Projection(float fFovDegrees, float fAspectRatio, float fNear, float fFar) {

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

class Triangle {
public:
	Vector3D points[3];
	VectorTex uvCords[3];

	sf::Color color;
};

class Mesh {



public:
	std::vector<Triangle> tris;

	


	bool LoadFromObjectFile(std::string sFilename, bool bHasTexture = false) {
		std::ifstream f(sFilename);
		if (!f.is_open())
			return false;

		//local chache of verts
		std::vector<Vector3D> verts;
		std::vector<VectorTex> texs;

		while (!f.eof()) {
			char line[128];
			f.getline(line, 128);

			std::strstream s;
			s << line;
			char junk;
			if (line[0] == 'v') {
				if (line[1] == 't') {

				}
				else {
					Vector3D v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}

			}

			if (!bHasTexture) {
				if (line[0] == 'f') {
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					tris.push_back(
						{ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] }
					);
				}
			}
			else {
				if (line[0] == 'f') {
					s >> junk;

					std::string tokens[6];
					int nTokenCount = -1;


					while (!s.eof())
					{
						char c = s.get();
						if (c == ' ' || c == '/')
							nTokenCount++;
						else
							tokens[nTokenCount].append(1, c);
					}

					tokens[nTokenCount].pop_back();


					tris.push_back({ verts[std::stoi(tokens[0]) - 1], verts[std::stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
						texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });
				}
			}

		}

		return true;
	}
};



