#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "camera.h"
#include "LibGL.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#define RENDER_FAR 500.0f
#define RENDER_NEAR 0.1f
//Debug Settings
#define ENABLE_DEBUG 0
//FPS
double fps = 0.0f;
int iterationsBlur = 6;
static bool lockedMouse = true;

// Cube Settings
unsigned int CubeVBO, CubeVAO;
float CubeVertices[] = {
    // Positions          // Normals           // Texture Coords  // Tangents         // Bitangents
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

    // Left face
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,

    // Right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

     // Bottom face
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,

     // Top face
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f
};

// Skybox Settings
unsigned int skyboxVAO, skyboxVBO;
unsigned int cubemapTexture;
ShaderCPP skyboxShader;

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};
// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
static void GL_InitCube();

// Settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// Camera
CameraCPP camera;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting system
bool lightingEnabled = true;
bool useBlinnPhong = true; // Use Blinn-Phong by default
bool normalMappingEnabled = false;
std::vector<Light> lights;
int activeLightIndex = 0;
Vector3 ambientLightColor = { 0.2f, 0.2f, 0.2f };
float ambientLightIntensity = 0.2f;

// Material properties
Vector3 materialAmbient = { 1.0f, 1.0f, 1.0f };
Vector3 materialDiffuse = { 1.0f, 1.0f, 1.0f };
Vector3 materialSpecular = { 1.0f, 1.0f, 1.0f };
float materialShininess = 32.0f;

GLFWwindow* window = NULL;
GLFWmonitor* monitor = NULL;
GLFWcursor* cursor = NULL;
static bool g_CursorHidden = false;
static bool g_CursorDisabled = false;
glm::mat4 projection;
glm::mat4 view;
ShaderCPP camShader;
ShaderCPP scrShader;
ShaderCPP texturein3dShader;
ShaderCPP lineShader;
ShaderCPP pointShader;


// Framebuffer (FBO) system
unsigned int pingpongFBO[2];
unsigned int pingpongColorbuffers[2];
unsigned int postFBO;
unsigned int postColorBuffer;

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};
unsigned int quadVAO, quadVBO;
unsigned int framebuffer;
unsigned int textureColorbuffer;
unsigned int depthTexture;
// post-processing
//Enables
bool sharpnessEnabled = false;
bool fxaaEnabled = false;
bool correctionEnabled = false;
bool motionBlurEnabled = false;
bool chromaticAberrationEnabled = false;
bool bloomEnabled = true;
//Settings
//Color Correction
float Cbrightness = 1.0f;
float Ccontrast = 1.0f;
float Csaturation = 1.0f;
float CHue = 0.0f;
//Sharpness
float sharpnessIntensity = 1.0f;
//Motion Blur
float motionBlurStrength = 1.0f;
//Chromatic Aberration
float chromaticAberrationStrength = 0.01f;
//Bloom
float bloomThreshold = 1.0f;
float bloomIntensity = 1.0f;
//Shaders
ShaderCPP sharpnessShader;
ShaderCPP fxaaShader;
ShaderCPP colorCorrectionShader;
ShaderCPP motionBlurShader;
ShaderCPP postProcessingShader;
ShaderCPP blurShader;



static const char* scrFS2 = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
} 
)";

// Globals to track drag state
static bool isDragging = false;
static bool leftPressed = false;
static int lastWX = 0, lastWY = 0;
// Callback: track left mouse button state
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS)
            leftPressed = true;
        else if (action == GLFW_RELEASE)
            leftPressed = false;
    }
}


// C-compatible function implementations
extern "C" {

    void GL_SetLightPosition(int lightIndex, Vector3 position) {
        if (lightIndex >= 0 && lightIndex < lights.size()) {
            lights[lightIndex].position = position;
        }
    }

    void GL_SetLightColor(int lightIndex, Vector3 color) {
        if (lightIndex >= 0 && lightIndex < lights.size()) {
            lights[lightIndex].color = color;
        }
    }

    void GL_SetLightIntensity(int lightIndex, float intensity) {
        if (lightIndex >= 0 && lightIndex < lights.size()) {
            lights[lightIndex].intensity = intensity;
        }
    }

    void GL_SetLightAttenuation(int lightIndex, float constant, float linear, float quadratic) {
        if (lightIndex >= 0 && lightIndex < lights.size()) {
            lights[lightIndex].constant = constant;
            lights[lightIndex].linear = linear;
            lights[lightIndex].quadratic = quadratic;
        }
    }

    void GL_EnableLight(int lightIndex, int enable) {
        if (lightIndex >= 0 && lightIndex < lights.size()) {
            lights[lightIndex].enabled = (enable != 0);
        }
    }

    int GL_GetNumLights() {
        return (int)lights.size();
    }

    int GL_GetNumActiveLights() {
        int count = 0;
        for (const auto& light : lights) {
            if (light.enabled) count++;
        }
        return count;
    }

    void GL_InitLighting() {
        lights.clear();
        lightingEnabled = true;
        useBlinnPhong = true;
        normalMappingEnabled = false;

        // Create a better default directional light
        Light defaultLight;
        defaultLight.enabled = false;
        defaultLight.type = LIGHT_DIRECTIONAL;
        defaultLight.position = { 0.0f, 10.0f, 0.0f };  // Position matters for some calculations
        defaultLight.direction = { -0.2f, -1.0f, -0.3f };
        defaultLight.color = { 1.0f, 1.0f, 1.0f };
        defaultLight.intensity = 1.0f;
        defaultLight.ambient = 0.1f;
        defaultLight.diffuse = 0.8f;
        defaultLight.specular = 1.0f;
        defaultLight.constant = 1.0f;
        defaultLight.linear = 0.09f;
        defaultLight.quadratic = 0.032f;
        defaultLight.cutOff = glm::cos(glm::radians(12.5f));
        defaultLight.outerCutOff = glm::cos(glm::radians(15.0f));

        lights.push_back(defaultLight);

        // Fix any potential issues
        GL_FixLightingIssues();
    }
    void GL_CreateDefaultLight(int create) {
        if (create && lights.empty()) {
            // Create a default directional light only if requested and no lights exist
            Light defaultLight;
            defaultLight.enabled = true;
            defaultLight.type = LIGHT_DIRECTIONAL;
            defaultLight.position = { 0.0f, 10.0f, 0.0f };
            defaultLight.direction = { -0.2f, -1.0f, -0.3f };
            defaultLight.color = { 1.0f, 1.0f, 1.0f };
            defaultLight.intensity = 1.0f;
            defaultLight.ambient = 0.1f;
            defaultLight.diffuse = 0.8f;
            defaultLight.specular = 1.0f;
            defaultLight.constant = 1.0f;
            defaultLight.linear = 0.09f;
            defaultLight.quadratic = 0.032f;
            defaultLight.cutOff = glm::cos(glm::radians(12.5f));
            defaultLight.outerCutOff = glm::cos(glm::radians(15.0f));

            lights.push_back(defaultLight);
            std::cout << "Default directional light created" << std::endl;
        }
    }
    // Additional lighting fixes
    void GL_FixLightingIssues() {
        // Ensure at least one light exists
        if (lights.empty()) {
            GL_InitLighting();
        }

        // Validate light data
        for (auto& light : lights) {
            // Ensure light direction is normalized
            float length = sqrt(light.direction.x * light.direction.x +
                light.direction.y * light.direction.y +
                light.direction.z * light.direction.z);
            if (length > 0) {
                light.direction.x /= length;
                light.direction.y /= length;
                light.direction.z /= length;
            }
        }
    }

    void GL_SetLightingEnabled(int enabled) {
        lightingEnabled = (enabled != 0);
    }

    void GL_SetShadingModel(int useBlinnPhongFlag) {
        useBlinnPhong = (useBlinnPhongFlag != 0);
    }

    void GL_SetNormalMappingEnabled(int enabled) {
        normalMappingEnabled = (enabled != 0);
    }
    
    int GL_CreateLight(LightType type) {
        Light newLight;
        newLight.enabled = true;
        newLight.type = type;
        newLight.position = { 0.0f, 0.0f, 0.0f };
        newLight.direction = { 0.0f, 0.0f, -1.0f };
        newLight.color = { 1.0f, 1.0f, 1.0f };
        newLight.intensity = 1.0f;
        newLight.ambient = 0.1f;
        newLight.diffuse = 0.5f;
        newLight.specular = 1.0f;
        newLight.constant = 1.0f;
        newLight.linear = 0.09f;
        newLight.quadratic = 0.032f;
        newLight.cutOff = glm::cos(glm::radians(12.5f));
        newLight.outerCutOff = glm::cos(glm::radians(15.0f));

        lights.push_back(newLight);
        return lights.size() - 1;
    }

    void GL_DeleteLight(int lightIndex) {
        if (lightIndex >= 0 && lightIndex < lights.size()) {
            lights.erase(lights.begin() + lightIndex);
        }
    }

    void GL_UpdateLight(int lightIndex, Light light) {
        if (lightIndex >= 0 && lightIndex < lights.size()) {
            lights[lightIndex] = light;
        }
    }

    Light GL_GetLight(int lightIndex) {
        if (lightIndex >= 0 && lightIndex < lights.size()) {
            return lights[lightIndex];
        }
        else {
            Light defaultLight = { 0 };
            return defaultLight;
        }
        /*// Return a default light if index is invalid
        */
    }

    void GL_SetActiveLight(int lightIndex) {
        if (lightIndex >= 0 && lightIndex < lights.size()) {
            activeLightIndex = lightIndex;
        }
    }

    void GL_SetMaterial(Vector3 ambient, Vector3 diffuse, Vector3 specular, float shininess) {
        materialAmbient = ambient;
        materialDiffuse = diffuse;
        materialSpecular = specular;
        materialShininess = shininess;
    }

    void GL_SetAmbientLight(Vector3 color, float intensity) {
        ambientLightColor = color;
        ambientLightIntensity = intensity;
    }

    void GL_EnableLighting(int enable) {
        lightingEnabled = (enable != 0);
    }

} // extern "C"
bool msaaEnabled = false;
void GLFWErrorCallback(int error, const char* description) {
    std::cout << "GLFW Error (" << error << "): " << description << std::endl;
}
// Existing functions (implementation remains mostly the same, but updated for lighting)
int GL_InitWindow(int width, int height, const char* title,int enableMSAA) {
    glfwInit();

    glfwSetErrorCallback(GLFWErrorCallback);

    if (enableMSAA) {
        glfwWindowHint(GLFW_SAMPLES, 4);
        msaaEnabled = enableMSAA;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    if (!width || !height) {
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);
    }
    else {
        window = glfwCreateWindow(width, height, title, NULL, NULL);
        SCR_WIDTH = width;
        SCR_HEIGHT = height;
    }
    monitor = glfwGetPrimaryMonitor();

    if (!monitor) {
        printf("No monitor found!\n");
    }


    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    GL_SetCenterWindowPos();
    // Initialize lighting system
    GL_InitLighting();

    return 1;
}

void GL_InitCamera(
    Camera cam,
    Vector3 pos,
    float Fovy,
    float movementSpeed,
    float sensitivity,
    int type,int enableZoom) {

    camera.InitCamera(
        glm::vec3(pos.x, pos.y, pos.z),
        { 0,1,0 }, -90, 0.0f
        , Fovy, movementSpeed, sensitivity, type, enableZoom);
}

void GL_InitSkybox();

void GL_CaptureMouse(int use) {
    if (!window) return;

    if (use) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        lockedMouse = true;  // Enable mouse locking
        firstMouse = true;   // Reset for smooth transition
        g_CursorDisabled = true;
        g_CursorHidden = false;

        // Optional: Center cursor when capturing
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetCursorPos(window, width / 2.0, height / 2.0);
    }
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        lockedMouse = false; // Disable mouse locking
        g_CursorDisabled = false;
        g_CursorHidden = false;
    }
}
void GL_InitPingPongFramebuffers(int width, int height)
{
    // --- Delete old resources if they exist (optional safety) ---
    if (pingpongFBO[0]) glDeleteFramebuffers(2, pingpongFBO);
    if (pingpongColorbuffers[0]) glDeleteTextures(2, pingpongColorbuffers);
    if (postFBO) glDeleteFramebuffers(1, &postFBO);
    if (postColorBuffer) glDeleteTextures(1, &postColorBuffer);

    // --- Generate Ping-Pong FBOs ---
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);

        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);

        // Use RGBA16F for safety with float buffers (works on all GPUs)
        GLenum internalFormat = bloomEnabled ? GL_RGBA16F : GL_RGB8;
        GLenum format = bloomEnabled ? GL_RGBA : GL_RGB;
        GLenum type = bloomEnabled ? GL_FLOAT : GL_UNSIGNED_BYTE;

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

        // --- Check completeness ---
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "ERROR::FRAMEBUFFER:: Pingpong FBO " << i << " is not complete!" << std::endl;
        }

        // Unbind for safety
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // --- Create optional Post FBO (used if you want one more intermediate target) ---
    glGenFramebuffers(1, &postFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postFBO);

    glGenTextures(1, &postColorBuffer);
    glBindTexture(GL_TEXTURE_2D, postColorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postColorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Post FBO not complete!" << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GL_InitPingPongFramebuffers3() {
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);

    int blurWidth = SCR_WIDTH; /// 2;
    int blurHeight = SCR_HEIGHT; /// 2;

    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        GLenum internalFormat = bloomEnabled ? GL_RGB16F : GL_RGB8;
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, blurWidth, blurHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create post FBO (1 color attachment)
    glGenFramebuffers(1, &postFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postFBO);

    glGenTextures(1, &postColorBuffer);
    glBindTexture(GL_TEXTURE_2D, postColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postColorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Post FBO not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void GL_InitPingPongFramebuffers2() {
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);

    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::PINGPONG:: Framebuffer " << i << " is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GL_ApplyGaussianBlur(int iterations) {
    bool horizontal = true, first_iteration = true;

    blurShader.use();
    for (unsigned int i = 0; i < iterations * 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        blurShader.setBool("horizontal", horizontal);

        // For first iteration, use the main framebuffer's color texture
        // For subsequent iterations, use the pingpong buffers
        glActiveTexture(GL_TEXTURE0);
        if (first_iteration) {
            glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
            first_iteration = false;
        }
        else {
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        }

        // Render quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        horizontal = !horizontal;
    }
    blurShader.unuse();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GL_RenderBlurredFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    scrShader.use();
    glBindVertexArray(quadVAO);
    // Use the last pingpong buffer (which contains the final blurred image)
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

unsigned int GL_GetPingPongTexture(int index) {
    if (index >= 0 && index < 2) {
        return pingpongColorbuffers[index];
    }
    return 0;
}

void GL_BindPingPongFBO(int index) {
    if (index >= 0 && index < 2) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[index]);
        glEnable(GL_DEPTH_TEST);
    }
}

