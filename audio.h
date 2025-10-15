#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

// Audio system initialization and cleanup
void init_audio(void);
void cleanup_audio(void);

// Note playback functions
void play_note_sound(int frequency);
void play_note_with_duration(int frequency, int duration_ms);

// Audio system status
int is_audio_active(void);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_H