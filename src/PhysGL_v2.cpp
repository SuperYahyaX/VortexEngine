/*
 * LibGL Physics Engine Implementation
 */

#include "PhysGL_v2.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

 // Predefined materials
const PhysicsMaterial MATERIAL_WOOD = { 0.6f, 0.3f, 0.5f, 0.4f };
const PhysicsMaterial MATERIAL_METAL = { 7.8f, 0.2f, 0.6f, 0.4f };
const PhysicsMaterial MATERIAL_RUBBER = { 1.1f, 0.8f, 0.9f, 0.7f };
const PhysicsMaterial MATERIAL_ICE = { 0.9f, 0.1f, 0.02f, 0.01f };
const PhysicsMaterial MATERIAL_STONE = { 2.5f, 0.15f, 0.7f, 0.6f };
const PhysicsMaterial MATERIAL_DEFAULT = { 1.0f, 0.3f, 0.5f, 0.3f };

// Physics world state
typedef struct PhysicsWorld {
    PhysicsBody bodies[PHYSICS_MAX_BODIES];
    CollisionManifold manifolds[PHYSICS_MAX_CONTACTS];
    int bodyCount;
    int manifestCount;
    Vector3 gravity;
    int substeps;
    int sleepEnabled;
    CollisionCallback collisionCallback;
    int initialized;
} PhysicsWorld;

static PhysicsWorld world = { 0 };

// Math helpers

