/*
* System Requirements -- Recommended:
*
*
Category	        Recommended
OS	                Windows 10 64-bit / Linux / macOS 10.13+
GPU	                NVIDIA GTX 750 / AMD R7 260 / Intel HD 520 or newer like
CPU	                Quad-core 3 GHz
RAM	                8 GB or 16GB
OpenGL	            3.3 Core Profile
Display	            1080p or any (for viewport-based effects)
*
*
*/


#ifndef LIBGL_H
#define LIBGL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>



    typedef struct Color4 {
        int r, g, b, a;
    } Color4;

    typedef struct Color4f {
        float r, g, b, a;
    } Color4f;

    typedef struct {
        unsigned char r, g, b, a;
    } Color4b;

    typedef struct Vector2 {
        float x, y;
    } Vector2;

    typedef struct Vector3 {
        float x, y, z;
    } Vector3;

    typedef struct Vector4 {
        float x, y, z, w;
    } Vector4;
    // Simple quaternion (for rotation)
    typedef struct {
        float x, y, z, w;
    } Quaternion2;


    // Define message types
    typedef enum {
        TRACE_INFO,
        TRACE_WARNING,
        TRACE_ERROR,
        TRACE_SUCCESS,
        TRACE_DEBUG
    } TraceType;


    typedef enum {
        CURSOR_ARROW_CURSOR = 0,
        CURSOR_IBEAM_CURSOR,
        CURSOR_CROSSHAIR_CURSOR,
        CURSOR_HAND_CURSOR,
        CURSOR_HRESIZE_CURSOR,
        CURSOR_VRESIZE_CURSOR
    } CursorType;



    // Camera Type
    typedef enum {
        TYPE_DEFAULT = 0,
        TYPE_FIRST_PERSON = 1,
        TYPE_EDITOR = 2
    } CameraType;



    typedef enum {
        DEPTH_FUNCTION_ALWAYS = 0,
        DEPTH_FUNCTION_NEVER,
        DEPTH_FUNCTION_LESS,
        DEPTH_FUNCTION_EQUAL,
        DEPTH_FUNCTION_LEQUAL,
        DEPTH_FUNCTION_GREATER,
        DEPTH_FUNCTION_NOTEQUAL,
        DEPTH_FUNCTION_GEQUAL
    } Depth_Function;

    typedef enum {
        STENCIL_FUNCTION_KEEP = 0,
        STENCIL_FUNCTION_ZERO,
        STENCIL_FUNCTION_REPLACE,
        STENCIL_FUNCTION_INCR,
        STENCIL_FUNCTION_INCR_WRAP,
        STENCIL_FUNCTION_DECR,
        STENCIL_FUNCTION_DECR_WRAP,
        STENCIL_FUNCTION_INVERT

    } Stencil_Function;





    //For Binding Textures
    typedef enum {
        GL_TEXTURE_0 = 0,
        GL_TEXTURE_1,
        GL_TEXTURE_2,
        GL_TEXTURE_3,
        GL_TEXTURE_4,
        GL_TEXTURE_5,
        GL_TEXTURE_6,
        GL_TEXTURE_7,
        GL_TEXTURE_8,
        GL_TEXTURE_9,
        GL_TEXTURE_10,
        GL_TEXTURE_11,
        GL_TEXTURE_12,
        GL_TEXTURE_13,
        GL_TEXTURE_14,
        GL_TEXTURE_15,
        GL_TEXTURE_16,
    } GL_Texture;







    typedef enum {
        // Basic filters
        TEXTURE_FILTER_NEAREST = 0,             // GL_NEAREST
        TEXTURE_FILTER_LINEAR,                  // GL_LINEAR
        // Mipmap minification filters
        TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST,  // GL_NEAREST_MIPMAP_NEAREST
        TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST,   // GL_LINEAR_MIPMAP_NEAREST
        TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR,   // GL_NEAREST_MIPMAP_LINEAR
        TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR,    // GL_LINEAR_MIPMAP_LINEAR
        // Anisotropic filtering (extension-based)
        TEXTURE_FILTER_ANISOTROPIC_2X,          // GL_TEXTURE_MAX_ANISOTROPY_EXT = 2.0
        TEXTURE_FILTER_ANISOTROPIC_4X,          // 4x anisotropy
        TEXTURE_FILTER_ANISOTROPIC_8X,          // 8x anisotropy
        TEXTURE_FILTER_ANISOTROPIC_16X,         // 16x anisotropy
        // Optional custom modes (for completeness or abstraction layers)
        TEXTURE_FILTER_DEFAULT,                 // Use context default or fallback
        TEXTURE_FILTER_CUSTOM                   // User-defined custom filter
    } Texture_Filter;

    typedef struct Texture {
        int width, height, nrChannels;
        unsigned char* data;
        unsigned int id;
        Texture_Filter filter;
    } Texture;

    typedef struct Camera {
        // camera Attributes
        Vector3 Position;
        Vector3 Front;
        Vector3 Up;
        Vector3 Right;
        Vector3 WorldUp;
        // euler Angles
        float Yaw;
        float Pitch;
        // camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Fovy;
        int type;
    } Camera;

    // Lighting types
    typedef enum {
        LIGHT_DIRECTIONAL = 0,
        LIGHT_POINT,
        LIGHT_SPOT
    } LightType;

    typedef struct Light {
        int enabled;
        LightType type;
        Vector3 position;
        Vector3 direction;
        Vector3 color;
        float intensity;
        float ambient;
        float diffuse;
        float specular;

        // Point light properties
        float constant;
        float linear;
        float quadratic;

        // Spot light properties
        float cutOff;
        float outerCutOff;
    } Light;

    // Lighting system functions
    void GL_InitLighting();
    void GL_SetLightingEnabled(int enabled);
    void GL_FixLightingIssues();
    void GL_SetShadingModel(int useBlinnPhong); // 0 = Phong, 1 = Blinn-Phong
    void GL_SetNormalMappingEnabled(int enabled);

    // Light management
    int GL_CreateLight(LightType type);
    void GL_DeleteLight(int lightIndex);
    void GL_UpdateLight(int lightIndex, Light light);
    Light GL_GetLight(int lightIndex);
    void GL_SetActiveLight(int lightIndex);

    // Material properties
    void GL_SetMaterial(Vector3 ambient, Vector3 diffuse, Vector3 specular, float shininess);

    // Basic lighting functions
    void GL_SetAmbientLight(Vector3 color, float intensity);
    void GL_EnableLighting(int enable);

    // Window and rendering functions
    int GL_InitWindow(int width, int height, const char* title, int enableMSAA);
    void GL_InitCamera(
        Camera cam,
        Vector3 pos,
        float Fovy,
        float movementSpeed,
        float sensitivity,
        int type, int enableZoom);
    void GL_CaptureMouse(int use);
    int GL_InitOpenGLLoader();
    void GL_EnableDepthTest(int enable);
    void GL_ClearDepth();
    void GL_ClearDepthStencil();
    void GL_StencilMask(int n);
    void GL_StencilFunction(Stencil_Function type, int index, int n);

    void GL_StencilOp_Replace();
    void GL_EnableCullFace(int use);



    void GL_CullFrontFace();
    void GL_CullBackFace();
    void GL_CullFontAndBackFace();
    void GL_FontFaceCCW();
    void GL_FontFaceCW();

    void GL_EnableMultisample(int use);



    void GL_EnableDepthMask(int use);
    void GL_DepthFunction(Depth_Function type);
    void GL_EnableStencilMode(int use);
    void GL_EnableFramebufferSRGB(int enable);
    int GL_Begin3D();
    void GL_End3D();
    Texture GL_LoadTexture(const char* filePath, Texture_Filter filter);
    Texture GL_LoadNormalMap(const char* filePath, Texture_Filter filter);
    Texture GL_LoadParallaxMap(const char* filePath, Texture_Filter filter);
    void GL_DestroyTexture(Texture tex);
    int GL_PollQuit();
    void GL_ProcessInputs();
    void GL_ClearDepthColor(float r, float g, float b);
    void GL_SwapBuffers();
    void GL_Quit();
    void GL_DrawCube(Vector3 pos, Vector3 rot, Vector3 size, Texture tex);
    void GL_DrawCubeWithNormalMap(Vector3 pos, Vector3 rot, Vector3 size, Texture tex, Texture normalMapTex);
    void GL_DrawCubeWithParllaxMap(Vector3 pos, Vector3 rot, Vector3 size, Texture tex, Texture normalMapTex, Texture parllaxMapTex,float heightScale);
    void GL_SetTextureFilter(unsigned int texture, Texture_Filter filter);
    /*
#ifdef __cplusplus
#include <vector>
#include <string>
    unsigned int GL_LoadCubemap(std::vector<std::string> faces);
#endif
*/
    int GL_LoadSkyboxTextures(const char* right, const char* left, const char* top,
        const char* bottom, const char* front, const char* back);
    void GL_DrawSkybox();
    void GL_RenderSkybox();





    //FBO System
    void GL_InitQuad();
    void GL_RenderFBO();
    void GL_BindFBO();





    // Post-processing effects
    void GL_EnableChromaticAberration(int enable);
    void GL_SetChromaticAberrationStrength(float strength);
    void GL_EnableBloom(int enable);
    void GL_SetBloomThreshold(float threshold);
    void GL_SetBloomIntensity(float intensity);
    void GL_EnableFXAA(int enable);
    void GL_EnableColorCorrection(int enable, float brightness, float saturation, float contrast, float hue);
    void GL_EnableMotionBlur(int enable, float strength);
    //void GL_ApplyPostProcessing();

    // Sharpness control
    void GL_EnableSharpness(int enable);
    void GL_SetSharpnessIntensity(float intensity);

    //Timing
    float GL_GetDeltaTime();
    int GL_GetFPS();
        // Font structure definition
    typedef struct Font {
        unsigned int textureID;
        int width, height;
        const char* fontPath; // Add this to identify the font
        //stbtt_bakedchar* charData; // pointer only — no definition needed
        int fontSize;
    } Font;

    typedef struct TextureQuad3D {
        unsigned int vao, vbo, ebo;
    } TextureQuad3D;



    // Text rendering functions
    Font GL_LoadFont(const char* fontPath, int fontSize);
    void GL_DrawText(Font font, const char* text, Vector2 position, float scale, Vector3 color);
    void GL_DrawTextWithGradientH(Font font, const char* text, Vector2 position, float scale, Color4 c1, Color4 c2);
    void GL_DrawTextWithGradientV(Font font, const char* text, Vector2 position, float scale, Color4 c1, Color4 c2);
    void GL_FreeFont(Font font);


    //Line Mode
    void GL_DrawCubeLine(Vector3 pos, Vector3 rot, Vector3 size, Vector3 color, float thickness);
    void GL_DrawPixel3D(Vector3 position, Vector3 color, float size);
    void GL_DrawLine3D(Vector3 start, Vector3 end, Vector3 color, float thickness);



    //Color Utilities
    Color4f GL_GradientColor(Color4 c1, Color4 c2, float t);
    Vector3 GL_ConvertColor4ToVector3(Color4 input);
    Vector3 GL_ConvertColor4FToVector3(Color4f input);
    Color4f GL_MixColor(Color4 c1, Color4 c2);
    Color4f GL_MixColorWeighted(Color4 c1, Color4 c2, float weight1, float weight2);
    Color4f* GL_GenerateGradient(Color4 c1, Color4 c2, int steps);

    //Math Utilities
    Vector3 Vector3Cross(Vector3 a, Vector3 b);
    float Vector3Dot(Vector3 a, Vector3 b);
    float Vector3Length(Vector3 v);
    Vector3 Vector3Normalize(Vector3 v);


    //Drawing Texture must needed before drawing other shapes
    void GL_DrawTextureIn3D(

        TextureQuad3D quad,
        Texture texture, Vector3 pos, Vector3 rot, Vector3 size,
        float alpha);


    TextureQuad3D CreateTexturedQuad3D();



















    //Model + animation support
    int GL_LoadModel(const char* path,int flipTex);
    void GL_DrawStaticModel(int modelID, Vector3 pos, Vector3 rot, Vector3 scale);
    int GL_LoadAnimation(const char* path, int modelID);
    void GL_UpdateAllAnimations(float deltaTime);
    void GL_ResetAnimation(int animatorID);
    void GL_DrawAnimatedModel(int modelID, int animatorID, Vector3 pos, Vector3 rot, Vector3 scale);
    // Model and animation hot-reloading
    int GL_SetModelFile(const char* filePath, int modelID,int flipTex);
    int GL_SetAnimationFile(const char* filePath, int animationID);
    const char* GL_GetModelPath(int modelID);
    const char* GL_GetAnimationPath(int animationID);

    //GL Advanced 3.3 for enhancing old games graphics
    void GL_BindTexture(GL_Texture textureEnum, unsigned int textureID);
    void GL_BindFramebuffer(unsigned int fboID);
    void GL_EnableBlend(int use);
    void GL_SimpleBlendFunc();















    //GLFW
    void GL_SetWindowTitle(const char* title);
    void GL_SetWindowPosition(Vector2 pos);
    void GL_SetWindowSize(Vector2 size);
    const char* GL_GetClipboardString();
    void GL_SetClipboardString(const char* stringText);
    const char* GL_GetPlatform();
    const char* GL_GetMonitorName();
    void GL_SetCenterWindowPos();
    void GL_SetWindowOpacity(float opacity);
    void GL_SetWindowIcon(const char* path);
    void GL_SetCursorType(CursorType type);
    void GL_SetCustomCursor(const char* filename, int hotspotX, int hotspotY);
    void GL_RestoreCursorToDefault();
