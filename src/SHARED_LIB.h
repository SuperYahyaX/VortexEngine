#ifndef SHARED_LIB_H
#define SHARED_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

    // ============================
    // Cross-platform Shared Library
    // ============================

#ifdef _WIN32
#include <windows.h>
    typedef HMODULE SHARED_LIB_Handle;
#else
#include <dlfcn.h>
    typedef void* SHARED_LIB_Handle;
#endif

    // ------------------------------
    // Struct for extended info
    // ------------------------------
    typedef struct {
        SHARED_LIB_Handle handle;
        char path[512];
        bool isLoaded;
        bool lazyLoad;
    } SHARED_LIB_Instance;

    // ------------------------------
    // Load options
    // ------------------------------
    typedef enum {
        SHARED_LIB_LOAD_NOW = 0,  // Load immediately
        SHARED_LIB_LOAD_LAZY = 1   // Load on-demand (POSIX only)
    } SHARED_LIB_LoadMode;

    // ------------------------------
    // Global control
    // ------------------------------

    /**
     * Enable or disable internal debug logs (to stdout).
     */
    void SHARED_LIB_EnableDebug(bool enabled);

    /**
     * Loads a shared library into an instance with optional mode.
     * Automatically appends correct extension (.dll/.so/.dylib) if missing.
     */
    bool SHARED_LIB_Load(SHARED_LIB_Instance* inst, const char* path, SHARED_LIB_LoadMode mode);

    /**
     * Gets a function or variable symbol safely.
     */
    void* SHARED_LIB_GetSymbol(SHARED_LIB_Instance* inst, const char* symbol);

    /**
     * Returns true if library is currently loaded.
     */
    bool SHARED_LIB_IsLoaded(SHARED_LIB_Instance* inst);

    /**
     * Reloads a library (unloads then loads again).
     */
    bool SHARED_LIB_Reload(SHARED_LIB_Instance* inst);

    /**
     * Unloads a library and clears the instance.
     */
    void SHARED_LIB_Unload(SHARED_LIB_Instance* inst);

    /**
     * Checks if a given symbol exists.
     */
    bool SHARED_LIB_HasSymbol(SHARED_LIB_Instance* inst, const char* symbol);

    /**
     * Gets the last error message.
     */
    const char* SHARED_LIB_GetLastError(void);

#ifdef __cplusplus
}
#endif

#endif // SHARED_LIB_H