Vector3 Vec3Add(Vector3 a, Vector3 b) {
    return  { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vector3 Vec3Sub(Vector3 a, Vector3 b) {
    return  { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vector3 Vec3Mul(Vector3 v, float s) {
    return  { v.x* s, v.y* s, v.z* s };
}

float Vec3Dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Vec3Cross(Vector3 a, Vector3 b) {
    return  {
        a.y* b.z - a.z * b.y,
            a.z* b.x - a.x * b.z,
            a.x* b.y - a.y * b.x
    };
}

float Vec3Length(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 Vec3Normalize(Vector3 v) {
    float len = Vec3Length(v);
    if (len < 0.0001f) return  { 0, 0, 0 };
    return Vec3Mul(v, 1.0f / len);
}

// Rotation matrix from Euler angles (degrees)
static void CreateRotationMatrix(Vector3 rotation, float matrix[9]) {
    float pitch = rotation.x * 3.14159265f / 180.0f;
    float yaw = rotation.y * 3.14159265f / 180.0f;
    float roll = rotation.z * 3.14159265f / 180.0f;

    float cp = cosf(pitch), sp = sinf(pitch);
    float cy = cosf(yaw), sy = sinf(yaw);
    float cr = cosf(roll), sr = sinf(roll);

    matrix[0] = cy * cr;
    matrix[1] = cy * sr;
    matrix[2] = -sy;
    matrix[3] = sp * sy * cr - cp * sr;
    matrix[4] = sp * sy * sr + cp * cr;
    matrix[5] = sp * cy;
    matrix[6] = cp * sy * cr + sp * sr;
    matrix[7] = cp * sy * sr - sp * cr;
    matrix[8] = cp * cy;
}

// Calculate box inertia tensor
static void CalculateBoxInertia(Vector3 halfExtents, float mass, float inertia[9]) {
    float w = halfExtents.x * 2.0f;
    float h = halfExtents.y * 2.0f;
    float d = halfExtents.z * 2.0f;

    float ix = (mass / 12.0f) * (h * h + d * d);
    float iy = (mass / 12.0f) * (w * w + d * d);
    float iz = (mass / 12.0f) * (w * w + h * h);

    memset(inertia, 0, 9 * sizeof(float));
    inertia[0] = ix;
    inertia[4] = iy;
    inertia[8] = iz;
}

// 3x3 matrix inversion
static void InvertMatrix3x3(float m[9], float inv[9]) {
    float det = m[0] * (m[4] * m[8] - m[7] * m[5]) -
        m[1] * (m[3] * m[8] - m[5] * m[6]) +
        m[2] * (m[3] * m[7] - m[4] * m[6]);

    if (fabsf(det) < 0.0001f) {
        memset(inv, 0, 9 * sizeof(float));
        return;
    }

    float invDet = 1.0f / det;
    inv[0] = (m[4] * m[8] - m[7] * m[5]) * invDet;
    inv[1] = (m[2] * m[7] - m[1] * m[8]) * invDet;
    inv[2] = (m[1] * m[5] - m[2] * m[4]) * invDet;
    inv[3] = (m[5] * m[6] - m[3] * m[8]) * invDet;
    inv[4] = (m[0] * m[8] - m[2] * m[6]) * invDet;
    inv[5] = (m[2] * m[3] - m[0] * m[5]) * invDet;
    inv[6] = (m[3] * m[7] - m[6] * m[4]) * invDet;
    inv[7] = (m[1] * m[6] - m[0] * m[7]) * invDet;
    inv[8] = (m[0] * m[4] - m[1] * m[3]) * invDet;
}

// Update OBB from body transform
static void UpdateOBB(PhysicsBody* body) {
    body->obb.center = body->position;
    body->obb.halfExtents = Vec3Mul(body->scale, 0.5f);
    body->obb.rotation = body->rotation;

    float rotMat[9];
    CreateRotationMatrix(body->rotation, rotMat);

    body->obb.axes[0] = { rotMat[0], rotMat[3], rotMat[6] };
    body->obb.axes[1] = { rotMat[1], rotMat[4], rotMat[7] };
    body->obb.axes[2] = { rotMat[2], rotMat[5], rotMat[8] };
}

// SAT (Separating Axis Theorem) for OBB collision detection
static float ProjectOBB(OBB* obb, Vector3 axis) {
    float r = fabsf(Vec3Dot(obb->axes[0], axis)) * obb->halfExtents.x +
        fabsf(Vec3Dot(obb->axes[1], axis)) * obb->halfExtents.y +
        fabsf(Vec3Dot(obb->axes[2], axis)) * obb->halfExtents.z;
    return r;
}

static int TestAxisSeparation(OBB* a, OBB* b, Vector3 axis, float* depth) {
    float len = Vec3Length(axis);
    if (len < 0.0001f) return 1; // Degenerate axis, skip

    axis = Vec3Normalize(axis);

    // Project both OBBs onto the axis
    float radiusA = ProjectOBB(a, axis);
    float radiusB = ProjectOBB(b, axis);

    // Project center distance onto axis
    Vector3 delta = Vec3Sub(b->center, a->center);
    float centerDistance = fabsf(Vec3Dot(delta, axis));

    // Calculate separation
    float separation = centerDistance - (radiusA + radiusB);

    // If separated, no collision
    if (separation > 0.0f) return 0;

    // Update minimum penetration depth
    if (depth) {
        float penetration = (radiusA + radiusB) - centerDistance;
        if (penetration < *depth) {
            *depth = penetration;
        }
    }

    return 1;
}

// Find contact points for box-box collision
static void FindContactPoints(OBB* a, OBB* b, CollisionManifold* manifold) {
    if (manifold->contactCount == 0) return;

    Vector3 normal = Vec3Normalize(manifold->contacts[0].normal);

    // Ensure normal points from A to B
    Vector3 delta = Vec3Sub(b->center, a->center);
    if (Vec3Dot(delta, normal) < 0.0f) {
        normal = Vec3Mul(normal, -1.0f);
    }

    // Calculate contact point at the midpoint of penetration
    float centerDist = Vec3Dot(delta, normal);
    Vector3 contactPoint = Vec3Add(a->center, Vec3Mul(normal, centerDist * 0.5f));

    // Update contact with correct data
    manifold->contacts[0].position = contactPoint;
    manifold->contacts[0].normal = normal;
    manifold->contacts[0].r1 = Vec3Sub(contactPoint, a->center);
    manifold->contacts[0].r2 = Vec3Sub(contactPoint, b->center);
    manifold->contactCount = 1;
}

// Full OBB-OBB collision test
int Physics_TestOBBCollision(OBB* a, OBB* b, CollisionManifold* manifold) {
    float minDepth = FLT_MAX;
    Vector3 collisionNormal = { 0, 0, 0 };
    int bestAxisIndex = -1;

    // Test all 15 potential separating axes

    // Test 3 face normals of A
    for (int i = 0; i < 3; i++) {
        Vector3 axis = a->axes[i];
        float depth = minDepth;
        if (!TestAxisSeparation(a, b, axis, &depth)) return 0;
        if (depth < minDepth) {
            minDepth = depth;
            collisionNormal = axis;
            bestAxisIndex = i;
        }
    }

    // Test 3 face normals of B
    for (int i = 0; i < 3; i++) {
        Vector3 axis = b->axes[i];
        float depth = minDepth;
        if (!TestAxisSeparation(a, b, axis, &depth)) return 0;
        if (depth < minDepth) {
            minDepth = depth;
            collisionNormal = axis;
            bestAxisIndex = i + 3;
        }
    }

    // Test 9 edge-edge cross products
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Vector3 axis = Vec3Cross(a->axes[i], b->axes[j]);
            float axisLen = Vec3Length(axis);

            // Skip nearly parallel edges
            if (axisLen < 0.0001f) continue;

            axis = Vec3Mul(axis, 1.0f / axisLen);
            float depth = minDepth;
            if (!TestAxisSeparation(a, b, axis, &depth)) return 0;
            if (depth < minDepth) {
                minDepth = depth;
                collisionNormal = axis;
                bestAxisIndex = 6 + i * 3 + j;
            }
        }
    }

    // Collision detected, fill manifold
    if (manifold) {
        // Ensure normal points from A to B
        Vector3 centerDelta = Vec3Sub(b->center, a->center);
        if (Vec3Dot(centerDelta, collisionNormal) < 0.0f) {
            collisionNormal = Vec3Mul(collisionNormal, -1.0f);
        }

        manifold->contactCount = 1;
        manifold->contacts[0].normal = Vec3Normalize(collisionNormal);
        manifold->contacts[0].penetration = minDepth;

        FindContactPoints(a, b, manifold);
    }

    return 1;
}


// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void Physics_Init(void) {
    memset(&world, 0, sizeof(PhysicsWorld));
    world.gravity = { 0, PHYSICS_GRAVITY, 0 };
    world.substeps = 4;
    world.sleepEnabled = 1;
    world.initialized = 1;
}

void Physics_Shutdown(void) {
    memset(&world, 0, sizeof(PhysicsWorld));
}

void Physics_SetGravity(Vector3 gravity) {
    world.gravity = gravity;
}

Vector3 Physics_GetGravity(void) {
    return world.gravity;
}

void Physics_SetSubsteps(int substeps) {
    world.substeps = maxf(1, substeps);
}

void Physics_SetSleepEnabled(int enabled) {
    world.sleepEnabled = enabled;
}

int Physics_CreateBoxBody(Vector3 position, Vector3 rotation, Vector3 halfExtents,
    BodyType type, PhysicsMaterial material) {
    if (world.bodyCount >= PHYSICS_MAX_BODIES) return -1;

    int id = world.bodyCount++;
    PhysicsBody* body = &world.bodies[id];

    memset(body, 0, sizeof(PhysicsBody));
    body->id = id;
    body->active = 1;
    body->type = type;
    body->position = position;
    body->rotation = rotation;
    body->scale = Vec3Mul(halfExtents, 2.0f);
    body->material = material;
    body->collisionLayer = 1;
    body->collisionMask = 0xFFFFFFFF;

    float volume = halfExtents.x * halfExtents.y * halfExtents.z * 8.0f;
    body->mass = (type == BODY_STATIC) ? 0.0f : volume * material.density;
    body->invMass = (body->mass > 0.0f) ? 1.0f / body->mass : 0.0f;

    CalculateBoxInertia(halfExtents, body->mass, body->inertia);
    InvertMatrix3x3(body->inertia, body->invInertia);

    UpdateOBB(body);

    return id;
}

void Physics_RemoveBody2(int bodyId) {
    if (bodyId < 0 || bodyId >= world.bodyCount) return;
    world.bodies[bodyId].active = 0;
}
void Physics_RemoveBody(int bodyId) {
    if (bodyId < 0 || bodyId >= world.bodyCount) return;

    // Swap with last active body and decrement count
    if (bodyId != world.bodyCount - 1) {
        world.bodies[bodyId] = world.bodies[world.bodyCount - 1];
        world.bodies[bodyId].id = bodyId; // Update ID
    }
    world.bodyCount--;
}

PhysicsBody* Physics_GetBody(int bodyId) {
    if (bodyId < 0 || bodyId >= world.bodyCount) return NULL;
    return &world.bodies[bodyId];
}

int Physics_IsBodyValid(int bodyId) {
    return (bodyId >= 0 && bodyId < world.bodyCount && world.bodies[bodyId].active);
}

void Physics_SetPosition(int bodyId, Vector3 position) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) {
        body->position = position;
        UpdateOBB(body);
    }
}

