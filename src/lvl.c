#include "lvl.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

// Global level state
static LevelObject g_objects[MAX_LEVEL_OBJECTS];
static int g_objectCount = 0;
static LevelMap g_map;
static LevelPostProcessing g_postFX;

void LevelSystem_Init(void) {
    memset(g_objects, 0, sizeof(g_objects));
    g_objectCount = 0;

    memset(&g_map, 0, sizeof(g_map));
    g_map.fogEnabled = 0;
    g_map.fogColor = (Vector3){ 0.5f, 0.5f, 0.5f };
    g_map.fogDensity = 0.1f;
    g_map.fogStart = 10.0f;
    g_map.fogEnd = 100.0f;

    memset(&g_postFX, 0, sizeof(g_postFX));
    g_postFX.bloomThreshold = 1.0f;
    g_postFX.bloomIntensity = 0.5f;
}

void LevelSystem_Shutdown(void) {
    // Clean up textures
    for (int i = 0; i < g_objectCount; i++) {
        if (g_objects[i].active && g_objects[i].material.loaded) {
            if (g_objects[i].material.diffuseTex.id > 0) {
                GL_DestroyTexture(g_objects[i].material.diffuseTex);
            }
            if (g_objects[i].material.normalTex.id > 0) {
                GL_DestroyTexture(g_objects[i].material.normalTex);
            }
            if (g_objects[i].material.parallaxTex.id > 0) {
                GL_DestroyTexture(g_objects[i].material.parallaxTex);
            }
        }
    }
    g_objectCount = 0;
}

int LevelSystem_AddObject(const char* typeName) {
    if (g_objectCount >= MAX_LEVEL_OBJECTS) {
        printf("Error: Maximum object count reached!\n");
        return -1;
    }

    int id = g_objectCount++;
    LevelObject* obj = &g_objects[id];

    memset(obj, 0, sizeof(LevelObject));
    obj->id = id;
    obj->active = 1;
    obj->size = (Vector3){ 1, 1, 1 };
    obj->moveSpeed = 2.0f;
    obj->scaleSpeed = 1.0f;

    if (strcmp(typeName, "Cube") == 0) {
        obj->type = OBJ_CUBE;
        strcpy(obj->name, "Cube");
    }
    else if (strcmp(typeName, "Model") == 0) {
        obj->type = OBJ_MODEL_STATIC;
        strcpy(obj->name, "Model");
    }
    else if (strcmp(typeName, "AnimatedModel") == 0) {
        obj->type = OBJ_MODEL_ANIMATED;
        strcpy(obj->name, "AnimatedModel");
    }
    else if (strcmp(typeName, "PointLight") == 0) {
        obj->type = OBJ_LIGHT;
        strcpy(obj->name, "PointLight");
        obj->light.type = LIGHT_POINT;
        obj->light.lightIndex = GL_CreateLight(LIGHT_POINT);
        obj->light.enabled = 1;
        obj->light.intensity = 1.0f;
        obj->light.specular = 1.0f;
        obj->light.color = (Vector3){ 1.0f, 1.0f, 1.0f };
    }
    else if (strcmp(typeName, "SpotLight") == 0) {
        obj->type = OBJ_LIGHT;
        strcpy(obj->name, "SpotLight");
        obj->light.type = LIGHT_SPOT;
        obj->light.lightIndex = GL_CreateLight(LIGHT_SPOT);
        obj->light.enabled = 1;
        obj->light.intensity = 1.0f;
        obj->light.specular = 1.0f;
        obj->light.color = (Vector3){ 1.0f, 1.0f, 1.0f };
        obj->light.cutOff = 12.5f;
        obj->light.outerCutOff = 17.5f;
    }
    else if (strcmp(typeName, "DirectionalLight") == 0 || strcmp(typeName, "DirectionLight") == 0) {
        obj->type = OBJ_LIGHT;
        strcpy(obj->name, "DirectionalLight");
        obj->light.type = LIGHT_DIRECTIONAL;
        obj->light.lightIndex = GL_CreateLight(LIGHT_DIRECTIONAL);
        obj->light.enabled = 1;
        obj->light.intensity = 1.0f;
        obj->light.specular = 1.0f;
        obj->light.color = (Vector3){ 1.0f, 1.0f, 1.0f };
        obj->light.direction = (Vector3){ -0.2f, -1.0f, -0.3f };
    }

    return id;
}

