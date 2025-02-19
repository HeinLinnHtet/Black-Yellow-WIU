#pragma once

#include <glm\glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/constants.hpp>
#include "PhysicsObject.h"
#include "MyMath.h"
#include <iostream>

struct CollisionData {
    float pd;  // Penetration depth
    glm::vec3 normal;
    PhysicsObject* pObj1;
    PhysicsObject* pObj2;
};

// Sphere-Sphere collision
bool OverlapSphere2Sphere(PhysicsObject& obj1, float r1, PhysicsObject& obj2, float r2, CollisionData& cd);

// AABB-AABB collision
bool OverlapAABB2AABB(const glm::vec3& min1, const glm::vec3& max1,
    const glm::vec3& min2, const glm::vec3& max2);

// Sphere-Line segment collision
bool OverlapSphere2Line(const glm::vec3& spherePos, float radius, const glm::vec3& lineStart, const glm::vec3& lineEnd);

// Sphere-AABB collision
bool OverlapSphere2AABB(PhysicsObject& obj1, float r, PhysicsObject& obj2, glm::vec3 Min, glm::vec3 Max, CollisionData& cd);

// AABB-Line segment collision (3D)
bool OverlapAABB2Line(const glm::vec3& boxMin, const glm::vec3& boxMax,
    const glm::vec3& lineStart, const glm::vec3& lineEnd);

// Physics resolution functions
void ResolveSphere2StaticLine(PhysicsObject& sphere, float radius, const glm::vec3& lineStart, const glm::vec3& lineEnd);
void ResolveCollision(CollisionData& cd);

bool OverlapAABB2AABB(PhysicsObject& box1, glm::vec3 box1min, glm::vec3 box1max, PhysicsObject& box2, glm::vec3 box2min, glm::vec3 box2max, CollisionData& cd);