Vector3 Physics_GetPosition(int bodyId) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    Vector3 ZERO = { 0,0,0 };
    return body ? body->position : ZERO;
}

void Physics_SetRotation(int bodyId, Vector3 rotation) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) {
        body->rotation = rotation;
        UpdateOBB(body);
    }
}

Vector3 Physics_GetRotation(int bodyId) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    Vector3 ZERO = { 0,0,0 };
    return body ? body->rotation : ZERO;
}

void Physics_SetVelocity(int bodyId, Vector3 velocity) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body && body->type == BODY_DYNAMIC) {
        body->velocity = velocity;
        body->sleeping = 0;
    }
}

Vector3 Physics_GetVelocity(int bodyId) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    Vector3 ZERO = { 0,0,0 };
    return body ? body->velocity : ZERO;
}

void Physics_SetMaterial(int bodyId, PhysicsMaterial material) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) body->material = material;
}

void Physics_AddForce(int bodyId, Vector3 force) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body && body->type == BODY_DYNAMIC) {
        body->force = Vec3Add(body->force, force);
        body->sleeping = 0;
    }
}

void Physics_AddImpulse(int bodyId, Vector3 impulse) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body && body->type == BODY_DYNAMIC) {
        body->velocity = Vec3Add(body->velocity, Vec3Mul(impulse, body->invMass));
        body->sleeping = 0;
    }
}