void GL_InitTextRendering();
void GL_GenerateVolumetricNoiseTexture();


int GL_InitOpenGLLoader() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Check for critical OpenGL features
    if (!GLAD_GL_VERSION_3_3) {
        std::cout << "OpenGL 3.3 not supported!" << std::endl;
        return -1;
    }

    if (GLAD_GL_KHR_debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
            GLsizei length, const GLchar* message, const void* userParam) {
                if (type == GL_DEBUG_TYPE_ERROR) {
                    std::cout << "OpenGL Error: " << message << std::endl;
                }
            }, nullptr);
    }

    // Add OpenGL version check
#if ENABLE_DEBUG
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
#endif
    // Check for required OpenGL version
    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (major < 3 || (major == 3 && minor < 3)) {
        std::cout << "OpenGL 3.3+ required!" << std::endl;
        return -1;
    }
    // Ensure correct initial viewport
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    SCR_WIDTH = fbWidth;
    SCR_HEIGHT = fbHeight;
    camShader.InitShaderFromFiles("shaders/camShader.vs", "shaders/camShader.fs");
    scrShader.InitShaderFromFiles("shaders/scr.vs", "shaders/scr.fs");
    texturein3dShader.InitShaderFromFiles("shaders/texin3d.vs", "shaders/texin3d.fs");
    // Initialize post-processing shader
    postProcessingShader.InitShaderFromFiles("shaders/scr.vs", "shaders/postprocessing.fs");
    sharpnessShader.InitShaderFromFiles("shaders/scr.vs", "shaders/sharpness.fs"); // Use the fast version
    fxaaShader.InitShaderFromFiles("shaders/scr.vs", "shaders/fxaa.fs"); // Use the fast version
    colorCorrectionShader.InitShaderFromFiles("shaders/scr.vs", "shaders/colorcorrection.fs"); // Use the fast version
    motionBlurShader.InitShaderFromFiles("shaders/scr.vs", "shaders/motionblur.fs");
    
    GL_InitCube();
    GL_InitSkybox();
    GL_InitQuad();

    scrShader.use();
    scrShader.setInt("screenTexture", 0);

    postProcessingShader.use();
    postProcessingShader.setInt("screenTexture", 0);
    postProcessingShader.setInt("bloomBlur", 1);
    // framebuffer configuration
    // -------------------------
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SCR_WIDTH, SCR_HEIGHT, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Initialize blur shader
    blurShader.InitShaderFromFiles("shaders/scr.vs", "shaders/blur.fs"); // Uses same vertex shader as screen quad
    // Initialize pingpong framebuffers
    GL_InitPingPongFramebuffers(SCR_WIDTH,SCR_HEIGHT);
    // Initialize text rendering system
    GL_InitTextRendering();

    if (msaaEnabled)
    {
        GL_EnableMultisample(1);
    }

    lineShader.InitShaderFromFiles("shaders/line.vs", "shaders/line.fs");

    pointShader.InitShaderFromFiles("shaders/point.vs", "shaders/point.fs");

    GL_GenerateVolumetricNoiseTexture();
    return 1;
}
void GL_EnableMultisample(int use) {
    if (use) {
        glEnable(GL_MULTISAMPLE);
    }
    else {
        glDisable(GL_MULTISAMPLE);
    }
}
void GL_EnableDepthTest(int enable) {
    if (enable)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}
void GL_EnableFramebufferSRGB(int enable) {
    if (enable)
    {
        glEnable(GL_FRAMEBUFFER_SRGB);
    }
    else
    {
        glDisable(GL_FRAMEBUFFER_SRGB);
    }
}
void GL_EnableStencilMode(int use) {
    if (use) {
        glEnable(GL_STENCIL_TEST);
    }
    else  {
        glDisable(GL_STENCIL_TEST);
    }
}
void GL_BindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Enhanced framebuffer status check
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: " << status << std::endl;

        // Fallback to default framebuffer with error details
        const char* errorMsg = "";
        switch (status) {
        case GL_FRAMEBUFFER_UNDEFINED: errorMsg = "GL_FRAMEBUFFER_UNDEFINED"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
        case GL_FRAMEBUFFER_UNSUPPORTED: errorMsg = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
        default: errorMsg = "UNKNOWN_ERROR"; break;
        }
        std::cout << "Framebuffer error details: " << errorMsg << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
}
bool GL_IsContextValid() {
    return window != NULL && glfwGetCurrentContext() != NULL;
}
// Add with other post-processing settings
bool fogEnabled = false;
FogType fogType = FOG_LINEAR;
Vector3 fogColor = { 0.5f, 0.5f, 0.5f };
float fogDensity = 0.01f;
float fogStart = 10.0f;
float fogEnd = 50.0f;
float fogOpacity = 1.0f;
// Add with other fog settings
bool volumetricFogEnabled = false;
int volumetricFogSteps = 16;
float volumetricFogScattering = 0.5f;
float volumetricFogAbsorption = 0.2f;
float volumetricFogPhase = 0.2f;
float volumetricFogNoiseScale = 0.1f;
float volumetricFogNoiseStrength = 0.3f;
Vector3 volumetricFogWind = { 0.1f, 0.0f, 0.1f };
bool volumetricFogAnimate = true;
unsigned int volumetricNoiseTexture = 0;

// Add noise texture generation function
void GL_GenerateVolumetricNoiseTexture() {
    const int width = 128;
    const int height = 128;
    const int depth = 128;

    std::vector<unsigned char> noiseData(width * height * depth);

    for (int z = 0; z < depth; z++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // Simple 3D noise generation (you can replace with better noise)
                float nx = (float)x / width;
                float ny = (float)y / height;
                float nz = (float)z / depth;

                // Simple value noise
                float noise = std::sin(nx * 10.0f) * std::sin(ny * 10.0f) * std::sin(nz * 10.0f);
                noise = (noise + 1.0f) * 0.5f; // Normalize to 0-1

                // Add some turbulence
                noise += std::sin(nx * 20.0f + ny * 15.0f) * 0.5f;
                noise += std::sin(ny * 25.0f + nz * 18.0f) * 0.25f;
                noise = fmod(noise, 1.0f);

                noiseData[z * width * height + y * width + x] = (unsigned char)(noise * 255.0f);
            }
        }
    }

    glGenTextures(1, &volumetricNoiseTexture);
    glBindTexture(GL_TEXTURE_3D, volumetricNoiseTexture);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, width, height, depth, 0, GL_RED, GL_UNSIGNED_BYTE, noiseData.data());

    std::cout << "Generated volumetric noise texture: " << volumetricNoiseTexture << std::endl;
}

    void GL_EnableFog(int enable) {
        fogEnabled = (enable != 0);
    }

    void GL_SetFogType(FogType type) {
        fogType = type;
    }

    void GL_SetFogColor(Vector3 color) {
        fogColor = color;
    }

    void GL_SetFogColor4f(Color4f color) {
        fogColor.x = color.r;
        fogColor.y = color.g;
        fogColor.z = color.b;
    }

    void GL_SetFogDensity(float density) {
        fogDensity = density;
    }

    void GL_SetFogStart(float start) {
        fogStart = start;
    }

    void GL_SetFogEnd(float end) {
        fogEnd = end;
    }

    void GL_SetFogOpacity(float opacity) {
        fogOpacity = opacity;
    }

    void GL_EnableVolumetricFog(int enable) {
        volumetricFogEnabled = (enable != 0);
    }

    void GL_SetVolumetricFogSteps(int steps) {
        volumetricFogSteps = steps;
    }

    void GL_SetVolumetricFogScattering(float scattering) {
        volumetricFogScattering = scattering;
    }

    void GL_SetVolumetricFogAbsorption(float absorption) {
        volumetricFogAbsorption = absorption;
    }

    void GL_SetVolumetricFogPhase(float phase) {
        volumetricFogPhase = phase;
    }

    void GL_SetVolumetricFogNoiseScale(float scale) {
        volumetricFogNoiseScale = scale;
    }

    void GL_SetVolumetricFogNoiseStrength(float strength) {
        volumetricFogNoiseStrength = strength;
    }

    void GL_SetVolumetricFogWind(Vector3 wind) {
        volumetricFogWind = wind;
    }

    void GL_SetVolumetricFogAnimate(int animate) {
        volumetricFogAnimate = (animate != 0);
    }

