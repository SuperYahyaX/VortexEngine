#include "SHARED_LIB.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
static char g_LastError[512] = "No error";
#else
static __thread char g_LastError[512] = "No error";
#endif

static bool g_Debug = false;

#define LOG(fmt, ...) if (g_Debug) printf("[SHARED_LIB] " fmt "\n", ##__VA_ARGS__)

// ------------------------------
// Platform-specific helpers
// ------------------------------

#ifdef _WIN32
#define EXTENSION ".dll"
#else
#if defined(__APPLE__)
#define EXTENSION ".dylib"
#else
#define EXTENSION ".so"
#endif
#endif

// Append extension if missing
static void append_extension(char* out, const char* in) {
    if (strstr(in, ".dll") || strstr(in, ".so") || strstr(in, ".dylib")) {
        strcpy(out, in);
    }
    else {
        snprintf(out, 512, "%s%s", in, EXTENSION);
    }
}

// ------------------------------
// API implementation
// ------------------------------

void SHARED_LIB_EnableDebug(bool enabled) {
    g_Debug = enabled;
}

bool SHARED_LIB_IsLoaded(SHARED_LIB_Instance* inst) {
    return inst && inst->isLoaded && inst->handle != NULL;
}

bool SHARED_LIB_Load(SHARED_LIB_Instance* inst, const char* path, SHARED_LIB_LoadMode mode) {
    if (!inst || !path) return false;
    memset(inst, 0, sizeof(*inst));

    char fullPath[512];
    append_extension(fullPath, path);
    strncpy(inst->path, fullPath, sizeof(inst->path));
    inst->lazyLoad = (mode == SHARED_LIB_LOAD_LAZY);

#ifdef _WIN32
    inst->handle = LoadLibraryA(fullPath);
    if (!inst->handle) {
        DWORD err = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, err, 0, g_LastError, sizeof(g_LastError), NULL);
        LOG("Failed to load %s: %s", fullPath, g_LastError);
        return false;
    }
#else
    int flags = (mode == SHARED_LIB_LOAD_LAZY) ? RTLD_LAZY : RTLD_NOW;
    dlerror(); // clear previous
    inst->handle = dlopen(fullPath, flags);
    const char* err = dlerror();
    if (err) {
        strncpy(g_LastError, err, sizeof(g_LastError));
        LOG("Failed to load %s: %s", fullPath, g_LastError);
        return false;
    }
#endif

    inst->isLoaded = true;
    strncpy(g_LastError, "No error", sizeof(g_LastError));
    LOG("Loaded library: %s", fullPath);
    return true;
}

void* SHARED_LIB_GetSymbol(SHARED_LIB_Instance* inst, const char* symbol) {
    if (!SHARED_LIB_IsLoaded(inst) || !symbol) return NULL;
#ifdef _WIN32
    void* sym = (void*)GetProcAddress(inst->handle, symbol);
    if (!sym) {
        DWORD err = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, err, 0, g_LastError, sizeof(g_LastError), NULL);
        LOG("Missing symbol %s in %s", symbol, inst->path);
    }
    else {
        strcpy_s(g_LastError, sizeof(g_LastError), "No error");
        LOG("Loaded symbol: %s", symbol);
    }
#else
    dlerror(); // clear
    void* sym = dlsym(inst->handle, symbol);
    const char* err = dlerror();
    if (err) {
        strncpy(g_LastError, err, sizeof(g_LastError));
        LOG("Missing symbol %s in %s: %s", symbol, inst->path, g_LastError);
        return NULL;
    }
    strncpy(g_LastError, "No error", sizeof(g_LastError));
    LOG("Loaded symbol: %s", symbol);
#endif
    return sym;
}

bool SHARED_LIB_HasSymbol(SHARED_LIB_Instance* inst, const char* symbol) {
    void* s = SHARED_LIB_GetSymbol(inst, symbol);
    return (s != NULL);
}

bool SHARED_LIB_Reload(SHARED_LIB_Instance* inst) {
    if (!inst || !inst->path[0]) return false;
    LOG("Reloading library: %s", inst->path);
    SHARED_LIB_Unload(inst);
    return SHARED_LIB_Load(inst, inst->path, inst->lazyLoad ? SHARED_LIB_LOAD_LAZY : SHARED_LIB_LOAD_NOW);
}

void SHARED_LIB_Unload(SHARED_LIB_Instance* inst) {
    if (!SHARED_LIB_IsLoaded(inst)) return;
#ifdef _WIN32
    FreeLibrary(inst->handle);
#else
    dlclose(inst->handle);
#endif
    inst->handle = NULL;
    inst->isLoaded = false;
    LOG("Unloaded library: %s", inst->path);
}

const char* SHARED_LIB_GetLastError(void) {
    return g_LastError;
}