void Physics_ClearForces(int bodyId) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) {
        body->force = { 0, 0, 0 };
        body->torque = { 0, 0, 0 };
    }
}

// Collision resolution with impulse
// Enhanced collision response with weight and friction
// Enhanced collision response with proper weight-based pushing
static void ResolveCollision(PhysicsBody* a, PhysicsBody* b, ContactPoint* contact) {
    if (a->isTrigger || b->isTrigger) return;

    Vector3 normal = Vec3Normalize(contact->normal);

    // Calculate relative velocity at contact point
    Vector3 rv = Vec3Sub(b->velocity, a->velocity);
    float velAlongNormal = Vec3Dot(rv, normal);

    // Don't resolve if velocities are separating
    if (velAlongNormal > 0.0f) return;

    // Calculate effective masses
    float invMassSum = a->invMass + b->invMass;
    if (invMassSum <= 0.0001f) return; // Both static or infinite mass

    // Combined restitution (bounciness)
    float e = minf(a->material.restitution, b->material.restitution);

    // Calculate impulse scalar with restitution
    float j = -(1.0f + e) * velAlongNormal;
    j /= invMassSum;

    Vector3 impulse = Vec3Mul(normal, j);

    // Apply impulse based on inverse mass (lighter objects move more)
    if (a->type == BODY_DYNAMIC) {
        a->velocity = Vec3Sub(a->velocity, Vec3Mul(impulse, a->invMass));
    }
    if (b->type == BODY_DYNAMIC) {
        b->velocity = Vec3Add(b->velocity, Vec3Mul(impulse, b->invMass));
    }

    // === FRICTION ===
    // Recalculate relative velocity after normal impulse
    rv = Vec3Sub(b->velocity, a->velocity);

    // Calculate tangent vector (perpendicular to normal)
    Vector3 tangent = Vec3Sub(rv, Vec3Mul(normal, Vec3Dot(rv, normal)));
    float tangentLength = Vec3Length(tangent);

    if (tangentLength > 0.0001f) {
        tangent = Vec3Mul(tangent, 1.0f / tangentLength);

        // Calculate friction impulse
        float jt = -Vec3Dot(rv, tangent);
        jt /= invMassSum;

        // Coulomb friction model
        float mu_s = sqrtf(a->material.staticFriction * b->material.staticFriction);
        float mu_d = sqrtf(a->material.dynamicFriction * b->material.dynamicFriction);

        Vector3 frictionImpulse;
        if (fabsf(jt) < j * mu_s) {
            // Static friction
            frictionImpulse = Vec3Mul(tangent, jt);
        }
        else {
            // Dynamic friction
            frictionImpulse = Vec3Mul(tangent, -j * mu_d);
        }

        if (a->type == BODY_DYNAMIC) {
            a->velocity = Vec3Sub(a->velocity, Vec3Mul(frictionImpulse, a->invMass));
        }
        if (b->type == BODY_DYNAMIC) {
            b->velocity = Vec3Add(b->velocity, Vec3Mul(frictionImpulse, b->invMass));
        }
    }

    // === POSITIONAL CORRECTION (Weight-based pushing) ===
    // This prevents sinking/overlap by directly adjusting positions
    const float percent = 0.8f;        // Penetration correction percentage (0.6-0.8)
    const float slop = 0.01f;          // Allowed penetration before correction

    float penetration = maxf(contact->penetration - slop, 0.0f);

    if (penetration > 0.0f) {
        // Calculate correction magnitude weighted by inverse mass
        float correctionMag = penetration / invMassSum * percent;
        Vector3 correction = Vec3Mul(normal, correctionMag);

        // Apply correction: lighter objects get pushed more
        if (a->type == BODY_DYNAMIC) {
            a->position = Vec3Sub(a->position, Vec3Mul(correction, a->invMass));
        }
        if (b->type == BODY_DYNAMIC) {
            b->position = Vec3Add(b->position, Vec3Mul(correction, b->invMass));
        }
    }
}