int GL_Begin3D() {
    if (!GL_IsContextValid()) {
        std::cout << "ERROR: No valid OpenGL context!" << std::endl;
        return 0;
    }


    if (fps < 35.0 && bloomEnabled)
    {
        GL_EnableBloom(0);
    }
    camShader.use();

    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, RENDER_NEAR, RENDER_FAR);
    camShader.setMat4("projection", projection);

    view = camera.GetViewMatrix();
    camShader.setMat4("view", view);

    // Set lighting uniforms
    camShader.setBool("lightingEnabled", lightingEnabled);
    camShader.setBool("useBlinnPhong", useBlinnPhong);
    camShader.setBool("normalMappingEnabled", normalMappingEnabled);

    glUniform1i(glGetUniformLocation(camShader.ID, "darknessMapEnabled"), false);  // or false

    camShader.setBool("parallaxMappingEnabled", false);
    camShader.setFloat("heightScale", 0.04f);



    camShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

    // Set light position for normal mapping (use first light)
    if (lights.size() > 0) {
        camShader.setVec3("lightPos", lights[0].position.x, lights[0].position.y, lights[0].position.z);
    }

    // Set material properties
    camShader.setVec3("material.ambient", materialAmbient.x, materialAmbient.y, materialAmbient.z);
    camShader.setVec3("material.diffuse", materialDiffuse.x, materialDiffuse.y, materialDiffuse.z);
    camShader.setVec3("material.specular", materialSpecular.x, materialSpecular.y, materialSpecular.z);
    camShader.setFloat("material.shininess", materialShininess);

    // Set ambient light
    camShader.setVec3("ambientLightColor", ambientLightColor.x, ambientLightColor.y, ambientLightColor.z);
    camShader.setFloat("ambientLightIntensity", ambientLightIntensity);

    // Set lights
    camShader.setInt("numLights", lights.size());
    for (int i = 0; i < lights.size(); i++) {
        std::string lightStr = "lights[" + std::to_string(i) + "]";
        camShader.setBool(lightStr + ".enabled", lights[i].enabled);
        camShader.setInt(lightStr + ".type", lights[i].type);
        camShader.setVec3(lightStr + ".position", lights[i].position.x, lights[i].position.y, lights[i].position.z);
        camShader.setVec3(lightStr + ".direction", lights[i].direction.x, lights[i].direction.y, lights[i].direction.z);
        camShader.setVec3(lightStr + ".color", lights[i].color.x, lights[i].color.y, lights[i].color.z);
        camShader.setFloat(lightStr + ".intensity", lights[i].intensity);
        camShader.setFloat(lightStr + ".ambient", lights[i].ambient);
        camShader.setFloat(lightStr + ".diffuse", lights[i].diffuse);
        camShader.setFloat(lightStr + ".specular", lights[i].specular);
        camShader.setFloat(lightStr + ".constant", lights[i].constant);
        camShader.setFloat(lightStr + ".linear", lights[i].linear);
        camShader.setFloat(lightStr + ".quadratic", lights[i].quadratic);
        camShader.setFloat(lightStr + ".cutOff", lights[i].cutOff);
        camShader.setFloat(lightStr + ".outerCutOff", lights[i].outerCutOff);
    }

    float currentTime = static_cast<float>(glfwGetTime());
    camShader.setFloat("time", currentTime);



    // Set fog uniforms
    camShader.setBool("fogEnabled", fogEnabled);
    camShader.setInt("fogType", fogType);
    camShader.setVec3("fogColor", fogColor.x, fogColor.y, fogColor.z);
    camShader.setFloat("fogDensity", fogDensity);
    camShader.setFloat("fogStart", fogStart);
    camShader.setFloat("fogEnd", fogEnd);
    camShader.setFloat("fogOpacity", fogOpacity);










    // Set volumetric fog uniforms
    camShader.setBool("volumetricFogEnabled", volumetricFogEnabled);
    camShader.setInt("volumetricFogSteps", volumetricFogSteps);
    camShader.setFloat("volumetricFogScattering", volumetricFogScattering);
    camShader.setFloat("volumetricFogAbsorption", volumetricFogAbsorption);
    camShader.setFloat("volumetricFogPhase", volumetricFogPhase);
    camShader.setFloat("volumetricFogNoiseScale", volumetricFogNoiseScale);
    camShader.setFloat("volumetricFogNoiseStrength", volumetricFogNoiseStrength);
    camShader.setVec3("volumetricFogWind", volumetricFogWind.x, volumetricFogWind.y, volumetricFogWind.z);
    camShader.setBool("volumetricFogAnimate", volumetricFogAnimate);

    // Bind noise texture
    if (volumetricNoiseTexture != 0) {
        glActiveTexture(GL_TEXTURE5); // Use texture unit 5 for noise
        glBindTexture(GL_TEXTURE_3D, volumetricNoiseTexture);
        camShader.setInt("volumetricNoiseTexture", 5);
    }


    return 1;
}
void GL_End3D() {
    camShader.unuse();
}
void GL_ExtractBrightAreas() {
    if (!bloomEnabled) return;

    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use a shader that extracts bright areas (values > bloomThreshold)
    scrShader.use();
    scrShader.setFloat("bloomThreshold", bloomThreshold);
    scrShader.setInt("screenTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer); // Main scene

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void GL_RenderFBO()
{
    // --- Get actual framebuffer size
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    // Disable depth test during post-processing
    glDisable(GL_DEPTH_TEST);

    // Start from the scene-rendered texture
    GLuint currentTexture = textureColorbuffer; // Scene render target
    int ping = 0;

    // ======================================================
    // SHARPNESS PASS
    // ======================================================
    if (sharpnessEnabled)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[ping]);
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        sharpnessShader.use();
        sharpnessShader.setFloat("sharpnessIntensity", sharpnessIntensity);
        sharpnessShader.setInt("screenTexture", 0); // Set texture unit

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Update current texture to the result
        currentTexture = pingpongColorbuffers[ping];
        ping = 1 - ping;
    }

    // ======================================================
    // FXAA PASS (anti-aliasing)
    // ======================================================
    if (fxaaEnabled)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[ping]);
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        fxaaShader.use();
        fxaaShader.setVec2("screenSize", glm::vec2((float)fbWidth, (float)fbHeight));
        fxaaShader.setInt("screenTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        currentTexture = pingpongColorbuffers[ping];
        ping = 1 - ping;
    }

    // ======================================================
    // MOTION BLUR PASS (Fixed duplicate FXAA check)
    // ======================================================
    if (motionBlurEnabled) // Add this flag to your code
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[ping]);
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        motionBlurShader.use();
        motionBlurShader.setFloat("motionBlurStrength", motionBlurStrength);
        motionBlurShader.setVec2("screenSize", glm::vec2((float)fbWidth, (float)fbHeight));
        motionBlurShader.setInt("screenTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        currentTexture = pingpongColorbuffers[ping];
        ping = 1 - ping;
    }

    // ======================================================
    // COLOR CORRECTION PASS
    // ======================================================
    if (correctionEnabled)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[ping]);
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        colorCorrectionShader.use();
        colorCorrectionShader.setFloat("brightness", Cbrightness);
        colorCorrectionShader.setFloat("contrast", Ccontrast);
        colorCorrectionShader.setFloat("saturation", Csaturation);
        colorCorrectionShader.setFloat("hue", CHue);
        colorCorrectionShader.setFloat("gamma", 1.0f);
        colorCorrectionShader.setFloat("exposure", 1.0f);
        colorCorrectionShader.setFloat("temperature", 0.0f);
        colorCorrectionShader.setInt("screenTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        currentTexture = pingpongColorbuffers[ping];
        ping = 1 - ping;
    }
    // BLOOM PASS - Add this before final composite
    if (bloomEnabled) {
        // Step 1: Extract bright areas
        GL_ExtractBrightAreas();

        // Step 2: Apply Gaussian blur to bright areas
        GL_ApplyGaussianBlur(iterationsBlur);
    }


    // ======================================================
    // FINAL COMPOSITE PASS
    // ======================================================
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, fbWidth, fbHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    postProcessingShader.use();
    postProcessingShader.setBool("chromaticAberrationEnabled", chromaticAberrationEnabled);
    postProcessingShader.setBool("bloomEnabled", bloomEnabled);
    postProcessingShader.setFloat("chromaticStrength", chromaticAberrationStrength);
    postProcessingShader.setFloat("bloomIntensity", bloomIntensity);
    postProcessingShader.setFloat("bloomThreshold", bloomThreshold);
    postProcessingShader.setInt("screenTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, currentTexture);

    // Bind bloom texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[0]); // Blurred bright areas



    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Reset texture binding
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // restore depth test
    glEnable(GL_DEPTH_TEST);
}
void GL_RenderFBOOOLD()
{
    // --- Get actual framebuffer size (important for FXAA + sharpness)
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    // Disable depth test during post-processing
    glDisable(GL_DEPTH_TEST);

    // Start from the scene-rendered texture
    GLuint currentTexture = textureColorbuffer; // Scene render target

    // ping index (0 or 1) for writing into pingpongColorbuffers[ping]
    int ping = 0;

    // Ensure shaders sample from texture unit 0
    fxaaShader.use();            fxaaShader.setInt("screenTexture", 0);
    sharpnessShader.use();       sharpnessShader.setInt("screenTexture", 0);
    colorCorrectionShader.use(); colorCorrectionShader.setInt("screenTexture", 0);
    motionBlurShader.use(); motionBlurShader.setInt("screenTexture", 0);
    postProcessingShader.use();  postProcessingShader.setInt("screenTexture", 0);

    // ======================================================
    // SHARPNESS PASS
    // ======================================================
    if (sharpnessEnabled)
    {
        // Write into ping-pong FBO[ping]
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[ping]);
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        sharpnessShader.use();
        sharpnessShader.setFloat("sharpnessIntensity", sharpnessIntensity);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // After rendering, pingpongColorbuffers[ping] holds the result
        currentTexture = pingpongColorbuffers[ping];

        // flip ping for next write
        ping = 1 - ping;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // ======================================================
    // FXAA PASS (anti-aliasing)
    // ======================================================
    if (fxaaEnabled)
    {
        // write into ping-pong FBO[ping]
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[ping]);
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        fxaaShader.use();
        fxaaShader.setVec2("screenSize", glm::vec2((float)fbWidth, (float)fbHeight));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        currentTexture = pingpongColorbuffers[ping];
        ping = 1 - ping;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    if (fxaaEnabled)
    {
        // write into ping-pong FBO[ping]
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[ping]);
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        motionBlurShader.use();
        motionBlurShader.setFloat("motionBlurStrength", motionBlurStrength);
        motionBlurShader.setVec2("screenSize", glm::vec2((float)fbWidth, (float)fbHeight));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        currentTexture = pingpongColorbuffers[ping];
        ping = 1 - ping;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // ======================================================
    // COLOR CORRECTION PASS
    // ======================================================
    if (correctionEnabled)
    {
        // write into ping-pong FBO[ping]
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[ping]);
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        colorCorrectionShader.use();
        colorCorrectionShader.setFloat("brightness", Cbrightness);
        colorCorrectionShader.setFloat("contrast", Ccontrast);
        colorCorrectionShader.setFloat("saturation", Csaturation);
        colorCorrectionShader.setFloat("hue", CHue);
        colorCorrectionShader.setFloat("gamma", 1.0f);
        colorCorrectionShader.setFloat("exposure", 1.0f);
        colorCorrectionShader.setFloat("temperature", 0.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        currentTexture = pingpongColorbuffers[ping];
        ping = 1 - ping;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // ======================================================
    // FINAL COMPOSITE PASS (NO BLOOM, KEEP OTHERS)
    // ======================================================
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, fbWidth, fbHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    postProcessingShader.use();
    postProcessingShader.setBool("chromaticAberrationEnabled", chromaticAberrationEnabled);
    postProcessingShader.setBool("bloomEnabled", bloomEnabled);
    postProcessingShader.setFloat("chromaticStrength", chromaticAberrationStrength);
    postProcessingShader.setFloat("bloomIntensity", bloomIntensity);
    postProcessingShader.setFloat("bloomThreshold", bloomThreshold);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, currentTexture);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // restore depth test if needed later
    glEnable(GL_DEPTH_TEST);
}

