#include "stdafx.h"
#include "Triangle.h"


tge::Triangle::Triangle() {};

tge::Triangle::Triangle(Vector3D p1, Vector3D p2, Vector3D p3) {
	points[0] = p1; points[1] = p2; points[2] = p3;
}

tge::Triangle::Triangle(Vector3D p1, Vector3D p2, Vector3D p3, VectorTex uv1, VectorTex uv2, VectorTex uv3) {
	points[0] = p1; points[1] = p2; points[2] = p3;
	uvCords[0] = uv1; uvCords[1] = uv2, uvCords[2] = uv3;
}



sf::Color color = sf::Color::White;

void tge::Triangle::ScalePointsByW() {
	for (int i = 0; i < 3; i++)
		points[i].ScaleByW();
}

void tge::Triangle::ScaleUvByW() {
	for (int i = 0; i < 3; i++)
		uvCords[i].ScaleByW();
}

const tge::Triangle tge::operator*(const Matrix3D& mat, Triangle tri) {
	tri.points[0] = mat * tri.points[0];
	tri.points[1] = mat * tri.points[1];
	tri.points[2] = mat * tri.points[2];
	return tri;

}

const tge::Triangle tge::operator*(Triangle tri, float scalar) {
	tri.points[0] = tri.points[0] * scalar;
	tri.points[1] = tri.points[1] * scalar;
	tri.points[2] = tri.points[2] * scalar;
	return tri;
}

const tge::Triangle tge::operator+(Triangle tri, Vector3D vec) {
	tri.points[0] = tri.points[0] + vec;
	tri.points[1] = tri.points[1] + vec;
	tri.points[2] = tri.points[2] + vec;
	return tri;
}



int tge::ClipAgainstPlane(const Vector3D& plane_p, const Vector3D& plane_n,
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