static void ResolveCollision2(PhysicsBody* a, PhysicsBody* b, ContactPoint* contact) {
    if (a->isTrigger || b->isTrigger) return;

    Vector3 rv = Vec3Sub(b->velocity, a->velocity);
    float velAlongNormal = Vec3Dot(rv, contact->normal);

    if (velAlongNormal > 0) return;

    float e = minf(a->material.restitution, b->material.restitution);
    float j = -(1.0f + e) * velAlongNormal;
    j /= (a->invMass + b->invMass);

    Vector3 impulse = Vec3Mul(contact->normal, j);

    if (a->type == BODY_DYNAMIC) {
        a->velocity = Vec3Sub(a->velocity, Vec3Mul(impulse, a->invMass));
    }
    if (b->type == BODY_DYNAMIC) {
        b->velocity = Vec3Add(b->velocity, Vec3Mul(impulse, b->invMass));
    }

    // Positional correction
    const float percent = 0.8f;
    const float slop = 0.01f;
    float correctionMag = maxf(contact->penetration - slop, 0.0f) / (a->invMass + b->invMass) * percent;
    Vector3 correction = Vec3Mul(contact->normal, correctionMag);

    if (a->type == BODY_DYNAMIC) {
        a->position = Vec3Sub(a->position, Vec3Mul(correction, a->invMass));
    }
    if (b->type == BODY_DYNAMIC) {
        b->position = Vec3Add(b->position, Vec3Mul(correction, b->invMass));
    }
}

