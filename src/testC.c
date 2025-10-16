#include "LibGL.h"
#include <stdio.h>

#include "lvl.h"
#include "lvlParser.h"





int main() {
    GL_InitWindow(800, 600, "Level System Test", 0);
    GL_InitOpenGLLoader();

    // Enable SRGB Framebuffer for HDR
    GL_EnableFramebufferSRGB(0);

    Camera cam = { 0 };
    int camLock = 1;
    GL_CaptureMouse(camLock);
    GL_InitCamera(cam, (Vector3) { 0, 5, 15 }, 70.0f, 2.5f, 0.1f, TYPE_EDITOR, 1);
    GL_EnableDepthTest(1);

    // Lighting setup
    GL_InitLighting();
    GL_SetShadingModel(0); // Use Blinn-Phong

    // Create a colored point light
    /*int pointLight = GL_CreateLight(LIGHT_POINT);
    Light redLight = GL_GetLight(pointLight);
    redLight.position = (Vector3){ 0.0f, 3.0f, 0.0f };
    redLight.color = (Vector3){ 1.0f, 1.0f, 1.0f };
    redLight.intensity = 1.0f;
    redLight.specular = 1.0f;
    GL_UpdateLight(pointLight, redLight);*/

    // Set material properties
    GL_SetMaterial(
        (Vector3) {
        0.1f, 0.1f, 0.1f
    },  // Ambient
        (Vector3) {
        0.6f, 0.6f, 0.6f
    },  // Diffuse  
        (Vector3) {
        1.0f, 1.0f, 1.0f
    },  // Specular
        32.0f                             // Shininess
    );

    GL_SetNormalMappingEnabled(0);

    // Post-processing defaults
    GL_EnableBloom(0);
    GL_EnableChromaticAberration(0);
    GL_EnableSharpness(0);
    GL_EnableFog(0);

    Font myFont = GL_LoadFont("arial.ttf", 24);
    GL_ImGui_Init();

    printf("=== LEVEL SYSTEM TEST ===\n");
    printf("Controls:\n");
    printf("  WASD - Move camera\n");
    printf("  M - Toggle mouse capture\n");
    printf("  ESC - Quit\n");
    printf("\n");

    // **LOAD THE LEVEL FILE**
    /*LvlParseResult result = LvlParser_LoadLevel("Level2.lvl");

    if (result != LVL_PARSE_OK) {
        printf("Failed to load level! Error code: %d\n", result);
        GL_Quit();
        return 1;
    }

    printf("Level loaded successfully!\n");
*/




    
    // Load models
    int modelA = GL_LoadModel("vampire/dancing_vampire.dae", 1);
    int modelA_anim = GL_LoadAnimation("vampire/dancing_vampire.dae", modelA);
    Vector3 pos1 = { 3, 0, 0 };
    Vector3 rot1 = { 0, 0, 0 };
    Vector3 scl1 = { 0.1f, 0.1f, 0.1f };



    while (!GL_PollQuit()) {
        GL_ProcessInputs();

        if (GL_KeyPressed(GL_KEY_M)) {
            camLock = !camLock;
            GL_CaptureMouse(camLock);
        }

        // ImGui interface
        if (GL_ImGui_ProcessBeforeRenderer(3)) {
            if (GL_ImGui_BeginWindow("Level Info", (Vector2) { 250, 150 }, (Vector2) { 10, 10 })) {
                GL_ImGui_Text((Vector2) { 10, 30 }, (Color4f) { 1, 1, 1, 1 }, "Level System Active");
                GL_ImGui_Text((Vector2) { 10, 50 }, (Color4f) { 0.8f, 0.8f, 0.8f, 1 }, "FPS: %d", GL_GetFPS());
                GL_ImGui_EndWindow();
            }
        }

        GL_ClearDepthColor(0.1f, 0.1f, 0.15f);

        // **UPDATE THE LEVEL (runs level logic)**
        //LvlParser_UpdateLevel(GL_GetDeltaTime());


        
        if (GL_Begin3D()) {
            GL_DrawAnimatedModel(modelA, modelA_anim, pos1, rot1, scl1);
            GL_End3D();
        }




        // **RENDER THE LEVEL (draws all level objects)**
        //LvlParser_RenderLevel();





        GL_RenderFBO();

        GL_DrawText(myFont, "Level System Test",
            (Vector2) {
            10.0f, 10.0f
        }, 1.0f,
            (Vector3) {
            1.0f, 1.0f, 1.0f
        });

            GL_ImGui_Render();
            GL_SwapBuffers();
    }

    // **CLEANUP THE LEVEL**
    //LvlParser_UnloadLevel();

    GL_FreeFont(myFont);
    GL_ImGui_Shutdown();
    GL_Quit();

    return 0;
}