#if defined(__APPLE__) || defined(__linux__) || defined(_WIN32)
    void GL_SetWindowMaximize(int w, int h);
    void GL_SetWindowMaximize(int w, int h);
#else
    //Not for other operating systems
#endif


    int GL_GetMonitorWidth();
    int GL_GetMonitorHeight();
    int GL_GetWindowWidth();
    int GL_GetWindowHeight();
    int GL_GetMonitorRefreshRate();
    int GL_GetPhysicalMonitorWidth();
    int GL_GetPhysicalMonitorHeight();
    void GL_HideWindow();
    void GL_ShowWindow();
    int GL_IsWindowVisible();
    //int GL_IsWindowDragging();
    void GL_ToggleFullscreen();
    void GL_ToggleBorderlessWindowed();
    void GL_ShowCursor(void);
    void GL_HideCursor(void);
    int GL_IsCursorHidden(void);
    void GL_EnableCursor(void);
    void GL_DisableCursor(void);
    int GL_IsCursorOnScreen(void);
    void GL_Wait(double seconds);

    //Inputs
    //Keyboard
    int GL_KeyPressed(int key);
    int GL_KeyHold(int key);
    int GL_KeyReleased(int key);
    int GL_KeyRelease(int key);
    int GL_KeyRepeat(int key);
    int GL_KeyDown(int key);
    //Mouse
    int GL_MouseLeftDown();
    int GL_MouseRightDown();
    int GL_MouseMiddleDown();
    int GetMouseX(void);
    int GetMouseY(void);
    int GL_GetMouseDeltaX(void);
    int GL_GetMouseDeltaY(void);
    float GL_GetMouseWheelMoveX(void);
    float GL_GetMouseWheelMoveY(void);


    //ImGui for C Language
    //Core
    void GL_ImGui_Init();
    void GL_ImGui_Shutdown();
    void GL_ImGui_Render();

    //Processing
    int GL_ImGui_ProcessBeforeRenderer(int style);
    int GL_ImGui_BeginWindow(const char* title, Vector2 size, Vector2 pos);
    void GL_ImGui_Text(Vector2 pos, Color4f color, const char* fmt, ...);
    void GL_ImGui_SliderFloat(const char* label, float value, float min, float max, Vector2 pos);
    int GL_ImGui_Button(const char* label, Vector2 size, Vector2 pos);
    void GL_ImGui_EndWindow();


    //ImGui Draw List Mode
    void GL_ImGuiDrawList_Text(const char* fontPath, const char* label, Vector2 pos, Color4 color, float textSize);
    void GL_ImGuiDrawList_CenterText(const char* fontPath, const char* label, Color4 color, float textSize);

    int GL_ImGuiDrawList_CheckBox(Vector2 pos, Vector2 size, const char* label, int* checked);
    void GL_ImGuiDrawList_Edit(
        const char* label,
        char* buffer,
        size_t buffer_size,
        const char* fontPath,
        Vector2 pos,
        Color4 textColor,
        Color4 bgColor,
        float textSize,
        int border);
    void GL_ImGuiDrawList_SliderFloat(Vector2 pos, Vector2 size, float* value, float v_min, float v_max, const char* label);
    void GL_ImGuiDrawList_ImagePanel(Vector2 pos, Vector2 size, Texture tex, Color4 tint);
    void GL_ImGuiDrawList_SimpleFPS(float x, float y);
    void GL_ImGuiDrawList_Crosshair(const char* pathTex, int sizeX, int sizeY);
    //Imgui Menu bar support
    int GL_ImGui_BeginMenuBar();
    void GL_ImGui_EndMenuBar();
    int GL_ImGui_BeginMenu(const char* label, int enabled);
    void GL_ImGui_EndMenu();
    int GL_ImGui_NewMenuItem(const char* label, const char* shortcut, int selected, int enabled);
    void GL_ImGui_AddSeparator();
    //Messages Box
    void GL_ImGui_MsgBoxEx(const char* captionName, const char* message, void (*callback)(void), void (*cancelCallback)(void));
    void GL_ImGui_MsgBox(const char* captionName, const char* message, void (*callback)(void));
    void GL_ImGui_OpenPopupMsgBox(const char* captionName);


    //Trace Logs
    void GL_TraceLog(TraceType type, const char* fmt, ...);


