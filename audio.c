#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

// Audio state
static ma_engine engine;
static int audio_initialized = 0;
static int use_miniaudio = 0;
static int audio_muted = 0;

// WAV file registry
#define MAX_SOUNDS 256
static char sound_registry[MAX_SOUNDS][256];
static int num_registered_sounds = 0;

void init_audio() {
    if (audio_initialized) return;
    
    printf("Initializing Enhanced Dual-Audio System...\n");
    
    // Try miniaudio first
    ma_engine_config engineConfig = ma_engine_config_init();
    ma_result result = ma_engine_init(&engineConfig, &engine);
    
    if (result == MA_SUCCESS) {
        use_miniaudio = 1;
        printf("Miniaudio engine initialized\n");
    } else {
        use_miniaudio = 0;
        printf("Miniaudio failed (error: %d), using Windows Beep fallback\n", result);
    }
    
    // Scan for WAV files
    scan_sound_files();
    
    audio_initialized = 1;
    
    // Test audio system
    printf("Testing audio system...\n");
    printf("Testing Windows Beep...\n");
#ifdef _WIN32
    Beep(440, 300);
#endif
    printf("   Audio test complete\n");
    
    printf("Enhanced Dual-Audio System Ready!\n");
    printf("   - Windows Beep: Always available\n");
    printf("   - Miniaudio: %s\n", use_miniaudio ? " Available" : " Fallback mode");
    printf("   - WAV files: %d found\n", num_registered_sounds);
}

void scan_sound_files() {
#ifdef _WIN32
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    
    CreateDirectory("sounds", NULL);
    hFind = FindFirstFile("sounds\\*.wav", &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No WAV files found in sounds/ folder\n");
        printf("Put .wav files in sounds/ folder for WAV audio support\n");
        return;
    }
    
    printf("🎵 Scanning sounds folder...\n");
    num_registered_sounds = 0;
    
    do {
        if (num_registered_sounds >= MAX_SOUNDS) break;
        
        strcpy(sound_registry[num_registered_sounds], findFileData.cFileName);
        printf("   %d: %s\n", num_registered_sounds, findFileData.cFileName);
        num_registered_sounds++;
        
    } while (FindNextFile(hFind, &findFileData) != 0 && num_registered_sounds < MAX_SOUNDS);
    
    FindClose(hFind);
    printf("🎵 Found %d WAV files\n", num_registered_sounds);
#endif
}

// Frequency-based audio functions
void play_frequency(int frequency, float duration) {
    if (audio_muted) return;
    
    printf("Playing %dHz for %.1fs\n", frequency, duration);
    
#ifdef _WIN32
    if (frequency >= 37 && frequency <= 32767) {
        int duration_ms = (int)(duration * 1000);
        Beep(frequency, duration_ms);
    } else {
        printf("→ Frequency out of range, using 440Hz\n");
        Beep(440, (int)(duration * 1000));
    }
#endif
}

void play_beep(int frequency) {
    play_frequency(frequency, 0.3f);
}

void play_tone_with_type(int frequency, int wave_type, float duration) {
    printf("Playing %dHz (wave_type=%d) for %.1fs\n", frequency, wave_type, duration);
    
    // For different wave types, we can modify the frequency slightly or use different durations
    switch(wave_type % 4) {
        case 0: // Sine - normal
            play_frequency(frequency, duration);
            break;
        case 1: // Square - shorter, punchier
            play_frequency(frequency, duration * 0.8f);
            break;
        case 2: // Triangle - slightly higher pitch
            play_frequency((int)(frequency * 1.1f), duration);
            break;
        case 3: // Sawtooth - lower pitch
            play_frequency((int)(frequency * 0.9f), duration);
            break;
    }
}

