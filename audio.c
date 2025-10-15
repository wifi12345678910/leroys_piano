#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "audio.h"
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SAMPLE_RATE 44100
#define CHANNELS 2

typedef struct {
    float frequency;
    float phase;
    float volume;
    int active;
    int duration_samples;
    int current_sample;
} Voice;

static ma_device device;
static Voice voice;
static int initialized = 0;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    float* output = (float*)pOutput;
    
    (void)pInput;
    (void)pDevice;
    
    for (ma_uint32 i = 0; i < frameCount; i++) {
        float sample = 0.0f;
        
        if (voice.active && voice.current_sample < voice.duration_samples) {
            // Simple envelope with fade in/out
            float envelope = 1.0f;
            int fade_samples = SAMPLE_RATE / 50; // 20ms fade
            
            if (voice.current_sample < fade_samples) {
                envelope = (float)voice.current_sample / fade_samples;
            } else if (voice.current_sample > voice.duration_samples - fade_samples) {
                envelope = (float)(voice.duration_samples - voice.current_sample) / fade_samples;
            }
            
            sample = sinf(voice.phase) * voice.volume * envelope * 0.3f;  // Reduced volume
            
            voice.phase += (2.0f * M_PI * voice.frequency) / SAMPLE_RATE;
            if (voice.phase > 2.0f * M_PI) {
                voice.phase -= 2.0f * M_PI;
            }
            
            voice.current_sample++;
        } else {
            voice.active = 0;
        }
        
        // Output to both channels
        output[i * CHANNELS] = sample;      // Left
        output[i * CHANNELS + 1] = sample;  // Right
    }
}

void init_audio(void) {
    if (initialized) {
        printf("Audio already initialized\n");
        return;
    }
    
    printf("Initializing audio system...\n");
    
    voice.active = 0;
    voice.frequency = 440.0f;
    voice.phase = 0.0f;
    voice.volume = 1.0f;
    voice.duration_samples = 0;
    voice.current_sample = 0;
    
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = ma_format_f32;
    deviceConfig.playback.channels = CHANNELS;
    deviceConfig.sampleRate        = SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("❌ Failed to initialize audio device\n");
        return;
    }
    
    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("❌ Failed to start audio device\n");
        ma_device_uninit(&device);
        return;
    }
    
    initialized = 1;
    printf("🔊 Audio system initialized successfully!\n");
}

void cleanup_audio(void) {
    if (initialized) {
        ma_device_uninit(&device);
        initialized = 0;
        printf("🔇 Audio system cleaned up\n");
    }
}

void play_note_sound(int frequency) {
    if (!initialized) {
        printf("❌ Audio not initialized - cannot play sound\n");
        return;
    }
    
    if (frequency <= 0 || frequency > 20000) {
        printf("❌ Invalid frequency: %d\n", frequency);
        return;
    }
    
    voice.frequency = (float)frequency;
    voice.phase = 0.0f;
    voice.volume = 1.0f;
    voice.duration_samples = SAMPLE_RATE / 2; // 500ms
    voice.current_sample = 0;
    voice.active = 1;
    
    printf("🎵 Playing %dHz\n", frequency);
}