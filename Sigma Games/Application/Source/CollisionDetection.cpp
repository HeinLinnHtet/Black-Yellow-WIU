#include "CollisionDetection.h"
#include "MyMath.h"
#include <iostream>
using namespace std;

bool OverlapSphere2Sphere(PhysicsObject& obj1, float r1, PhysicsObject& obj2, float r2, CollisionData& cd)
{
	glm::vec3 dispVec = (obj1.pos - obj2.pos);
	if (glm::length2(dispVec) <= (r1 + r2) * (r1 + r2))
	{
		cd.pObj1 = &obj1;
		cd.pObj2 = &obj2;

		float len = glm::length(dispVec);
		cd.pd = (r1 + r2) - len;
		cd.normal = dispVec * (1.f / len);

		return true;
	}
	return false;
}

bool OverlapSphere2AABB(PhysicsObject& obj1, float r, PhysicsObject& obj2, glm::vec3 Min, glm::vec3 Max, CollisionData& cd)
{
	glm::vec3 closestPoint;
	closestPoint.x = Math::Clamp(obj1.pos.x, Min.x, Max.x);
	closestPoint.y = Math::Clamp(obj1.pos.y, Min.y, Max.y);
	closestPoint.z = Math::Clamp(obj1.pos.z, Min.z, Max.z);

	bool insideX = (obj1.pos.x >= Min.x) && (obj1.pos.x <= Max.x);
	bool insideY = (obj1.pos.y >= Min.y) && (obj1.pos.y <= Max.y);
	bool insideZ = (obj1.pos.z >= Min.z) && (obj1.pos.z <= Max.z);
	bool isInsideAABB = insideX && insideY && insideZ;

	glm::vec3 diff;
	float distanceSq;

	if (isInsideAABB) {
		float dxLeft = obj1.pos.x - Min.x;
		float dxRight = Max.x - obj1.pos.x;
		float dyBottom = obj1.pos.y - Min.y;
		float dyTop = Max.y - obj1.pos.y;
		float dzNear = obj1.pos.z - Min.z;
		float dzFar = Max.z - obj1.pos.z;

		float minDistX = std::min(dxLeft, dxRight);
		float minDistY = std::min(dyBottom, dyTop);
		float minDistZ = std::min(dzNear, dzFar);
		float minDist = std::min({ minDistX, minDistY, minDistZ });

		if (minDist >= r) return false;

		cd.pd = r - minDist;

		if (minDist == minDistX) {
			cd.normal = (minDist == dxLeft) ? glm::vec3(-1, 0, 0) : glm::vec3(1, 0, 0);
		}
		else if (minDist == minDistY) {
			cd.normal = (minDist == dyBottom) ? glm::vec3(0, -1, 0) : glm::vec3(0, 1, 0);
		}
		else {
			cd.normal = (minDist == dzNear) ? glm::vec3(0, 0, -1) : glm::vec3(0, 0, 1);
		}
	}
	else {
		diff = obj1.pos - closestPoint;
		distanceSq = length2(diff);
		if (distanceSq > r * r) return false;

		float distance = std::sqrt(distanceSq);
		cd.pd = r - distance;
		cd.normal = -diff * (1.0f / distance);
	}

	if (obj1.mass == 0)
	{
		cd.pObj1 = &obj2;
		cd.pObj2 = &obj1;
	}
	else
	{
		cd.pObj1 = &obj1;
		cd.pObj2 = &obj2;
	}
	return true;
}

