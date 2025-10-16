#ifndef LVLPARSER_H
#define LVLPARSER_H

#include "lvl.h"

#ifdef __cplusplus
extern "C" {
#endif

    // Parser result codes
    typedef enum {
        LVL_PARSE_OK = 0,
        LVL_PARSE_FILE_NOT_FOUND,
        LVL_PARSE_SYNTAX_ERROR,
        LVL_PARSE_MEMORY_ERROR,
        LVL_PARSE_EXECUTION_ERROR
    } LvlParseResult;

    // Level context
    typedef struct {
        int initialized;
        int loopActive;
        float deltaTime;

        // User variables (simple key-value store)
        struct {
            char name[64];
            float value;
        } variables[256];
        int variableCount;
    } LevelContext;

    // Main parser functions
    LvlParseResult LvlParser_LoadLevel(const char* filepath);
    void LvlParser_UpdateLevel(float deltaTime);
    void LvlParser_RenderLevel(void);
    void LvlParser_UnloadLevel(void);

    // Context management
    LevelContext* LvlParser_GetContext(void);
    void LvlParser_SetVariable(const char* name, float value);
    float LvlParser_GetVariable(const char* name);

#ifdef __cplusplus
}
#endif

#endif // LVLPARSER_H