void GL_RenderFBO2() {
    //GL_ApplyGaussianBlur(16);
    if (bloomEnabled) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
        glClear(GL_COLOR_BUFFER_BIT);

        scrShader.use();
        scrShader.setFloat("bloomThreshold", bloomThreshold);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Apply Gaussian blur to the bright parts
        if (fps >= 50.0) {
            GL_ApplyGaussianBlur(6);
        }
        else if (fps <= 40.0) {
            GL_ApplyGaussianBlur(4);
        }
        else if (fps <= 35.0) {
            GL_ApplyGaussianBlur(2);
        }

        else if (fps <= 25.0) {
            GL_ApplyGaussianBlur(1);
        }
    }




    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    // clear all relevant buffers
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);







    postProcessingShader.use();
    postProcessingShader.setBool("chromaticAberrationEnabled", chromaticAberrationEnabled);
    postProcessingShader.setBool("bloomEnabled", bloomEnabled);
    postProcessingShader.setFloat("chromaticStrength", chromaticAberrationStrength);
    postProcessingShader.setFloat("bloomIntensity", bloomIntensity);
    postProcessingShader.setFloat("bloomThreshold", bloomThreshold);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[0]); // Use blurred bloom texture
    
    
    
    //scrShader.use();
    glBindVertexArray(quadVAO);

    //glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[0]);
    //glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Reset texture units
    glActiveTexture(GL_TEXTURE0);
}
void GL_SetTextureFilter(unsigned int texture, Texture_Filter filter)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    // Check for anisotropic filtering extension
    bool hasAnisotropic = GLAD_GL_EXT_texture_filter_anisotropic;

    switch (filter)
    {
    case TEXTURE_FILTER_NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;

    case TEXTURE_FILTER_LINEAR:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    case TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;

    case TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    case TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;

    case TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    case TEXTURE_FILTER_ANISOTROPIC_2X:
    case TEXTURE_FILTER_ANISOTROPIC_4X:
    case TEXTURE_FILTER_ANISOTROPIC_8X:
    case TEXTURE_FILTER_ANISOTROPIC_16X: {
        if (hasAnisotropic) {
            float level = 1.0f;
            switch (filter) {
            case TEXTURE_FILTER_ANISOTROPIC_2X:  level = 2.0f; break;
            case TEXTURE_FILTER_ANISOTROPIC_4X:  level = 4.0f; break;
            case TEXTURE_FILTER_ANISOTROPIC_8X:  level = 8.0f; break;
            case TEXTURE_FILTER_ANISOTROPIC_16X: level = 16.0f; break;
            default: break;
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            GLfloat maxAniso = 0.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
            if (level > maxAniso) level = maxAniso;
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
        }
        else {
            std::cout << "Anisotropic filtering not supported, falling back to trilinear" << std::endl;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        break;
    }

    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    }

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
}

Texture GL_LoadTexture(const char* filePath, Texture_Filter filter)
{
    Texture texture = { 0 };
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    // Set texture wrapping parameters FIRST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set initial filtering to avoid black textures during loading
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true); // Usually you want this TRUE for OpenGL

    texture.data = stbi_load(filePath, &texture.width, &texture.height, &texture.nrChannels, 0);

    if (texture.data)
    {
        GLenum format;
        if (texture.nrChannels == 1)
            format = GL_RED;
        else if (texture.nrChannels == 3)
            format = GL_RGB;
        else if (texture.nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cout << "Unsupported number of channels: " << texture.nrChannels << " in texture: " << filePath << std::endl;
            stbi_image_free(texture.data);
            glDeleteTextures(1, &texture.id);
            return { 0 };
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Now apply the actual filter settings
        GL_SetTextureFilter(texture.id, filter);

        std::cout << "Loaded texture: " << filePath << " (" << texture.width << "x" << texture.height << ", channels: " << texture.nrChannels << ")" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture: " << filePath << std::endl;
        glDeleteTextures(1, &texture.id);
        texture.id = 0;
    }

    stbi_image_free(texture.data);
    return texture;
}

Texture GL_LoadNormalMap(const char* filePath, Texture_Filter filter)
{
    Texture texture = { 0 };
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    // Normal maps should use GL_REPEAT and specific filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set initial filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(false); // Normal maps usually shouldn't be flipped

    texture.data = stbi_load(filePath, &texture.width, &texture.height, &texture.nrChannels, 0);

    if (texture.data) {
        GLenum format = GL_RGB; // Normal maps are typically RGB
        if (texture.nrChannels == 1) format = GL_RED;
        else if (texture.nrChannels == 3) format = GL_RGB;
        else if (texture.nrChannels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Apply final filter settings
        GL_SetTextureFilter(texture.id, filter);

        std::cout << "Loaded normal map: " << filePath << std::endl;
    }
    else {
        std::cout << "Failed to load normal map: " << filePath << std::endl;
        glDeleteTextures(1, &texture.id);
        texture.id = 0;
    }

    stbi_image_free(texture.data);
    return texture;
}

Texture GL_LoadParallaxMap(const char* filePath, Texture_Filter filter)
{
    Texture texture = { 0 };
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set initial filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(false);
    texture.data = stbi_load(filePath, &texture.width, &texture.height, &texture.nrChannels, 0);

    if (texture.data)
    {
        GLenum format;
        if (texture.nrChannels == 1)
            format = GL_RED; // Height maps are often single channel
        else if (texture.nrChannels == 3)
            format = GL_RGB;
        else if (texture.nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cout << "Unsupported channel count in parallax map: " << filePath << std::endl;
            stbi_image_free(texture.data);
            glDeleteTextures(1, &texture.id);
            return { 0 };
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        GL_SetTextureFilter(texture.id, filter);

        std::cout << "Loaded parallax map: " << filePath << std::endl;
    }
    else
    {
        std::cout << "Failed to load parallax (height) map: " << filePath << std::endl;
        glDeleteTextures(1, &texture.id);
        texture.id = 0;
    }

    stbi_image_free(texture.data);
    return texture;
}
void GL_SetTextureFilter2(unsigned int texture, Texture_Filter filter)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    // Check for anisotropic filtering extension
    bool hasAnisotropic = false;
    if (GLAD_GL_EXT_texture_filter_anisotropic) {
        hasAnisotropic = true;
    }


    switch (filter)
    {
    case TEXTURE_FILTER_NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;

    case TEXTURE_FILTER_LINEAR:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    case TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;

    case TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    case TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;

    case TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    case TEXTURE_FILTER_ANISOTROPIC_2X:
    case TEXTURE_FILTER_ANISOTROPIC_4X:
    case TEXTURE_FILTER_ANISOTROPIC_8X:
    case TEXTURE_FILTER_ANISOTROPIC_16X: {
        if (hasAnisotropic) {
            float level = 1.0f;
            switch (filter) {
            case TEXTURE_FILTER_ANISOTROPIC_2X:  level = 2.0f; break;
            case TEXTURE_FILTER_ANISOTROPIC_4X:  level = 4.0f; break;
            case TEXTURE_FILTER_ANISOTROPIC_8X:  level = 8.0f; break;
            case TEXTURE_FILTER_ANISOTROPIC_16X: level = 16.0f; break;
            default: break;
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            if (GLAD_GL_EXT_texture_filter_anisotropic) {
                GLfloat maxAniso = 0.0f;
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
                if (level > maxAniso) level = maxAniso;
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
            }
        }
        else {
            std::cout << "Anisotropic filtering not supported" << std::endl;
        }
        
        break;
    }

    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    }
}
Texture GL_LoadNormalMap2(const char* filePath, Texture_Filter filter)
{
    Texture texture;
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    // Important: Use GL_REPEAT for normal maps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_set_flip_vertically_on_load(false);
    texture.data = stbi_load(filePath, &texture.width, &texture.height, &texture.nrChannels, 0);

    if (texture.data) {
        GLenum format = GL_RGB; // Normal maps are typically RGB
        glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data);
        glGenerateMipmap(GL_TEXTURE_2D);
        GL_SetTextureFilter(texture.id, filter);
    }
    else {
        std::cout << "Failed to load normal map: " << filePath << std::endl;
    }

    stbi_image_free(texture.data);
    return texture;
}
Texture GL_LoadParallaxMap2(const char* filePath, Texture_Filter filter)
{
    Texture texture;
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    // Parallax maps can repeat like other maps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_set_flip_vertically_on_load(false);
    texture.data = stbi_load(filePath, &texture.width, &texture.height, &texture.nrChannels, 0);

    if (texture.data)
    {
        // Height maps can be grayscale (1 channel) or RGB
        GLenum format = GL_RED;
        if (texture.nrChannels == 3)
            format = GL_RGB;
        else if (texture.nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Apply your chosen filtering
        GL_SetTextureFilter(texture.id, filter);
    }
    else
    {
        std::cout << "Failed to load parallax (height) map: " << filePath << std::endl;
    }

    stbi_image_free(texture.data);
    return texture;
}

Texture GL_LoadTexture2(const char* filePath, Texture_Filter filter)
{
    Texture texture = { 0 };
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    stbi_set_flip_vertically_on_load(false);
    texture.data = stbi_load(filePath, &texture.width, &texture.height, &texture.nrChannels, 0);
    if (texture.data)
    {
        GLenum format = (texture.nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data);
        glGenerateMipmap(GL_TEXTURE_2D);
        GL_SetTextureFilter(texture.id, filter);
    }
    else
    {
        std::cout << " Failed to load texture: " << filePath << std::endl;
    }

    stbi_image_free(texture.data);
    texture.data = nullptr; // Prevent dangling pointer
    return texture;
}

void GL_DestroyTexture(Texture tex) {
    glDeleteTextures(1, &tex.id);
    tex.id = 0;
}

int GL_PollQuit() {
    return glfwWindowShouldClose(window);
}
void GL_ProcessInputs() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    fps = 1.0f / deltaTime;

    processInput(window);

    GL_BindFBO();
}
float GL_GetDeltaTime() {
    return (float)deltaTime;
}
int GL_GetFPS() {
    return (int)fps;
}
void GL_ClearDepthColor(float r,float g,float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void GL_ClearDepth() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void GL_ClearDepthStencil() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void GL_StencilMask(int n) {
    glStencilMask(n);
}
void GL_StencilOp_Replace() {
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}
void GL_EnableCullFace(int use) {
    if (use) {
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }
}
void GL_CullFrontFace() {
    glCullFace(GL_FRONT);
}
void GL_CullBackFace() {
    glCullFace(GL_BACK);
}
void GL_CullFontAndBackFace() {
    glCullFace(GL_FRONT_AND_BACK);
}
void GL_FontFaceCCW() {
    glFrontFace(GL_CCW);
}
void GL_FontFaceCW() {
    glFrontFace(GL_CW);
}
void GL_StencilFunction(Stencil_Function type, int index, int n) {
    switch (type) {
    case STENCIL_FUNCTION_KEEP:
        glStencilFunc(GL_KEEP, index, n);
        break;
    case STENCIL_FUNCTION_ZERO:
        glStencilFunc(GL_ZERO, index, n);
        break;
    case STENCIL_FUNCTION_REPLACE:
        glStencilFunc(GL_REPLACE, index, n);
        break;

    case STENCIL_FUNCTION_INCR:
        glStencilFunc(GL_INCR, index, n);
        break;

    case STENCIL_FUNCTION_INCR_WRAP:
        glStencilFunc(GL_INCR_WRAP, index, n);
        break;

    case STENCIL_FUNCTION_DECR:
        glStencilFunc(GL_DECR, index, n);
        break;

    case STENCIL_FUNCTION_DECR_WRAP:
        glStencilFunc(GL_DECR_WRAP, index, n);
        break;


    case STENCIL_FUNCTION_INVERT:
        glStencilFunc(GL_INVERT, index, n);
        break;


    default:
        glStencilFunc(GL_KEEP, index, n);
        break;
    }
}
void GL_EnableDepthMask(int use) {
    if (use) {
        glDepthMask(GL_TRUE);
    }
    else {
        glDepthMask(GL_FALSE);
    }
}
void GL_DepthFunction(Depth_Function type) {
    switch (type) {
    case DEPTH_FUNCTION_ALWAYS:
        glDepthFunc(GL_ALWAYS);
        break;
    case DEPTH_FUNCTION_NEVER:
        glDepthFunc(GL_NEVER);
        break;
    case DEPTH_FUNCTION_LESS:
        glDepthFunc(GL_LESS);
        break;
    case DEPTH_FUNCTION_EQUAL:
        glDepthFunc(GL_EQUAL);
        break;
    case DEPTH_FUNCTION_LEQUAL:
        glDepthFunc(GL_LEQUAL);
        break;
    case DEPTH_FUNCTION_GREATER:
        glDepthFunc(GL_GREATER);
        break;
    case DEPTH_FUNCTION_NOTEQUAL:
        glDepthFunc(GL_NOTEQUAL);
        break;
    case DEPTH_FUNCTION_GEQUAL:
        glDepthFunc(GL_GEQUAL);
        break;
    default:
        glDepthFunc(GL_ALWAYS);
        break;
    }
}
void GL_SwapBuffers() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

// Add these global variables with other global variables
unsigned int textVAO, textVBO;
ShaderCPP textShader;
std::map<std::string, stbtt_bakedchar*> fontCharData; // Store char data per font

#include "model_animation.h"
#include "animator.h"


static std::vector<ModelCPP> models;
static std::vector<Animator> animators;

// Add these global variables
struct ModelAnimationPair {
    int modelID;
    int animatorID;
    std::string modelPath;
    std::string animationPath;
};

static std::vector<ModelAnimationPair> modelAnimationPairs;


void GL_Quit() {
    glfwDestroyCursor(cursor);
    // Clean up all allocated resources
    glDeleteVertexArrays(1, &CubeVAO);
    glDeleteBuffers(1, &CubeVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);

    glDeleteTextures(1, &depthTexture);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteTextures(1, &cubemapTexture);

    glDeleteFramebuffers(1, &framebuffer);
    glDeleteFramebuffers(2, pingpongFBO);
    glDeleteTextures(2, pingpongColorbuffers);

    // Clean up fonts
    for (auto& pair : fontCharData) {
        free(pair.second);
    }
    fontCharData.clear();

    // Clean up models
    models.clear();
    animators.clear();
    modelAnimationPairs.clear();

    glfwTerminate();
}
// Add validation for VAOs and VBOs
void GL_ValidateBuffers() {
    GLint vao, vbo;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);

    if (vao == 0) {
        //std::cout << "WARNING: No VAO bound!" << std::endl;
    }
    if (vbo == 0) {
        //std::cout << "WARNING: No VBO bound!" << std::endl;
    }
}
void GL_DrawCube(Vector3 pos, Vector3 rot, Vector3 size, Texture tex)
{
    camShader.use();
    camShader.setInt("texture1", 0);

    if (CubeVAO == 0) {
        //std::cout << "ERROR: Cube VAO not initialized!" << std::endl;
        return;
    }

    GL_ValidateBuffers();


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glBindVertexArray(CubeVAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
    model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(size.x, size.y, size.z));
    camShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
void GL_DrawCubeWithNormalMap(Vector3 pos, Vector3 rot, Vector3 size, Texture tex, Texture normalMapTex)
{
    camShader.use();

    // Enable normal mapping BEFORE setting uniforms
    camShader.setBool("normalMappingEnabled", true);

    // Bind diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    camShader.setInt("texture1", 0);

    // Bind normal map texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMapTex.id);
    camShader.setInt("normalMap", 1);

    // Update light position for normal mapping (use first light)
    if (lights.size() > 0) {
        camShader.setVec3("lightPos", lights[0].position.x, lights[0].position.y, lights[0].position.z);
    }

    glBindVertexArray(CubeVAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
    model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(size.x, size.y, size.z));
    camShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // IMPORTANT: Disable normal mapping after drawing
    camShader.setBool("normalMappingEnabled", false);

    // Reset texture units
    glActiveTexture(GL_TEXTURE0);
}
void GL_DrawCubeWithParllaxMap(Vector3 pos, Vector3 rot, Vector3 size, Texture tex, Texture normalMapTex, Texture parllaxMapTex,float heightScale)
{
    camShader.use();

    // Enable normal mapping BEFORE setting uniforms
    camShader.setBool("normalMappingEnabled", true);

    camShader.setBool("parallaxMappingEnabled", true);
    camShader.setFloat("heightScale", heightScale);

    // Bind diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    camShader.setInt("texture1", 0);

    // Bind normal map texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMapTex.id);
    camShader.setInt("normalMap", 1);


    // Bind parllax map texture
    camShader.setInt("heightMap", 3); // e.g. texture unit 3
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, parllaxMapTex.id);

    // Update light position for normal mapping (use first light)
    if (lights.size() > 0) {
        camShader.setVec3("lightPos", lights[0].position.x, lights[0].position.y, lights[0].position.z);
    }

    glBindVertexArray(CubeVAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
    model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(size.x, size.y, size.z));
    camShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // IMPORTANT: Disable normal mapping after drawing
    camShader.setBool("normalMappingEnabled", false);

    // Reset texture units
    glActiveTexture(GL_TEXTURE0);
}




void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (GL_KeyPressed(GL_KEY_F11)) {
        GL_ToggleFullscreen();
    }

    if (GL_KeyPressed(GL_KEY_F10)) {
        GL_ToggleBorderlessWindowed();
    }
    

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);


    if (camera.Type == 2) {
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
            camera.ProcessZoomKeyboard(false, deltaTime);
        }
        else if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
            camera.ProcessZoomKeyboard(true, deltaTime);
        }
    }




}






//GLFW Implement
#include <unordered_map>


void GL_SetWindowTitle(const char* title) {
    glfwSetWindowTitle(window, title);
}
void GL_SetWindowPosition(Vector2 pos) {
    glfwSetWindowPos(window, (int)pos.x, (int)pos.y);
}
void GL_SetWindowSize(Vector2 size) {
    glfwSetWindowSize(window, (int)size.x, (int)size.y);
}
const char* GL_GetClipboardString() {
    return glfwGetClipboardString(window);
}
void GL_SetClipboardString(const char* stringText) {
    glfwSetClipboardString(window, stringText);
}
const char* GL_GetPlatform() {
    int platform = glfwGetPlatform();
    switch (platform) {
    case GLFW_PLATFORM_WIN32:
        return "Windows";
        break;
    case GLFW_PLATFORM_X11:
        return "LinuxX11";
        break;
    case GLFW_PLATFORM_WAYLAND:
        return "LinuxWayLand";
        break;
    case GLFW_PLATFORM_COCOA:
        return "MacOS";
        break;
    case GLFW_PLATFORM_NULL:
        return "NULL";
        break;
    default:
        return "Unknown Platform";
        break;
    }
}
const char* GL_GetMonitorName() {
    return glfwGetMonitorName(monitor);
}
void GL_SetWindowIcon(const char* path)
{
    if (!path) {
        printf("No icon path provided.\n");
        return;
    }

    // Check file extension — only .png or .ico allowed
    const char* ext = strrchr(path, '.');
    if (!ext || (strcmp(ext, ".png") != 0 && strcmp(ext, ".ico") != 0)) {
        printf("Unsupported icon format: %s (only .png or .ico allowed)\n", path);
        return;
    }

    int width, height, channels;
    unsigned char* pixels = stbi_load(path, &width, &height, &channels, 4);
    if (!pixels) {
        printf("Failed to load icon: %s\n", path);
        return;
    }

    GLFWimage images[1];
    images[0].width = width;
    images[0].height = height;
    images[0].pixels = pixels;

    glfwSetWindowIcon(window, 1, images);
#if ENABLE_DEBUG
    printf("Icon set successfully from: %s (%dx%d)\n", path, width, height);
#endif

    stbi_image_free(pixels);
}
void GL_SetCursorType(CursorType type) {
    switch (type) {
    case CURSOR_ARROW_CURSOR:
        cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        glfwSetCursor(window, cursor);
        break;
    case CURSOR_IBEAM_CURSOR:
        cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        glfwSetCursor(window, cursor);
        break;
    case CURSOR_CROSSHAIR_CURSOR:
        cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
        glfwSetCursor(window, cursor);
        break;
    case CURSOR_HAND_CURSOR:
        cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        glfwSetCursor(window, cursor);
        break;
    case CURSOR_HRESIZE_CURSOR:
        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        glfwSetCursor(window, cursor);
        break;
    case CURSOR_VRESIZE_CURSOR:
        cursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
        glfwSetCursor(window, cursor);
        break;
    }
}
void GL_SetCustomCursor(const char* filename, int hotspotX, int hotspotY) {
    int width, height, channels;
    unsigned char* pixels = stbi_load(filename, &width, &height, &channels, 4);
    if (!pixels) {
        fprintf(stderr, "Failed to load cursor image: %s\n", filename);
        return;
    }

    GLFWimage image;
    image.width = width;
    image.height = height;
    image.pixels = pixels;

    cursor = glfwCreateCursor(&image, hotspotX, hotspotY);

    stbi_image_free(pixels);

    glfwSetCursor(window, cursor);
}
void GL_RestoreCursorToDefault() {
    glfwSetCursor(window, NULL);
}
void GL_SetWindowOpacity(float opacity) {
    glfwSetWindowOpacity(window,opacity);
}
int GL_GetMonitorWidth() {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    return mode->width;
}
int GL_GetMonitorHeight() {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
#if ENABLE_DEBUG
    printf("Monitor resolution: %d x %d @ %d Hz\n",
        mode->width, mode->height, mode->refreshRate);
#endif
    return mode->height;
}
int GL_GetWindowWidth() {
    return SCR_WIDTH;
}
int GL_GetWindowHeight() {
    return SCR_HEIGHT;
}
int GL_GetMonitorRefreshRate() {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    return mode->refreshRate;
}
int GL_GetPhysicalMonitorWidth() {
    int widthMM, heightMM;
    glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
    return widthMM;
}
int GL_GetPhysicalMonitorHeight() {
    int widthMM, heightMM;
    glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
    return heightMM;
}
void GL_HideWindow() {
    glfwHideWindow(window);
}
void GL_ShowWindow() {
    glfwShowWindow(window);
}
int GL_IsWindowVisible()
{
    return glfwGetWindowAttrib(window, GLFW_VISIBLE);
}
int GL_IsWindowDragging() {
    if (!leftPressed) {
        isDragging = false;
        return false;
    }

    int x, y;
    glfwGetWindowPos(window, &x, &y);

    // Detect change in window position while mouse is pressed
    if (x != lastWX || y != lastWY) {
        isDragging = true;
    }
    else if (!leftPressed) {
        isDragging = false;
    }

    lastWX = x;
    lastWY = y;
    return isDragging;
}
void GL_ToggleFullscreen()
{
    static bool isFullscreen = false;
    static int windowX, windowY;
    static int windowWidth, windowHeight;

    if (!window)
        return;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (!monitor || !mode)
    {
        printf("No monitor & mode found!\n");
        return;
    }

    if (!isFullscreen)
    {
        // Save window position and size
        glfwGetWindowPos(window, &windowX, &windowY);
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        // Go fullscreen
        glfwSetWindowMonitor(window, monitor, 0, 0,
            mode->width, mode->height, mode->refreshRate);
        isFullscreen = true;
#if ENABLE_DEBUG
        printf("Fullscreen ON (%dx%d)\n", mode->width, mode->height);
#endif
    }
    else
    {
        // Restore to windowed
        glfwSetWindowMonitor(window, NULL,
            windowX, windowY,
            windowWidth, windowHeight,
            0);
        isFullscreen = false;
#if ENABLE_DEBUG
        printf("Windowed mode (%dx%d)\n", windowWidth, windowHeight);
#endif
    }
}
void GL_ToggleBorderlessWindowed()
{
    static bool isBorderless = false;
    static int windowX, windowY;
    static int windowWidth, windowHeight;

    if (!window)
        return;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (!monitor || !mode)
    {
        printf("No monitor & mode detected!\n");
        return;
    }

    if (!isBorderless)
    {
        // Save window position and size before switching
        glfwGetWindowPos(window, &windowX, &windowY);
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        // Make it borderless (no decorations) and maximized to monitor size
        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
        glfwSetWindowMonitor(window, NULL, 0, 0, mode->width, mode->height, 0);
        isBorderless = true;
#if ENABLE_DEBUG
        printf(" Borderless windowed ON (%dx%d)\n", mode->width, mode->height);
#endif
    }
    else
    {
        // Restore original size and decorations
        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
        glfwSetWindowMonitor(window, NULL, windowX, windowY, windowWidth, windowHeight, 0);
        isBorderless = false;
#if ENABLE_DEBUG
        printf(" Borderless windowed OFF (%dx%d)\n", windowWidth, windowHeight);
#endif
    }
}
void GL_SetCenterWindowPos()
{
    if (!window) return;
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) return;

    // Get window size
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Compute centered position
    int xpos = (mode->width - windowWidth) / 2;
    int ypos = (mode->height - windowHeight) / 2;

    // Set window position
    glfwSetWindowPos(window, xpos, ypos);
}
void GL_ShowCursor(void)
{
    if (!window) return;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    g_CursorHidden = false;
    g_CursorDisabled = false;
}

