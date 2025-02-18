#include "CollisionDetection.h"

bool OverlapCircle2Circle(const Vector3& pos1, float r1, const Vector3& pos2, float r2)
{
	//TODO: do it yourself
	if((pos2 - pos1).LengthSquared() <= (r1 + r2) * (r1 + r2))
	{
		return true;
	}
	return false;
}

bool OverlapAABB2AABB(const Vector3& min1, const Vector3& max1, const Vector3& min2, const Vector3& max2)
{
	//TODO: do it yourself
	if (max1.x < min2.x or
		min1.x > max2.x or
		max1.y < min2.y or
		min1.y > max2.y)
	{
		return false;
	}
	return true;
}

bool OverlapCircle2Line(const Vector3& circlePos, float radius,
	const Vector3& lineStart, const Vector3& lineEnd)
{
	Vector3 lineTangent = lineEnd - lineStart;
	if (lineTangent.IsZero()) return false;  // Not a line, just a point.

	float lineLength = lineTangent.Length();
	lineTangent.Normalize();

	// Step 1: Calculate normal to the line (CCW 90-deg rotation)
	Vector3 lineNormal = Vector3(-lineTangent.y, lineTangent.x, 0);  // Assuming 2D vectors

	// Step 2: Project circle position onto the line's normal to find shortest distance
	Vector3 toCircle = circlePos - lineStart;
	float distanceToLine = abs(toCircle.Dot(lineNormal));

	// Step 3: Check if the distance exceeds the circle's radius
	if (distanceToLine > radius) return false;

	// Step 4: Project circle position onto the line's tangent
	float projectedDist = toCircle.Dot(lineTangent);

	// Step 5: Check if the projection lies within the line segment
	if (projectedDist > lineLength) {
		// a) Outside segment: check distance to lineEnd
		distanceToLine = (circlePos - lineEnd).Length();
	}
	else if (projectedDist < 0) {
		// b) Outside segment: check distance to lineStart
		distanceToLine = (circlePos - lineStart).Length();
	}

	// Step 6: Determine if the distance is within the radius for collision
	return distanceToLine <= radius;
}

void ResolveCircle2StaticLine(PhysicsObject& ball, float radius, const Vector3& lineStart, const Vector3& lineEnd)
{
	// Calculate line direction (tangent) and normalize
	Vector3 lineTangent = lineEnd - lineStart;
	float lineLength = lineTangent.Length();
	if (lineLength == 0.0f) return; // Degenerate line (single point), no resolution needed.
	lineTangent.Normalize();

	// Vector from line start to the circle
	Vector3 toCircle = ball.pos - lineStart;

	// Project the circle's position onto the line's tangent
	float projectedDist = toCircle.Dot(lineTangent);

	// Determine the closest point on the line to the circle
	Vector3 closestPoint;
	if (projectedDist < 0.0f)
	{
		// Circle is closest to lineStart
		closestPoint = lineStart;
	}
	else if (projectedDist > lineLength)
	{
		// Circle is closest to lineEnd
		closestPoint = lineEnd;
	}
	else
	{
		// Circle is within the segment; find the projection
		closestPoint = lineStart + projectedDist * lineTangent;
	}

	// Calculate the collision vector from the closest point to the circle's center
	Vector3 collisionVec = ball.pos - closestPoint;
	float distance = collisionVec.Length();

	// Check for collision (if distance is less than the radius)
	if (distance < radius)
	{
		// Calculate penetration depth
		float penetrationDist = radius - distance;

		// Resolve penetration
		Vector3 normal = collisionVec * (1 / distance); // Normalize collision vector
		ball.pos += normal * penetrationDist;

		// Resolve velocity
		ball.vel -= ball.vel.Dot(normal) * normal; // Reflect velocity along the normal
	}
}
//NOTE: consider shifting this to CollisionDetection.cpp so you can reuse the function
bool OverlapCircle2AABB(Vector3 circlePos, float radius, Vector3 boxMin, Vector3 boxMax)
{
	Vector3 nearest{};
	// 1) Find the nearest point on the AABB to the circle
	nearest.x = Math::Clamp(circlePos.x, boxMin.x, boxMax.x);
	nearest.y = Math::Clamp(circlePos.y, boxMin.y, boxMax.y);

	// 2) Calculate the squared distance from the nearest point to the circle center
	float dx = nearest.x - circlePos.x;
	float dy = nearest.y - circlePos.y;
	float distancesquared = dx * dx + dy * dy;

	// 3) Check if the squared distance is less than or equal to the squared radius
	return distancesquared <= radius * radius;
}
bool OverlapAABB2Line(const Vector3& boxMin, const Vector3& boxMax, const Vector3& lineStart, const Vector3& lineEnd)
{
	Vector3 lineTangent = lineEnd - lineStart;
	if (lineTangent.IsZero()) return false;  // Not a line, just a point.

	// Step 1: Check if either endpoint of the line is inside the AABB
	if (lineStart.x >= boxMin.x && lineStart.x <= boxMax.x &&
		lineStart.y >= boxMin.y && lineStart.y <= boxMax.y)
	{
		return true;  // The line start point is inside the AABB.
	}

	if (lineEnd.x >= boxMin.x && lineEnd.x <= boxMax.x &&
		lineEnd.y >= boxMin.y && lineEnd.y <= boxMax.y)
	{
		return true;  // The line end point is inside the AABB.
	}

	// Step 2: Check for X axis intersection
	if ((lineStart.x < boxMin.x && lineEnd.x < boxMin.x) || (lineStart.x > boxMax.x && lineEnd.x > boxMax.x)) {
		// No intersection along the X axis
		return false;
	}
	float tXmin = (boxMin.x - lineStart.x) / (lineEnd.x - lineStart.x);
	float tXmax = (boxMax.x - lineStart.x) / (lineEnd.x - lineStart.x);
	if (tXmin > tXmax) std::swap(tXmin, tXmax);
	if (tXmax < 0 || tXmin > 1) return false;  // No intersection along the X axis

	// Step 3: Check for Y axis intersection
	if ((lineStart.y < boxMin.y && lineEnd.y < boxMin.y) || (lineStart.y > boxMax.y && lineEnd.y > boxMax.y)) {
		// No intersection along the Y axis
		return false;
	}
	float tYmin = (boxMin.y - lineStart.y) / (lineEnd.y - lineStart.y);
	float tYmax = (boxMax.y - lineStart.y) / (lineEnd.y - lineStart.y);
	if (tYmin > tYmax) std::swap(tYmin, tYmax);
	if (tYmax < 0 || tYmin > 1) return false;  // No intersection along the Y axis

	// If all checks pass, return true (overlap occurs)
	return true;
}

