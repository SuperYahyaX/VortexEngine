/*
 * LibGL Physics Engine - OBB (Oriented Bounding Box) System
 * Full 3D physics simulation with realistic collision detection and resolution
 */

#ifndef LIBGL_PHYSICS_H
#define LIBGL_PHYSICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "LibGL.h"
#include <stdint.h>

    // Physics constants
#define PHYSICS_MAX_BODIES 512
#define PHYSICS_MAX_CONTACTS 1024
#define PHYSICS_GRAVITY -9.81f
#define PHYSICS_LINEAR_DAMPING 0.98f
#define PHYSICS_ANGULAR_DAMPING 0.95f
#define PHYSICS_MIN_BOUNCE 0.001f
#define PHYSICS_PENETRATION_SLOP 0.01f
#define PHYSICS_BIAS_FACTOR 0.2f

// Physics material properties
    typedef struct PhysicsMaterial {
        float density;          // kg/m³ (default: 1.0)
        float restitution;      // Bounciness 0-1 (default: 0.3)
        float staticFriction;   // 0-1 (default: 0.5)
        float dynamicFriction;  // 0-1 (default: 0.3)
    } PhysicsMaterial;

    // Predefined materials
    extern const PhysicsMaterial MATERIAL_WOOD;
    extern const PhysicsMaterial MATERIAL_METAL;
    extern const PhysicsMaterial MATERIAL_RUBBER;
    extern const PhysicsMaterial MATERIAL_ICE;
    extern const PhysicsMaterial MATERIAL_STONE;
    extern const PhysicsMaterial MATERIAL_DEFAULT;

    // Physics body type
    typedef enum {
        BODY_STATIC = 0,    // Immovable (infinite mass)
        BODY_DYNAMIC,       // Movable with physics
        BODY_KINEMATIC      // Movable but not affected by forces
    } BodyType;

    // OBB (Oriented Bounding Box) structure
    typedef struct OBB {
        Vector3 center;           // Center position in world space
        Vector3 halfExtents;      // Half-widths along each local axis
        Vector3 axes[3];          // Local orientation axes (right, up, forward)
        Vector3 rotation;         // Euler angles (degrees)
    } OBB;

    // Physics body
    typedef struct PhysicsBody {
        int id;
        int active;
        BodyType type;

        // Transform
        Vector3 position;
        Vector3 rotation;         // Euler angles (degrees)
        Vector3 scale;

        // OBB collision shape
        OBB obb;

        // Physics properties
        float mass;
        float invMass;            // 1/mass (0 for static)
        Vector3 velocity;
        Vector3 angularVelocity;
        Vector3 force;
        Vector3 torque;

        // Material
        PhysicsMaterial material;

        // Inertia tensor (simplified for box)
        float inertia[9];
        float invInertia[9];

        // Collision settings
        int isTrigger;            // If 1, no physical response
        int collisionLayer;       // Bit mask for filtering
        int collisionMask;        // What layers to collide with

        // User data
        void* userData;

        // Internal
        int sleepCounter;
        int sleeping;
    } PhysicsBody;

    // Contact point information
    typedef struct ContactPoint {
        Vector3 position;         // Contact point in world space
        Vector3 normal;           // From body A to body B
        float penetration;        // Penetration depth
        Vector3 r1, r2;          // Relative positions from centers
    } ContactPoint;

    // Collision manifold
    typedef struct CollisionManifold {
        int bodyA;
        int bodyB;
        int contactCount;
        ContactPoint contacts[4]; // Up to 4 contact points
    } CollisionManifold;

    // Collision callback
    typedef void (*CollisionCallback)(int bodyA, int bodyB, CollisionManifold* manifold);

    // Ray cast result
    typedef struct RaycastHit {
        int hit;
        int bodyId;
        Vector3 point;
        Vector3 normal;
        float distance;
    } RaycastHit;

    // ============================================================================
    // PHYSICS WORLD MANAGEMENT
    // ============================================================================

    // Initialize physics system
    void Physics_Init(void);

    // Shutdown physics system
    void Physics_Shutdown(void);

    // Update physics simulation (call once per frame)
    void Physics_Update(float deltaTime);

    // Set global gravity
    void Physics_SetGravity(Vector3 gravity);

    // Get global gravity
    Vector3 Physics_GetGravity(void);

    // Set simulation substeps (more = accurate but slower)
    void Physics_SetSubsteps(int substeps);

    // Enable/disable sleeping optimization
    void Physics_SetSleepEnabled(int enabled);

    // ============================================================================
    // BODY CREATION AND MANAGEMENT
    // ============================================================================

    // Create a physics body with box shape
    int Physics_CreateBoxBody(Vector3 position, Vector3 rotation, Vector3 halfExtents,
        BodyType type, PhysicsMaterial material);

    // Remove a physics body
    void Physics_RemoveBody(int bodyId);

    // Get body by ID
    PhysicsBody* Physics_GetBody(int bodyId);

    // Check if body is valid
    int Physics_IsBodyValid(int bodyId);

    // ============================================================================
    // BODY PROPERTIES
    // ============================================================================

    // Position and rotation
    void Physics_SetPosition(int bodyId, Vector3 position);
    Vector3 Physics_GetPosition(int bodyId);
    void Physics_SetRotation(int bodyId, Vector3 rotation);
    Vector3 Physics_GetRotation(int bodyId);
    void Physics_Translate(int bodyId, Vector3 translation);
    void Physics_Rotate(int bodyId, Vector3 rotation);

    // Velocity
    void Physics_SetVelocity(int bodyId, Vector3 velocity);
    Vector3 Physics_GetVelocity(int bodyId);
    void Physics_SetAngularVelocity(int bodyId, Vector3 angularVel);
    Vector3 Physics_GetAngularVelocity(int bodyId);

    // Mass and material
    void Physics_SetMass(int bodyId, float mass);
    float Physics_GetMass(int bodyId);
    void Physics_SetMaterial(int bodyId, PhysicsMaterial material);
    PhysicsMaterial Physics_GetMaterial(int bodyId);

    // Body type
    void Physics_SetBodyType(int bodyId, BodyType type);
    BodyType Physics_GetBodyType(int bodyId);

    // Trigger mode
    void Physics_SetTrigger(int bodyId, int isTrigger);
    int Physics_IsTrigger(int bodyId);

    // Collision filtering
    void Physics_SetCollisionLayer(int bodyId, int layer);
    void Physics_SetCollisionMask(int bodyId, int mask);

    // User data
    void Physics_SetUserData(int bodyId, void* data);
    void* Physics_GetUserData(int bodyId);

    // ============================================================================
    // FORCES AND IMPULSES
    // ============================================================================

    // Apply force at center of mass
    void Physics_AddForce(int bodyId, Vector3 force);

    // Apply force at world position (creates torque)
    void Physics_AddForceAtPosition(int bodyId, Vector3 force, Vector3 position);

    // Apply impulse (instant velocity change)
    void Physics_AddImpulse(int bodyId, Vector3 impulse);

    // Apply impulse at world position
    void Physics_AddImpulseAtPosition(int bodyId, Vector3 impulse, Vector3 position);

    // Apply torque
    void Physics_AddTorque(int bodyId, Vector3 torque);

    // Clear all forces and torques
    void Physics_ClearForces(int bodyId);

    // ============================================================================
    // COLLISION QUERIES
    // ============================================================================

    // Check if two bodies are colliding
    int Physics_CheckCollision(int bodyA, int bodyB);

    // Get collision manifold between two bodies
    CollisionManifold Physics_GetCollisionManifold(int bodyA, int bodyB);

    // Raycast - returns first hit
    RaycastHit Physics_Raycast(Vector3 origin, Vector3 direction, float maxDistance);

    // Raycast with layer mask
    RaycastHit Physics_RaycastLayer(Vector3 origin, Vector3 direction,
        float maxDistance, int layerMask);

    // Check if point is inside any body
    int Physics_PointInside(Vector3 point);

    // ============================================================================
    // CALLBACKS
    // ============================================================================

    // Set collision callback (called when collision detected)
    void Physics_SetCollisionCallback(CollisionCallback callback);

    // ============================================================================
    // DEBUG VISUALIZATION
    // ============================================================================

    // Draw all physics bodies (wireframe)
    void Physics_DrawDebug(Vector3 color, float thickness);

    // Draw single body
    void Physics_DrawBodyDebug(int bodyId, Vector3 color, float thickness);

    // Draw collision contacts
    void Physics_DrawContacts(Vector3 normalColor, Vector3 pointColor, float size);

    // ============================================================================
    // UTILITY FUNCTIONS
    // ============================================================================

    // Create default material
    PhysicsMaterial Physics_CreateMaterial(float density, float restitution,
        float staticFriction, float dynamicFriction);

    // OBB construction from body
    OBB Physics_CreateOBB(Vector3 center, Vector3 halfExtents, Vector3 rotation);

    // Test OBB-OBB collision
    int Physics_TestOBBCollision(OBB* a, OBB* b, CollisionManifold* manifold);

    // Get body count
    int Physics_GetBodyCount(void);

    // Get active body count
    int Physics_GetActiveBodyCount(void);

    // Get collision count (last frame)
    int Physics_GetCollisionCount(void);




    inline float minf(float a, float b) { return a < b ? a : b; };
    inline float maxf(float a, float b) { return a > b ? a : b; };
    inline float clamp(float v, float min, float max) {
        return v < min ? min : (v > max ? max : v);
    }
    Vector3 Vec3Add(Vector3 a, Vector3 b);

    Vector3 Vec3Sub(Vector3 a, Vector3 b);

    Vector3 Vec3Mul(Vector3 v, float s);

    float Vec3Dot(Vector3 a, Vector3 b);
    Vector3 Vec3Cross(Vector3 a, Vector3 b);

    float Vec3Length(Vector3 v);


    Vector3 Vec3Normalize(Vector3 v);

#ifdef __cplusplus
}
#endif

#endif // LIBGL_PHYSICS_H