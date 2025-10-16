#ifndef PHYSGL_H
#define PHYSGL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "LibGL.h"

    // Physics body types
    typedef enum {
        PHYSICS_STATIC = 0,
        PHYSICS_DYNAMIC,
        PHYSICS_KINEMATIC
    } PhysicsBodyType;

    // AABB structure
    typedef struct AABB {
        Vector3 min;
        Vector3 max;
        Vector3 center;
        Vector3 halfSize;
    } AABB;

    // Physics body
    typedef struct PhysicsBody {
        int id;
        PhysicsBodyType type;
        Vector3 position;
        Vector3 size;
        Vector3 velocity;
        Vector3 acceleration;
        Vector3 force;
        float mass;
        float invMass;
        float restitution; // Bounciness
        float friction;
        AABB bounds;
        int isColliding;
        int enabled;
    } PhysicsBody;

    // Collision information
    typedef struct Collision {
        int bodyA;
        int bodyB;
        Vector3 normal;
        float depth;
        Vector3 contactPoint;
    } Collision;

    // Physics world
    typedef struct PhysicsWorld {
        Vector3 gravity;
        float fixedTimeStep;
        int iterations;
        int bodyCount;
        int maxBodies;
    } PhysicsWorld;

    // Physics manager functions
    void PHYS_InitWorld(PhysicsWorld* world, Vector3 gravity, float fixedTimeStep, int iterations, int maxBodies);
    void PHYS_UpdateWorld(PhysicsWorld* world, float deltaTime);
    void PHYS_CleanupWorld(PhysicsWorld* world);

    // Body management
    int PHYS_CreateBody(PhysicsWorld* world, PhysicsBodyType type, Vector3 position, Vector3 size, float mass, float restitution);
    void PHYS_DestroyBody(PhysicsWorld* world, int bodyId);
    PhysicsBody* PHYS_GetBody(PhysicsWorld* world, int bodyId);
    void PHYS_SetBodyPosition(PhysicsWorld* world, int bodyId, Vector3 position);
    void PHYS_SetBodyVelocity(PhysicsWorld* world, int bodyId, Vector3 velocity);
    void PHYS_ApplyForce(PhysicsWorld* world, int bodyId, Vector3 force);
    void PHYS_ApplyImpulse(PhysicsWorld* world, int bodyId, Vector3 impulse);

    // AABB functions
    AABB PHYS_CreateAABB(Vector3 center, Vector3 halfSize);
    void PHYS_UpdateAABB(PhysicsBody* body, Vector3 position);
    int PHYS_AABBIntersect(const AABB* a, const AABB* b);
    int PHYS_AABBContainsPoint(const AABB* aabb, Vector3 point);
    float PHYS_AABBIntersectRay(const AABB* aabb, Vector3 rayOrigin, Vector3 rayDir, Vector3* outNormal);
    void PHYS_SetBodySize(PhysicsWorld* world, int bodyId, Vector3 size);



    // Collision detection
    int PHYS_CheckCollision(PhysicsWorld* world, int bodyA, int bodyB, Collision* collision);
    void PHYS_ResolveCollision(PhysicsWorld* world, Collision* collision);

    // Debug rendering
    void PHYS_DebugDrawAABB(PhysicsWorld* world, int bodyId, Vector3 color);
    void PHYS_DebugDrawAllAABBs(PhysicsWorld* world, Vector3 color);

    // Utility functions
    Vector3 PHYS_CalculateAABBCenter(const AABB* aabb);
    Vector3 PHYS_CalculateAABBHalfSize(const AABB* aabb);

#ifdef __cplusplus
}
#endif

#endif // PHYSGL_H