int main23() {
    GL_InitWindow(800, 600, "Physics Test - Fixed", 0);
    GL_InitOpenGLLoader();

    // Enable SRGB Framebuffer for HDR
    GL_EnableFramebufferSRGB(0);

    Camera cam = { 0 };
    int camLock = 1;
    GL_CaptureMouse(camLock);
    GL_InitCamera(cam, (Vector3) { 0, 5, 15 }, 70.0f, 2.5f, 0.1f, TYPE_EDITOR, 1);
    GL_EnableDepthTest(1);
    Texture tex = GL_LoadTexture("test.png", TEXTURE_FILTER_LINEAR);


    Texture texNormal = GL_LoadNormalMap("testN.png", TEXTURE_FILTER_LINEAR);
    Texture texNormal2 = GL_LoadNormalMap("testN.png", TEXTURE_FILTER_LINEAR);
    TextureQuad3D quad = CreateTexturedQuad3D();

    GL_LoadSkyboxTextures(
        "skybox/right.jpg",
        "skybox/left.jpg",
        "skybox/top.jpg",
        "skybox/bottom.jpg",
        "skybox/front.jpg",
        "skybox/back.jpg");

    // Lighting setup
    GL_InitLighting();
    GL_SetShadingModel(0); // Use Blinn-Phong

    // Create a colored point light
    int pointLight = GL_CreateLight(LIGHT_POINT);
    Light redLight = GL_GetLight(pointLight);
    redLight.position = (Vector3){ 0.0f, 1.0f, 0.0f };
    redLight.color = (Vector3){ 1.0f, 0.0f, 0.0f }; // Warm white light
    redLight.intensity = 1.0f;
    redLight.specular = 1.0f;
    GL_UpdateLight(pointLight, redLight);

    // Set material properties
    GL_SetMaterial(
        (Vector3) {
        0.1f, 0.1f, 0.1f
    },  // Ambient
        (Vector3) {
        0.6f, 0.6f, 0.6f
    },  // Diffuse  
        (Vector3) {
        1.0f, 1.0f, 1.0f
    },  // Specular
        32.0f                            // Shininess
    );

    GL_SetNormalMappingEnabled(0);
    GL_EnableBloom(0);
    GL_EnableChromaticAberration(0);
    GL_SetBloomThreshold(1.1f);
    GL_SetBloomIntensity(0.05f);
    GL_SetChromaticAberrationStrength(0.015f);
    GL_EnableSharpness(0);
    GL_SetSharpnessIntensity(5.5f);

    GL_EnableFog(0);

    /*


    // Enable volumetric fog
    GL_EnableVolumetricFog(0);
    GL_SetFogType(FOG_VOLUMETRIC);

    // Configure volumetric fog
    GL_SetVolumetricFogSteps(16);          // More steps = better quality but slower
    GL_SetVolumetricFogScattering(0.8f);   // How much light scatters
    GL_SetVolumetricFogAbsorption(0.3f);   // How much light is absorbed
    GL_SetVolumetricFogPhase(0.2f);        // Directional scattering (-1 to 1)
    GL_SetVolumetricFogNoiseScale(0.05f);  // Noise frequency
    GL_SetVolumetricFogNoiseStrength(0.4f);// Noise intensity
    GL_SetVolumetricFogWind((Vector3) { 0.02f, 0.01f, 0.03f }); // Animated wind
    GL_SetVolumetricFogAnimate(0);         // Enable animation

    // Set basic fog properties (used by volumetric fog)
    GL_SetFogColor((Vector3) { 0.8f, 0.9f, 1.0f }); // Light blue fog
    GL_SetFogDensity(0.1f);
    GL_SetFogStart(5.0f);
    GL_SetFogEnd(100.0f);*/










    // Load models
    int modelA = GL_LoadModel("vampire/dancing_vampire.dae",1);
    int modelA_anim = GL_LoadAnimation("vampire/dancing_vampire.dae", modelA);
    int modelB = GL_LoadModel("backpack/backpack.obj",0);

    Vector3 pos1 = { 3, 0, 0 };
    Vector3 rot1 = { 0, 0, 0 };
    Vector3 scl1 = { 0.1f, 0.1f, 0.1f };

    Vector3 pos2 = { -3, 0, 0 };
    Vector3 rot2 = { 0, 45, 0 };
    Vector3 scl2 = { 1, 1, 1 };

    Font myFont = GL_LoadFont("arial.ttf", 24);
    int update1 = 1;
    Color4 red = { 255, 0, 0, 255 };
    Color4 blue = { 0, 0, 255, 255 };

    GL_ImGui_Init();

    printf("=== PHYSICS TEST ===\n");
    printf("Controls:\n");
    printf("  Arrow Keys - Move player\n");
    printf("  Space - Jump\n");
    printf("  P - Pause animations\n");
    printf("  ESC - Quit\n");

    while (!GL_PollQuit()) {
        GL_ProcessInputs();



        // ImGui interface
        if (GL_ImGui_ProcessBeforeRenderer(3)) {
            static char myText[128] = "Physics Engine Test!";
            Vector2 pos = { 100.0f, 190.0f };
            Color4 textColor = { 255, 255, 255, 255 };
            Color4 bgColor = { 40, 40, 40, 255 };

            GL_ImGuiDrawList_Edit("MyEdit", myText, sizeof(myText), "arial.ttf", pos, textColor, bgColor, 18.0f, 0);

            if (GL_ImGui_BeginWindow("Physics Info", (Vector2) { 250, 150 }, (Vector2) { 0, 0 })) {
                

                GL_ImGui_EndWindow();
            }
        }

        GL_ClearDepthColor(0, 0, 0);

        if (update1) {
            GL_UpdateAllAnimations(GL_GetDeltaTime());
        }

        if (GL_KeyPressed(GL_KEY_P)) {
            update1 = !update1;
        }

        if (GL_KeyPressed(GL_KEY_M)) {
            camLock = !camLock;
            GL_CaptureMouse(camLock);
        }
        GL_RenderSkybox();

        if (GL_Begin3D()) {
            GL_DrawTextureIn3D(quad, texNormal2,
                (Vector3) {
                10, 0, 2
            }, (Vector3) { 0, 0, 0 }, (Vector3) { 1, 1, 1 }, 0.5f);


            GL_DrawAnimatedModel(modelA, modelA_anim, pos1, rot1, scl1);
            GL_DrawStaticModel(modelB, pos2, rot2, scl2);

            GL_DrawCube(
                (Vector3) {
                0.0f, 0.0f, 0.0f
            },
                (Vector3) {
                0.0f, 0.0f, 0.0f
            },
                (Vector3) {
                10.0f, 1.0f, 10.0f
            },
                
                tex);
            GL_DrawCubeWithNormalMap(
                (Vector3) {
                0.0f, 1.0f, 0.0f
            },
                (Vector3) {
                0.0f, 0.0f, 0.0f
            },
                (Vector3) {
                1.0f, 1.0f, 1.0f
            },

                tex,texNormal);
            GL_End3D();
        }

        GL_RenderFBO();
        GL_DrawTextWithGradientV(
            myFont, "Physics Engine Test!",
            (Vector2) {
            10.0f, 16.0f
        }, 1.0f,
            red, blue);

        GL_ImGui_Render();
        GL_SwapBuffers();
    }

    GL_FreeFont(myFont);
    GL_ImGui_Shutdown();
    GL_Quit();

    return 0;
}