/*#include "PhysGL.h"
#include <vector>
#include <algorithm>

// Internal physics manager
struct PhysicsManager {
    PhysicsWorld world;
    std::vector<PhysicsBody> bodies;
    std::vector<Collision> collisions;
    int nextBodyId;
};

static PhysicsManager g_PhysicsManager;

// Initialize physics world
void PHYS_InitWorld(PhysicsWorld* world, Vector3 gravity, float fixedTimeStep, int iterations, int maxBodies) {
    if (!world) return;

    world->gravity = gravity;
    world->fixedTimeStep = fixedTimeStep;
    world->iterations = iterations;
    world->maxBodies = maxBodies;
    world->bodyCount = 0;

    g_PhysicsManager.world = *world;
    g_PhysicsManager.bodies.clear();
    g_PhysicsManager.bodies.reserve(maxBodies);
    g_PhysicsManager.collisions.clear();
    g_PhysicsManager.collisions.reserve(maxBodies * 4); // Reserve space for collisions
    g_PhysicsManager.nextBodyId = 1;

    GL_TraceLog(TRACE_INFO, "Physics world initialized with gravity: (%f, %f, %f)",
        gravity.x, gravity.y, gravity.z);
}

// Cleanup physics world
void PHYS_CleanupWorld(PhysicsWorld* world) {
    g_PhysicsManager.bodies.clear();
    g_PhysicsManager.collisions.clear();
    g_PhysicsManager.nextBodyId = 1;
    GL_TraceLog(TRACE_INFO, "Physics world cleaned up");
}

// Create AABB from center and half size
AABB PHYS_CreateAABB(Vector3 center, Vector3 halfSize) {
    AABB aabb;
    aabb.center = center;
    aabb.halfSize = halfSize;
    aabb.min = { center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z };
    aabb.max = { center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z };
    return aabb;
}

// Update AABB based on body position
void PHYS_UpdateAABB(PhysicsBody* body, Vector3 position) {
    if (!body) return;

    body->bounds.center = position;
    body->bounds.min = {
        position.x - body->bounds.halfSize.x,
        position.y - body->bounds.halfSize.y,
        position.z - body->bounds.halfSize.z
    };
    body->bounds.max = {
        position.x + body->bounds.halfSize.x,
        position.y + body->bounds.halfSize.y,
        position.z + body->bounds.halfSize.z
    };
}

// Check if two AABBs intersect
int PHYS_AABBIntersect(const AABB* a, const AABB* b) {
    if (!a || !b) return 0;

    return (a->min.x <= b->max.x && a->max.x >= b->min.x) &&
        (a->min.y <= b->max.y && a->max.y >= b->min.y) &&
        (a->min.z <= b->max.z && a->max.z >= b->min.z);
}

// Check if AABB contains point
int PHYS_AABBContainsPoint(const AABB* aabb, Vector3 point) {
    if (!aabb) return 0;

    return (point.x >= aabb->min.x && point.x <= aabb->max.x) &&
        (point.y >= aabb->min.y && point.y <= aabb->max.y) &&
        (point.z >= aabb->min.z && point.z <= aabb->max.z);
}

// Create physics body
int PHYS_CreateBody(PhysicsWorld* world, PhysicsBodyType type, Vector3 position, Vector3 size, float mass, float restitution) {
    if (g_PhysicsManager.bodies.size() >= world->maxBodies) {
        GL_TraceLog(TRACE_WARNING, "Cannot create physics body: maximum bodies reached (%d)", world->maxBodies);
        return -1;
    }

    PhysicsBody body;
    body.id = g_PhysicsManager.nextBodyId++;
    body.type = type;
    body.position = position;
    body.velocity = { 0, 0, 0 };
    body.acceleration = { 0, 0, 0 };
    body.force = { 0, 0, 0 };
    body.size = size;
    body.mass = mass;
    body.invMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
    body.restitution = restitution;
    body.friction = 0.2f;
    body.isColliding = 0;
    body.enabled = 1;

    // Create AABB bounds
    Vector3 halfSize = { size.x * 0.5f, size.y * 0.5f, size.z * 0.5f };
    body.bounds = PHYS_CreateAABB(position, halfSize);

    g_PhysicsManager.bodies.push_back(body);
    world->bodyCount = (int)g_PhysicsManager.bodies.size();

    GL_TraceLog(TRACE_INFO, "Created physics body ID: %d, type: %d, mass: %f",
        body.id, type, mass);

    return body.id;
}

// Destroy physics body
void PHYS_DestroyBody(PhysicsWorld* world, int bodyId) {
    auto it = std::find_if(g_PhysicsManager.bodies.begin(), g_PhysicsManager.bodies.end(),
        [bodyId](const PhysicsBody& body) { return body.id == bodyId; });

    if (it != g_PhysicsManager.bodies.end()) {
        g_PhysicsManager.bodies.erase(it);
        world->bodyCount = (int)g_PhysicsManager.bodies.size();
        GL_TraceLog(TRACE_INFO, "Destroyed physics body ID: %d", bodyId);
    }
}

// Get physics body
PhysicsBody* PHYS_GetBody(PhysicsWorld* world, int bodyId) {
    auto it = std::find_if(g_PhysicsManager.bodies.begin(), g_PhysicsManager.bodies.end(),
        [bodyId](const PhysicsBody& body) { return body.id == bodyId; });

    if (it != g_PhysicsManager.bodies.end()) {
        return &(*it);
    }
    return nullptr;
}

// Set body position
void PHYS_SetBodyPosition(PhysicsWorld* world, int bodyId, Vector3 position) {
    PhysicsBody* body = PHYS_GetBody(world, bodyId);
    if (body) {
        body->position = position;
        PHYS_UpdateAABB(body, position);
    }
}


void PHYS_SetBodySize(PhysicsWorld* world, int bodyId, Vector3 size) {
    PhysicsBody* body = PHYS_GetBody(world, bodyId);

    if (body) {

        Vector3 halfSize = { size.x * 0.5f, size.y * 0.5f, size.z * 0.5f };
        body->bounds = PHYS_CreateAABB(body->position, halfSize);
        PHYS_UpdateAABB(body, body->position);
        body->size = size;
    }
}
// Set body velocity
void PHYS_SetBodyVelocity(PhysicsWorld* world, int bodyId, Vector3 velocity) {
    PhysicsBody* body = PHYS_GetBody(world, bodyId);
    if (body) {
        body->velocity = velocity;
    }
}

// Apply force to body
void PHYS_ApplyForce(PhysicsWorld* world, int bodyId, Vector3 force) {
    PhysicsBody* body = PHYS_GetBody(world, bodyId);
    if (body && body->type == PHYSICS_DYNAMIC) {
        body->force.x += force.x * 2.0f;
        body->force.y += force.y * 4.0f;
        body->force.z += force.z * 2.0f;
    }
}

// Apply impulse to body
void PHYS_ApplyImpulse(PhysicsWorld* world, int bodyId, Vector3 impulse) {
    PhysicsBody* body = PHYS_GetBody(world, bodyId);
    if (body && body->type == PHYSICS_DYNAMIC) {
        body->velocity.x += impulse.x * body->invMass;
        body->velocity.y += impulse.y * body->invMass;
        body->velocity.z += impulse.z * body->invMass;
    }
}

// Check collision between two bodies
int PHYS_CheckCollision(PhysicsWorld* world, int bodyAId, int bodyBId, Collision* collision) {
    PhysicsBody* bodyA = PHYS_GetBody(world, bodyAId);
    PhysicsBody* bodyB = PHYS_GetBody(world, bodyBId);

    if (!bodyA || !bodyB || !bodyA->enabled || !bodyB->enabled) {
        return 0;
    }

    // Check AABB intersection
    if (!PHYS_AABBIntersect(&bodyA->bounds, &bodyB->bounds)) {
        return 0;
    }

    if (collision) {
        collision->bodyA = bodyAId;
        collision->bodyB = bodyBId;

        // Calculate collision normal and depth (simplified for AABB)
        Vector3 centerA = bodyA->bounds.center;
        Vector3 centerB = bodyB->bounds.center;
        Vector3 delta = { centerB.x - centerA.x, centerB.y - centerA.y, centerB.z - centerA.z };

        // Calculate overlap on each axis
        float overlapX = bodyA->bounds.halfSize.x + bodyB->bounds.halfSize.x - fabs(delta.x);
        float overlapY = bodyA->bounds.halfSize.y + bodyB->bounds.halfSize.y - fabs(delta.y);
        float overlapZ = bodyA->bounds.halfSize.z + bodyB->bounds.halfSize.z - fabs(delta.z);

        // Find the axis of minimum penetration
        if (overlapX < overlapY && overlapX < overlapZ) {
            collision->depth = overlapX;
            collision->normal = { (delta.x > 0) ? 1.0f : -1.0f, 0.0f, 0.0f };
        }
        else if (overlapY < overlapZ) {
            collision->depth = overlapY;
            collision->normal = { 0.0f, (delta.y > 0) ? 1.0f : -1.0f, 0.0f };
        }
        else {
            collision->depth = overlapZ;
            collision->normal = { 0.0f, 0.0f, (delta.z > 0) ? 1.0f : -1.0f };
        }

        // Calculate contact point (simplified)
        collision->contactPoint = {
            (bodyA->bounds.min.x + bodyA->bounds.max.x + bodyB->bounds.min.x + bodyB->bounds.max.x) * 0.25f,
            (bodyA->bounds.min.y + bodyA->bounds.max.y + bodyB->bounds.min.y + bodyB->bounds.max.y) * 0.25f,
            (bodyA->bounds.min.z + bodyA->bounds.max.z + bodyB->bounds.min.z + bodyB->bounds.max.z) * 0.25f
        };
    }

    return 1;
}

// Resolve collision
void PHYS_ResolveCollision(PhysicsWorld* world, Collision* collision) {
    if (!collision) return;

    PhysicsBody* bodyA = PHYS_GetBody(world, collision->bodyA);
    PhysicsBody* bodyB = PHYS_GetBody(world, collision->bodyB);

    if (!bodyA || !bodyB) return;

    // Skip if both are static
    if (bodyA->type == PHYSICS_STATIC && bodyB->type == PHYSICS_STATIC) {
        return;
    }

    // Relative velocity
    Vector3 relativeVel = {
        bodyB->velocity.x - bodyA->velocity.x,
        bodyB->velocity.y - bodyA->velocity.y,
        bodyB->velocity.z - bodyA->velocity.z
    };

    // Velocity along normal
    float velAlongNormal = relativeVel.x * collision->normal.x +
        relativeVel.y * collision->normal.y +
        relativeVel.z * collision->normal.z;

    // Do not resolve if objects are separating
    if (velAlongNormal > 0) {
        return;
    }

    // Calculate restitution (bounciness)
    float restitution = fmin(bodyA->restitution, bodyB->restitution);

    // Calculate impulse scalar
    float impulseScalar = -(1.0f + restitution) * velAlongNormal;
    impulseScalar /= bodyA->invMass + bodyB->invMass;

    // Apply impulse
    Vector3 impulse = {
        impulseScalar * collision->normal.x,
        impulseScalar * collision->normal.y,
        impulseScalar * collision->normal.z
    };

    // Apply impulses to velocities
    if (bodyA->type != PHYSICS_STATIC) {
        bodyA->velocity.x -= impulse.x * bodyA->invMass;
        bodyA->velocity.y -= impulse.y * bodyA->invMass;
        bodyA->velocity.z -= impulse.z * bodyA->invMass;
    }

    if (bodyB->type != PHYSICS_STATIC) {
        bodyB->velocity.x += impulse.x * bodyB->invMass;
        bodyB->velocity.y += impulse.y * bodyB->invMass;
        bodyB->velocity.z += impulse.z * bodyB->invMass;
    }

    // Position correction to prevent sinking
    const float percent = 0.2f; // usually 20% to 80%
    const float slop = 0.01f;   // usually 0.01 to 0.1
    Vector3 correction = {
        fmax(collision->depth - slop, 0.0f) / (bodyA->invMass + bodyB->invMass) * percent * collision->normal.x,
        fmax(collision->depth - slop, 0.0f) / (bodyA->invMass + bodyB->invMass) * percent * collision->normal.y,
        fmax(collision->depth - slop, 0.0f) / (bodyA->invMass + bodyB->invMass) * percent * collision->normal.z
    };

    if (bodyA->type != PHYSICS_STATIC) {
        bodyA->position.x -= correction.x * bodyA->invMass;
        bodyA->position.y -= correction.y * bodyA->invMass;
        bodyA->position.z -= correction.z * bodyA->invMass;
        PHYS_UpdateAABB(bodyA, bodyA->position);
    }

    if (bodyB->type != PHYSICS_STATIC) {
        bodyB->position.x += correction.x * bodyB->invMass;
        bodyB->position.y += correction.y * bodyB->invMass;
        bodyB->position.z += correction.z * bodyB->invMass;
        PHYS_UpdateAABB(bodyB, bodyB->position);
    }

    bodyA->isColliding = 1;
    bodyB->isColliding = 1;
}

// Update physics world
void PHYS_UpdateWorld(PhysicsWorld* world, float deltaTime) {
    // Clear collision states
    for (auto& body : g_PhysicsManager.bodies) {
        body.isColliding = 0;
    }

    g_PhysicsManager.collisions.clear();

    // Integrate forces and update positions
    for (auto& body : g_PhysicsManager.bodies) {
        if (!body.enabled || body.type == PHYSICS_STATIC) continue;

        // Apply gravity to dynamic bodies
        if (body.type == PHYSICS_DYNAMIC) {
            body.force.x += world->gravity.x * body.mass;
            body.force.y += world->gravity.y * body.mass;
            body.force.z += world->gravity.z * body.mass;
        }

        // Integrate acceleration
        body.acceleration.x = body.force.x * body.invMass;
        body.acceleration.y = body.force.y * body.invMass;
        body.acceleration.z = body.force.z * body.invMass;

        // Integrate velocity (semi-implicit Euler)
        body.velocity.x += body.acceleration.x * deltaTime;
        body.velocity.y += body.acceleration.y * deltaTime;
        body.velocity.z += body.acceleration.z * deltaTime;

        // Integrate position
        body.position.x += body.velocity.x * deltaTime;
        body.position.y += body.velocity.y * deltaTime;
        body.position.z += body.velocity.z * deltaTime;

        // Update AABB
        PHYS_UpdateAABB(&body, body.position);

        // Reset forces
        body.force = { 0, 0, 0 };
    }

    // Broad phase: detect collisions
    for (size_t i = 0; i < g_PhysicsManager.bodies.size(); ++i) {
        for (size_t j = i + 1; j < g_PhysicsManager.bodies.size(); ++j) {
            Collision collision;
            if (PHYS_CheckCollision(world, g_PhysicsManager.bodies[i].id, g_PhysicsManager.bodies[j].id, &collision)) {
                g_PhysicsManager.collisions.push_back(collision);
            }
        }
    }

    // Narrow phase: resolve collisions
    for (int iter = 0; iter < world->iterations; ++iter) {
        for (auto& collision : g_PhysicsManager.collisions) {
            PHYS_ResolveCollision(world, &collision);
        }
    }
}


// Debug draw AABB
void PHYS_DebugDrawAABB(PhysicsWorld* world, int bodyId, Vector3 color) {
    PhysicsBody* body = PHYS_GetBody(world, bodyId);
    if (!body) return;

    Vector3 size = {
        body->bounds.halfSize.x * 2.0f,
        body->bounds.halfSize.y * 2.0f,
        body->bounds.halfSize.z * 2.0f
    };

    // Draw wireframe cube for AABB
    GL_DrawCubeLine(body->bounds.center, { 0, 0, 0 }, size, color, 2.0f);
}

// Debug draw all AABBs
void PHYS_DebugDrawAllAABBs(PhysicsWorld* world, Vector3 color) {
    for (const auto& body : g_PhysicsManager.bodies) {
        if (body.enabled) {
            PHYS_DebugDrawAABB(world, body.id, color);
        }
    }
}

// Utility functions
Vector3 PHYS_CalculateAABBCenter(const AABB* aabb) {
    if (!aabb) return { 0, 0, 0 };
    return {
        (aabb->min.x + aabb->max.x) * 0.5f,
        (aabb->min.y + aabb->max.y) * 0.5f,
        (aabb->min.z + aabb->max.z) * 0.5f
    };
}

Vector3 PHYS_CalculateAABBHalfSize(const AABB* aabb) {
    if (!aabb) return { 0, 0, 0 };
    return {
        (aabb->max.x - aabb->min.x) * 0.5f,
        (aabb->max.y - aabb->min.y) * 0.5f,
        (aabb->max.z - aabb->min.z) * 0.5f
    };
}*/