void GL_HideCursor(void)
{
    if (!window) return;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    g_CursorHidden = true;
    g_CursorDisabled = false;
}

int GL_IsCursorHidden(void)
{
    return g_CursorHidden;
}
void GL_EnableCursor(void)
{
    if (!window) return;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    g_CursorDisabled = false;
    g_CursorHidden = false;
}

void GL_DisableCursor(void)
{
    if (!window) return;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    g_CursorDisabled = true;
    g_CursorHidden = false;
}
int GL_IsCursorOnScreen(void)
{
    if (!window) return false;

    double x, y;
    int width, height;
    glfwGetCursorPos(window, &x, &y);
    glfwGetWindowSize(window, &width, &height);

    return (x >= 0 && y >= 0 && x < width && y < height);
}
void GL_Wait(double seconds)
{
    double startTime = glfwGetTime();
    while ((glfwGetTime() - startTime) < seconds)
    {
        // You can optionally call this to avoid high CPU usage
        glfwWaitEventsTimeout(0.001); // Sleep for ~1ms while waiting
    }
}
int GL_KeyPressed(int key)
{
        static std::unordered_map<int, int> prevState;

        int state = glfwGetKey(window, key);
        bool pressed = (state == GLFW_PRESS && prevState[key] != GLFW_PRESS);

        prevState[key] = state;
        return (int)pressed;
}
// Detects if a key is being held down (returns true every frame while pressed)
int GL_KeyHold(int key)
{
        int state = glfwGetKey(window, key);
        return (state == GLFW_PRESS);
}
// Detects when a key is released (only returns true the first frame it's released)
int GL_KeyReleased(int key)
{
        static std::unordered_map<int, int> prevState;

        int state = glfwGetKey(window, key);
        bool released = (state == GLFW_RELEASE && prevState[key] == GLFW_PRESS);

        prevState[key] = state;
        return released;
}
int GL_KeyRelease(int key) {
        if (glfwGetKey(window, key) == GLFW_RELEASE) {
            return true;
        }
        return false;
}
int GL_KeyRepeat(int key) {
        if (glfwGetKey(window, key) == GLFW_REPEAT) {
            return true;
        }
        return false;
}
int GL_KeyDown(int key)
{
        if (glfwGetKey(window, key) == GLFW_PRESS)
        {
            return true;
        }
        return false;
}
int GL_MouseLeftDown() {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        return true;
    }
    return false;
}
int GL_MouseRightDown() {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        return true;
    }
    return false;
}
int GL_MouseMiddleDown() {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        return true;
    }
    return false;
}
int GetMouseX(void)
{
    if (!window) return 0;
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return (int)x;
}
int GetMouseY(void)
{
    if (!window) return 0;
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return (int)y;
}

float xoffset;
float yoffset;
double scrollX;
double scrollY;



int GL_GetMouseDeltaX(void)
{
    return (int)xoffset;
}
int GL_GetMouseDeltaY(void)
{
    return (int)yoffset;
}

float GL_GetMouseWheelMoveX(void) {
    return (float)scrollX;
}

float GL_GetMouseWheelMoveY(void) {
    return (float)scrollY;
}







// Our helper function
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Get actual framebuffer size (pixel size, not logical)
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    // Safety: avoid zero or negative sizes
    if (fbWidth <= 0)  fbWidth = 1;
    if (fbHeight <= 0) fbHeight = 1;

    // Update global screen size
    SCR_WIDTH = fbWidth;
    SCR_HEIGHT = fbHeight;

    lastX = SCR_WIDTH / 2.0f;
    lastY = SCR_HEIGHT / 2.0f;

#if ENABLE_DEBUG
    std::cout << "Framebuffer resized to: " << SCR_WIDTH << "x" << SCR_HEIGHT << std::endl;
#endif


    // --- Update OpenGL viewport ---
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // --- Resize main scene color buffer ---
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

    // --- Resize depth buffer ---
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    // --- Resize bloom ping-pong colorbuffers ---
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    }

    // --- Resize post-processing framebuffer (FXAA / sharpness) ---
    glBindTexture(GL_TEXTURE_2D, postColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

    // --- Unbind framebuffer ---
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // --- Update FXAA shader uniform ---
    fxaaShader.use();
    fxaaShader.setVec2("screenSize", glm::vec2((float)SCR_WIDTH, (float)SCR_HEIGHT));
}

// Add this global variable with your other mouse variables


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Only process mouse movement if mouse is locked
    if (lockedMouse)
    {
        xoffset = xpos - lastX;
        yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    else
    {
        // Update last positions without processing movement
        lastX = xpos;
        lastY = ypos;

        // Optional: Reset offsets when not locked
        xoffset = 0.0f;
        yoffset = 0.0f;
    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scrollX = xoffset;
    scrollY = yoffset;
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


static void GL_InitCube() {
    glGenVertexArrays(1, &CubeVAO);
    glGenBuffers(1, &CubeVBO);

    glBindVertexArray(CubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

    // position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // tangent attribute (location = 3)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // bitangent attribute (location = 4)
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
































//Skybox
void GL_InitSkybox() {
    // Initialize skybox shader
    skyboxShader.InitShaderFromFiles("shaders/skybox.vs", "shaders/skybox.fs");

    // Set up skybox VAO/VBO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#if ENABLE_DEBUG
    std::cout << "Skybox initialized successfully" << std::endl;
#endif
}
unsigned int GL_LoadCubemap(std::vector<std::string> faces);
// In the extern "C" section, add:
int GL_LoadSkyboxTextures(const char* right, const char* left, const char* top,
    const char* bottom, const char* front, const char* back) {
    std::vector<std::string> faces;

    // Check if files exist before adding
    if (right) faces.push_back(right);
    if (left) faces.push_back(left);
    if (top) faces.push_back(top);
    if (bottom) faces.push_back(bottom);
    if (front) faces.push_back(front);
    if (back) faces.push_back(back);

    if (faces.size() != 6) {
        std::cout << "Error: Need exactly 6 texture paths for cubemap" << std::endl;
        return 0;
    }

    cubemapTexture = GL_LoadCubemap(faces);
    if (cubemapTexture == 0) {
        std::cout << "Failed to load cubemap textures!" << std::endl;
        return 0;
    }
#if ENABLE_DEBUG
    std::cout << "Cubemap loaded successfully with texture ID: " << cubemapTexture << std::endl;
#endif
    return 1;
}
unsigned int GL_LoadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // Important: Don't flip cubemap textures

    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
            return 0;
        }
    }

    // Set proper texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Generate mipmaps if needed
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return textureID;
}
void GL_DrawSkybox() {
    // Change depth function so depth test passes when values are equal to depth buffer's content
    glDepthFunc(GL_LEQUAL);

    skyboxShader.use();

    // Remove translation from the view matrix - FIXED
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Remove translation
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);

    // Bind cubemap texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    skyboxShader.setInt("skybox", 0);

    // Draw skybox
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    skyboxShader.unuse();



    // Set depth function back to default
    glDepthFunc(GL_LESS);
}
void GL_RenderSkybox() {
    GL_DrawSkybox();
}









//FBO System
void GL_InitQuad() {
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


















void GL_EnableChromaticAberration(int enable) {
    chromaticAberrationEnabled = (enable != 0);
}

void GL_SetChromaticAberrationStrength(float strength) {
    chromaticAberrationStrength = strength;
}

void GL_EnableBloom(int enable) {
    bloomEnabled = (enable != 0);
}

void GL_SetBloomThreshold(float threshold) {
    bloomThreshold = threshold;
}

void GL_SetBloomIntensity(float intensity) {
    bloomIntensity = intensity;
}














































































































































// Line drawing function
void GL_DrawLine3D(Vector3 start, Vector3 end, Vector3 color, float thickness) {
    // Simple line drawing using immediate mode-like approach
    float lineVertices[] = {
        start.x, start.y, start.z,
        end.x, end.y, end.z
    };

    unsigned int lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Use a simple shader for lines
    

    lineShader.use();
    lineShader.setMat4("projection", projection);
    lineShader.setMat4("view", view);
    lineShader.setVec3("lineColor", color.x, color.y, color.z);

    glLineWidth(thickness);
    glDrawArrays(GL_LINES, 0, 2);

    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
    lineShader.unuse();

}

// Pixel drawing function (point in 3D space)
void GL_DrawPixel3D(Vector3 position, Vector3 color, float size) {
    float pointVertices[] = { position.x, position.y, position.z };

    unsigned int pointVAO, pointVBO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);

    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertices), pointVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

   
    pointShader.use();
    pointShader.setMat4("projection", projection);
    pointShader.setMat4("view", view);
    pointShader.setVec3("pointColor", color.x, color.y, color.z);

    glPointSize(size);
    glDrawArrays(GL_POINTS, 0, 1);

    glDeleteVertexArrays(1, &pointVAO);
    glDeleteBuffers(1, &pointVBO);
    pointShader.unuse();
}

// Wireframe cube drawing function
void GL_DrawCubeLine(Vector3 pos, Vector3 rot, Vector3 size, Vector3 color, float thickness) {
    // Define the 8 vertices of the cube
    Vector3 vertices[8] = {
        { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f },
        { 0.5f,  0.5f, -0.5f }, { -0.5f,  0.5f, -0.5f },
        { -0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f },
        { 0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f }
    };

    // Apply transformations
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
    model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(size.x, size.y, size.z));

    // Transform vertices
    Vector3 transformed[8];
    for (int i = 0; i < 8; i++) {
        glm::vec4 result = model * glm::vec4(vertices[i].x, vertices[i].y, vertices[i].z, 1.0f);
        transformed[i] = { result.x, result.y, result.z };
    }

    // Define the 12 lines of the cube
    int lines[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0}, // bottom face
        {4,5}, {5,6}, {6,7}, {7,4}, // top face  
        {0,4}, {1,5}, {2,6}, {3,7}  // connecting lines
    };

    // Draw each line
    for (int i = 0; i < 12; i++) {
        GL_DrawLine3D(transformed[lines[i][0]], transformed[lines[i][1]], color, thickness);
    }
}















// Add this function implementation in LibGL.cpp

void GL_DrawBillboard(Texture texture, Vector3 position, Vector2 size, float alpha, BillboardType type) {
    GL_DrawBillboardEx(texture, position, size, { 1.0f, 1.0f, 1.0f }, alpha, type);
}