// WAV file functions
void play_wav_file_by_id(int sound_id) {
    if (audio_muted) return;
    
    printf("Playing WAV ID %d\n", sound_id);
    
    if (sound_id < 0 || sound_id >= num_registered_sounds) {
        printf("Invalid WAV ID: %d (available: 0-%d)\n", sound_id, num_registered_sounds-1);
        play_beep(220);
        return;
    }
    
    printf("→ Playing: %s\n", sound_registry[sound_id]);
    
    if (use_miniaudio) {
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "sounds\\%s", sound_registry[sound_id]);
        
        ma_result result = ma_engine_play_sound(&engine, filepath, NULL);
        if (result == MA_SUCCESS) {
            printf("WAV played successfully\n");
        } else {
            printf("WAV playback failed (error: %d), using beep\n", result);
            play_beep(440 + (sound_id * 100));
        }
    } else {
        printf("   → Miniaudio unavailable, playing beep substitute\n");
        play_beep(440 + (sound_id * 100));
    }
}

void play_wav_file_by_name(const char* filename) {
    for (int i = 0; i < num_registered_sounds; i++) {
        if (strcmp(sound_registry[i], filename) == 0) {
            play_wav_file_by_id(i);
            return;
        }
    }
    
    printf("WAV file '%s' not found\n", filename);
    play_beep(220);
}

// Combined functions
void play_sound_mixed(int frequency, int sound_id, float duration) {
    printf("Playing MIXED: %dHz + WAV %d for %.1fs\n", frequency, sound_id, duration);
    
    // Play frequency
    play_frequency(frequency, duration);
    
    // Brief pause
#ifdef _WIN32
    Sleep(50);
#endif
    
    // Play WAV if valid
    if (sound_id >= 0 && sound_id < num_registered_sounds) {
        play_wav_file_by_id(sound_id);
    }
}

void play_key_sound(char key, int frequency, int wav_id, int wave_type) {
    printf("Key '%c': freq=%dHz, wav=%d, wave_type=%d\n", key, frequency, wav_id, wave_type);
    
    if (wav_id >= 0 && wav_id < num_registered_sounds) {
        play_sound_mixed(frequency, wav_id, 0.3f);
    } else {
        play_tone_with_type(frequency, wave_type, 0.3f);
    }
}

// Information functions
int get_sound_count() {
    return num_registered_sounds;
}

const char* get_sound_name_by_id(int sound_id) {
    if (sound_id < 0 || sound_id >= num_registered_sounds) {
        return "Invalid";
    }
    return sound_registry[sound_id];
}

void list_available_sounds() {
    printf("=== AVAILABLE WAV FILES ===\n");
    if (num_registered_sounds == 0) {
        printf("   No WAV files found\n");
        printf("   Put .wav files in sounds/ folder\n");
    } else {
        for (int i = 0; i < num_registered_sounds; i++) {
            printf("   ID %d: %s\n", i, sound_registry[i]);
        }
    }
    printf("=== END OF LIST ===\n");
}

// Control functions
void stop_all_sounds() {
    if (use_miniaudio) {
        ma_engine_stop(&engine);
        printf("All sounds stopped\n");
    }
}

void set_master_volume(float volume) {
    if (use_miniaudio) {
        ma_engine_set_volume(&engine, volume);
        printf("Volume set to %.1f\n", volume);
    }
}

void mute_audio(int mute) {
    audio_muted = mute;
    printf("Audio %s\n", mute ? "muted" : "unmuted");
}

int is_audio_initialized() {
    return audio_initialized;
}

// Legacy compatibility functions
void play_sound(int frequency) {
    play_frequency(frequency, 0.3f);
}

void play_sound_with_duration(int frequency, float duration) {
    play_frequency(frequency, duration);
}

void play_note(int frequency, float duration) {
    play_frequency(frequency, duration);
}

void play_sound_by_id(int sound_id) {
    play_wav_file_by_id(sound_id);
}

void cleanup_audio() {
    if (audio_initialized && use_miniaudio) {
        ma_engine_uninit(&engine);
        printf("Enhanced audio system cleaned up\n");
    }
    audio_initialized = 0;
}