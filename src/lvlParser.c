#include "lvlParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_LINE 2048
#define MAX_TOKEN 256

static LevelContext g_context;
static char* g_levelCode = NULL;
static size_t g_codeSize = 0;

// Helper: Skip whitespace and comments
static char* skipWhitespace(char* p) {
    while (*p) {
        if (isspace(*p)) {
            p++;
        }
        else if (*p == '/' && *(p + 1) == '/') {
            // Single-line comment: skip until end of line
            while (*p && *p != '\n') p++;
        }
        else if (*p == '/' && *(p + 1) == '*') {
            // Multi-line comment: skip until */
            p += 2; // Skip the opening /*
            while (*p && !(*p == '*' && *(p + 1) == '/')) {
                p++;
            }
            if (*p) {
                p += 2; // Skip the closing */
            }
        }
        else {
            break;
        }
    }
    return p;
}

// Helper: Extract string literal
static char* extractString(char* p, char* out, int maxLen) {
    if (*p != '"') return p;
    p++; // Skip opening quote

    int i = 0;
    while (*p && *p != '"' && i < maxLen - 1) {
        out[i++] = *p++;
    }
    out[i] = '\0';

    if (*p == '"') p++; // Skip closing quote
    return p;
}

// Helper: Extract identifier or keyword
static char* extractIdentifier(char* p, char* out, int maxLen) {
    int i = 0;
    while (*p && (isalnum(*p) || *p == '_') && i < maxLen - 1) {
        out[i++] = *p++;
    }
    out[i] = '\0';
    return p;
}

// Helper: Extract number
static char* extractNumber(char* p, float* out) {
    char numStr[64];
    int i = 0;
    int hasDecimal = 0;

    if (*p == '-') numStr[i++] = *p++;

    while (*p && (isdigit(*p) || (*p == '.' && !hasDecimal)) && i < 63) {
        if (*p == '.') hasDecimal = 1;
        numStr[i++] = *p++;
    }
    numStr[i] = '\0';

    if (i > 0) {
        *out = (float)atof(numStr);
    }

    // Skip 'f' suffix
    if (*p == 'f') p++;

    return p;
}

// Helper: Parse Vector3 {x, y, z}
static char* parseVector3(char* p, Vector3* v) {
    p = skipWhitespace(p);
    if (*p != '{') return p;
    p++;

    // Parse x
    p = skipWhitespace(p);
    p = extractNumber(p, &v->x);

    // Skip comma
    p = skipWhitespace(p);
    if (*p == ',') p++;

    // Parse y
    p = skipWhitespace(p);
    p = extractNumber(p, &v->y);

    // Skip comma
    p = skipWhitespace(p);
    if (*p == ',') p++;

    // Parse z
    p = skipWhitespace(p);
    p = extractNumber(p, &v->z);

    // Skip closing brace
    p = skipWhitespace(p);
    if (*p == '}') p++;

    return p;
}

// Find matching closing brace
static char* findClosingBrace(char* p) {
    int depth = 0;
    while (*p) {
        if (*p == '{') depth++;
        if (*p == '}') {
            depth--;
            if (depth == 0) return p;
        }
        p++;
    }
    return NULL;
}