void Physics_Update(float deltaTime) {
    if (!world.initialized) return;

    float dt = deltaTime / (float)world.substeps;

    for (int step = 0; step < world.substeps; step++) {
        // Apply forces
        for (int i = 0; i < world.bodyCount; i++) {
            PhysicsBody* body = &world.bodies[i];
            if (!body->active || body->type != BODY_DYNAMIC) continue;

            // Gravity
            body->force = Vec3Add(body->force, Vec3Mul(world.gravity, body->mass));

            // Integrate velocity
            body->velocity = Vec3Add(body->velocity, Vec3Mul(body->force, body->invMass * dt));
            body->velocity = Vec3Mul(body->velocity, PHYSICS_LINEAR_DAMPING);

            // Clear forces
            body->force = { 0, 0, 0 };

            // Integrate position
            body->position = Vec3Add(body->position, Vec3Mul(body->velocity, dt));

            UpdateOBB(body);
        }

        // Collision detection and resolution
        world.manifestCount = 0;
        for (int i = 0; i < world.bodyCount; i++) {
            PhysicsBody* a = &world.bodies[i];
            if (!a->active) continue;

            for (int j = i + 1; j < world.bodyCount; j++) {
                PhysicsBody* b = &world.bodies[j];
                if (!b->active) continue;

                // Skip if both static
                if (a->type == BODY_STATIC && b->type == BODY_STATIC) continue;

                // Collision filtering
                if (!(a->collisionLayer & b->collisionMask) ||
                    !(b->collisionLayer & a->collisionMask)) continue;

                CollisionManifold manifold = { 0 };
                manifold.bodyA = i;
                manifold.bodyB = j;

                if (Physics_TestOBBCollision(&a->obb, &b->obb, &manifold)) {
                    // Store manifold
                    if (world.manifestCount < PHYSICS_MAX_CONTACTS) {
                        world.manifolds[world.manifestCount++] = manifold;
                    }

                    // Resolve collision
                    for (int k = 0; k < manifold.contactCount; k++) {
                        ResolveCollision(a, b, &manifold.contacts[k]);
                    }

                    // Update OBBs after resolution
                    UpdateOBB(a);
                    UpdateOBB(b);

                    // Callback
                    if (world.collisionCallback) {
                        world.collisionCallback(i, j, &manifold);
                    }
                }
            }
        }
    }
}

void Physics_SetCollisionCallback(CollisionCallback callback) {
    world.collisionCallback = callback;
}

int Physics_CheckCollision(int bodyA, int bodyB) {
    PhysicsBody* a = Physics_GetBody(bodyA);
    PhysicsBody* b = Physics_GetBody(bodyB);
    if (!a || !b) return 0;

    return Physics_TestOBBCollision(&a->obb, &b->obb, NULL);
}

CollisionManifold Physics_GetCollisionManifold(int bodyA, int bodyB) {
    CollisionManifold manifold = { 0 };
    PhysicsBody* a = Physics_GetBody(bodyA);
    PhysicsBody* b = Physics_GetBody(bodyB);
    if (!a || !b) return manifold;

    manifold.bodyA = bodyA;
    manifold.bodyB = bodyB;
    Physics_TestOBBCollision(&a->obb, &b->obb, &manifold);
    return manifold;
}

RaycastHit Physics_Raycast(Vector3 origin, Vector3 direction, float maxDistance) {
    return Physics_RaycastLayer(origin, direction, maxDistance, 0xFFFFFFFF);
}

