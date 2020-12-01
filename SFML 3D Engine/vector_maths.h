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

	void ScaleByW() {
		u /= w;
		v /= w;
		w = 1;
	}
	
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


	Vector3D() {};
	Vector3D(float x, float y, float z, float w = 1)
		:x(x), y(y), z(z), w(w)
	{}

	float Dot(const Vector3D& vec) const {
		return x * vec.x + y * vec.y + z * vec.z;
	}

	Vector3D Cross(const Vector3D& v) {
		float x = y * v.z - z * v.y;
		float y = z * v.x - x * v.z;
		float z = x * v.y - y * v.x;
		return Vector3D(x, y, z, w);
	}

	float Length() const {
		return sqrtf(x * x + y*y + z*z);
	}

	Vector3D Normalized() const {
		float l = Length();
		return Vector3D(x / l, y / l, z / l, w);
	}

	void Normalize() {
		float l = Length();
		x /= l; y /= l; z /= l;
	}

	void ScaleByW(){
		x /= w;
		y /= w;
		z /= w;
		w = 1;
	}

};

const Vector3D operator+(const Vector3D& vec1, const Vector3D& vec2) {
	return Vector3D(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z, vec1.w + vec2.w);
}

const Vector3D operator-(const Vector3D& vec1, const Vector3D& vec2) {
	return Vector3D(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z, vec1.w - vec2.w);
}

const Vector3D operator*(float scalar, const Vector3D& vector) {
	return Vector3D(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w);
}

const Vector3D operator*(const Vector3D& vector, float scalar) {
	return Vector3D(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w);
}

