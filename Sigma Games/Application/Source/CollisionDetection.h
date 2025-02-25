#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
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

// Sphere-AABB collision
bool OverlapSphere2AABB(PhysicsObject& obj1, float r, PhysicsObject& obj2, glm::vec3 Min, glm::vec3 Max, CollisionData& cd);

void ResolveCollision(CollisionData& cd);

bool OverlapAABB2AABB(PhysicsObject& box1, float w1, float h1, float d1, PhysicsObject& box2, float w2, float h2, float d2, CollisionData& cd);

bool OverlapRing2Cube(PhysicsObject& ring, float innerR, float outerR, PhysicsObject& box, glm::vec3 cubeMin, glm::vec3 cubeMax, CollisionData& cd);

bool OverlapRing2Ring(PhysicsObject& ring1, float inner1, float outer1, PhysicsObject& ring2, float inner2, float outer2, CollisionData& cd);

bool OverlapSphere2Plane(PhysicsObject& obj1, float r, PhysicsObject& obj2, glm::vec3 plStart, glm::vec3 plEnd, float width, CollisionData& cd);

bool PointRing2Cube(PhysicsObject& ring, float innerR, float outerR, PhysicsObject& box, glm::vec3 cubeMin, glm::vec3 cubeMax);