LevelObject* LevelSystem_GetObject(int id) {
    if (id < 0 || id >= g_objectCount) return NULL;
    if (!g_objects[id].active) return NULL;
    return &g_objects[id];
}

void LevelSystem_RemoveObject(int id) {
    if (id < 0 || id >= g_objectCount) return;
    g_objects[id].active = 0;
}

void LevelSystem_ClearAllObjects(void) {
    for (int i = 0; i < g_objectCount; i++) {
        g_objects[i].active = 0;
    }
    g_objectCount = 0;
}

void LevelSystem_Update(float deltaTime) {
    for (int i = 0; i < g_objectCount; i++) {
        LevelObject* obj = &g_objects[i];
        if (!obj->active) continue;

        // Update animated models
        if (obj->type == OBJ_MODEL_ANIMATED && obj->animationPlaying && obj->animatorID >= 0) {
            // This updates all animations - individual control can be added if needed
            GL_UpdateAllAnimations(deltaTime * obj->animationSpeed);
        }

        // Update animated movement (existing code)
        if (obj->isMoving) {
            Vector3 dir = {
                obj->targetPosition.x - obj->position.x,
                obj->targetPosition.y - obj->position.y,
                obj->targetPosition.z - obj->position.z
            };
            float dist = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

            if (dist < 0.01f) {
                obj->position = obj->targetPosition;
                obj->isMoving = 0;
            }
            else {
                float step = obj->moveSpeed * deltaTime;
                if (step > dist) step = dist;

                dir.x /= dist; dir.y /= dist; dir.z /= dist;
                obj->position.x += dir.x * step;
                obj->position.y += dir.y * step;
                obj->position.z += dir.z * step;
            }
        }

        // Update animated scaling
        if (obj->isScaling) {
            Vector3 diff = {
                obj->targetScale.x - obj->size.x,
                obj->targetScale.y - obj->size.y,
                obj->targetScale.z - obj->size.z
            };
            float dist = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

            if (dist < 0.01f) {
                obj->size = obj->targetScale;
                obj->isScaling = 0;
            }
            else {
                float step = obj->scaleSpeed * deltaTime;
                if (step > dist) step = dist;

                diff.x /= dist; diff.y /= dist; diff.z /= dist;
                obj->size.x += diff.x * step;
                obj->size.y += diff.y * step;
                obj->size.z += diff.z * step;
            }
        }

        // Update lights
        if (obj->type == OBJ_LIGHT && obj->light.enabled) {
            Light light = GL_GetLight(obj->light.lightIndex);
            light.position = obj->position;
            light.color = obj->light.color;
            light.intensity = obj->light.intensity;
            light.specular = obj->light.specular;
            light.enabled = obj->light.enabled;

            if (obj->light.type == LIGHT_SPOT) {
                light.direction = obj->light.direction;
                light.cutOff = obj->light.cutOff;
                light.outerCutOff = obj->light.outerCutOff;
            }
            else if (obj->light.type == LIGHT_DIRECTIONAL) {
                light.direction = obj->light.direction;
            }

            GL_UpdateLight(obj->light.lightIndex, light);
        }
    }
}