void GL_DrawBillboardEx(Texture texture, Vector3 position, Vector2 size, Vector3 color, float alpha, BillboardType type) {
    // Use the texturein3d shader
    texturein3dShader.use();

    // Calculate billboard rotation based on type
    Vector3 rotation = { 0.0f, 0.0f, 0.0f };
    Vector3 scale = { size.x, size.y, 1.0f };

    if (type == BILLBOARD_FACE_CAMERA) {
        // Extract rotation from view matrix to make quad face camera
        glm::mat4 viewMatrix = camera.GetViewMatrix();

        // Remove translation to get just rotation
        glm::mat4 rotationMatrix = glm::mat4(glm::mat3(viewMatrix));

        // Convert to rotation angles (simplified approach)
        // For proper billboarding, we'd use the rotation matrix directly
        // This is a simplified version that works well for most cases
        glm::vec3 front = glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
        glm::vec3 up = glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
        glm::vec3 right = glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);

        // Calculate Y rotation (yaw)
        rotation.y = atan2f(front.x, front.z) * (180.0f / 3.14159f);

        // For full camera-facing, also calculate pitch
        rotation.x = asinf(-front.y) * (180.0f / 3.14159f);

    }
    else if (type == BILLBOARD_FACE_CAMERA_Y) {
        // Only rotate around Y axis to face camera (maintain vertical orientation)
        glm::vec3 cameraPos = { camera.Position.x, camera.Position.y, camera.Position.z };
        glm::vec3 billboardPos = { position.x, position.y, position.z };

        // Calculate direction from billboard to camera
        glm::vec3 direction = cameraPos - billboardPos;

        // Only use the XZ plane for Y rotation
        rotation.y = atan2f(direction.x, direction.z) * (180.0f / 3.14159f);

    }
    else if (type == BILLBOARD_AXIAL_X) {
        // Custom rotation around X axis
        rotation.x = 90.0f; // Example: face upward

    }
    else if (type == BILLBOARD_AXIAL_Y) {
        // Face along Y axis (already the default)
        rotation.y = 0.0f;

    }
    else if (type == BILLBOARD_AXIAL_Z) {
        // Custom rotation around Z axis  
        rotation.z = 45.0f; // Example: 45 degree tilt
    }

    // Create the textured quad if it doesn't exist
    static TextureQuad3D billboardQuad = { 0 };
    if (billboardQuad.vao == 0) {
        billboardQuad = CreateTexturedQuad3D();
    }

    // Set up transformation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

    // Set shader uniforms
    texturein3dShader.setMat4("model", model);
    texturein3dShader.setMat4("view", view);
    texturein3dShader.setMat4("projection", projection);
    texturein3dShader.setVec3("color", color.x, color.y, color.z);
    texturein3dShader.setFloat("alpha", alpha);

    // Enable blending and disable depth test for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // For billboards that should always be visible, you might want to disable depth test
    // glDisable(GL_DEPTH_TEST);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    texturein3dShader.setInt("tex", 0);

    // Draw the quad
    glBindVertexArray(billboardQuad.vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Restore state
    glDisable(GL_BLEND);
    // glEnable(GL_DEPTH_TEST); // If you disabled it

    texturein3dShader.unuse();
}







float clampf(float value, float minVal, float maxVal)
{
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}
// Add this function to initialize text rendering
void GL_InitTextRendering() {
    // Configure text VAO/VBO
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Initialize text shader
    textShader.InitShaderFromFiles("shaders/text.vs", "shaders/text.fs");
    textShader.use();
    textShader.setInt("text", 0);

#if ENABLE_DEBUG
    std::cout << "Text rendering initialized successfully" << std::endl;
#endif

    textShader.unuse();
}

// Add this function to load fonts
Font GL_LoadFont(const char* fontPath, int fontSize) {
    Font font;
    font.fontSize = fontSize;
    font.textureID = 0;
    font.fontPath = strdup(fontPath); // Store the path for identification

#if ENABLE_DEBUG
    std::cout << "Loading font: " << fontPath << " size: " << fontSize << std::endl;
#endif
    // Read font file
    FILE* fontFile = fopen(fontPath, "rb");
    if (!fontFile) {
        std::cout << "Failed to open font file: " << fontPath << std::endl;
        return font;
    }

    fseek(fontFile, 0, SEEK_END);
    long size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    if (size <= 0) {
        std::cout << "Font file is empty or invalid: " << fontPath << std::endl;
        fclose(fontFile);
        return font;
    }

    unsigned char* fontBuffer = (unsigned char*)malloc(size);
    if (!fontBuffer) {
        std::cout << "Failed to allocate memory for font buffer" << std::endl;
        fclose(fontFile);
        return font;
    }

    size_t read = fread(fontBuffer, 1, size, fontFile);
    fclose(fontFile);

    if (read != size) {
        std::cout << "Failed to read font file completely" << std::endl;
        free(fontBuffer);
        return font;
    }

    // Create bitmap for font
    int bitmapWidth = 512;
    int bitmapHeight = 512;
    unsigned char* bitmap = (unsigned char*)malloc(bitmapWidth * bitmapHeight);
    if (!bitmap) {
        std::cout << "Failed to allocate memory for font bitmap" << std::endl;
        free(fontBuffer);
        return font;
    }

    // Clear bitmap to black (0)
    memset(bitmap, 0, bitmapWidth * bitmapHeight);

    // Allocate character data
    stbtt_bakedchar* charData = (stbtt_bakedchar*)malloc(96 * sizeof(stbtt_bakedchar));
    if (!charData) {
        std::cout << "Failed to allocate memory for character data" << std::endl;
        free(bitmap);
        free(fontBuffer);
        return font;
    }

    // Bake font into bitmap
    int result = stbtt_BakeFontBitmap(fontBuffer, 0, fontSize, bitmap, bitmapWidth, bitmapHeight, 32, 96, charData);

    std::cout << "Font baking result: " << result << " (positive is good)" << std::endl;

    if (result <= 0) {
        std::cout << "Failed to bake font bitmap - no pixels generated" << std::endl;
        free(bitmap);
        free(fontBuffer);
        free(charData);
        return font;
    }

    // Create OpenGL texture
    glGenTextures(1, &font.textureID);
    glBindTexture(GL_TEXTURE_2D, font.textureID);

    // Use GL_RED format and upload bitmap
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmapWidth, bitmapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    font.width = bitmapWidth;
    font.height = bitmapHeight;

    // Store char data with font path as key
    std::string key = std::string(fontPath) + "_" + std::to_string(fontSize);
    fontCharData[key] = charData;

    free(bitmap);
    free(fontBuffer);

#if ENABLE_DEBUG
    std::cout << "Font loaded successfully. Texture ID: " << font.textureID << std::endl;
#endif
    // Debug: print first few characters
#if ENABLE_DEBUG

    for (int i = 0; i < 3; i++) {
        std::cout << "Char " << (char)(i + 32) << " - x0:" << charData[i].x0
            << " y0:" << charData[i].y0 << " x1:" << charData[i].x1
            << " y1:" << charData[i].y1 << " xadvance:" << charData[i].xadvance << std::endl;
    }
#endif

    return font;
}

// Add this function to render 2D text
void GL_DrawText(Font font, const char* text, Vector2 position, float scale, Vector3 color) {
    if (font.textureID == 0) {
        std::cout << "Font texture not loaded!" << std::endl;
        return;
    }

    // Enable blending for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST); // Disable depth test for 2D text

    // Activate corresponding render state
    textShader.use();
    textShader.setVec3("textColor", color.x, color.y, color.z);

    // Create orthographic projection matrix for 2D text
    glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f); // Flip Y
    textShader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.textureID);
    glBindVertexArray(textVAO);

    float x = position.x;
    float y = position.y;

    // Get character data for this font (you'll need to store this properly)
    // For now, we'll use a simple approach - you need to modify this based on how you store char data
    stbtt_bakedchar* charData = nullptr;
    for (auto& pair : fontCharData) {
        charData = pair.second;
        break; // Get first available char data (you need a better way to associate with font)
    }

    if (!charData) {
        std::cout << "Character data not found for font!" << std::endl;
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        return;
    }

    // Iterate through all characters
    for (const char* c = text; *c; c++) {
        if (*c < 32 || *c > 126) continue; // Only ASCII 32..126 is supported

        stbtt_bakedchar* charInfo = &charData[*c - 32];

        // Calculate position and size
        float xpos = x + charInfo->xoff * scale;
        float ypos = y + charInfo->yoff * scale;

        float w = (charInfo->x1 - charInfo->x0) * scale;
        float h = (charInfo->y1 - charInfo->y0) * scale;

        // Calculate texture coordinates (normalized)
        float texX0 = charInfo->x0 / (float)font.width;
        float texX1 = charInfo->x1 / (float)font.width;
        float texY0 = charInfo->y0 / (float)font.height;
        float texY1 = charInfo->y1 / (float)font.height;

        // Update VBO for each character
        float vertices[6][4] = {
            // Position        // Texture Coords
            { xpos,     ypos + h,   texX0, texY1 },
            { xpos,     ypos,       texX0, texY0 },
            { xpos + w, ypos,       texX1, texY0 },

            { xpos,     ypos + h,   texX0, texY1 },
            { xpos + w, ypos,       texX1, texY0 },
            { xpos + w, ypos + h,   texX1, texY1 }
        };

        // Render glyph texture over quad
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Now advance cursors for next glyph (note: advance is divided by 10 in stb_truetype)
        x += charInfo->xadvance * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    textShader.unuse();

    // Restore state
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
void GL_DrawTextWithGradientH(Font font, const char* text, Vector2 position, float scale, Color4 c1, Color4 c2)
{
    if (font.textureID == 0) {
        std::cout << "Font texture not loaded!" << std::endl;
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    textShader.use();

    // Create orthographic projection for 2D
    glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f);
    textShader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.textureID);
    glBindVertexArray(textVAO);

    // Prepare baked font data (same as your GL_DrawText)
    stbtt_bakedchar* charData = nullptr;
    for (auto& pair : fontCharData) {
        charData = pair.second;
        break;
    }

    if (!charData) {
        std::cout << "Character data not found for font!" << std::endl;
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        return;
    }

    // Compute total text width to map gradient horizontally
    float totalWidth = 0.0f;
    for (const char* c = text; *c; ++c) {
        if (*c < 32 || *c > 126) continue;
        stbtt_bakedchar* ch = &charData[*c - 32];
        totalWidth += ch->xadvance * scale;
    }

    float x = position.x;
    float y = position.y;

    // Render each character with gradient interpolation
    float accumulatedX = 0.0f;

    for (const char* c = text; *c; ++c)
    {
        if (*c < 32 || *c > 126) continue;

        stbtt_bakedchar* ch = &charData[*c - 32];

        float xpos = x + ch->xoff * scale;
        float ypos = y + ch->yoff * scale;
        float w = (ch->x1 - ch->x0) * scale;
        float h = (ch->y1 - ch->y0) * scale;

        // Normalized texture coordinates
        float texX0 = ch->x0 / (float)font.width;
        float texX1 = ch->x1 / (float)font.width;
        float texY0 = ch->y0 / (float)font.height;
        float texY1 = ch->y1 / (float)font.height;

        // Compute gradient color ratio (left edge of glyph)
        float t = (accumulatedX + w * 0.5f) / totalWidth;
        t = clampf(t, 0.0f, 1.0f);

        // Interpolate between c1 and c2 (convert to [0–1])
        float r1 = c1.r / 255.0f, g1 = c1.g / 255.0f, b1 = c1.b / 255.0f;
        float r2 = c2.r / 255.0f, g2 = c2.g / 255.0f, b2 = c2.b / 255.0f;

        float r = r1 + (r2 - r1) * t;
        float g = g1 + (g2 - g1) * t;
        float b = b1 + (b2 - b1) * t;

        // Send color to shader
        textShader.setVec3("textColor", r, g, b);

        // Update vertex data for this glyph
        float vertices[6][4] = {
            { xpos,     ypos + h,   texX0, texY1 },
            { xpos,     ypos,       texX0, texY0 },
            { xpos + w, ypos,       texX1, texY0 },

            { xpos,     ypos + h,   texX0, texY1 },
            { xpos + w, ypos,       texX1, texY0 },
            { xpos + w, ypos + h,   texX1, texY1 }
        };

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        accumulatedX += ch->xadvance * scale;
        x += ch->xadvance * scale;
    }

    textShader.unuse();

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}




void GL_DrawTextWithGradientV(Font font, const char* text, Vector2 position, float scale, Color4 c1, Color4 c2)
{
    if (font.textureID == 0) {
        std::cout << "Font texture not loaded!" << std::endl;
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    textShader.use();

    // Orthographic projection for 2D
    glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f);
    textShader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.textureID);
    glBindVertexArray(textVAO);

    stbtt_bakedchar* charData = nullptr;
    for (auto& pair : fontCharData) {
        charData = pair.second;
        break;
    }

    if (!charData) {
        std::cout << "Character data not found for font!" << std::endl;
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        return;
    }

    // -----------------------------------------------------
    // Compute text height range (to normalize gradient)
    // -----------------------------------------------------
    float minY = FLT_MAX;
    float maxY = -FLT_MAX;
    float totalWidth = 0.5f;

    for (const char* c = text; *c; ++c) {
        if (*c < 32 || *c > 126) continue;
        stbtt_bakedchar* ch = &charData[*c - 32];

        float h = (ch->y1 - ch->y0) * scale;
        float yTop = position.y + ch->yoff * scale;
        float yBottom = yTop + h;

        if (yTop < minY) minY = yTop;
        if (yBottom > maxY) maxY = yBottom;

        totalWidth += ch->xadvance * scale;
    }

    float gradientHeight = maxY - minY;
    if (gradientHeight < 1.0f) gradientHeight = 1.0f; // avoid div by zero

    // Convert color1, color2 to [0–1]
    float r1 = c1.r / 255.0f, g1 = c1.g / 255.0f, b1 = c1.b / 255.0f;
    float r2 = c2.r / 255.0f, g2 = c2.g / 255.0f, b2 = c2.b / 255.0f;

    float x = position.x;
    float y = position.y;

    // -----------------------------------------------------
    // Render each character
    // -----------------------------------------------------
    for (const char* c = text; *c; ++c)
    {
        if (*c < 32 || *c > 126) continue;

        stbtt_bakedchar* ch = &charData[*c - 32];

        float xpos = x + ch->xoff * scale;
        float ypos = y + ch->yoff * scale;
        float w = (ch->x1 - ch->x0) * scale;
        float h = (ch->y1 - ch->y0) * scale;

        float texX0 = ch->x0 / (float)font.width;
        float texX1 = ch->x1 / (float)font.width;
        float texY0 = ch->y0 / (float)font.height;
        float texY1 = ch->y1 / (float)font.height;

        // --------------------------------------------
        // Vertical gradient based on Y position
        // --------------------------------------------
        float glyphMidY = ypos + h * 0.5f;
        float t = (glyphMidY - minY) / gradientHeight;  // 0 = top, 1 = bottom
        t = clampf(t, 0.0f, 1.0f);

        float r = r1 + (r2 - r1) * t;
        float g = g1 + (g2 - g1) * t;
        float b = b1 + (b2 - b1) * t;

        textShader.setVec3("textColor", r, g, b);

        // Update vertex data for this glyph
        float vertices[6][4] = {
            { xpos,     ypos + h,   texX0, texY1 },
            { xpos,     ypos,       texX0, texY0 },
            { xpos + w, ypos,       texX1, texY0 },

            { xpos,     ypos + h,   texX0, texY1 },
            { xpos + w, ypos,       texX1, texY0 },
            { xpos + w, ypos + h,   texX1, texY1 }
        };

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += ch->xadvance * scale;
    }

    textShader.unuse();
    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}