// Initialize the random generator
    void Rand_Seed(uint64_t seed);
    void Rand_SeedTime(void);
    // Random integer
    int Rand_Int(int min, int max);
    // Random float (0.0 to 1.0)
    float Rand_Float(void);
    // Random float in range (min to max)
    float Rand_FloatRange(float min, float max);
    // Random double (0.0 to 1.0)
    double Rand_Double(void);
    // Random boolean (true or false)
    int Rand_Bool(void);
    // Random sign (+1 or -1)
    int Rand_Sign(void);
    // Random 3D vector (x,y,z in range)
    Vector3 Rand_Vector3(float min, float max);
    // Random RGBA color (0.0–1.0)
    Color4 Rand_Color(void);
    // Generate raw 64-bit value
    uint64_t Rand_U64(void);


    //URL
    void GL_OpenURL(const char* url);

    //Simple FileSystem

    // Check if a file exists (returns 1 if yes, 0 if not)
    int GL_FileExists(const char* fileName);

    // Check if a directory exists (returns 1 if yes, 0 if not)
    int GL_DirectoryExists(const char* dirPath);

    // Check if a file has a specific extension (case-insensitive, including dot: ".png")
    int GL_IsFileExtension(const char* fileName, const char* ext);


    // File info
    int GL_GetFileLength(const char* fileName);                       // Get file length in bytes
    const char* GL_GetFileExtension(const char* fileName);             // Pointer to extension (includes dot)
    const char* GL_GetFileName(const char* filePath);                  // Pointer to filename (after last / or \)
    const char* GL_GetFileNameWithoutExt(const char* filePath);        // Static buffer without extension








    //MathLib Helper
    float clampf(float value, float minVal, float maxVal);








    //Generate Checkerboard texture if texture or material not found like source engine.
    Texture GL_GenerateCheckerTexture(int width, int height, int checkSize);







    // In the enum section, add:
    typedef enum {
        FOG_LINEAR = 0,
        FOG_EXP,
        FOG_EXP2,
        FOG_VOLUMETRIC
    } FogType;

    // Add these function declarations in the appropriate sections:

    // Fog system functions
    void GL_EnableFog(int enable);
    void GL_SetFogType(FogType type);
    void GL_SetFogColor(Vector3 color);
    void GL_SetFogColor4f(Color4f color);
    void GL_SetFogDensity(float density);
    void GL_SetFogStart(float start);
    void GL_SetFogEnd(float end);
    void GL_SetFogOpacity(float opacity);




    // Volumetric fog functions
    void GL_EnableVolumetricFog(int enable);
    void GL_SetVolumetricFogSteps(int steps);
    void GL_SetVolumetricFogScattering(float scattering);
    void GL_SetVolumetricFogAbsorption(float absorption);
    void GL_SetVolumetricFogPhase(float phase);
    void GL_SetVolumetricFogNoiseScale(float scale);
    void GL_SetVolumetricFogNoiseStrength(float strength);
    void GL_SetVolumetricFogWind(Vector3 wind);
    void GL_SetVolumetricFogAnimate(int animate);




    
    // Billboard functions
    typedef enum {
        BILLBOARD_FACE_CAMERA = 0,      // Always face camera directly
        BILLBOARD_FACE_CAMERA_Y,        // Face camera but maintain Y-axis up
        BILLBOARD_AXIAL_X,              // Rotate around X-axis only
        BILLBOARD_AXIAL_Y,              // Rotate around Y-axis only  
        BILLBOARD_AXIAL_Z               // Rotate around Z-axis only
    } BillboardType;

    void GL_DrawBillboard(Texture texture, Vector3 position, Vector2 size, float alpha, BillboardType type);
    void GL_DrawBillboardEx(Texture texture, Vector3 position, Vector2 size, Vector3 color, float alpha, BillboardType type);









    //ALL Keys

    /* Printable keys */
