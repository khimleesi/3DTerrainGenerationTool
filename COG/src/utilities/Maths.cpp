#include "Maths.h"

namespace maths {

	/****************************************************************************************
		BARYCENTRIC COORDINATES - A popular formula used for multi-height terrain collision
	
		The Barycentric formula is used to return a given point P(x,y) within a triangle
		The formula of which can be found here:
		http://2000clicks.com/MathHelp/GeometryTriangleBarycentricCoordinates.aspx

		There are much simpler ways of calculating the equation, however I wrote
		it out by hand in its simplest form to make it easier to understand.
		A simpler equation:
		https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates

		Given vertex points A, B and C of a triangle and a 2D point P(x,y) of an object
		within this triangle, we distinguish the P(x,y) Barycentric coordinates using
		the forumlas below. Barycentric coordinates are usually represented as t1, t2, t3
		l1, l2, l3 or more commonly v, w, u.

		Once we have these coordinates, we multiply them by the height of each
		vertex (A, B, C), which will return us the height of the terrain at that given spot.
	****************************************************************************************/
	float Barycentric(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec2& P) {

		float determinant = (A.x * B.z) + (B.x * C.z) + (C.x * A.z) - (A.z * B.x) - (B.z * C.x) - (C.z * A.x);

		float v = (P.x * B.z) + (B.x * C.z) + (C.x * P.y) - (P.y * B.x) - (B.z * C.x) - (C.z * P.x) / determinant;
		float w = (A.x * P.y) + (P.x * C.z) + (C.x * A.z) - (A.z * P.x) - (P.y * C.x) - (C.z * A.x) / determinant;
		float u = 1.0f - v - w;

		return v * A.y + w * B.y + u * C.y;
	}
}