bool OverlapSphere2Sphere(PhysicsObject& obj1, float r1, PhysicsObject& obj2, float r2, CollisionData& cd)
{
	Vector3 dispVec = obj1.pos - obj2.pos;

	if (dispVec.LengthSquared() <= (r1 + r2) * (r1 + r2))
	{
		cd.pObj1 = &obj1;
		cd.pObj2 = &obj2;

		float len = std::sqrt(dispVec.LengthSquared());
		if (len != 0)
		{
			cd.pd = (r1 + r2) - len;
			cd.normal = dispVec * (1.f / len);
		}
		else
		{
			cd.pd = r1;
			cd.normal = Vector3(1, 0, 0);
		}

		return true;
	}
	return false;
}


bool OverlapCircle2Circle(PhysicsObject& obj1, float r1, PhysicsObject& obj2, float r2, CollisionData& cd)
{
	Vector3 dispVec = (obj1.pos - obj2.pos);
	if (dispVec.LengthSquared() <= (r1 + r2) * (r1 + r2))
	{
		cd.pObj1 = &obj1;
		cd.pObj2 = &obj2;

		float len = dispVec.Length();
		cd.pd = (r1 + r2) - len;
		cd.normal = dispVec * (1.f / len);

		return true;
	}
	return false;
}

bool OverlapSphere2AABB(PhysicsObject& obj1, float r, PhysicsObject& obj2, Vector3 Min, Vector3 Max, CollisionData& cd)
{
	Vector3 closestPoint;
	closestPoint.x = Math::Clamp(obj1.pos.x, Min.x, Max.x);
	closestPoint.y = Math::Clamp(obj1.pos.y, Min.y, Max.y);
	closestPoint.z = Math::Clamp(obj1.pos.z, Min.z, Max.z);

	bool insideX = (obj1.pos.x >= Min.x) && (obj1.pos.x <= Max.x);
	bool insideY = (obj1.pos.y >= Min.y) && (obj1.pos.y <= Max.y);
	bool insideZ = (obj1.pos.z >= Min.z) && (obj1.pos.z <= Max.z);
	bool isInsideAABB = insideX && insideY && insideZ;

	Vector3 diff;
	float distanceSq;

	if (isInsideAABB) {
		float dxLeft = obj1.pos.x - Min.x;
		float dxRight = Max.x - obj1.pos.x;
		float dyBottom = obj1.pos.y - Min.y;
		float dyTop = Max.y - obj1.pos.y;
		float dzFront = obj1.pos.z - Min.z;
		float dzBack = Max.z - obj1.pos.z;

		float minDistX = std::min(dxLeft, dxRight);
		float minDistY = std::min(dyBottom, dyTop);
		float minDistZ = std::min(dzFront, dzBack);
		float minDist = std::min({ minDistX, minDistY, minDistZ });

		if (minDist >= r) return false;

		cd.pd = r - minDist;

		if (minDist == dxLeft) {
			cd.normal = Vector3(-1, 0, 0);
		}
		else if (minDist == dxRight) {
			cd.normal = Vector3(1, 0, 0);
		}
		else if (minDist == dyBottom) {
			cd.normal = Vector3(0, -1, 0);
		}
		else if (minDist == dyTop) {
			cd.normal = Vector3(0, 1, 0);
		}
		else if (minDist == dzFront) {
			cd.normal = Vector3(0, 0, -1);
		}
		else {
			cd.normal = Vector3(0, 0, 1);
		}
	}
	else {
		diff = obj1.pos - closestPoint;
		distanceSq = diff.LengthSquared();
		if (distanceSq > r * r) return false;

		float distance = std::sqrt(distanceSq);
		cd.pd = r - distance;
		cd.normal = diff * (1.0f / distance);
	}

	cd.pObj1 = &obj1;
	cd.pObj2 = &obj2;
	return true;
}