RaycastHit Physics_RaycastLayer(Vector3 origin, Vector3 direction,
    float maxDistance, int layerMask) {
    RaycastHit hit = { 0 };
    hit.distance = maxDistance;

    direction = Vec3Normalize(direction);

    for (int i = 0; i < world.bodyCount; i++) {
        PhysicsBody* body = &world.bodies[i];
        if (!body->active) continue;
        if (!(body->collisionLayer & layerMask)) continue;

        // Simple AABB ray test (can be improved)
        Vector3 min = Vec3Sub(body->position, body->obb.halfExtents);
        Vector3 max = Vec3Add(body->position, body->obb.halfExtents);

        float tmin = 0.0f;
        float tmax = maxDistance;

        for (int axis = 0; axis < 3; axis++) {
            float o = ((float*)&origin)[axis];
            float d = ((float*)&direction)[axis];
            float bmin = ((float*)&min)[axis];
            float bmax = ((float*)&max)[axis];

            if (fabsf(d) < 0.0001f) {
                if (o < bmin || o > bmax) break;
            }
            else {
                float t1 = (bmin - o) / d;
                float t2 = (bmax - o) / d;
                if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
                tmin = maxf(tmin, t1);
                tmax = minf(tmax, t2);
                if (tmin > tmax) break;
            }

            if (axis == 2 && tmin < hit.distance) {
                hit.hit = 1;
                hit.bodyId = i;
                hit.distance = tmin;
                hit.point = Vec3Add(origin, Vec3Mul(direction, tmin));
                hit.normal = { 0, 1, 0 }; // Simplified
            }
        }
    }

    return hit;
}

void Physics_DrawDebug(Vector3 color, float thickness) {
    for (int i = 0; i < world.bodyCount; i++) {
        if (world.bodies[i].active) {
            Physics_DrawBodyDebug(i, color, thickness);
        }
    }
}

void Physics_DrawBodyDebug(int bodyId, Vector3 color, float thickness) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (!body) return;

    GL_DrawCubeLine(body->position, body->rotation, body->scale, color, thickness);
}

void Physics_DrawContacts(Vector3 normalColor, Vector3 pointColor, float size) {
    for (int i = 0; i < world.manifestCount; i++) {
        CollisionManifold* m = &world.manifolds[i];
        for (int j = 0; j < m->contactCount; j++) {
            ContactPoint* c = &m->contacts[j];

            // Draw contact point
            GL_DrawPixel3D(c->position, pointColor, size);

            // Draw normal
            Vector3 end = Vec3Add(c->position, Vec3Mul(c->normal, 0.5f));
            GL_DrawLine3D(c->position, end, normalColor, 16.0f);
        }
    }
}

PhysicsMaterial Physics_CreateMaterial(float density, float restitution,
    float staticFriction, float dynamicFriction) {
    PhysicsMaterial mat;
    mat.density = density;
    mat.restitution = clamp(restitution, 0.0f, 1.0f);
    mat.staticFriction = clamp(staticFriction, 0.0f, 1.0f);
    mat.dynamicFriction = clamp(dynamicFriction, 0.0f, 1.0f);
    return mat;
}

OBB Physics_CreateOBB(Vector3 center, Vector3 halfExtents, Vector3 rotation) {
    OBB obb;
    obb.center = center;
    obb.halfExtents = halfExtents;
    obb.rotation = rotation;

    float rotMat[9];
    CreateRotationMatrix(rotation, rotMat);

    obb.axes[0] = { rotMat[0], rotMat[3], rotMat[6] };
    obb.axes[1] = { rotMat[1], rotMat[4], rotMat[7] };
    obb.axes[2] = { rotMat[2], rotMat[5], rotMat[8] };

    return obb;
}

int Physics_GetBodyCount(void) {
    return world.bodyCount;
}

int Physics_GetActiveBodyCount(void) {
    int count = 0;
    for (int i = 0; i < world.bodyCount; i++) {
        if (world.bodies[i].active) count++;
    }
    return count;
}

int Physics_GetCollisionCount(void) {
    return world.manifestCount;
}

void Physics_Translate(int bodyId, Vector3 translation) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) {
        body->position = Vec3Add(body->position, translation);
        UpdateOBB(body);
    }
}

void Physics_Rotate(int bodyId, Vector3 rotation) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) {
        body->rotation = Vec3Add(body->rotation, rotation);
        UpdateOBB(body);
    }
}