// Execute level script
static void executeScript(char* script) {
    char* p = script;
    char token[MAX_TOKEN];

    printf("Starting script execution...\n");

    while (*p) {
        p = skipWhitespace(p);
        if (!*p) break;

        // Skip preprocessor directives
        if (*p == '#') {
            while (*p && *p != '\n') p++;
            continue;
        }

        // Handle integer variable declarations: int VAR = value;
        if (strncmp(p, "int ", 4) == 0) {
            p += 4;
            p = skipWhitespace(p);
            p = extractIdentifier(p, token, MAX_TOKEN);

            printf("Declared variable: %s\n", token);

            p = skipWhitespace(p);
            if (*p == '=') {
                p++;
                p = skipWhitespace(p);
                float value;
                p = extractNumber(p, &value);
                LvlParser_SetVariable(token, value);
                printf("  Set %s = %.0f\n", token, value);
            }
            else {
                LvlParser_SetVariable(token, -1);
            }

            while (*p && *p != ';') p++;
            if (*p == ';') p++;
            continue;
        }

        // Handle variable assignments: VAR = value;
        if (isalpha(*p)) {
            char varName[MAX_TOKEN];
            char* checkPoint = p;
            p = extractIdentifier(p, varName, MAX_TOKEN);

            p = skipWhitespace(p);

            // Check for assignment
            if (*p == '=') {
                p++;
                p = skipWhitespace(p);

                // Check for function call
                if (isalpha(*p)) {
                    char funcName[MAX_TOKEN];
                    p = extractIdentifier(p, funcName, MAX_TOKEN);

                    if (strcmp(funcName, "AddObject") == 0) {
                        p = skipWhitespace(p);
                        if (*p == '(') p++;

                        char objType[MAX_TOKEN];
                        p = skipWhitespace(p);
                        p = extractString(p, objType, MAX_TOKEN);

                        int id = LevelSystem_AddObject(objType);
                        LvlParser_SetVariable(varName, (float)id);

                        printf("Created object '%s' with ID %d (stored in %s)\n", objType, id, varName);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(funcName, "GL_LoadModel") == 0) {
                        p = skipWhitespace(p);
                        if (*p == '(') p++;

                        char modelPath[MAX_TEXTURE_PATH];
                        p = skipWhitespace(p);
                        p = extractString(p, modelPath, MAX_TEXTURE_PATH);

                        p = skipWhitespace(p);
                        if (*p == ',') p++;

                        float flipTex = 0;
                        p = skipWhitespace(p);
                        p = extractNumber(p, &flipTex);

                        int id = LevelSystem_LoadModel(modelPath, (int)flipTex);
                        LvlParser_SetVariable(varName, (float)id);

                        printf("Loaded model '%s' with ID %d (flip: %d)\n", modelPath, id, (int)flipTex);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    
                    else if (strcmp(funcName, "GL_LoadAnimation") == 0) {
                        p = skipWhitespace(p);
                        if (*p == '(') p++;

                        char animPath[MAX_TEXTURE_PATH];
                        p = skipWhitespace(p);
                        p = extractString(p, animPath, MAX_TEXTURE_PATH);

                        p = skipWhitespace(p);
                        if (*p == ',') p++;

                        char modelVar[MAX_TOKEN];
                        p = skipWhitespace(p);
                        p = extractIdentifier(p, modelVar, MAX_TOKEN);

                        int modelObjID = (int)LvlParser_GetVariable(modelVar);
                        int animID = LevelSystem_LoadAnimationForModel(animPath, modelObjID);
                        LvlParser_SetVariable(varName, (float)animID);

                        printf("Loaded animation '%s' with ID %d for model %d\n", animPath, animID, modelObjID);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                }

                // Regular number assignment
                float value;
                p = extractNumber(p, &value);
                LvlParser_SetVariable(varName, value);

                while (*p && *p != ';') p++;
                if (*p == ';') p++;
                continue;
            }

            // Restore checkpoint if not assignment
            p = checkPoint;
        }

        // Handle Object declarations: Object name = GetObject(ID);
        if (strncmp(p, "Object ", 7) == 0) {
            p += 7;
            p = skipWhitespace(p);

            char objVarName[MAX_TOKEN];
            p = extractIdentifier(p, objVarName, MAX_TOKEN);

            p = skipWhitespace(p);
            if (*p == '=') {
                p++;
                p = skipWhitespace(p);

                // Parse GetObject(...)
                if (strncmp(p, "GetObject", 9) == 0) {
                    p += 9;
                    p = skipWhitespace(p);
                    if (*p == '(') p++;

                    char idVar[MAX_TOKEN];
                    p = skipWhitespace(p);
                    p = extractIdentifier(p, idVar, MAX_TOKEN);

                    float objID = LvlParser_GetVariable(idVar);
                    LvlParser_SetVariable(objVarName, objID);

                    printf("Object variable '%s' references ID %.0f\n", objVarName, objID);

                    p = skipWhitespace(p);
                    if (*p == ')') p++;
                }
            }

            while (*p && *p != ';') p++;
            if (*p == ';') p++;
            continue;
        }

        // Handle Light declarations: Light name = GetObject(ID);
        if (strncmp(p, "Light ", 6) == 0) {
            p += 6;
            p = skipWhitespace(p);

            char lightVarName[MAX_TOKEN];
            p = extractIdentifier(p, lightVarName, MAX_TOKEN);

            p = skipWhitespace(p);
            if (*p == '=') {
                p++;
                p = skipWhitespace(p);

                // Parse GetObject(...)
                if (strncmp(p, "GetObject", 9) == 0) {
                    p += 9;
                    p = skipWhitespace(p);
                    if (*p == '(') p++;

                    char idVar[MAX_TOKEN];
                    p = skipWhitespace(p);
                    p = extractIdentifier(p, idVar, MAX_TOKEN);

                    float objID = LvlParser_GetVariable(idVar);
                    LvlParser_SetVariable(lightVarName, objID);

                    printf("Light variable '%s' references ID %.0f\n", lightVarName, objID);

                    p = skipWhitespace(p);
                    if (*p == ')') p++;
                }
            }

            while (*p && *p != ';') p++;
            if (*p == ';') p++;
            continue;
        }

        // Handle Model declarations: Model name = GetModel(ID);
        if (strncmp(p, "Model ", 6) == 0) {
            p += 6;
            p = skipWhitespace(p);

            char modelVarName[MAX_TOKEN];
            p = extractIdentifier(p, modelVarName, MAX_TOKEN);

            p = skipWhitespace(p);
            if (*p == '=') {
                p++;
                p = skipWhitespace(p);

                // Parse GetModel(...)
                if (strncmp(p, "GetModel", 8) == 0) {
                    p += 8;
                    p = skipWhitespace(p);
                    if (*p == '(') p++;

                    char idVar[MAX_TOKEN];
                    p = skipWhitespace(p);
                    p = extractIdentifier(p, idVar, MAX_TOKEN);

                    float objID = LvlParser_GetVariable(idVar);
                    LvlParser_SetVariable(modelVarName, objID);

                    printf("Model variable '%s' references ID %.0f\n", modelVarName, objID);

                    p = skipWhitespace(p);
                    if (*p == ')') p++;
                }
            }

            while (*p && *p != ';') p++;
            if (*p == ';') p++;
            continue;
        }

        // Handle Material declarations
        if (strncmp(p, "Material ", 9) == 0) {
            p += 9;
            p = skipWhitespace(p);

            char matVarName[MAX_TOKEN];
            p = extractIdentifier(p, matVarName, MAX_TOKEN);

            printf("Material variable: %s\n", matVarName);

            // Store material name for later use
            LvlParser_SetVariable(matVarName, 1.0f);

            while (*p && *p != ';') p++;
            if (*p == ';') p++;
            continue;
        }

        // Handle Map declarations: Map worldMap;
        if (strncmp(p, "Map ", 4) == 0) {
            p += 4;
            p = skipWhitespace(p);

            char mapVarName[MAX_TOKEN];
            p = extractIdentifier(p, mapVarName, MAX_TOKEN);

            printf("Map variable: %s\n", mapVarName);

            // Store map name for later use
            LvlParser_SetVariable(mapVarName, 1.0f);

            while (*p && *p != ';') p++;
            if (*p == ';') p++;
            continue;
        }

        // Handle PostProcessing declarations: PostProcessing effects;
        if (strncmp(p, "PostProcessing ", 15) == 0) {
            p += 15;
            p = skipWhitespace(p);

            char effectsVarName[MAX_TOKEN];
            p = extractIdentifier(p, effectsVarName, MAX_TOKEN);

            printf("PostProcessing variable: %s\n", effectsVarName);

            // Store effects name for later use
            LvlParser_SetVariable(effectsVarName, 1.0f);

            while (*p && *p != ';') p++;
            if (*p == ';') p++;
            continue;
        }

        // Handle TraceLog declarations: TraceLog log;
        if (strncmp(p, "TraceLog ", 9) == 0) {
            p += 9;
            p = skipWhitespace(p);

            char logVarName[MAX_TOKEN];
            p = extractIdentifier(p, logVarName, MAX_TOKEN);

            printf("TraceLog variable: %s\n", logVarName);

            // Store log name for later use
            LvlParser_SetVariable(logVarName, 1.0f);

            while (*p && *p != ';') p++;
            if (*p == ';') p++;
            continue;
        }

        // Handle property access: object.property = value
        if (isalpha(*p)) {
            char objName[MAX_TOKEN];
            char* checkpoint = p;
            p = extractIdentifier(p, objName, MAX_TOKEN);

            p = skipWhitespace(p);
            if (*p == '.') {
                p++;
                char propName[MAX_TOKEN];
                p = extractIdentifier(p, propName, MAX_TOKEN);

                p = skipWhitespace(p);

                // Handle function calls: worldMap.SetSkyBox(...)
                if (*p == '(') {
                    p++;

                    if (strcmp(propName, "SetSkyBox") == 0) {
                        char paths[6][MAX_TEXTURE_PATH];

                        // Parse 6 texture paths
                        for (int i = 0; i < 6; i++) {
                            p = skipWhitespace(p);
                            p = extractString(p, paths[i], MAX_TEXTURE_PATH);
                            p = skipWhitespace(p);
                            if (*p == ',') p++;
                        }

                        LevelMap_SetSkyBox(paths[0], paths[1], paths[2],
                            paths[3], paths[4], paths[5]);

                        printf("  Set skybox: right=%s, left=%s, top=%s, bottom=%s, front=%s, back=%s\n",
                            paths[0], paths[1], paths[2], paths[3], paths[4], paths[5]);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "EnableFog") == 0) {
                        p = skipWhitespace(p);

                        // Parse boolean (true/false or 1/0)
                        int enable = 0;
                        if (strncmp(p, "true", 4) == 0) {
                            enable = 1;
                            p += 4;
                        }
                        else if (strncmp(p, "false", 5) == 0) {
                            enable = 0;
                            p += 5;
                        }
                        else {
                            float val;
                            p = extractNumber(p, &val);
                            enable = (int)val;
                        }

                        LevelMap_EnableFog(enable);
                        printf("  Set fog enabled: %d\n", enable);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "EnableBloom") == 0) {
                        p = skipWhitespace(p);

                        int enable = 0;
                        if (strncmp(p, "true", 4) == 0) {
                            enable = 1;
                            p += 4;
                        }
                        else if (strncmp(p, "false", 5) == 0) {
                            enable = 0;
                            p += 5;
                        }
                        else {
                            float val;
                            p = extractNumber(p, &val);
                            enable = (int)val;
                        }

                        LevelPostProcessing_EnableBloom(enable);
                        printf("  Set bloom enabled: %d\n", enable);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "EnableFXAA") == 0) {
                        p = skipWhitespace(p);

                        int enable = 0;
                        if (strncmp(p, "true", 4) == 0) {
                            enable = 1;
                            p += 4;
                        }
                        else if (strncmp(p, "false", 5) == 0) {
                            enable = 0;
                            p += 5;
                        }
                        else {
                            float val;
                            p = extractNumber(p, &val);
                            enable = (int)val;
                        }

                        LevelPostProcessing_EnableFXAA(enable);
                        printf("  Set FXAA enabled: %d\n", enable);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "Print") == 0) {
                        p = skipWhitespace(p);
                        char msg[256];
                        p = extractString(p, msg, 256);
                        LevelTrace_Print(msg);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "PrintWarn") == 0) {
                        p = skipWhitespace(p);
                        char msg[256];
                        p = extractString(p, msg, 256);
                        LevelTrace_PrintWarn(msg);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "PrintError") == 0) {
                        p = skipWhitespace(p);
                        char msg[256];
                        p = extractString(p, msg, 256);
                        LevelTrace_PrintError(msg);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "PrintInfo") == 0) {
                        p = skipWhitespace(p);
                        char msg[256];
                        p = extractString(p, msg, 256);
                        LevelTrace_PrintInfo(msg);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "AnimatedMoveObjectTo") == 0) {
                        // object.AnimatedMoveObjectTo({x,y,z}, speed)
                        p = skipWhitespace(p);
                        Vector3 target;
                        p = parseVector3(p, &target);

                        p = skipWhitespace(p);
                        if (*p == ',') p++;

                        float speed = 2.0f;
                        p = skipWhitespace(p);
                        if (isdigit(*p) || *p == '-') {
                            p = extractNumber(p, &speed);
                        }

                        int objID = (int)LvlParser_GetVariable(objName);
                        LevelObject_AnimatedMoveObjectTo(objID, target, speed);
                        printf("  Set %s to move to {%.1f, %.1f, %.1f} at speed %.1f\n",
                            objName, target.x, target.y, target.z, speed);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "AnimatedScaleObjectTo") == 0) {
                        // object.AnimatedScaleObjectTo({x,y,z}, speed)
                        p = skipWhitespace(p);
                        Vector3 target;
                        p = parseVector3(p, &target);

                        p = skipWhitespace(p);
                        if (*p == ',') p++;

                        float speed = 1.0f;
                        p = skipWhitespace(p);
                        if (isdigit(*p) || *p == '-') {
                            p = extractNumber(p, &speed);
                        }

                        int objID = (int)LvlParser_GetVariable(objName);
                        LevelObject_AnimatedScaleObjectTo(objID, target, speed);
                        printf("  Set %s to scale to {%.1f, %.1f, %.1f} at speed %.1f\n",
                            objName, target.x, target.y, target.z, speed);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "toggle") == 0) {
                        // light.toggle()
                        int objID = (int)LvlParser_GetVariable(objName);
                        LevelObject_ToggleLight(objID);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "destroy") == 0) {
                        // light.destroy()
                        int objID = (int)LvlParser_GetVariable(objName);
                        LevelObject_DestroyLight(objID);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "Draw") == 0) {
                        // mdl.Draw(animID, pos, rot, scale)
                        p = skipWhitespace(p);

                        char animVar[MAX_TOKEN];
                        p = extractIdentifier(p, animVar, MAX_TOKEN);
                        int animID = (int)LvlParser_GetVariable(animVar);

                        p = skipWhitespace(p);
                        if (*p == ',') p++;

                        Vector3 pos, rot, scale;
                        p = skipWhitespace(p);
                        p = parseVector3(p, &pos);

                        p = skipWhitespace(p);
                        if (*p == ',') p++;
                        p = skipWhitespace(p);
                        p = parseVector3(p, &rot);

                        p = skipWhitespace(p);
                        if (*p == ',') p++;
                        p = skipWhitespace(p);
                        p = parseVector3(p, &scale);

                        int modelID = (int)LvlParser_GetVariable(objName);
                        LevelModel_DrawAnimated(modelID, animID, pos, rot, scale);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "PlayAnimation") == 0) {
                        // mdl.PlayAnimation(deltaTime)
                        p = skipWhitespace(p);

                        float deltaTime;
                        p = extractNumber(p, &deltaTime);

                        int modelID = (int)LvlParser_GetVariable(objName);
                        LevelModel_PlayAnimation(modelID, deltaTime);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "StopAnimation") == 0) {
                        // mdl.StopAnimation()
                        int modelID = (int)LvlParser_GetVariable(objName);
                        LevelModel_StopAnimation(modelID);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                    else if (strcmp(propName, "ResetAnimation") == 0) {
                        // mdl.ResetAnimation()
                        int modelID = (int)LvlParser_GetVariable(objName);
                        LevelModel_ResetAnimation(modelID);

                        p = skipWhitespace(p);
                        if (*p == ')') p++;
                        while (*p && *p != ';') p++;
                        if (*p == ';') p++;
                        continue;
                    }
                }

                // Handle property assignments
                if (*p == '=') {
                    p++;
                    p = skipWhitespace(p);

                    int objID = (int)LvlParser_GetVariable(objName);
                    LevelObject* obj = LevelSystem_GetObject(objID);

                    if (obj) {
                        if (strcmp(propName, "position") == 0) {
                            p = parseVector3(p, &obj->position);
                            printf("  Set %s.position = {%.1f, %.1f, %.1f}\n",
                                objName, obj->position.x, obj->position.y, obj->position.z);
                        }
                        else if (strcmp(propName, "size") == 0) {
                            p = parseVector3(p, &obj->size);
                            printf("  Set %s.size = {%.1f, %.1f, %.1f}\n",
                                objName, obj->size.x, obj->size.y, obj->size.z);
                        }
                        else if (strcmp(propName, "rotation") == 0) {
                            p = parseVector3(p, &obj->rotation);
                            printf("  Set %s.rotation = {%.1f, %.1f, %.1f}\n",
                                objName, obj->rotation.x, obj->rotation.y, obj->rotation.z);
                        }
                        else if (strcmp(propName, "material") == 0) {
                            // Material assignment handled via material variable
                            p = skipWhitespace(p);
                            char matVar[MAX_TOKEN];
                            p = extractIdentifier(p, matVar, MAX_TOKEN);
                            printf("  Set %s.material = %s\n", objName, matVar);
                        }
                        else if (strcmp(propName, "DiffuseTexture") == 0) {
                            // Direct texture assignment: object.DiffuseTexture = "path"
                            char texPath[MAX_TEXTURE_PATH];
                            p = extractString(p, texPath, MAX_TEXTURE_PATH);
                            strcpy(obj->material.diffuseTexture, texPath);
                            printf("  Set %s.DiffuseTexture = %s\n", objName, texPath);
                        }
                        else if (strcmp(propName, "NormalMap") == 0) {
                            char texPath[MAX_TEXTURE_PATH];
                            p = extractString(p, texPath, MAX_TEXTURE_PATH);
                            strcpy(obj->material.normalMap, texPath);
                            printf("  Set %s.NormalMap = %s\n", objName, texPath);
                        }
                        else if (strcmp(propName, "ParallaxMap") == 0) {
                            char texPath[MAX_TEXTURE_PATH];
                            p = extractString(p, texPath, MAX_TEXTURE_PATH);
                            strcpy(obj->material.parallaxMap, texPath);
                            printf("  Set %s.ParallaxMap = %s\n", objName, texPath);
                        }
                        else if (strcmp(propName, "intensity") == 0) {
                            // Light intensity
                            float value;
                            p = extractNumber(p, &value);
                            if (obj->type == OBJ_LIGHT) {
                                obj->light.intensity = value;
                                printf("  Set %s.intensity = %.2f\n", objName, value);
                            }
                        }
                        else if (strcmp(propName, "color") == 0) {
                            // Light color (can be used for both lights and objects)
                            Vector3 color;
                            p = parseVector3(p, &color);
                            if (obj->type == OBJ_LIGHT) {
                                obj->light.color = color;
                                printf("  Set %s.color = {%.2f, %.2f, %.2f}\n", objName, color.x, color.y, color.z);
                            }
                        }
                        else if (strcmp(propName, "specular") == 0) {
                            // Light specular (can be float or Vector3)
                            p = skipWhitespace(p);
                            if (*p == '{') {
                                Vector3 spec;
                                p = parseVector3(p, &spec);
                                if (obj->type == OBJ_LIGHT) {
                                    obj->light.specular = (spec.x + spec.y + spec.z) / 3.0f;
                                    printf("  Set %s.specular = {%.2f, %.2f, %.2f}\n", objName, spec.x, spec.y, spec.z);
                                }
                            }
                            else {
                                float value;
                                p = extractNumber(p, &value);
                                if (obj->type == OBJ_LIGHT) {
                                    obj->light.specular = value;
                                    printf("  Set %s.specular = %.2f\n", objName, value);
                                }
                            }
                        }
                        else if (strcmp(propName, "enabled") == 0) {
                            // Light enabled
                            p = skipWhitespace(p);
                            int enable = 0;
                            if (strncmp(p, "true", 4) == 0) {
                                enable = 1;
                                p += 4;
                            }
                            else if (strncmp(p, "false", 5) == 0) {
                                enable = 0;
                                p += 5;
                            }
                            else {
                                float val;
                                p = extractNumber(p, &val);
                                enable = (int)val;
                            }

                            if (obj->type == OBJ_LIGHT) {
                                obj->light.enabled = enable;
                                printf("  Set %s.enabled = %d\n", objName, enable);
                            }
                        }
                        else if (strcmp(propName, "direction") == 0) {
                            // Light direction (for spot/directional)
                            Vector3 dir;
                            p = parseVector3(p, &dir);
                            if (obj->type == OBJ_LIGHT) {
                                obj->light.direction = dir;
                                printf("  Set %s.direction = {%.2f, %.2f, %.2f}\n", objName, dir.x, dir.y, dir.z);
                            }
                        }
                        else if (strcmp(propName, "cutOff") == 0) {
                            // Spot light cutoff
                            float value;
                            p = extractNumber(p, &value);
                            if (obj->type == OBJ_LIGHT) {
                                obj->light.cutOff = value;
                                printf("  Set %s.cutOff = %.2f\n", objName, value);
                            }
                        }
                        else if (strcmp(propName, "outerCutOff") == 0) {
                            // Spot light outer cutoff
                            float value;
                            p = extractNumber(p, &value);
                            if (obj->type == OBJ_LIGHT) {
                                obj->light.outerCutOff = value;
                                printf("  Set %s.outerCutOff = %.2f\n", objName, value);
                            }
                        }
                    }

                    while (*p && *p != ';') p++;
                    if (*p == ';') p++;
                    continue;
                }
            }

            p = checkpoint;
        }

        // Handle material property: mat.DiffuseTexture = "...";
        if (isalpha(*p)) {
            char matName[MAX_TOKEN];
            char* checkpoint = p;
            p = extractIdentifier(p, matName, MAX_TOKEN);

            p = skipWhitespace(p);
            if (*p == '.') {
                p++;
                char propName[MAX_TOKEN];
                p = extractIdentifier(p, propName, MAX_TOKEN);

                p = skipWhitespace(p);
                if (*p == '=') {
                    p++;
                    p = skipWhitespace(p);

                    // Store material property for later assignment
                    if (strcmp(propName, "DiffuseTexture") == 0) {
                        char texPath[MAX_TEXTURE_PATH];
                        p = extractString(p, texPath, MAX_TEXTURE_PATH);

                        // Store in a temp variable for next object.material assignment
                        char key[128];
                        snprintf(key, sizeof(key), "_mat_%s_diffuse", matName);

                        // Store string as float (hack for now - store index)
                        static char tempTextures[10][MAX_TEXTURE_PATH];
                        static int texCount = 0;

                        if (texCount < 10) {
                            strcpy(tempTextures[texCount], texPath);
                            LvlParser_SetVariable(key, (float)texCount);
                            printf("  Stored material texture: %s = %s (index %d)\n",
                                matName, texPath, texCount);
                            texCount++;
                        }
                    }

                    while (*p && *p != ';') p++;
                    if (*p == ';') p++;
                    continue;
                }
            }

            p = checkpoint;
        }

        // Skip unknown characters
        p++;
    }

    printf("Script execution complete.\n");
}

