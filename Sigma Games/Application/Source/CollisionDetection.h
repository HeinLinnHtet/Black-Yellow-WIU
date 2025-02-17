#pragma once

#include "Vector3.h"
#include "PhysicsObject.h"


struct CollisionData
{
	float pd{};
	Vector3 normal;

	PhysicsObject* pObj1{};
	PhysicsObject* pObj2{};
	float penetrationDepth;
	Vector3 relativeVelocity;
};

//global collision detection functions
bool OverlapCircle2Circle(const Vector3& pos1, float r1, const Vector3& pos2, float r2);
bool OverlapAABB2AABB(const Vector3& min1, const Vector3& max1,
					  const Vector3& min2, const Vector3& max2);
bool OverlapCircle2Line(const Vector3& circlePos, float radius,
						const Vector3& lineStart, 
						const Vector3& lineEnd);

//collision resolution function
void ResolveCircle2StaticLine(PhysicsObject& ball, float radius, const Vector3& lineStart, const Vector3& lineEnd);

bool OverlapCircle2AABB(Vector3 circlePos, float radius, Vector3 boxMin, Vector3 boxMax);

bool OverlapAABB2Line(const Vector3& boxMin, const Vector3& boxMax, const Vector3& lineStart, const Vector3& lineEnd);

bool OverlapCircle2Circle(PhysicsObject& obj1, float r1, PhysicsObject& obj2, float r2, CollisionData& cd);

bool OverlapCircle2AABB(PhysicsObject& obj1, float r, PhysicsObject& obj2, Vector3 Min, Vector3 Max, CollisionData& cd);

bool OverlapCircle2Line(PhysicsObject& obj1, float radius, const Vector3& lineA, const Vector3& lineB, CollisionData& cd);

void ResolveCollision(CollisionData& cd);