void LevelSystem_Render(void) {
    // Apply post-processing settings
    GL_EnableBloom(g_postFX.bloomEnabled);
    GL_EnableFXAA(g_postFX.fxaaEnabled);
    GL_SetBloomThreshold(g_postFX.bloomThreshold);
    GL_SetBloomIntensity(g_postFX.bloomIntensity);

    // Apply fog settings
    GL_EnableFog(g_map.fogEnabled);
    if (g_map.fogEnabled) {
        GL_SetFogColor(g_map.fogColor);
        GL_SetFogDensity(g_map.fogDensity);
        GL_SetFogStart(g_map.fogStart);
        GL_SetFogEnd(g_map.fogEnd);
    }

    // Render skybox if loaded
    if (g_map.skyboxLoaded) {
        GL_RenderSkybox();
    }

    // Render all objects
    if (GL_Begin3D()) {
        for (int i = 0; i < g_objectCount; i++) {
            LevelObject* obj = &g_objects[i];
            if (!obj->active) continue;

            // Load textures if needed
            if (!obj->material.loaded && strlen(obj->material.diffuseTexture) > 0) {
                obj->material.diffuseTex = GL_LoadTexture(obj->material.diffuseTexture, TEXTURE_FILTER_LINEAR);

                if (strlen(obj->material.normalMap) > 0) {
                    obj->material.normalTex = GL_LoadNormalMap(obj->material.normalMap, TEXTURE_FILTER_LINEAR);
                }
                if (strlen(obj->material.parallaxMap) > 0) {
                    obj->material.parallaxTex = GL_LoadParallaxMap(obj->material.parallaxMap, TEXTURE_FILTER_LINEAR);
                }
                obj->material.loaded = 1;
            }

            // Render based on type
            switch (obj->type) {
            case OBJ_CUBE:
                if (obj->material.normalTex.id > 0) {
                    GL_DrawCubeWithNormalMap(obj->position, obj->rotation, obj->size,
                        obj->material.diffuseTex, obj->material.normalTex);
                }
                else {
                    GL_DrawCube(obj->position, obj->rotation, obj->size, obj->material.diffuseTex);
                }
                break;

            case OBJ_MODEL_STATIC:
                if (obj->modelID >= 0) {
                    GL_DrawStaticModel(obj->modelID, obj->position, obj->rotation, obj->size);
                }
                break;

            case OBJ_MODEL_ANIMATED:
                if (obj->modelID >= 0) {
                    if (obj->animatorID >= 0 && obj->animationPlaying) {
                        // Draw as animated
                        GL_DrawAnimatedModel(obj->modelID, obj->animatorID, obj->position, obj->rotation, obj->size);
                    }
                    else {
                        // Fallback to static drawing if no animator or animation stopped
                        GL_DrawStaticModel(obj->modelID, obj->position, obj->rotation, obj->size);
                    }
                }
                break;
            }
        }
        GL_End3D();
    }
}

// Map functions
void LevelMap_SetSkyBox(const char* right, const char* left, const char* top,
    const char* bottom, const char* front, const char* back) {
    strncpy(g_map.skyboxPaths[0], right, MAX_TEXTURE_PATH - 1);
    strncpy(g_map.skyboxPaths[1], left, MAX_TEXTURE_PATH - 1);
    strncpy(g_map.skyboxPaths[2], top, MAX_TEXTURE_PATH - 1);
    strncpy(g_map.skyboxPaths[3], bottom, MAX_TEXTURE_PATH - 1);
    strncpy(g_map.skyboxPaths[4], front, MAX_TEXTURE_PATH - 1);
    strncpy(g_map.skyboxPaths[5], back, MAX_TEXTURE_PATH - 1);

    GL_LoadSkyboxTextures(right, left, top, bottom, front, back);
    g_map.skyboxLoaded = 1;
}

void LevelMap_EnableFog(int enable) {
    g_map.fogEnabled = enable;
}

void LevelMap_SetFogColor(Vector3 color) {
    g_map.fogColor = color;
}

void LevelMap_SetFogDensity(float density) {
    g_map.fogDensity = density;
}

// Post processing
void LevelPostProcessing_EnableBloom(int enable) {
    g_postFX.bloomEnabled = enable;
}

void LevelPostProcessing_EnableFXAA(int enable) {
    g_postFX.fxaaEnabled = enable;
}

void LevelPostProcessing_SetBloomThreshold(float threshold) {
    g_postFX.bloomThreshold = threshold;
}

void LevelPostProcessing_SetBloomIntensity(float intensity) {
    g_postFX.bloomIntensity = intensity;
}

// Animation functions
void LevelObject_AnimatedMoveObjectTo(int id, Vector3 target, float speed) {
    LevelObject* obj = LevelSystem_GetObject(id);
    if (obj) {
        obj->targetPosition = target;
        obj->moveSpeed = speed;
        obj->isMoving = 1;
    }
}

void LevelObject_AnimatedScaleObjectTo(int id, Vector3 target, float speed) {
    LevelObject* obj = LevelSystem_GetObject(id);
    if (obj) {
        obj->targetScale = target;
        obj->scaleSpeed = speed;
        obj->isScaling = 1;
    }
}

// Trace functions
void LevelTrace_Print(const char* msg) {
    GL_TraceLog(TRACE_INFO, msg);
}

