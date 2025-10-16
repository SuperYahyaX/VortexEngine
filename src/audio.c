#define MINIAUDIO_IMPLEMENTATION
#define MA_ENABLE_VORBIS
#define MA_ENABLE_FLAC
#define MA_ENABLE_MP3
#include "miniaudio.h"
#include "audio.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Global audio manager
AudioManager gAudio = { 0 };

// =========================
// INTERNAL HELPER FUNCTIONS
// =========================

static void safe_uninit_sound(AudioSound* snd)
{
    if (snd && snd->isLoaded) {
        ma_sound_uninit(&snd->sound);
        snd->isLoaded = false;
    }
}

// Helper: get sample rate from a ma_sound via its data source
static ma_uint32 get_sample_rate_from_sound(AudioSound* snd)
{
    if (!snd || !snd->isLoaded) return 0;

    ma_data_source* pDataSource = ma_sound_get_data_source(&snd->sound);
    if (pDataSource == NULL) {
        return 0;
    }

    ma_uint32 sampleRate = 0;
    ma_result result = ma_data_source_get_data_format(pDataSource, NULL, NULL, &sampleRate, NULL, 0);
    if (result != MA_SUCCESS) {
        return 0;
    }

    return sampleRate;
}

// ====================
// AUDIO MANAGER SYSTEM
// ====================

int Audio_Init(void)
{
    if (gAudio.initialized) return true;

    ma_result result = ma_engine_init(NULL, &gAudio.engine);
    if (result != MA_SUCCESS) {
        printf("[Audio] Failed to initialize miniaudio engine!\n");
        return false;
    }

    gAudio.initialized = true;
    printf("[Audio] Initialized successfully.\n");
    return true;
}

void Audio_Shutdown(void)
{
    if (!gAudio.initialized) return;
    ma_engine_uninit(&gAudio.engine);
    gAudio.initialized = false;
    printf("[Audio] Shutdown complete.\n");
}

// ====================
// SOUND CONTROL LOGIC
// ====================

AudioSound* Audio_Load(const char* path)
{
    if (!gAudio.initialized) {
        printf("[Audio] Not initialized!\n");
        return NULL;
    }

    AudioSound* snd = (AudioSound*)malloc(sizeof(AudioSound));
    if (!snd) return NULL;

    memset(snd, 0, sizeof(*snd));

    ma_result result = ma_sound_init_from_file(&gAudio.engine, path, 0, NULL, NULL, &snd->sound);
    if (result != MA_SUCCESS) {
        printf("[Audio] Failed to load: %s\n", path);
        free(snd);
        return NULL;
    }

    snd->isLoaded = true;
    snd->isPaused = false;
    snprintf(snd->path, sizeof(snd->path), "%s", path);

    printf("[Audio] Loaded: %s\n", path);
    return snd;
}

void Audio_Unload(AudioSound* snd)
{
    if (!snd) return;
    if (snd->isLoaded) {
        safe_uninit_sound(snd);
    }
    free(snd);
}

// Play / stop / pause / resume / restart

void Audio_Play(AudioSound* snd)
{
    if (!snd || !snd->isLoaded) return;
    ma_sound_start(&snd->sound);
    snd->isPaused = false;
}

void Audio_Stop(AudioSound* snd)
{
    if (!snd || !snd->isLoaded) return;
    ma_sound_stop(&snd->sound);
    snd->isPaused = false;
}

void Audio_Pause(AudioSound* snd)
{
    if (!snd || !snd->isLoaded) return;
    ma_sound_stop(&snd->sound);
    snd->isPaused = true;
}

void Audio_Resume(AudioSound* snd)
{
    if (!snd || !snd->isLoaded) return;
    if (snd->isPaused) {
        ma_sound_start(&snd->sound);
        snd->isPaused = false;
    }
}

void Audio_Restart(AudioSound* snd)
{
    if (!snd || !snd->isLoaded) return;
    ma_sound_seek_to_pcm_frame(&snd->sound, 0);
    ma_sound_start(&snd->sound);
    snd->isPaused = false;
}