// Load level file
LvlParseResult LvlParser_LoadLevel(const char* filepath) {
    printf("Loading level: %s\n", filepath);

    // Initialize context
    memset(&g_context, 0, sizeof(g_context));
    g_context.initialized = 0;
    g_context.loopActive = 0;

    // Initialize level system
    LevelSystem_Init();

    // Read file
    FILE* f = fopen(filepath, "rb");
    if (!f) {
        printf("Error: Could not open level file: %s\n", filepath);
        return LVL_PARSE_FILE_NOT_FOUND;
    }

    fseek(f, 0, SEEK_END);
    g_codeSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    g_levelCode = (char*)malloc(g_codeSize + 1);
    if (!g_levelCode) {
        fclose(f);
        return LVL_PARSE_MEMORY_ERROR;
    }

    fread(g_levelCode, 1, g_codeSize, f);
    g_levelCode[g_codeSize] = '\0';
    fclose(f);

    printf("Level file loaded: %zu bytes\n", g_codeSize);

    // Find and execute main() function
    char* mainFunc = strstr(g_levelCode, "void main()");
    if (!mainFunc) {
        printf("Error: No main() function found!\n");
        free(g_levelCode);
        return LVL_PARSE_SYNTAX_ERROR;
    }

    char* mainStart = strchr(mainFunc, '{');
    if (!mainStart) {
        printf("Error: No opening brace for main()!\n");
        free(g_levelCode);
        return LVL_PARSE_SYNTAX_ERROR;
    }

    mainStart++; // Skip opening brace

    // Find closing brace
    char* mainEnd = findClosingBrace(mainFunc);
    if (!mainEnd) {
        printf("Error: No closing brace for main()!\n");
        free(g_levelCode);
        return LVL_PARSE_SYNTAX_ERROR;
    }

    // Execute main function body
    *mainEnd = '\0'; // Temporarily null-terminate
    executeScript(mainStart);
    *mainEnd = '}'; // Restore

    g_context.initialized = 1;
    printf("Level initialized successfully!\n");

    return LVL_PARSE_OK;
}

