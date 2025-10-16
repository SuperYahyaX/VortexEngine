#ifndef LVL_H
#define LVL_H

#include "LibGL.h"

#ifdef __cplusplus
extern "C" {
#endif

    // Maximum limits
#define MAX_LEVEL_OBJECTS 1000
#define MAX_TEXTURE_PATH 256

// Object types
    typedef enum {
        OBJ_CUBE = 0,
        OBJ_MODEL_STATIC,
        OBJ_MODEL_ANIMATED,
        OBJ_BILLBOARD,
        OBJ_LIGHT
    } LevelObjectType;

    // Light data for level objects
    typedef struct {
        int lightIndex;  // Index in LibGL lighting system
        LightType type;
        Vector3 color;
        float intensity;
        float specular;
        Vector3 direction;  // For directional/spot lights
        float cutOff;       // For spot lights
        float outerCutOff;  // For spot lights
        int enabled;
    } LevelLight;

    // Level Material
    typedef struct {
        char diffuseTexture[MAX_TEXTURE_PATH];
        char normalMap[MAX_TEXTURE_PATH];
        char parallaxMap[MAX_TEXTURE_PATH];
        Texture diffuseTex;
        Texture normalTex;
        Texture parallaxTex;
        int loaded;
    } LevelMaterial;

    // Level Object
    typedef struct {
        int id;
        int active;
        LevelObjectType type;
        char name[64];

        Vector3 position;
        Vector3 rotation;
        Vector3 size;

        LevelMaterial material;

        // For models
        int modelID;
        int animatorID;
        char modelPath[MAX_TEXTURE_PATH];
        char animationPath[MAX_TEXTURE_PATH];
        int flipTextures;
        int animationPlaying;

        // Animation targets
        Vector3 targetPosition;
        Vector3 targetScale;
        int isMoving;
        int isScaling;
        float moveSpeed;
        float scaleSpeed;

        // For lights
        LevelLight light;
    } LevelObject2;
    // Add to LevelObject struct
    typedef struct {
        int id;
        int active;
        LevelObjectType type;
        char name[64];

        Vector3 position;
        Vector3 rotation;
        Vector3 size;

        LevelMaterial material;

        // For models - separate static and animated
        int modelID;
        int animatorID;  // Only for animated models
        char modelPath[MAX_TEXTURE_PATH];
        char animationPath[MAX_TEXTURE_PATH];
        int flipTextures;

        // Animation control (only for animated models)
        int animationPlaying;
        float animationSpeed;

        // Animation targets
        Vector3 targetPosition;
        Vector3 targetScale;
        int isMoving;
        int isScaling;
        float moveSpeed;
        float scaleSpeed;

        // For lights
        LevelLight light;
    } LevelObject;
    // Model wrapper structure
    typedef struct {
        int objectID;
        int modelID;
        int animatorID;
    } LevelModel;

    // Level World Map
    typedef struct {
        char skyboxPaths[6][MAX_TEXTURE_PATH];
        int skyboxLoaded;
        int fogEnabled;
        Vector3 fogColor;
        float fogDensity;
        float fogStart;
        float fogEnd;
    } LevelMap;

    // Post Processing
    typedef struct {
        int bloomEnabled;
        int fxaaEnabled;
        int chromaticEnabled;
        float bloomThreshold;
        float bloomIntensity;
    } LevelPostProcessing;

    // Level System Functions
    void LevelSystem_Init(void);
    void LevelSystem_Shutdown(void);
    void LevelSystem_Update(float deltaTime);
    void LevelSystem_Render(void);

    // Object Management
    int LevelSystem_AddObject(const char* typeName);
    LevelObject* LevelSystem_GetObject(int id);
    void LevelSystem_RemoveObject(int id);
    void LevelSystem_ClearAllObjects(void);

    // Map Settings
    void LevelMap_SetSkyBox(const char* right, const char* left, const char* top,
        const char* bottom, const char* front, const char* back);
    void LevelMap_EnableFog(int enable);
    void LevelMap_SetFogColor(Vector3 color);
    void LevelMap_SetFogDensity(float density);

    // Post Processing
    void LevelPostProcessing_EnableBloom(int enable);
    void LevelPostProcessing_EnableFXAA(int enable);
    void LevelPostProcessing_SetBloomThreshold(float threshold);
    void LevelPostProcessing_SetBloomIntensity(float intensity);

    // Object Animation
    void LevelObject_AnimatedMoveObjectTo(int id, Vector3 target, float speed);
    void LevelObject_AnimatedScaleObjectTo(int id, Vector3 target, float speed);

    // Light object functions
    void LevelObject_ToggleLight(int id);
    void LevelObject_DestroyLight(int id);

    // Model functions
    int LevelSystem_LoadModel(const char* path, int flipTextures);
    //int LevelSystem_LoadAnimatedModel(const char* path, int flipTextures);
    int LevelSystem_LoadAnimation(const char* path, int modelObjectID);


    // Add these new functions to lvl.h
    int LevelSystem_LoadStaticModel(const char* path, int flipTextures);
    int LevelSystem_LoadAnimationForModel(const char* path, int modelObjectID);
    void LevelModel_UpdateAnimations(float deltaTime);



    LevelModel* LevelSystem_GetModel(int objectID);
    void LevelModel_Draw(int objectID, int animatorID, Vector3 pos, Vector3 rot, Vector3 scale);
    void LevelModel_DrawAnimated(int objectID, int animatorID, Vector3 pos, Vector3 rot, Vector3 scale);
    void LevelModel_PlayAnimation(int objectID, float deltaTime);
    void LevelModel_StopAnimation(int objectID);
    void LevelModel_ResetAnimation(int objectID);
    void LevelModel_SetAnimationSpeed(int objectID, float speed);


    // Trace Log (wrapper)
    void LevelTrace_Print(const char* msg);
    void LevelTrace_PrintWarn(const char* msg);
    void LevelTrace_PrintError(const char* msg);
    void LevelTrace_PrintInfo(const char* msg);

#ifdef __cplusplus
}
#endif

#endif // LVL_H