void Audio_PlayWithSeek(AudioSound* snd, float seconds)
{
    if (!snd || !snd->isLoaded) return;

    ma_uint32 sampleRate = get_sample_rate_from_sound(snd);
    if (sampleRate == 0) {
        // fallback to engine sample rate
        sampleRate = ma_engine_get_sample_rate(&gAudio.engine);
        if (sampleRate == 0) return;
    }

    double frame_d = (double)seconds * (double)sampleRate;
    if (frame_d < 0.0) frame_d = 0.0;
    ma_uint64 frameIndex = (ma_uint64)frame_d;

    ma_sound_seek_to_pcm_frame(&snd->sound, frameIndex);
    ma_sound_start(&snd->sound);
    snd->isPaused = false;
}

// ====================
// CONTROL PARAMETERS
// ====================

void Audio_SetVolume(AudioSound* snd, float volume)
{
    if (!snd || !snd->isLoaded) return;
    ma_sound_set_volume(&snd->sound, volume);
}

void Audio_SetLooping(AudioSound* snd, int loop)
{
    if (!snd || !snd->isLoaded) return;
    ma_sound_set_looping(&snd->sound, loop);
}

int Audio_IsPlaying(AudioSound* snd)
{
    if (!snd || !snd->isLoaded) return false;
    return ma_sound_is_playing(&snd->sound);
}

// ====================
// GLOBAL ENGINE CONTROL
// ====================

void Audio_SetGlobalVolume(float vol)
{
    if (!gAudio.initialized) return;
    ma_engine_set_volume(&gAudio.engine, vol);
}

float Audio_GetGlobalVolume(void)
{
    if (!gAudio.initialized) return 0.0f;
    return ma_engine_get_volume(&gAudio.engine);
}

// ====================
// SAMPLE RATE HELPERS
// ====================

unsigned int Audio_GetSampleRate(AudioSound* snd)
{
    ma_uint32 sr = get_sample_rate_from_sound(snd);
    return (unsigned int)sr;
}

unsigned int Audio_GetEngineSampleRate(void)
{
    return (unsigned int)ma_engine_get_sample_rate(&gAudio.engine);
}










// ====================
// ADVANCED CONTROLS
// ====================

void Audio_SetSpeed(AudioSound* snd, float speed)
{
    if (!snd || !snd->isLoaded) return;
    // Speed changes playback rate and pitch together.
    ma_sound_set_pitch(&snd->sound, speed);
}

void Audio_SetPitch(AudioSound* snd, float pitch)
{
    if (!snd || !snd->isLoaded) return;
    // Pitch shifting only (miniaudio handles time stretching internally)
    ma_sound_set_pitch(&snd->sound, pitch);
}

void Audio_SetPan(AudioSound* snd, float pan)
{
    if (!snd || !snd->isLoaded) return;
    // pan: -1.0 = left, 0 = center, 1.0 = right
    ma_sound_set_pan(&snd->sound, pan);
}

void Audio_SetPosition3D(AudioSound* snd, float x, float y, float z)
{
    if (!snd || !snd->isLoaded) return;
    ma_sound_set_position(&snd->sound, x, y, z);
    ma_sound_set_spatialization_enabled(&snd->sound, MA_TRUE);
}

void Audio_UpdateListener3D(float x, float y, float z)
{
    if (!gAudio.initialized) return;
    ma_engine_listener_set_position(&gAudio.engine, 0, x, y, z);
}

// ====================
// SIMPLE EFFECTS
// ====================

void Audio_SetLowpass(AudioSound* snd, float cutoffFreq)
{
    if (!snd || !snd->isLoaded) return;
    // crude simulation: lower volume as cutoff decreases
    float factor = cutoffFreq / 10000.0f;
    if (factor < 0.1f) factor = 0.1f;
    if (factor > 1.0f) factor = 1.0f;
    ma_sound_set_volume(&snd->sound, factor);
}
