#define GL_KEY_SPACE              32
#define GL_KEY_APOSTROPHE         39  /* ' */
#define GL_KEY_COMMA              44  /* , */
#define GL_KEY_MINUS              45  /* - */
#define GL_KEY_PERIOD             46  /* . */
#define GL_KEY_SLASH              47  /* / */
#define GL_KEY_0                  48
#define GL_KEY_1                  49
#define GL_KEY_2                  50
#define GL_KEY_3                  51
#define GL_KEY_4                  52
#define GL_KEY_5                  53
#define GL_KEY_6                  54
#define GL_KEY_7                  55
#define GL_KEY_8                  56
#define GL_KEY_9                  57
#define GL_KEY_SEMICOLON          59  /* ; */
#define GL_KEY_EQUAL              61  /* = */
#define GL_KEY_A                  65
#define GL_KEY_B                  66
#define GL_KEY_C                  67
#define GL_KEY_D                  68
#define GL_KEY_E                  69
#define GL_KEY_F                  70
#define GL_KEY_G                  71
#define GL_KEY_H                  72
#define GL_KEY_I                  73
#define GL_KEY_J                  74
#define GL_KEY_K                  75
#define GL_KEY_L                  76
#define GL_KEY_M                  77
#define GL_KEY_N                  78
#define GL_KEY_O                  79
#define GL_KEY_P                  80
#define GL_KEY_Q                  81
#define GL_KEY_R                  82
#define GL_KEY_S                  83
#define GL_KEY_T                  84
#define GL_KEY_U                  85
#define GL_KEY_V                  86
#define GL_KEY_W                  87
#define GL_KEY_X                  88
#define GL_KEY_Y                  89
#define GL_KEY_Z                  90
#define GL_KEY_LEFT_BRACKET       91  /* [ */
#define GL_KEY_BACKSLASH          92  /* \ */
#define GL_KEY_RIGHT_BRACKET      93  /* ] */
#define GL_KEY_GRAVE_ACCENT       96  /* ` */
#define GL_KEY_WORLD_1            161 /* non-US #1 */
#define GL_KEY_WORLD_2            162 /* non-US #2 */
/* Function keys */
#define GL_KEY_ESCAPE             256
#define GL_KEY_ENTER              257
#define GL_KEY_TAB                258
#define GL_KEY_BACKSPACE          259
#define GL_KEY_INSERT             260
#define GL_KEY_DELETE             261
#define GL_KEY_RIGHT              262
#define GL_KEY_LEFT               263
#define GL_KEY_DOWN               264
#define GL_KEY_UP                 265
#define GL_KEY_PAGE_UP            266
#define GL_KEY_PAGE_DOWN          267
#define GL_KEY_HOME               268
#define GL_KEY_END                269
#define GL_KEY_CAPS_LOCK          280
#define GL_KEY_SCROLL_LOCK        281
#define GL_KEY_NUM_LOCK           282
#define GL_KEY_PRINT_SCREEN       283
#define GL_KEY_PAUSE              284
#define GL_KEY_F1                 290
#define GL_KEY_F2                 291
#define GL_KEY_F3                 292
#define GL_KEY_F4                 293
#define GL_KEY_F5                 294
#define GL_KEY_F6                 295
#define GL_KEY_F7                 296
#define GL_KEY_F8                 297
#define GL_KEY_F9                 298
#define GL_KEY_F10                299
#define GL_KEY_F11                300
#define GL_KEY_F12                301
#define GL_KEY_F13                302
#define GL_KEY_F14                303
#define GL_KEY_F15                304
#define GL_KEY_F16                305
#define GL_KEY_F17                306
#define GL_KEY_F18                307
#define GL_KEY_F19                308
#define GL_KEY_F20                309
#define GL_KEY_F21                310
#define GL_KEY_F22                311
#define GL_KEY_F23                312
#define GL_KEY_F24                313
#define GL_KEY_F25                314
#define GL_KEY_KP_0               320
#define GL_KEY_KP_1               321
#define GL_KEY_KP_2               322
#define GL_KEY_KP_3               323
#define GL_KEY_KP_4               324
#define GL_KEY_KP_5               325
#define GL_KEY_KP_6               326
#define GL_KEY_KP_7               327
#define GL_KEY_KP_8               328
#define GL_KEY_KP_9               329
#define GL_KEY_KP_DECIMAL         330
#define GL_KEY_KP_DIVIDE          331
#define GL_KEY_KP_MULTIPLY        332
#define GL_KEY_KP_SUBTRACT        333
#define GL_KEY_KP_ADD             334
#define GL_KEY_KP_ENTER           335
#define GL_KEY_KP_EQUAL           336
#define GL_KEY_LEFT_SHIFT         340
#define GL_KEY_LEFT_CONTROL       341
#define GL_KEY_LEFT_ALT           342
#define GL_KEY_LEFT_SUPER         343
#define GL_KEY_RIGHT_SHIFT        344
#define GL_KEY_RIGHT_CONTROL      345
#define GL_KEY_RIGHT_ALT          346
#define GL_KEY_RIGHT_SUPER        347
#define GL_KEY_MENU               348
#define GL_KEY_LAST               GL_KEY_MENU
#define GL_MOD_SHIFT           0x0001
#define GL_MOD_CONTROL         0x0002
#define GL_MOD_ALT             0x0004
#define GL_MOD_SUPER           0x0008
#define GL_MOD_CAPS_LOCK       0x0010
#define GL_MOD_NUM_LOCK        0x0020
#define GL_MOUSE_BUTTON_1         0
#define GL_MOUSE_BUTTON_2         1
#define GL_MOUSE_BUTTON_3         2
#define GL_MOUSE_BUTTON_4         3
#define GL_MOUSE_BUTTON_5         4
#define GL_MOUSE_BUTTON_6         5
#define GL_MOUSE_BUTTON_7         6
#define GL_MOUSE_BUTTON_8         7
#define GL_MOUSE_BUTTON_LAST      GL_MOUSE_BUTTON_8
#define GL_MOUSE_BUTTON_LEFT      GL_MOUSE_BUTTON_1
#define GL_MOUSE_BUTTON_RIGHT     GL_MOUSE_BUTTON_2
#define GL_MOUSE_BUTTON_MIDDLE    GL_MOUSE_BUTTON_3




#ifdef __cplusplus
}
#endif

#endif //LIBGL_H