void Physics_SetAngularVelocity(int bodyId, Vector3 angularVel) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body && body->type == BODY_DYNAMIC) {
        body->angularVelocity = angularVel;
    }
}

Vector3 Physics_GetAngularVelocity(int bodyId) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    Vector3 ZERO = { 0,0,0 };
    return body ? body->angularVelocity : ZERO;
}

void Physics_SetMass(int bodyId, float mass) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body && body->type == BODY_DYNAMIC) {
        body->mass = maxf(0.0f, mass);
        body->invMass = (body->mass > 0.0f) ? 1.0f / body->mass : 0.0f;
        CalculateBoxInertia(body->obb.halfExtents, body->mass, body->inertia);
        InvertMatrix3x3(body->inertia, body->invInertia);
    }
}

float Physics_GetMass(int bodyId) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    return body ? body->mass : 0.0f;
}

PhysicsMaterial Physics_GetMaterial(int bodyId) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    return body ? body->material : MATERIAL_DEFAULT;
}

void Physics_SetBodyType(int bodyId, BodyType type) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) {
        body->type = type;
        if (type == BODY_STATIC) {
            body->invMass = 0.0f;
            body->velocity = { 0, 0, 0 };
            body->angularVelocity = { 0, 0, 0 };
        }
        else if (body->mass > 0.0f) {
            body->invMass = 1.0f / body->mass;
        }
    }
}

BodyType Physics_GetBodyType(int bodyId) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    return body ? body->type : BODY_STATIC;
}

void Physics_SetTrigger(int bodyId, int isTrigger) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) body->isTrigger = isTrigger;
}

int Physics_IsTrigger(int bodyId) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    return body ? body->isTrigger : 0;
}

void Physics_SetCollisionLayer(int bodyId, int layer) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) body->collisionLayer = layer;
}

void Physics_SetCollisionMask(int bodyId, int mask) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) body->collisionMask = mask;
}

void Physics_SetUserData(int bodyId, void* data) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body) body->userData = data;
}

void* Physics_GetUserData(int bodyId) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    return body ? body->userData : NULL;
}

void Physics_AddForceAtPosition(int bodyId, Vector3 force, Vector3 position) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body && body->type == BODY_DYNAMIC) {
        body->force = Vec3Add(body->force, force);
        Vector3 r = Vec3Sub(position, body->position);
        body->torque = Vec3Add(body->torque, Vec3Cross(r, force));
        body->sleeping = 0;
    }
}

void Physics_AddImpulseAtPosition(int bodyId, Vector3 impulse, Vector3 position) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body && body->type == BODY_DYNAMIC) {
        body->velocity = Vec3Add(body->velocity, Vec3Mul(impulse, body->invMass));
        Vector3 r = Vec3Sub(position, body->position);
        Vector3 angImpulse = Vec3Cross(r, impulse);
        body->angularVelocity = Vec3Add(body->angularVelocity, angImpulse);
        body->sleeping = 0;
    }
}

void Physics_AddTorque(int bodyId, Vector3 torque) {
    PhysicsBody* body = Physics_GetBody(bodyId);
    if (body && body->type == BODY_DYNAMIC) {
        body->torque = Vec3Add(body->torque, torque);
        body->sleeping = 0;
    }
}

int Physics_PointInside(Vector3 point) {
    for (int i = 0; i < world.bodyCount; i++) {
        PhysicsBody* body = &world.bodies[i];
        if (!body->active) continue;

        Vector3 local = Vec3Sub(point, body->position);

        // Transform point to OBB local space
        float x = Vec3Dot(local, body->obb.axes[0]);
        float y = Vec3Dot(local, body->obb.axes[1]);
        float z = Vec3Dot(local, body->obb.axes[2]);

        if (fabsf(x) <= body->obb.halfExtents.x &&
            fabsf(y) <= body->obb.halfExtents.y &&
            fabsf(z) <= body->obb.halfExtents.z) {
            return i;
        }
    }
    return -1;
}