void LevelTrace_PrintWarn(const char* msg) {
    GL_TraceLog(TRACE_WARNING, msg);
}

void LevelTrace_PrintError(const char* msg) {
    GL_TraceLog(TRACE_ERROR, msg);
}

void LevelTrace_PrintInfo(const char* msg) {
    GL_TraceLog(TRACE_INFO, msg);
}

// Light object functions
void LevelObject_ToggleLight(int id) {
    LevelObject* obj = LevelSystem_GetObject(id);
    if (obj && obj->type == OBJ_LIGHT) {
        obj->light.enabled = !obj->light.enabled;

        Light light = GL_GetLight(obj->light.lightIndex);
        light.enabled = obj->light.enabled;
        GL_UpdateLight(obj->light.lightIndex, light);

        printf("Light %d toggled to %s\n", id, obj->light.enabled ? "ON" : "OFF");
    }
}

void LevelObject_DestroyLight(int id) {
    LevelObject* obj = LevelSystem_GetObject(id);
    if (obj && obj->type == OBJ_LIGHT) {
        GL_DeleteLight(obj->light.lightIndex);
        obj->active = 0;
        printf("Light %d destroyed\n", id);
    }
}

// Model functions
int LevelSystem_LoadModel(const char* path, int flipTextures) {
    return LevelSystem_LoadStaticModel(path, flipTextures);
}
// Load static model (no animation)
int LevelSystem_LoadStaticModel(const char* path, int flipTextures) {
    if (g_objectCount >= MAX_LEVEL_OBJECTS) {
        printf("Error: Maximum object count reached!\n");
        return -1;
    }

    int id = g_objectCount++;
    LevelObject* obj = &g_objects[id];

    memset(obj, 0, sizeof(LevelObject));
    obj->id = id;
    obj->active = 1;
    obj->type = OBJ_MODEL_STATIC;  // Explicitly static
    obj->size = (Vector3){ 1, 1, 1 };
    obj->flipTextures = flipTextures;
    obj->animatorID = -1;  // No animator for static models

    strncpy(obj->modelPath, path, MAX_TEXTURE_PATH - 1);
    obj->modelID = GL_LoadModel(path, flipTextures);

    if (obj->modelID < 0) {
        printf("Error: Failed to load static model: %s\n", path);
        obj->active = 0;
        g_objectCount--;
        return -1;
    }

    strcpy(obj->name, "StaticModel");
    printf("Loaded static model: %s (ModelID: %d, ObjectID: %d)\n", path, obj->modelID, id);

    return id;
}
/*
// Load animated model
int LevelSystem_LoadAnimatedModel(const char* path, int flipTextures) {
    if (g_objectCount >= MAX_LEVEL_OBJECTS) {
        printf("Error: Maximum object count reached!\n");
        return -1;
    }

    int id = g_objectCount++;
    LevelObject* obj = &g_objects[id];

    memset(obj, 0, sizeof(LevelObject));
    obj->id = id;
    obj->active = 1;
    obj->type = OBJ_MODEL_ANIMATED;  // Explicitly animated
    obj->size = (Vector3){ 1, 1, 1 };
    obj->flipTextures = flipTextures;
    obj->animatorID = -1;  // Will be set when animation is loaded
    obj->animationPlaying = 0;
    obj->animationSpeed = 1.0f;

    strncpy(obj->modelPath, path, MAX_TEXTURE_PATH - 1);
    obj->modelID = GL_LoadModel(path, flipTextures);

    if (obj->modelID < 0) {
        printf("Error: Failed to load animated model: %s\n", path);
        obj->active = 0;
        g_objectCount--;
        return -1;
    }

    strcpy(obj->name, "AnimatedModel");
    printf("Loaded animated model: %s (ModelID: %d, ObjectID: %d)\n", path, obj->modelID, id);

    return id;
}
*/
// Load animation for an existing animated model
int LevelSystem_LoadAnimationForModel(const char* path, int modelObjectID) {
    LevelObject* obj = LevelSystem_GetObject(modelObjectID);
    if (!obj) {
        printf("Error: Invalid model object ID: %d\n", modelObjectID);
        return -1;
    }

    if (obj->type != OBJ_MODEL_ANIMATED) {
        printf("Error: Object %d is not an animated model\n", modelObjectID);
        return -1;
    }

    strncpy(obj->animationPath, path, MAX_TEXTURE_PATH - 1);
    obj->animatorID = GL_LoadAnimation(path, obj->modelID);

    if (obj->animatorID < 0) {
        printf("Error: Failed to load animation: %s for model %d\n", path, modelObjectID);
        return -1;
    }

    obj->animationPlaying = 1;
    printf("Loaded animation: %s (AnimatorID: %d) for model %d\n", path, obj->animatorID, modelObjectID);

    return obj->animatorID;
}

