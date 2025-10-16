#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "miniaudio.h"

    typedef struct AudioSound {
        ma_sound sound;
        int isLoaded;
        int isPaused;
        char path[256];
    } AudioSound;

    typedef struct AudioManager {
        ma_engine engine;
        int initialized;
        int reverbEnabled;
    } AudioManager;

    extern AudioManager gAudio;

    int Audio_Init(void);
    void Audio_Shutdown(void);

    // Basic sound operations
    AudioSound* Audio_Load(const char* path);
    void Audio_Unload(AudioSound* snd);
    void Audio_Play(AudioSound* snd);
    void Audio_Stop(AudioSound* snd);
    void Audio_Pause(AudioSound* snd);
    void Audio_Resume(AudioSound* snd);
    void Audio_Restart(AudioSound* snd);
    void Audio_PlayWithSeek(AudioSound* snd, float seconds);

    // Control parameters
    void Audio_SetVolume(AudioSound* snd, float volume);
    void Audio_SetLooping(AudioSound* snd, int loop);
    int Audio_IsPlaying(AudioSound* snd);

    // Advanced features
    void Audio_SetSpeed(AudioSound* snd, float speed);
    void Audio_SetPitch(AudioSound* snd, float pitch);
    void Audio_SetPan(AudioSound* snd, float pan);
    void Audio_SetPosition3D(AudioSound* snd, float x, float y, float z);
    void Audio_UpdateListener3D(float x, float y, float z);

    // Global effects
    void Audio_SetGlobalVolume(float vol);
    float Audio_GetGlobalVolume(void);
    void Audio_SetLowpass(AudioSound* snd, float cutoffFreq);
    unsigned int Audio_GetSampleRate(AudioSound* snd);
    unsigned int Audio_GetEngineSampleRate(void);

    //Helpers
    float Audio_GetCurrentVolume(void);
    int   Audio_GetWaveform(float* outBuffer, int maxSamples);

    // Node graph visualization
    int Audio_EnableVisualizer(void);
    void Audio_DisableVisualizer(void);

    // Get real-time data for visualization
    float Audio_GetCurrentVolume(void);
    int   Audio_GetWaveform(float* outBuffer, int maxSamples);
    /*For Example if you want use node graphs here:
    Visualizer 

    if (Audio_EnableVisualizer()) {
    float samples[128];
    int n = Audio_GetWaveform(samples, 128);
    float vol = Audio_GetCurrentVolume();

    for (int i = 0; i < n; i++) {
        float x = (float)i / n * screenWidth;
        float y = screenHeight / 2 + samples[i] * 200.0f;
        DrawLine(x, screenHeight / 2, x, y, Color_Lime);
    }

    DrawTextf("Volume: %.2f", 10, 10, 18, Color_White, vol);
    }
    */

#ifdef __cplusplus
}
#endif
#endif // AUDIO_H
