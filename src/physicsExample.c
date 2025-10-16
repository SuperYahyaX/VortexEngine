#include "LibGL.h"
#include "PhysGL_v2.h"
#include <stdio.h>
#include <stdlib.h>

// Physics body IDs
int groundBody;
int playerBody;
int boxes[20];
int numBoxes = 0;

// Player state
typedef struct {
    float moveSpeed;
    float jumpForce;
    int onGround;
    int canJump;
} PlayerState;

PlayerState player = { 0 };

// Collision callback
void OnCollision(int bodyA, int bodyB, CollisionManifold* manifold) {
    // Check if player is on ground
    if (bodyA == playerBody || bodyB == playerBody) {
        int other = (bodyA == playerBody) ? bodyB : bodyA;

        // Check if collision normal points upward (on top of something)
        if (manifold->contacts[0].normal.y > 0.7f) {
            player.onGround = 1;
            player.canJump = 1;
        }
    }
}

void InitPhysicsScene() {
    // Initialize physics system
    Physics_Init();
    Physics_SetGravity((Vector3) { 0, -98.0f, 0 });
    Physics_SetSubsteps(6);
    Physics_SetCollisionCallback(OnCollision);

    // Create ground (static)
    groundBody = Physics_CreateBoxBody(
        (Vector3) {
        0, -0.5f, 0
    },           // position
        (Vector3) {
        0, 0, 0
    },            // rotation
        (Vector3) {
        25, 0.5f, 25
    },       // half extents
        BODY_STATIC,
        MATERIAL_STONE
    );

    // Create walls
    Physics_CreateBoxBody(
        (Vector3) {
        25, 5, 0
    }, (Vector3) { 0, 0, 0 }, (Vector3) { 0.5f, 5, 25 },
            BODY_STATIC, MATERIAL_STONE
            );
    Physics_CreateBoxBody(
        (Vector3) {
        -25, 5, 0
    }, (Vector3) { 0, 0, 0 }, (Vector3) { 0.5f, 5, 25 },
            BODY_STATIC, MATERIAL_STONE
            );
    Physics_CreateBoxBody(
        (Vector3) {
        0, 5, 25
    }, (Vector3) { 0, 0, 0 }, (Vector3) { 25, 5, 0.5f },
            BODY_STATIC, MATERIAL_STONE
            );
    Physics_CreateBoxBody(
        (Vector3) {
        0, 5, -25
    }, (Vector3) { 0, 0, 0 }, (Vector3) { 25, 5, 0.5f },
            BODY_STATIC, MATERIAL_STONE
            );

    // Create player (dynamic)
    playerBody = Physics_CreateBoxBody(
        (Vector3) {
        0, 3, 0
    },
        (Vector3) {
        0, 0, 0
    },
        (Vector3) {
        0.5f, 1.0f, 0.5f
    },
        BODY_DYNAMIC,
        MATERIAL_WOOD
    );
    Physics_SetMass(playerBody, 70.0f); // 70kg player

    // Create some boxes to interact with
    for (int i = 0; i < 10; i++) {
        float x = (rand() % 40 - 20) / 2.0f;
        float z = (rand() % 40 - 20) / 2.0f;
        float y = 5.0f + i * 3.0f;

        boxes[numBoxes++] = Physics_CreateBoxBody(
            (Vector3) {
            x, y, z
        },
            (Vector3) {
            0, rand() % 360, 0
        },
            (Vector3) {
            0.5f, 0.5f, 0.5f
        },
            BODY_DYNAMIC,
            (i % 2 == 0) ? MATERIAL_WOOD : MATERIAL_METAL
        );
    }

    // Create pyramid of boxes
    int pyramidStart = numBoxes;
    float boxSize = 1.0f;
    int levels = 5;
    float startX = -10.0f;
    float startZ = 0.0f;

    for (int level = 0; level < levels; level++) {
        int boxesInLevel = levels - level;
        float levelY = 0.5f + level * boxSize;

        for (int i = 0; i < boxesInLevel; i++) {
            float x = startX + i * boxSize - (boxesInLevel - 1) * boxSize * 0.5f;

            if (numBoxes < 20) {
                boxes[numBoxes++] = Physics_CreateBoxBody(
                    (Vector3) {
                    x, levelY, startZ
                },
                    (Vector3) {
                    0, 0, 0
                },
                    (Vector3) {
                    0.4f, 0.4f, 0.4f
                },
                    BODY_DYNAMIC,
                    MATERIAL_WOOD
                );
            }
        }
    }

    // Initialize player state
    player.moveSpeed = 8.0f;
    player.jumpForce = 2200.0f;
    player.onGround = 0;
    player.canJump = 1;

    printf("=== PHYSICS ENGINE DEMO ===\n");
    printf("Controls:\n");
    printf("  WASD/Arrow Keys - Move player\n");
    printf("  SPACE - Jump\n");
    printf("  F - Apply upward force to all boxes\n");
    printf("  R - Reset scene\n");
    printf("  E - Spawn box at player position\n");
    printf("  Q - Explode boxes outward\n");
    printf("  M - Toggle mouse capture\n");
    printf("  ESC - Quit\n\n");
}