void LvlParser_UpdateLevel(float deltaTime) {
    g_context.deltaTime = deltaTime;
    g_context.loopActive = 1;

    LevelSystem_Update(deltaTime);
}

void LvlParser_RenderLevel(void) {
    LevelSystem_Render();
}

void LvlParser_UnloadLevel(void) {
    LevelSystem_Shutdown();

    if (g_levelCode) {
        free(g_levelCode);
        g_levelCode = NULL;
    }

    g_context.initialized = 0;
}

LevelContext* LvlParser_GetContext(void) {
    return &g_context;
}

void LvlParser_SetVariable(const char* name, float value) {
    for (int i = 0; i < g_context.variableCount; i++) {
        if (strcmp(g_context.variables[i].name, name) == 0) {
            g_context.variables[i].value = value;
            return;
        }
    }

    if (g_context.variableCount < 256) {
        strncpy(g_context.variables[g_context.variableCount].name, name, 63);
        g_context.variables[g_context.variableCount].value = value;
        g_context.variableCount++;
    }
}

float LvlParser_GetVariable(const char* name) {
    for (int i = 0; i < g_context.variableCount; i++) {
        if (strcmp(g_context.variables[i].name, name) == 0) {
            return g_context.variables[i].value;
        }
    }
    return 0.0f;
}