bool OverlapAABB2AABB(PhysicsObject& box1, float w1, float h1, float d1, PhysicsObject& box2, float w2, float h2, float d2, CollisionData& cd)
{
	cd.pObj1 = &box1;
	cd.pObj2 = &box2;

	const float halfW1 = w1 * 0.5f;
	const float halfH1 = h1 * 0.5f;
	const float halfD1 = d1 * 0.5f;

	const float halfW2 = w2 * 0.5f;
	const float halfH2 = h2 * 0.5f;
	const float halfD2 = d2 * 0.5f;

	// Calculate the min and max points for each box in 3D
	glm::vec3 min1{ box1.pos.x - halfW1, box1.pos.y - halfH1, box1.pos.z - halfD1 };
	glm::vec3 max1{ box1.pos.x + halfW1, box1.pos.y + halfH1, box1.pos.z + halfD1 };
	glm::vec3 min2{ box2.pos.x - halfW2, box2.pos.y - halfH2, box2.pos.z - halfD2 };
	glm::vec3 max2{ box2.pos.x + halfW2, box2.pos.y + halfH2, box2.pos.z + halfD2 };

	// Check for overlap in all three dimensions (x, y, z)
	if (!(max1.x < min2.x || min1.x > max2.x ||
		max1.y < min2.y || min1.y > max2.y ||
		max1.z < min2.z || min1.z > max2.z))
	{
		// If there's an overlap, compute the penetration depths along each axis
		float left = max1.x - min2.x;
		float right = max2.x - min1.x;
		float bottom = max1.y - min2.y;
		float top = max2.y - min1.y;
		float front = max1.z - min2.z;
		float back = max2.z - min1.z;

		// Determine the penetration depth and normal
		float minPenetration = std::min({ left, right, bottom, top, front, back });

		if (minPenetration == left) {
			cd.normal = glm::vec3(-1, 0, 0);
		}
		else if (minPenetration == right) {
			cd.normal = glm::vec3(1, 0, 0);
		}
		else if (minPenetration == bottom) {
			cd.normal = glm::vec3(0, -1, 0);
		}
		else if (minPenetration == top) {
			cd.normal = glm::vec3(0, 1, 0);
		}
		else if (minPenetration == front) {
			cd.normal = glm::vec3(0, 0, -1);
		}
		else {  // minPenetration == back
			cd.normal = glm::vec3(0, 0, 1);
		}

		// Set the penetration depth
		cd.pd = minPenetration;

		return true;  // There is an overlap
	}

	return false;  // No overlap
}

bool OverlapRing2Cube(PhysicsObject& ring, float innerR, float outerR, PhysicsObject& box, glm::vec3 cubeMin, glm::vec3 cubeMax, CollisionData& cd)
{
	const int ballAmt = 10 * outerR;
	bool collided = false;

	for (int i = 0; i < ballAmt; ++i)
	{
		float angle = Math::DegreeToRadian(i * 360.0f / ballAmt);
		glm::vec3 tempBall = ring.pos + glm::vec3(outerR * cos(angle), 0, outerR * sin(angle));

		glm::vec3 nearest;
		nearest.x = glm::clamp(tempBall.x, cubeMin.x, cubeMax.x);
		nearest.y = glm::clamp(tempBall.y, cubeMin.y, cubeMax.y);
		nearest.z = glm::clamp(tempBall.z, cubeMin.z, cubeMax.z);

		float dx = nearest.x - tempBall.x;
		float dy = nearest.y - tempBall.y;
		float dz = nearest.z - tempBall.z;
		float distancesquared = dx * dx + dy * dy + dz * dz;

		if (distancesquared <= innerR * innerR)
		{
			glm::vec3 closestPoint;
			closestPoint.y = glm::clamp(tempBall.y, cubeMin.y, cubeMax.y);
			closestPoint.x = glm::clamp(tempBall.x, cubeMin.x, cubeMax.x);
			closestPoint.z = glm::clamp(tempBall.z, cubeMin.z, cubeMax.z);

			glm::vec3 ringToCube = closestPoint - tempBall;
			float distance = glm::length(ringToCube);

			float penetrationDepth = innerR - distance;

			cd.pd = penetrationDepth;
			cd.normal = -glm::normalize(ringToCube);
			collided = true;
		}
	}

	if (collided)
	{
		//glm::vec3 closestPoint;
		//closestPoint.x = glm::clamp(ring.pos.x, cubeMin.x, cubeMax.x);
		//closestPoint.y = glm::clamp(ring.pos.y, cubeMin.y, cubeMax.y);
		//closestPoint.z = glm::clamp(ring.pos.z, cubeMin.z, cubeMax.z);

		//glm::vec3 ringToCube = closestPoint - ring.pos;
		//float distance = glm::length(ringToCube);

		//float penetrationDepth = innerR - distance;
		//
		//cd.pd = penetrationDepth;
		//cd.normal = -glm::normalize(ringToCube);
		cd.pObj1 = &ring;
		cd.pObj2 = &box;

		return true;
	}
	else
	{
		return false;
	}
}