// Add this function to free font resources
void GL_FreeFont(Font font) {
    if (font.textureID != 0) {
        glDeleteTextures(1, &font.textureID);
    }

    // Find and free the character data
    for (auto it = fontCharData.begin(); it != fontCharData.end(); ) {
        if (it->second) {
            free(it->second);
            it = fontCharData.erase(it);
        }
        else {
            ++it;
        }
    }
}






//Color Utilities
// ----------------------------------------------------------------
//  Real-time gradient between two integer colors (0–255)
//  Returns normalized float color (0.0–1.0)
// ----------------------------------------------------------------


Color4f GL_MixColor(Color4 c1, Color4 c2)
{
    Color4f result;
    result.r = ((float)c1.r + (float)c2.r) / 2.0f / 255.0f;
    result.g = ((float)c1.g + (float)c2.g) / 2.0f / 255.0f;
    result.b = ((float)c1.b + (float)c2.b) / 2.0f / 255.0f;
    result.a = ((float)c1.a + (float)c2.a) / 2.0f / 255.0f;
    return result;
}

Color4f GL_MixColorWeighted(Color4 c1, Color4 c2, float weight1, float weight2)
{
    float total = weight1 + weight2;
    Color4f result;
    result.r = ((c1.r * weight1) + (c2.r * weight2)) / total / 255.0f;
    result.g = ((c1.g * weight1) + (c2.g * weight2)) / total / 255.0f;
    result.b = ((c1.b * weight1) + (c2.b * weight2)) / total / 255.0f;
    result.a = ((c1.a * weight1) + (c2.a * weight2)) / total / 255.0f;
    return result;
}


Color4f GL_GradientColor(Color4 c1, Color4 c2, float t)
{
    t = clampf(t, 0.0f, 1.0f);

    Color4f result;

    result.r = ((float)c1.r + ((float)c2.r - (float)c1.r) * t) / 255.0f;
    result.g = ((float)c1.g + ((float)c2.g - (float)c1.g) * t) / 255.0f;
    result.b = ((float)c1.b + ((float)c2.b - (float)c1.b) * t) / 255.0f;
    result.a = ((float)c1.a + ((float)c2.a - (float)c1.a) * t) / 255.0f;

    return result;
}

// ----------------------------------------------------------------
// Convert integer color (0–255) → Vector3 (normalized 0.0–1.0)
// ----------------------------------------------------------------
Vector3 GL_ConvertColor4ToVector3(Color4 input)
{
    Vector3 result = {
        (float)input.r / 255.0f,
        (float)input.g / 255.0f,
        (float)input.b / 255.0f
    };
    return result;
}
Vector3 GL_ConvertColor4FToVector3(Color4f input)
{
    Vector3 result = { input.r, input.g, input.b };
    return result;
}
Color4f* GL_GenerateGradient(Color4 c1, Color4 c2, int steps)
{
    if (steps < 2)
        return NULL;

    Color4f* gradient = (Color4f*)malloc(sizeof(Color4f) * steps);
    if (!gradient)
        return NULL;

    for (int i = 0; i < steps; ++i)
    {
        float t = (float)i / (float)(steps - 1);
        gradient[i].r = ((float)c1.r + ((float)c2.r - (float)c1.r) * t) / 255.0f;
        gradient[i].g = ((float)c1.g + ((float)c2.g - (float)c1.g) * t) / 255.0f;
        gradient[i].b = ((float)c1.b + ((float)c2.b - (float)c1.b) * t) / 255.0f;
        gradient[i].a = ((float)c1.a + ((float)c2.a - (float)c1.a) * t) / 255.0f;
    }

    return gradient;
}