void ResolveCollision(CollisionData& cd) 
{
	if(cd.pObj2 == nullptr)
	{
		PhysicsObject& circle = *cd.pObj1;

		// Resolve penetration
		circle.pos += cd.normal * cd.pd;

		// Resolve velocity (reflect over normal)
		float velocityDot = circle.vel.Dot(cd.normal);
		circle.vel -= 2.0f * velocityDot * cd.normal;
	}
	else if(cd.pObj2 != nullptr)
	{

		PhysicsObject& obj1 = *cd.pObj1;
		PhysicsObject& obj2 = *cd.pObj2;

		if (obj1.mass > 0 && obj2.mass > 0)
		{
			float totalInvMass = 1.f / (obj1.mass + obj2.mass); // Total Inverse Mass

			// Resolve Penetration
			obj1.pos += obj2.mass * totalInvMass * cd.pd * cd.normal;
			obj2.pos -= obj1.mass * totalInvMass * cd.pd * cd.normal;

			// Resolve Velocity
			Vector3 deltaVel = (obj1.vel.Dot(cd.normal) - obj2.vel.Dot(cd.normal)) * cd.normal;
			obj1.vel -= 2.f * obj2.mass * totalInvMass * deltaVel;
			obj2.vel += 2.f * obj1.mass * totalInvMass * deltaVel;
		}
		else
		{
			float totalInvMass = 1.0f / (obj1.mass + 0.0f);

			// Resolve penetration (move circle only)
			obj1.pos += cd.normal * cd.pd;

			// Resolve velocity (reflect circle's velocity over the normal)
			float dot = obj1.vel.Dot(cd.normal);
			obj1.vel -= 2.0f * dot * cd.normal;
		}
	}
}

bool OverlapSphere2Line(PhysicsObject& obj1, float radius, const Vector3& lineA, const Vector3& lineB, CollisionData& cd)
{
	Vector3 lineDir = lineB - lineA;
	Vector3 toSphere = obj1.pos - lineA;

	float t = toSphere.Dot(lineDir) / lineDir.LengthSquared();
	t = Math::Clamp(t, 0.0f, 1.0f);
	Vector3 closestPoint = lineA + lineDir * t;

	Vector3 diff = obj1.pos - closestPoint;
	float distanceSq = diff.LengthSquared();

	if (distanceSq > radius * radius) return false;

	cd.pObj1 = &obj1;
	cd.pObj2 = nullptr;

	float distance = std::sqrt(distanceSq);
	if (distance > 0)
	{
		cd.pd = radius - distance;
		cd.normal = diff * (1.0f / distance);
	}
	else
	{
		cd.pd = radius;
		cd.normal = Vector3(1, 0, 0);
	}

	return true;
}
