#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

// Core audio functions
void init_audio();
void cleanup_audio();
int is_audio_initialized();

// Basic sound playback
void play_sound(int frequency);
void play_sound_with_duration(int frequency, float duration);
void play_note(int frequency, float duration);

// Enhanced frequency functions
void play_frequency(int frequency, float duration);
void play_beep(int frequency);
void play_tone_with_type(int frequency, int wave_type, float duration);

// WAV file functions
void scan_sound_files();
int get_sound_count();
const char* get_sound_name_by_id(int sound_id);
void list_available_sounds();
void play_wav_file_by_id(int sound_id);
void play_wav_file_by_name(const char* filename);

// Combined audio functions
void play_sound_mixed(int frequency, int sound_id, float duration);
void play_key_sound(char key, int frequency, int wav_id, int wave_type);

// Audio control
void stop_all_sounds();
void set_master_volume(float volume);
void mute_audio(int mute);

// Legacy compatibility
void play_sound_by_id(int sound_id);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_H