#pragma once

#include "Vector3.h"
#include "PhysicsObject.h"

struct CollisionData {
    float pd;  // Penetration depth
    Vector3 normal;
    PhysicsObject* pObj1;
    PhysicsObject* pObj2;
};

// Sphere-Sphere collision
bool OverlapSphere2Sphere(PhysicsObject& obj1, float r1, PhysicsObject& obj2, float r2, CollisionData& cd);

// AABB-AABB collision
bool OverlapAABB2AABB(const Vector3& min1, const Vector3& max1,
    const Vector3& min2, const Vector3& max2);

// Sphere-Line segment collision
bool OverlapSphere2Line(const Vector3& spherePos, float radius,
    const Vector3& lineStart, const Vector3& lineEnd);

// Sphere-AABB collision
bool OverlapSphere2AABB(PhysicsObject& obj1, float r, PhysicsObject& obj2, Vector3 Min, Vector3 Max, CollisionData& cd);

// AABB-Line segment collision (3D)
bool OverlapAABB2Line(const Vector3& boxMin, const Vector3& boxMax,
    const Vector3& lineStart, const Vector3& lineEnd);

// Physics resolution functions
void ResolveSphere2StaticLine(PhysicsObject& sphere, float radius,
    const Vector3& lineStart, const Vector3& lineEnd);
void ResolveCollision(CollisionData& cd);

bool OverlapAABB2AABB(PhysicsObject& box1, Vector3 box1min, Vector3 box1max, PhysicsObject& box2, Vector3 box2min, Vector3 box2max, CollisionData& cd);