bool OverlapRing2Ring(PhysicsObject& ring1, float inner1, float outer1, PhysicsObject& ring2, float inner2, float outer2, CollisionData& cd)
{
	const int ballAmt1 = 10 * outer1;
	const int ballAmt2 = 10 * outer2;
	bool collided = false;
	float distSq = 0;

	glm::vec3 tempBall1;
	glm::vec3 tempBall2;

	for (int i = 0; i < ballAmt1; ++i)
	{
		float angle = Math::DegreeToRadian(i * 360.0f / ballAmt1);
		tempBall1 = ring1.pos + glm::vec3(outer1 * cos(angle), 0, outer1 * sin(angle));

		for (int j = 0; j < ballAmt2; ++j)
		{
			float angle = Math::DegreeToRadian(j * 360.0f / ballAmt2);
			tempBall2 = ring2.pos + glm::vec3(outer2 * cos(angle), 0, outer2 * sin(angle));
			glm::vec3 dispVec;

			dispVec = (tempBall1 - tempBall2);

			if (glm::length2(dispVec) <= inner1 + inner2)
			{
				collided = true;
				glm::vec3 ball2ball = tempBall1 - tempBall2;
				cd.normal = -glm::normalize(ball2ball);

				float distance = glm::length(ball2ball);
				float penetrationDepth = (inner1 + inner2) - distance;
				cd.pd = penetrationDepth;
			}
		}
	}
	if (collided)
	{

		cd.pObj1 = &ring1;
		cd.pObj2 = &ring2;

		return true;
	}
	else
		return false;
}

bool PointRing2Cube(PhysicsObject& ring, float innerR, float outerR, PhysicsObject& box, glm::vec3 cubeMin, glm::vec3 cubeMax)
{

	glm::vec3 closestPoint;
	closestPoint.x = glm::clamp(ring.pos.x, cubeMin.x, cubeMax.x);
	closestPoint.y = ring.pos.y;
	closestPoint.z = glm::clamp(ring.pos.z, cubeMin.z, cubeMax.z);

	glm::vec3 ringToCube = closestPoint - ring.pos;
	float distance = glm::length(ringToCube);

	if (outerR - innerR > distance && (ring.pos.y - innerR > cubeMin.y && ring.pos.y + innerR < cubeMax.y))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ResolveCollision(CollisionData& cd)
{
	PhysicsObject& obj1 = *cd.pObj1;
	PhysicsObject& obj2 = *cd.pObj2;

	float totalInvMass = 1.f / (obj1.mass + obj2.mass);
	float bounciness = std::min(obj1.bounciness, obj2.bounciness);
	glm::vec3 deltaVel;

	deltaVel = (glm::dot(obj1.vel, cd.normal) - glm::dot(obj2.vel, cd.normal)) * cd.normal;

	//Resolve velocity 
	if (obj1.mass > 0 && obj2.mass == 0) {
		obj1.pos += cd.pd * cd.normal;
		obj1.vel -= (bounciness + 1) * deltaVel;
	}
	else {
		obj1.pos += obj2.mass * totalInvMass * cd.pd * cd.normal;
		obj2.pos -= obj1.mass * totalInvMass * cd.pd * cd.normal;
		obj1.vel -= (bounciness + 1) * obj2.mass * totalInvMass * deltaVel;
		obj2.vel += (bounciness + 1) * obj1.mass * totalInvMass * deltaVel;
	}
}