void UpdatePlayerPhysics(float deltaTime) {
    player.onGround = 0; // Reset, will be set by collision callback

    Vector3 currentVel = Physics_GetVelocity(playerBody);
    Vector3 moveVel = { 0, currentVel.y, 0 }; // Keep Y velocity

    // Horizontal movement
    if ( GL_KeyHold(GL_KEY_UP)) {
        moveVel.z -= player.moveSpeed;
    }
    if (GL_KeyHold(GL_KEY_DOWN)) {
        moveVel.z += player.moveSpeed;
    }
    if (GL_KeyHold(GL_KEY_LEFT)) {
        moveVel.x -= player.moveSpeed;
    }
    if (GL_KeyHold(GL_KEY_RIGHT)) {
        moveVel.x += player.moveSpeed;
    }

    Physics_SetVelocity(playerBody, moveVel);

    // Jump
    if (GL_KeyPressed(GL_KEY_SPACE) && player.canJump) {
        Physics_AddImpulse(playerBody, (Vector3) { 0, player.jumpForce, 0 });
        player.canJump = 0;
    }

    // Apply upward force to all boxes
    if (GL_KeyPressed(GL_KEY_F)) {
        for (int i = 0; i < numBoxes; i++) {
            Physics_AddForce(boxes[i], (Vector3) { 0, 5000.0f, 0 });
        }
    }

    // Spawn box
    if (GL_KeyPressed(GL_KEY_E) && numBoxes < 20) {
        Vector3 playerPos = Physics_GetPosition(playerBody);
        playerPos.y += 3.0f;
        playerPos.z -= 2.0f;

        boxes[numBoxes++] = Physics_CreateBoxBody(
            playerPos,
            (Vector3) {
            0, 0, 0
        },
            (Vector3) {
            0.5f, 0.5f, 0.5f
        },
            BODY_DYNAMIC,
            MATERIAL_METAL
        );
    }

    // Explosion effect
    if (GL_KeyPressed(GL_KEY_Q)) {
        Vector3 playerPos = Physics_GetPosition(playerBody);

        for (int i = 0; i < numBoxes; i++) {
            Vector3 boxPos = Physics_GetPosition(boxes[i]);
            Vector3 dir = Vec3Sub(boxPos, playerPos);
            float dist = Vec3Length(dir);

            if (dist < 15.0f && dist > 0.1f) {
                dir = Vec3Normalize(dir);
                float forceMag = 2000.0f / (dist * dist + 1.0f);
                Physics_AddImpulse(boxes[i], Vec3Mul(dir, forceMag));
            }
        }
    }

    // Reset scene
    if (GL_KeyPressed(GL_KEY_R)) {
        Physics_Shutdown();
        numBoxes = 0;
        InitPhysicsScene();
    }
}

void RenderPhysicsScene() {
    // Render ground
    Vector3 groundPos = Physics_GetPosition(groundBody);
    Vector3 groundRot = Physics_GetRotation(groundBody);
    PhysicsBody* ground = Physics_GetBody(groundBody);
    GL_DrawCube(groundPos, groundRot, Vec3Mul(ground->obb.halfExtents, 2.0f),
        GL_GenerateCheckerTexture(64, 64, 8));

    // Render player (green)
    Vector3 playerPos = Physics_GetPosition(playerBody);
    Vector3 playerRot = Physics_GetRotation(playerBody);
    PhysicsBody* playerBody_ptr = Physics_GetBody(playerBody);
    GL_DrawCubeLine(playerPos, playerRot, Vec3Mul(playerBody_ptr->obb.halfExtents, 2.0f),
        (Vector3) {
        0, 1, 0
    }, 3.0f);

    // Render boxes
    for (int i = 0; i < numBoxes; i++) {
        if (!Physics_IsBodyValid(boxes[i])) continue;

        Vector3 pos = Physics_GetPosition(boxes[i]);
        Vector3 rot = Physics_GetRotation(boxes[i]);
        PhysicsBody* box = Physics_GetBody(boxes[i]);

        // Color based on material
        Vector3 color;
        if (box->material.density < 1.0f) {
            color = (Vector3){ 0.8f, 0.6f, 0.4f }; // Wood color
        }
        else {
            color = (Vector3){ 0.7f, 0.7f, 0.8f }; // Metal color
        }

        GL_DrawCubeLine(pos, rot, Vec3Mul(box->obb.halfExtents, 2.0f), color, 2.0f);
    }

    // Draw physics debug
    Physics_DrawDebug((Vector3) { 1, 1, 0 }, 1.0f);
    Physics_DrawContacts((Vector3) { 1, 0, 0 }, (Vector3) { 1, 1, 1 }, 0.1f);
}