// ====================================
// AUDIO VISUALIZER (NODE GRAPH TAP NODE)
// ====================================

typedef struct {
    ma_node_base base;
    ma_uint32 channels;
    float lastVolume;
    float waveform[512];
    int waveformWritePos;
} AudioTapNode;

static void AudioTapNode_Process(ma_node* pNode,
    const float** ppInputFrames, ma_uint32* pFrameCountIn,
    float** ppOutputFrames, ma_uint32* pFrameCountOut)
{
    AudioTapNode* tap = (AudioTapNode*)pNode;
    ma_uint32 frameCount = *pFrameCountIn;

    // Pass-through audio (copy input -> output)
    for (ma_uint32 i = 0; i < frameCount * tap->channels; i++) {
        ppOutputFrames[0][i] = ppInputFrames[0][i];
    }

    // Compute RMS volume + capture waveform
    float sum = 0.0f;
    for (ma_uint32 i = 0; i < frameCount * tap->channels; i++) {
        float s = ppInputFrames[0][i];
        sum += s * s;
        tap->waveform[tap->waveformWritePos++ % 512] = s;
    }
    tap->lastVolume = sqrtf(sum / (float)(frameCount * tap->channels));
}

static ma_node_vtable AudioTapNode_VTable = {
    AudioTapNode_Process,
    NULL,
    1, // input buses
    1, // output buses
    MA_NODE_FLAG_CONTINUOUS_PROCESSING
};

static AudioTapNode gTapNode;
static ma_node_graph gGraph;
static bool gVisualizerEnabled = false;

int Audio_EnableVisualizer(void)
{
    if (!gAudio.initialized || gVisualizerEnabled) return false;

    ma_result result;
    ma_node_graph_config graphCfg = ma_node_graph_config_init(ma_engine_get_sample_rate(&gAudio.engine));
    result = ma_node_graph_init(&graphCfg, NULL, &gGraph);
    if (result != MA_SUCCESS) {
        printf("[Audio] Failed to init node graph.\n");
        return false;
    }

    // Create tap node
    ma_node_config tapCfg = ma_node_config_init();
    tapCfg.vtable = &AudioTapNode_VTable;
    tapCfg.pInputChannels = ma_engine_get_channels(&gAudio.engine);
    tapCfg.pOutputChannels = ma_engine_get_channels(&gAudio.engine);

    result = ma_node_init(&gGraph, &tapCfg, NULL, &gTapNode.base);
    if (result != MA_SUCCESS) {
        printf("[Audio] Failed to init tap node.\n");
        ma_node_graph_uninit(&gGraph, NULL);
        return false;
    }

    gTapNode.channels = ma_engine_get_channels(&gAudio.engine);
    gTapNode.waveformWritePos = 0;
    memset(gTapNode.waveform, 0, sizeof(gTapNode.waveform));

    // Connect tap node directly before the engine endpoint node
    ma_node_attach_output_bus(&gTapNode.base, 0, ma_engine_get_endpoint(&gAudio.engine), 0);

    gVisualizerEnabled = true;
    printf("[Audio] Visualizer node graph enabled.\n");
    return true;
}

void Audio_DisableVisualizer(void)
{
    if (!gVisualizerEnabled) return;
    ma_node_uninit(&gTapNode.base, NULL);
    ma_node_graph_uninit(&gGraph, NULL);
    gVisualizerEnabled = false;
    printf("[Audio] Visualizer node disabled.\n");
}

// ====================
// VISUALIZATION HELPERS
// ====================

float Audio_GetCurrentVolume(void)
{
    if (!gVisualizerEnabled) return 0.0f;
    return gTapNode.lastVolume;
}

int Audio_GetWaveform(float* outBuffer, int maxSamples)
{
    if (!gVisualizerEnabled || !outBuffer) return 0;
    int count = (maxSamples < 512) ? maxSamples : 512;
    for (int i = 0; i < count; i++) {
        outBuffer[i] = gTapNode.waveform[(gTapNode.waveformWritePos + i) % 512];
    }
    return count;
}