//Math Utilities
Vector3 Vector3Cross(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}
// Dot product: scalar projection of a onto b
float Vector3Dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Length (magnitude) of vector
float Vector3Length(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Normalize: make vector unit length
Vector3 Vector3Normalize(Vector3 v) {
    float len = Vector3Length(v);
    if (len > 0.000001f) { // avoid division by zero
        v.x /= len;
        v.y /= len;
        v.z /= len;
    }
    else {
        v.x = v.y = v.z = 0.0f;
    }
    return v;
}







//GL 3.3 Advanced for remaking games into gl 3.3 graphics + shaders support
static void ActivateTextureGL(GL_Texture texNumber) {
    switch (texNumber) {
    case GL_TEXTURE_0:
        glActiveTexture(GL_TEXTURE0);
        break;
    case GL_TEXTURE_1:
        glActiveTexture(GL_TEXTURE1);
        break;
    case GL_TEXTURE_2:
        glActiveTexture(GL_TEXTURE2);
        break;
    case GL_TEXTURE_3:
        glActiveTexture(GL_TEXTURE3);
        break;
    case GL_TEXTURE_4:
        glActiveTexture(GL_TEXTURE4);
        break;
    case GL_TEXTURE_5:
        glActiveTexture(GL_TEXTURE5);
        break;
    case GL_TEXTURE_6:
        glActiveTexture(GL_TEXTURE6);
        break;
    case GL_TEXTURE_7:
        glActiveTexture(GL_TEXTURE7);
        break;
    case GL_TEXTURE_8:
        glActiveTexture(GL_TEXTURE8);
        break;
    case GL_TEXTURE_9:
        glActiveTexture(GL_TEXTURE9);
        break;
    case GL_TEXTURE_10:
        glActiveTexture(GL_TEXTURE10);
        break;
    case GL_TEXTURE_11:
        glActiveTexture(GL_TEXTURE11);
        break;
    case GL_TEXTURE_12:
        glActiveTexture(GL_TEXTURE12);
        break;
    case GL_TEXTURE_13:
        glActiveTexture(GL_TEXTURE13);
        break;
    case GL_TEXTURE_14:
        glActiveTexture(GL_TEXTURE14);
        break;
    case GL_TEXTURE_15:
        glActiveTexture(GL_TEXTURE15);
        break;
    case GL_TEXTURE_16:
        glActiveTexture(GL_TEXTURE16);
        break;

    default:
        glActiveTexture(GL_TEXTURE0);
        break;
    }
}
void GL_BindTexture(GL_Texture textureEnum,unsigned int textureID) {
    ActivateTextureGL(textureEnum);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void GL_BindFramebuffer(unsigned int fboID) {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}


void GL_EnableBlend(int use) {
    if (use) {
        glEnable(GL_BLEND);
    }
    else {
        glDisable(GL_BLEND);
    }
}
void GL_SimpleBlendFunc() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}









































TextureQuad3D CreateTexturedQuad3D()
{
    TextureQuad3D quad;

    float vertices[] = {
        // positions        // texcoords
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &quad.vao);
    glGenBuffers(1, &quad.vbo);
    glGenBuffers(1, &quad.ebo);

    glBindVertexArray(quad.vao);

    glBindBuffer(GL_ARRAY_BUFFER, quad.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return quad;
}

void GL_DrawTextureIn3D(

    TextureQuad3D quad,
    Texture texture,Vector3 pos,Vector3 rot,Vector3 size,float alpha)
{


    texturein3dShader.use();


    glm::mat4 model = glm::translate(glm::mat4(1.0f),
        glm::vec3(pos.x, pos.y, pos.z));
    model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(size.x, size.y, size.z));


    // Set uniforms
    GLint modelLoc = glGetUniformLocation(texturein3dShader.ID, "model");
    GLint viewLoc = glGetUniformLocation(texturein3dShader.ID, "view");
    GLint projLoc = glGetUniformLocation(texturein3dShader.ID, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);



    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glUniform1i(glGetUniformLocation(texturein3dShader.ID, "tex"), 0);
    glUniform1f(glGetUniformLocation(texturein3dShader.ID, "alpha"), alpha);

    glBindVertexArray(quad.vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    texturein3dShader.unuse();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}








// In the extern "C" section
void GL_EnableSharpness(int enable) {
    sharpnessEnabled = (enable != 0);
}
void GL_EnableFXAA(int enable) {
    fxaaEnabled = (enable != 0);
}
void GL_EnableColorCorrection(int enable, float brightness, float saturation, float contrast, float hue) {
    correctionEnabled = (enable != 0);
    Cbrightness = brightness;
    Csaturation = saturation;
    Ccontrast = contrast;
    CHue = hue;
}
void GL_EnableMotionBlur(int enable, float strength) {
    motionBlurEnabled = (enable != 0);
    motionBlurStrength = strength;
}
void GL_SetSharpnessIntensity(float intensity) {
    sharpnessIntensity = intensity;
}





// Enhanced GL_LoadModel function
int GL_LoadModel(const char* path,int flipTex) {
    models.emplace_back();
    models.back().LoadModel(path,false, flipTex);
    int modelID = (int)models.size() - 1;

    // Store the path
    ModelAnimationPair pair;
    pair.modelID = modelID;
    pair.modelPath = path;
    pair.animatorID = -1; // No animation initially
    modelAnimationPairs.push_back(pair);

    return modelID;
}



// Enhanced GL_LoadAnimation function  
int GL_LoadAnimation(const char* path, int modelID) {
    if (modelID < 0 || modelID >= (int)models.size()) {
        std::cerr << "[ERROR] Invalid model ID: " << modelID << std::endl;
        return -1;
    }

    Animation* animation = new Animation();
    if (!animation->LoadAnimation(path, &models[modelID])) {
        std::cerr << "[ERROR] Failed to load animation: " << path << std::endl;
        delete animation;
        return -1;
    }

    animators.emplace_back();
    animators.back().LoadAnimator(animation);
    int animatorID = (int)animators.size() - 1;
    
    // Update the pair - find existing or create new
    bool found = false;
    for (auto& pair : modelAnimationPairs) {
        if (pair.modelID == modelID) {
            pair.animatorID = animatorID;
            pair.animationPath = path;
            found = true;
            break;
        }
    }
    
    // If no pair exists for this model, create one
    if (!found) {
        ModelAnimationPair newPair;
        newPair.modelID = modelID;
        newPair.animatorID = animatorID;
        newPair.modelPath = ""; // We don't have the original model path
        newPair.animationPath = path;
        modelAnimationPairs.push_back(newPair);
    }
    
    return animatorID;
}

// GL_SetModelFile implementation
int GL_SetModelFile(const char* filePath, int modelID,int flipTex) {
    if (modelID < 0 || modelID >= (int)models.size()) {
        std::cerr << "[ERROR] Invalid model ID: " << modelID << std::endl;
        return 0;
    }
    
    if (!filePath) {
        std::cerr << "[ERROR] Invalid file path" << std::endl;
        return 0;
    }
    
#if ENABLE_DEBUG
    std::cout << "[INFO] Reloading model ID " << modelID << " with file: " << filePath << std::endl;
#endif
    try {
        // Store the current animation ID associated with this model
        int currentAnimatorID = -1;
        std::string currentAnimationPath;
        
        for (const auto& pair : modelAnimationPairs) {
            if (pair.modelID == modelID) {
                currentAnimatorID = pair.animatorID;
                currentAnimationPath = pair.animationPath;
                break;
            }
        }
        
        // Clear the existing model and load new one
        models[modelID] = ModelCPP();
        models[modelID].LoadModel(filePath,false, flipTex);
        
        // Update the stored model path
        for (auto& pair : modelAnimationPairs) {
            if (pair.modelID == modelID) {
                pair.modelPath = filePath;
                break;
            }
        }
        
        // If there was an animation associated, reload it to match the new model
        if (currentAnimatorID != -1 && !currentAnimationPath.empty()) {
#if ENABLE_DEBUG
            std::cout << "[INFO] Reloading associated animation for updated model" << std::endl;
#endif
            GL_SetAnimationFile(currentAnimationPath.c_str(), currentAnimatorID);
        }
#if ENABLE_DEBUG
        std::cout << "[SUCCESS] Model " << modelID << " reloaded with: " << filePath << std::endl;
#endif
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to reload model: " << e.what() << std::endl;
        return 0;
    }
}

// Enhanced GL_SetAnimationFile with proper model association
int GL_SetAnimationFile(const char* filePath, int animationID) {
    if (animationID < 0 || animationID >= (int)animators.size()) {
        std::cerr << "[ERROR] Invalid animation ID: " << animationID << std::endl;
        return 0;
    }
    
    // Find the associated model
    int associatedModelID = -1;
    for (const auto& pair : modelAnimationPairs) {
        if (pair.animatorID == animationID) {
            associatedModelID = pair.modelID;
            break;
        }
    }
    
    if (associatedModelID == -1) {
        std::cerr << "[ERROR] No associated model found for animation ID: " << animationID << std::endl;
        return 0;
    }
    
    // Create new animation
    Animation* newAnimation = new Animation();
    if (!newAnimation->LoadAnimation(filePath, &models[associatedModelID])) {
        std::cerr << "[ERROR] Failed to load animation file: " << filePath << std::endl;
        delete newAnimation;
        return 0;
    }
    
    // Update the animator
    animators[animationID].LoadAnimator(newAnimation);
    
    // Update the stored path
    for (auto& pair : modelAnimationPairs) {
        if (pair.animatorID == animationID) {
            pair.animationPath = filePath;
            break;
        }
    }
#if ENABLE_DEBUG
    std::cout << "[SUCCESS] Animation " << animationID << " reloaded with: " << filePath << std::endl;
#endif
    return 1;
}

// Optional: Helper function to get current file paths
const char* GL_GetModelPath(int modelID) {
    for (const auto& pair : modelAnimationPairs) {
        if (pair.modelID == modelID) {
            return pair.modelPath.c_str();
        }
    }
    return "";
}

const char* GL_GetAnimationPath(int animationID) {
    for (const auto& pair : modelAnimationPairs) {
        if (pair.animatorID == animationID) {
            return pair.animationPath.c_str();
        }
    }
    return "";
}

void GL_UpdateAllAnimations(float deltaTime)
{
    if (animators.empty())
        return;

    for (auto& animator : animators)
    {
        animator.UpdateAnimation(deltaTime);
    }
}
void GL_ResetAnimation(int animatorID) {
    if (animatorID < 0 || animatorID >= (int)animators.size()) {
        return;
    }
    animators[animatorID].ResetAnimation();
}

void GL_DrawStaticModel(int modelID, Vector3 pos, Vector3 rot, Vector3 scale) {
    if (modelID < 0 || modelID >= (int)models.size()) return;

    camShader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
    model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

    camShader.setMat4("model", model);
    models[modelID].Draw(camShader);
}

void GL_DebugAnimationSystem(int modelID, int animatorID) {

#if ENABLE_DEBUG
    std::cout << "=== ANIMATION DEBUG ===" << std::endl;
    std::cout << "Model ID: " << modelID << ", Models count: " << models.size() << std::endl;
    std::cout << "Animator ID: " << animatorID << ", Animators count: " << animators.size() << std::endl;

    if (modelID >= 0 && modelID < models.size()) {
        std::cout << "Model valid: YES" << std::endl;
        std::cout << "Model bone count: " << models[modelID].GetBoneInfoMap().size() << std::endl;
    }
    else {
        std::cout << "Model valid: NO" << std::endl;
    }

    if (animatorID >= 0 && animatorID < animators.size()) {
        std::cout << "Animator valid: YES" << std::endl;
        auto transforms = animators[animatorID].GetFinalBoneMatrices();
        std::cout << "Bone matrices count: " << transforms.size() << std::endl;
        if (!transforms.empty()) {
            std::cout << "First bone matrix: " << std::endl;
            for (int i = 0; i < 4; i++) {
                std::cout << transforms[0][i][0] << " " << transforms[0][i][1] << " " << transforms[0][i][2] << " " << transforms[0][i][3] << std::endl;
            }
        }
    }
    else {
        std::cout << "Animator valid: NO" << std::endl;
    }
    std::cout << "=====================" << std::endl;
#endif
}
void GL_DrawAnimatedModel(int modelID, int animatorID, Vector3 pos, Vector3 rot, Vector3 scale) {
    // Debug first
    //GL_DebugAnimationSystem(modelID, animatorID);

    if (modelID < 0 || modelID >= (int)models.size()) {
        std::cerr << "[ERROR] Invalid model ID: " << modelID << std::endl;
        return;
    }

    if (animatorID < 0 || animatorID >= (int)animators.size()) {
        std::cerr << "[ERROR] Invalid animator ID: " << animatorID << std::endl;
        return;
    }

    camShader.use();

    // CRITICAL: Set skeletal animation uniform FIRST
    camShader.setBool("skeletalEnabled", true);

    // CRITICAL: Get bone matrices before setting up model matrix
    auto transforms = animators[animatorID].GetFinalBoneMatrices();

    // CRITICAL: Set all bone matrices, even if some are empty
    for (int i = 0; i < 100; ++i) { // Set up to 100 bones
        if (i < transforms.size()) {
            camShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
        else {
            // Set identity matrix for unused bones
            camShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", glm::mat4(1.0f));
        }
    }

    // CRITICAL: Set model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
    model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

    camShader.setMat4("model", model);

    // CRITICAL: Draw the model
    //std::cout << "Drawing animated model..." << std::endl;
    models[modelID].Draw(camShader);
   // std::cout << "Finished drawing animated model" << std::endl;

    // CRITICAL: Disable skeletal animation after drawing
    camShader.setBool("skeletalEnabled", false);
}





// Set to 1 to enable colored output
#define TRACE_USE_COLOR 1

// Internal color codes (ANSI)
#if TRACE_USE_COLOR
#define TRACE_COLOR_RESET   "\033[0m"
#define TRACE_COLOR_INFO    "\033[36m"   // Cyan
#define TRACE_COLOR_WARN    "\033[33m"   // Yellow
#define TRACE_COLOR_ERROR   "\033[31m"   // Red
#define TRACE_COLOR_SUCCESS "\033[32m"   // Green
#define TRACE_COLOR_DEBUG   "\033[35m"   // Magenta
#else
#define TRACE_COLOR_RESET   ""
#define TRACE_COLOR_INFO    ""
#define TRACE_COLOR_WARN    ""
#define TRACE_COLOR_ERROR   ""
#define TRACE_COLOR_SUCCESS ""
#define TRACE_COLOR_DEBUG   ""
#endif




void GL_TraceLog(TraceType type, const char* fmt, ...) {
    const char* typeStr = "";
    const char* color = "";

    switch (type) {
    case TRACE_INFO:    typeStr = "INFO";    color = TRACE_COLOR_INFO;    break;
    case TRACE_WARNING: typeStr = "WARNING"; color = TRACE_COLOR_WARN;    break;
    case TRACE_ERROR:   typeStr = "ERROR";   color = TRACE_COLOR_ERROR;   break;
    case TRACE_SUCCESS: typeStr = "SUCCESS"; color = TRACE_COLOR_SUCCESS; break;
    case TRACE_DEBUG:   typeStr = "DEBUG";   color = TRACE_COLOR_DEBUG;   break;
    default:            typeStr = "LOG";     color = TRACE_COLOR_RESET;   break;
    }

    // Get current time
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char timeStr[9];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm_info);

    // Print formatted message
    printf("%s[%s] [%s]: ", color, timeStr, typeStr);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("%s\n", TRACE_COLOR_RESET);
}



static uint64_t rand_state = 0x853c49e6748fea9bULL; // Default seed

// --- Internal XORSHIFT64* algorithm (fast & decent quality) ---
static uint64_t xorshift64(void) {
    uint64_t x = rand_state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    rand_state = x;
    return x * 0x2545F4914F6CDD1DULL;
}

// --- Initialization ---
void Rand_Seed(uint64_t seed) {
    if (seed == 0) seed = 0xDEADBEEF;
    rand_state = seed;
}

void Rand_SeedTime(void) {
    Rand_Seed((uint64_t)time(NULL));
}

// --- Core generators ---
uint64_t Rand_U64(void) {
    return xorshift64();
}

int Rand_Int(int min, int max) {
    if (max <= min) return min;
    return (int)(Rand_U64() % (uint64_t)(max - min + 1)) + min;
}

float Rand_Float(void) {
    return (float)((Rand_U64() & 0xFFFFFF) / (float)0xFFFFFF);
}

float Rand_FloatRange(float min, float max) {
    return min + (max - min) * Rand_Float();
}

double Rand_Double(void) {
    return (double)((Rand_U64() & 0xFFFFFFFFFFFFULL) / (double)0xFFFFFFFFFFFFULL);
}

int Rand_Bool(void) {
    return (Rand_U64() & 1ULL) != 0;
}

int Rand_Sign(void) {
    return Rand_Bool() ? 1 : -1;
}

Vector3 Rand_Vector3(float min, float max) {
    Vector3 v;
    v.x = Rand_FloatRange(min, max);
    v.y = Rand_FloatRange(min, max);
    v.z = Rand_FloatRange(min, max);
    return v;
}

Color4 Rand_Color(void) {
    Color4 c;
    c.r = Rand_Float();
    c.g = Rand_Float();
    c.b = Rand_Float();
    c.a = Rand_Float();
    return c;
}














#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#include <unistd.h>
#include <pthread.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif

#if defined(_WIN32)

// --- Windows: ShellExecute is already async ---
void GL_OpenURL(const char* url)
{
    if (!url || !*url) return;
    ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

#else

// --- POSIX (macOS, Linux): use a background thread ---
static void* open_url_thread(void* arg)
{
    const char* url = (const char*)arg;
    if (!url || !*url) return NULL;

    char cmd[512];
#if defined(__APPLE__)
    snprintf(cmd, sizeof(cmd), "open \"%s\" >/dev/null 2>&1 &", url);
#else
    snprintf(cmd, sizeof(cmd),
        "xdg-open \"%s\" >/dev/null 2>&1 || sensible-browser \"%s\" >/dev/null 2>&1 &",
        url, url);
#endif
    system(cmd);
    return NULL;
}

void GL_OpenURL(const char* url)
{
    if (!url || !*url) return;

    // Run on detached thread so it doesn’t block main loop
    pthread_t tid;
    pthread_create(&tid, NULL, open_url_thread, strdup(url));
    pthread_detach(tid);
}

#endif






#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

#if defined(_WIN32)
#include <direct.h>
#define stat _stat
#endif

// --------------------------------------------------------
// Check if a file exists
// --------------------------------------------------------
int GL_FileExists(const char* fileName)
{
    if (!fileName || !*fileName) return 0;

    struct stat info;
    return (stat(fileName, &info) == 0 && (info.st_mode & S_IFREG));
}

// --------------------------------------------------------
// Check if a directory exists
// --------------------------------------------------------
int GL_DirectoryExists(const char* dirPath)
{
    if (!dirPath || !*dirPath) return 0;

    struct stat info;
    return (stat(dirPath, &info) == 0 && (info.st_mode & S_IFDIR));
}

// --------------------------------------------------------
// Check if file has specific extension (case-insensitive)
// Example: IsFileExtension("data/image.PNG", ".png") -> 1
// --------------------------------------------------------
int GL_IsFileExtension(const char* fileName, const char* ext)
{
    if (!fileName || !ext || !*fileName || !*ext) return 0;

    const char* dot = strrchr(fileName, '.');
    if (!dot) return 0; // No extension in file name

    // Compare case-insensitively
    while (*dot && *ext)
    {
        char c1 = tolower((unsigned char)*dot++);
        char c2 = tolower((unsigned char)*ext++);
        if (c1 != c2) return 0;
    }

    return (*dot == '\0' && *ext == '\0');
}



// --------------------------------------------------------
int GL_GetFileLength(const char* fileName)
{
    if (!fileName || !*fileName) return -1;
    FILE* f = fopen(fileName, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fclose(f);
    return (int)len;
}

// --------------------------------------------------------
const char* GL_GetFileExtension(const char* fileName)
{
    if (!fileName) return "";
    const char* dot = strrchr(fileName, '.');
    if (!dot || dot == fileName) return ""; // No extension
    return dot; // Includes the dot, e.g., ".png"
}

// --------------------------------------------------------
const char* GL_GetFileName(const char* filePath)
{
    if (!filePath) return "";
    const char* slash1 = strrchr(filePath, '/');
    const char* slash2 = strrchr(filePath, '\\');
    const char* slash = (slash1 > slash2) ? slash1 : slash2;
    return (slash) ? slash + 1 : filePath;
}

// --------------------------------------------------------
const char* GL_GetFileNameWithoutExt(const char* filePath)
{
    static char name[256];
    if (!filePath) return "";

    const char* file = GL_GetFileName(filePath);
    const char* dot = strrchr(file, '.');
    size_t len = (dot) ? (size_t)(dot - file) : strlen(file);

    if (len >= sizeof(name)) len = sizeof(name) - 1;
    strncpy(name, file, len);
    name[len] = '\0';

    return name;
}

Texture GL_GenerateCheckerTexture(int width, int height, int checkSize)
{
    uint8_t* pixels = (uint8_t*)malloc(width * height * 4);
    if (!pixels) return Texture{ 0 };

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int i = (y * width + x) * 4;
            int checkX = x / checkSize;
            int checkY = y / checkSize;
            int checker = (checkX + checkY) % 2;

            if (checker == 0)
            {
                // Magenta (like Source "missing texture")
                pixels[i + 0] = 255;
                pixels[i + 1] = 0;
                pixels[i + 2] = 255;
                pixels[i + 3] = 255;
            }
            else
            {
                // Black
                pixels[i + 0] = 0;
                pixels[i + 1] = 0;
                pixels[i + 2] = 0;
                pixels[i + 3] = 255;
            }
        }
    }

    Texture tex;
    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}



















































#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>

void GL_SetWindowMaximize(int w, int h)
{
    HWND hwnd = glfwGetWin32Window(window);
    if (!hwnd) return;

    // Restore first if minimized or maximized
    ShowWindow(hwnd, SW_RESTORE);

    // Resize before maximizing (Windows ignores size in SW_MAXIMIZE)
    SetWindowPos(hwnd, HWND_TOP, 0, 0, w, h, SWP_NOMOVE | SWP_SHOWWINDOW);
    ShowWindow(hwnd, SW_MAXIMIZE);

}

void GL_SetWindowMinimize(int w, int h)
{
    HWND hwnd = glfwGetWin32Window(window);
    if (!hwnd) return;

    // Resize (if restored later)
    SetWindowPos(hwnd, HWND_TOP, 0, 0, w, h, SWP_NOMOVE | SWP_SHOWWINDOW);
    ShowWindow(hwnd, SW_MINIMIZE);
}

#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

void GL_SetWindowMaximize(int w, int h)
{
    Display* display = glfwGetX11Display();
    Window xwin = glfwGetX11Window(window);
    if (!display || !xwin) return;

    // Resize manually first
    XResizeWindow(display, xwin, (unsigned int)w, (unsigned int)h);

    // Send maximize event to window manager
    Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
    Atom max_h = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom max_v = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

    XEvent e = { 0 };
    e.xclient.type = ClientMessage;
    e.xclient.serial = 0;
    e.xclient.send_event = True;
    e.xclient.message_type = wm_state;
    e.xclient.window = xwin;
    e.xclient.format = 32;
    e.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
    e.xclient.data.l[1] = max_h;
    e.xclient.data.l[2] = max_v;
    e.xclient.data.l[3] = 1;

    XSendEvent(display, DefaultRootWindow(display), False,
        SubstructureRedirectMask | SubstructureNotifyMask, &e);
    XFlush(display);
}

void GL_SetWindowMinimize(int w, int h)
{
    Display* display = glfwGetX11Display();
    Window xwin = glfwGetX11Window(window);
    if (!display || !xwin) return;

    // Resize before minimize (for when restored)
    XResizeWindow(display, xwin, (unsigned int)w, (unsigned int)h);
    XIconifyWindow(display, xwin, DefaultScreen(display));
    XFlush(display);
}

#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#import <Cocoa/Cocoa.h>

void GL_SetWindowMaximize(int w, int h)
{
    NSWindow* nswin = glfwGetCocoaWindow(window);
    if (!nswin) return;

    NSRect frame = [nswin frame];
    frame.size.width = w;
    frame.size.height = h;
    [nswin setFrame : frame display : YES] ;

    if (![nswin isZoomed])
        [nswin zoom : nil]; // maximize
}

void GL_SetWindowMinimize(int w, int h)
{
    NSWindow* nswin = glfwGetCocoaWindow(window);
    if (!nswin) return;

    NSRect frame = [nswin frame];
    frame.size.width = w;
    frame.size.height = h;
    [nswin setFrame : frame display : YES] ;

    [nswin miniaturize : nil] ; // minimize
}

#else
//#error "Unsupported platform for GLFW_SetWindowMaximize/Minimize"
#endif