int main2() {
    GL_InitWindow(1280, 720, "LibGL Physics Engine - OBB Collision Demo", 1);
    GL_InitOpenGLLoader();
    GL_EnableFramebufferSRGB(0);

    Camera cam = { 0 };
    int camLock = 1;
    GL_CaptureMouse(camLock);
    GL_InitCamera(cam, (Vector3) { 0, 10, 25 }, 70.0f, 5.0f, 0.1f, TYPE_EDITOR, 1);
    GL_EnableDepthTest(1);

    // Load skybox
    GL_LoadSkyboxTextures(
        "skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg",
        "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg"
    );

    // Lighting
    GL_InitLighting();
    GL_SetShadingModel(1);

    int sunLight = GL_CreateLight(LIGHT_DIRECTIONAL);
    Light sun = GL_GetLight(sunLight);
    sun.direction = (Vector3){ 0.3f, -1.0f, 0.2f };
    sun.color = (Vector3){ 1.0f, 0.95f, 0.9f };
    sun.intensity = 1.2f;
    GL_UpdateLight(sunLight, sun);

    // Initialize font
    Font myFont = GL_LoadFont("arial.ttf", 18);

    // Initialize physics
    InitPhysicsScene();

    // ImGui
    GL_ImGui_Init();

    // Info display
    int showDebug = 1;
    int showContacts = 1;
    float timeAccum = 0.0f;

    GL_EnableFXAA(1);
    GL_EnableBloom(1);
    GL_SetBloomThreshold(1.0f);
    GL_SetBloomIntensity(1.0f);

    GL_EnableSharpness(1);
    GL_SetSharpnessIntensity(1.0f);

    GL_EnableChromaticAberration(0);
    GL_EnableMotionBlur(0, 1.0f);
    GL_SetChromaticAberrationStrength(0.015f);

    GL_EnableColorCorrection(1, 0.0f, 1.0f, 1.0f, 0.0f);

    while (!GL_PollQuit()) {
        GL_ProcessInputs();
        float dt = GL_GetDeltaTime();
        timeAccum += dt;

        // Toggle mouse capture
        if (GL_KeyPressed(GL_KEY_M)) {
            camLock = !camLock;
            GL_CaptureMouse(camLock);
        }

        // Toggle debug rendering
        if (GL_KeyPressed(GL_KEY_1)) showDebug = !showDebug;
        if (GL_KeyPressed(GL_KEY_2)) showContacts = !showContacts;

        // Update player physics
        UpdatePlayerPhysics(dt);

        // Update physics simulation
        Physics_Update(dt);

        // Rendering
        GL_ClearDepthColor(0.2f, 0.3f, 0.4f);
        GL_RenderSkybox();

        if (GL_Begin3D()) {
            RenderPhysicsScene();
            GL_End3D();
        }

        // UI
        if (GL_ImGui_ProcessBeforeRenderer(3)) {
            if (GL_ImGui_BeginWindow("Physics Info", (Vector2) { 300, 250 }, (Vector2) { 10, 10 })) {
                char info[512];
                Vector3 playerPos = Physics_GetPosition(playerBody);
                Vector3 playerVel = Physics_GetVelocity(playerBody);

                snprintf(info, sizeof(info),
                    "FPS: %d\n"
                    "Bodies: %d / %d\n"
                    "Collisions: %d\n"
                    "\nPlayer:\n"
                    "Pos: (%.1f, %.1f, %.1f)\n"
                    "Vel: (%.1f, %.1f, %.1f)\n"
                    "On Ground: %s\n"
                    "\nControls:\n"
                    "WASD - Move\n"
                    "Space - Jump\n"
                    "F - Lift boxes\n"
                    "E - Spawn box\n"
                    "Q - Explode\n"
                    "R - Reset\n"
                    "1 - Toggle Debug\n"
                    "2 - Toggle Contacts",
                    GL_GetFPS(),
                    Physics_GetActiveBodyCount(),
                    Physics_GetBodyCount(),
                    Physics_GetCollisionCount(),
                    playerPos.x, playerPos.y, playerPos.z,
                    playerVel.x, playerVel.y, playerVel.z,
                    player.onGround ? "Yes" : "No"
                );

                GL_ImGui_Text((Vector2) { 15, 30 }, (Color4f) { 1, 1, 1, 1 }, info);

                GL_ImGui_EndWindow();
            }
        }

        // Render to screen
        GL_RenderFBO();

        // Draw HUD text
        Color4 cyan = { 0, 255, 255, 255 };
        Color4 white = { 255, 255, 255, 255 };
        GL_DrawTextWithGradientH(myFont, "LibGL Physics Engine - OBB Demo",
            (Vector2) {
            10, 10
        }, 1.2f, cyan, white);

        GL_ImGui_Render();
        GL_SwapBuffers();
    }

    // Cleanup
    GL_FreeFont(myFont);
    GL_ImGui_Shutdown();
    Physics_Shutdown();
    GL_Quit();

    return 0;
}
