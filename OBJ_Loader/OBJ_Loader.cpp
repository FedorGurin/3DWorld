#include "OBJ_Loader.h"

// Vector3 Multiplication Opertor Overload
glm::vec3 operator*(const float& left, const glm::vec3& right)
{
    return glm::vec3(right.x * left, right.y * left, right.z * left);
}
// A test to see if P1 is on the same side as P2 of a line segment ab
bool SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b)
{
    glm::vec3 cp1 = glm::cross(b - a, p1 - a);
    glm::vec3 cp2 = glm::cross(b - a, p2 - a);

    if (glm::dot(cp1, cp2) >= 0)
				return true;
			else
				return false;
		}
        float AngleBetweenV3(const glm::vec3 a, const glm::vec3 b)
        {
            float angle = glm::dot(a, b);
            angle /= (glm::length(a) * glm::length(b));
            return angle = acosf(angle);
        }

		// Generate a cross produect normal for a triangle
        glm::vec3 GenTriNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
		{
            glm::vec3 u = t2 - t1;
            glm::vec3 v = t3 - t1;

            glm::vec3 normal = glm::cross(u,v);

			return normal;
		}

		// Check to see if a Vector3 Point is within a 3 Vector3 Triangle
        bool inTriangle(glm::vec3 point, glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3)
		{
			// Test to see if it is within an infinite prism that the triangle outlines.
			bool within_tri_prisim = SameSide(point, tri1, tri2, tri3) && SameSide(point, tri2, tri1, tri3)
				&& SameSide(point, tri3, tri1, tri2);

			// If it isn't it will never be on the triangle
			if (!within_tri_prisim)
				return false;

			// Calulate Triangle's Normal
            glm::vec3 n = GenTriNormal(tri1, tri2, tri3);

			// Project the point onto this normal
            glm::vec3 proj = glm::proj(point, n);

			// If the distance from the triangle to the point is 0
			//	it lies on the triangle
            if (glm::length(proj) == 0)
				return true;
			else
				return false;
		}