int LevelSystem_LoadAnimation(const char* path, int modelObjectID) {
    LevelObject* obj = LevelSystem_GetObject(modelObjectID);
    if (!obj || obj->modelID < 0) {
        printf("Error: Invalid model object ID\n");
        return -1;
    }

    obj->type = OBJ_MODEL_ANIMATED;
    strncpy(obj->animationPath, path, MAX_TEXTURE_PATH - 1);
    obj->animatorID = GL_LoadAnimation(path, obj->modelID);
    obj->animationPlaying = 1;

    if (obj->animatorID < 0) {
        printf("Error: Failed to load animation: %s\n", path);
        return -1;
    }

    printf("Loaded animation: %s (AnimID: %d)\n", path, obj->animatorID);
    return obj->animatorID;
}

LevelModel* LevelSystem_GetModel(int objectID) {
    static LevelModel model;
    LevelObject* obj = LevelSystem_GetObject(objectID);

    if (!obj) return NULL;

    model.objectID = objectID;
    model.modelID = obj->modelID;
    model.animatorID = obj->animatorID;

    return &model;
}

void LevelModel_Draw(int objectID, int animatorID, Vector3 pos, Vector3 rot, Vector3 scale) {
    LevelObject* obj = LevelSystem_GetObject(objectID);
    if (!obj || obj->modelID < 0) return;

    if (animatorID >= 0 && obj->type == OBJ_MODEL_ANIMATED) {
        GL_DrawAnimatedModel(obj->modelID, animatorID, pos, rot, scale);
    }
    else {
        GL_DrawStaticModel(obj->modelID, pos, rot, scale);
    }
}

// Draw animated model
void LevelModel_DrawAnimated(int objectID, int animatorID, Vector3 pos, Vector3 rot, Vector3 scale) {
    LevelObject* obj = LevelSystem_GetObject(objectID);
    if (!obj || obj->modelID < 0) return;

    if (obj->type == OBJ_MODEL_ANIMATED) {
        int useAnimatorID = (animatorID >= 0) ? animatorID : obj->animatorID;
        if (useAnimatorID >= 0) {
            GL_DrawAnimatedModel(obj->modelID, useAnimatorID, pos, rot, scale);
        }
        else {
            // Fallback to static if no animator available
            GL_DrawStaticModel(obj->modelID, pos, rot, scale);
        }
    }
    else {
        // Draw as static if not animated
        GL_DrawStaticModel(obj->modelID, pos, rot, scale);
    }
}

void LevelModel_PlayAnimation(int objectID, float deltaTime) {
    LevelObject* obj = LevelSystem_GetObject(objectID);
    if (!obj || obj->type != OBJ_MODEL_ANIMATED) return;

    obj->animationPlaying = 1;
    printf("Animation playing for model %d\n", objectID);
}

void LevelModel_StopAnimation(int objectID) {
    LevelObject* obj = LevelSystem_GetObject(objectID);
    if (!obj || obj->type != OBJ_MODEL_ANIMATED) return;

    obj->animationPlaying = 0;
    printf("Animation stopped for model %d\n", objectID);
}

void LevelModel_ResetAnimation(int objectID) {
    LevelObject* obj = LevelSystem_GetObject(objectID);
    if (!obj || obj->type != OBJ_MODEL_ANIMATED || obj->animatorID < 0) return;

    GL_ResetAnimation(obj->animatorID);
    printf("Animation reset for model %d\n", objectID);
}

void LevelModel_SetAnimationSpeed(int objectID, float speed) {
    LevelObject* obj = LevelSystem_GetObject(objectID);
    if (!obj || obj->type != OBJ_MODEL_ANIMATED) return;

    obj->animationSpeed = speed;
    printf("Animation speed set to %.2f for model %d\n", speed, objectID);
}