const Vector3D operator/(const Vector3D& vector, float scalar) {
	return Vector3D(vector.x / scalar, vector.y / scalar, vector.z / scalar, vector.w);
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


const Vector3D operator*(const Matrix3D& m,   Vector3D v) {
	Vector3D v1 = v;
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

const Matrix3D MatQuickInverse(const Matrix3D& m) {

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

const Matrix3D MatRotX(float angleRad) {
	Matrix3D matrix;
	matrix[0][0] = 1;
	matrix[1][1] = cosf(angleRad);
	matrix[1][2] = sinf(angleRad);
	matrix[2][1] = -sinf(angleRad);
	matrix[2][2] = cosf(angleRad);
	matrix[3][3] = 1.f;
	return matrix;
}

const Matrix3D MatRotY(float angleRad) {
	Matrix3D matrix;
	matrix[0][0] = cosf(angleRad);
	matrix[0][2] = -sinf(angleRad);
	matrix[1][1] = 1;
	matrix[2][0] = sinf(angleRad);
	matrix[2][2] = cosf(angleRad);
	matrix[3][3] = 1.f;
	return matrix;
}

const Matrix3D MatRotZ(float angleRad) {
	Matrix3D matrix;
	matrix[0][0] = cosf(angleRad);
	matrix[0][1] = sinf(angleRad);
	matrix[1][0] = -sinf(angleRad);
	matrix[1][1] = cosf(angleRad);
	matrix[2][2] = 1.f;
	matrix[3][3] = 1.f;
	return matrix;
}

const Matrix3D MatTrans(float x, float y, float z) {
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

Matrix3D MatProject(float fFovDegrees, float fAspectRatio, float fNear, float fFar) {

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

Matrix3D MatLookAt(const Vector3D& pos, const Vector3D& target, const Vector3D up) {
	//Calculate new forward direction
	Vector3D newForward = (target - pos).Normalized();
	

	//Calculate new Up direction
	//I dont get it
	//a -> projection of old up vector on newForward vecotr

	Vector3D a = newForward *  DotProduct(up, newForward);
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

class Triangle {
public:
	Triangle() {};

	Triangle(Vector3D p1, Vector3D p2, Vector3D p3) {
		points[0] = p1; points[1] = p2; points[2] = p3;
	}

	Triangle(Vector3D p1, Vector3D p2, Vector3D p3, VectorTex uv1, VectorTex uv2, VectorTex uv3) {
		points[0] = p1; points[1] = p2; points[2] = p3;
		uvCords[0] = uv1; uvCords[1] = uv2, uvCords[2] = uv3;
	}

	Vector3D points[3];
	VectorTex uvCords[3];

	sf::Color color = sf::Color::White;

	void ScalePointsByW() {
		for(int i = 0; i < 3; i++)
			points[i].ScaleByW();
	}

	void ScaleUvByW() {
		for (int i = 0; i < 3; i++)
			uvCords[i].ScaleByW();
	}

	
};

const Triangle operator*(const Matrix3D& mat, Triangle tri) {
	tri.points[0] = mat * tri.points[0];
	tri.points[1] = mat * tri.points[1];
	tri.points[2] = mat * tri.points[2];
	return tri;

}

const Triangle operator*(Triangle tri, float scalar) {
	tri.points[0] = tri.points[0] * scalar;
	tri.points[1] = tri.points[1] * scalar;
	tri.points[2] = tri.points[2] * scalar;
	return tri;
}

const Triangle operator+(Triangle tri, Vector3D vec) {
	tri.points[0] = tri.points[0] + vec;
	tri.points[1] = tri.points[1] + vec;
	tri.points[2] = tri.points[2] + vec;
	return tri;
}

int ClipAgainstPlane(const Vector3D& plane_p, const Vector3D& plane_n,
	Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2)
{
	// Make sure plane normal is indeed normal
	Vector3D planeNormal = Normalize(plane_n);

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](Vector3D& p)
	{
		//Vector3D n = Vector_Normalize(p);

		//To samo
		// float dist = dotProduct(plane_normal, (vectorSubtract(point, plane_point)));

		return (planeNormal.x * p.x + planeNormal.y * p.y + planeNormal.z * p.z - DotProduct(planeNormal, plane_p));
	};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	Vector3D* inside_points[3];  int nInsidePointCount = 0;
	Vector3D* outside_points[3]; int nOutsidePointCount = 0;
	VectorTex* inside_tex[3]; int nInsideTexCount = 0;
	VectorTex* outside_tex[3]; int nOutsideTexCount = 0;

	// Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.points[0]);
	float d1 = dist(in_tri.points[1]);
	float d2 = dist(in_tri.points[2]);

	if (d0 >= 0) { 
		inside_points[nInsidePointCount++] = &in_tri.points[0];
		inside_tex[nInsideTexCount++] = &in_tri.uvCords[0];
	}
	else {
		outside_points[nOutsidePointCount++] = &in_tri.points[0];
		outside_tex[nOutsideTexCount++] = &in_tri.uvCords[0];
	}
	if (d1 >= 0) {
		inside_points[nInsidePointCount++] = &in_tri.points[1];
		inside_tex[nInsideTexCount++] = &in_tri.uvCords[1];
	}
	else {
		outside_points[nOutsidePointCount++] = &in_tri.points[1];
		outside_tex[nOutsideTexCount++] = &in_tri.uvCords[1];
	}
	if (d2 >= 0) {
		inside_points[nInsidePointCount++] = &in_tri.points[2];
		inside_tex[nInsideTexCount++] = &in_tri.uvCords[2];
	}
	else {
		outside_points[nOutsidePointCount++] = &in_tri.points[2];
		outside_tex[nOutsideTexCount++] = &in_tri.uvCords[2];
	}

	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// Copy appearance info to new triangle
		out_tri1.color = in_tri.color;


		// The inside point is valid, so keep that...
		out_tri1.points[0] = *inside_points[0];
		out_tri1.uvCords[0] = *inside_tex[0];

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		float t;
		out_tri1.points[1] = PlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.uvCords[1].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.uvCords[1].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.uvCords[1].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

		out_tri1.points[2] = PlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
		out_tri1.uvCords[2].u = t * (outside_tex[1]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.uvCords[2].v = t * (outside_tex[1]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.uvCords[2].w = t * (outside_tex[1]->w - inside_tex[0]->w) + inside_tex[0]->w;
		return 1; // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// Copy appearance info to new triangles
		out_tri1.color = in_tri.color;
		out_tri2.color = in_tri.color;

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.points[0] = *inside_points[0];
		out_tri1.points[1] = *inside_points[1];
		out_tri1.uvCords[0] = *inside_tex[0];
		out_tri1.uvCords[1] = *inside_tex[1];

		float t;
		out_tri1.points[2] = PlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.uvCords[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.uvCords[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.uvCords[2].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.points[0] = *inside_points[1];
		out_tri2.uvCords[0] = *inside_tex[1];

		out_tri2.points[1] = out_tri1.points[2];
		out_tri2.uvCords[1] = out_tri1.uvCords[2];


		out_tri2.points[2] = PlaneIntersect(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
		out_tri2.uvCords[2].u = t * (outside_tex[0]->u - inside_tex[1]->u) + inside_tex[1]->u;
		out_tri2.uvCords[2].v = t * (outside_tex[0]->v - inside_tex[1]->v) + inside_tex[1]->v;
		out_tri2.uvCords[2].w = t * (outside_tex[0]->w - inside_tex[1]->w) + inside_tex[1]->w;

		return 2; // Return two newly formed triangles which form a quad
	}
}

class Mesh {



public:
	std::vector<Triangle> tris;

	Mesh() {};
	Mesh(std::vector<Triangle> tris)
		:tris(tris)
	{}


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


					tris.push_back(Triangle( verts[std::stoi(tokens[0]) - 1], verts[std::stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
						texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] ));
				}
			}

		}

		return true;
	}
};



