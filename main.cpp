#include <iostream>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <vector>
#include <cstdio>
#include <windows.h>
#include "../teenyat.h"
#include "audio.h"
#include "graphics.h"


using namespace std;

// Enhanced I/O Port addresses
const tny_uword GET_KEY = 0x9000;           // Read key pressed
const tny_uword PLAY_FREQUENCY = 0x9001;    // Play frequency/beep sound
const tny_uword PLAY_WAV_ID = 0x9002;       // Play WAV file by ID
const tny_uword SHOW_KEY = 0x9003;          // Visual feedback + sound
const tny_uword SET_KEY_FREQ = 0x9004;      // Map key to frequency
const tny_uword SET_KEY_WAV = 0x9005;       // Map key to WAV file
const tny_uword SET_KEY_COLOR = 0x9006;     // Set key color
const tny_uword GET_WAV_COUNT = 0x9007;     // Get number of WAV files
const tny_uword LIST_WAVS = 0x9008;         // List available WAV files
const tny_uword PLAY_COMBINED = 0x9009;     // Play frequency + WAV together
const tny_uword SET_KEY_MODE = 0x900A;      // Set key audio mode

// Enhanced piano state
struct EnhancedPianoState {
    char last_key_pressed = 0;
    bool key_available = false;
    char current_key_for_setup = 0;
    
    // Audio mappings
    std::map<char, uint16_t> key_to_frequency;     // Key → frequency mapping
    std::map<char, int> key_to_wav_id;             // Key → WAV file ID mapping
    std::map<char, int> key_to_audio_mode;         // Key → audio mode (0=freq, 1=wav, 2=both)
    std::map<char, int> key_to_wave_type;          // Key → waveform type for frequency
    
    // Visual mappings
    std::map<char, uint32_t> key_to_color;         // Key → color mapping
    std::map<char, int> key_highlight_timers;      // Key highlight timers
    std::set<char> currently_highlighted;          // Currently highlighted keys
    
    // System state
    int current_time = 0;
} piano_state;

void init_enhanced_piano_system() {
    cout << "Initializing Enhanced Dual-Audio Piano System..." << endl;
    /* Don't need default frequency mappings anymore
    // Default frequency mappings (piano scale)
    piano_state.key_to_frequency['q'] = 261;  // C4
    piano_state.key_to_frequency['w'] = 277;  // C#4  
    piano_state.key_to_frequency['e'] = 293;  // D4
    piano_state.key_to_frequency['r'] = 311;  // D#4
    piano_state.key_to_frequency['t'] = 329;  // E4
    piano_state.key_to_frequency['y'] = 349;  // F4
    piano_state.key_to_frequency['u'] = 369;  // F#4
    piano_state.key_to_frequency['i'] = 391;  // G4
    piano_state.key_to_frequency['o'] = 415;  // G#4
    piano_state.key_to_frequency['p'] = 440;  // A4
    
    piano_state.key_to_frequency['a'] = 466;  // A#4
    piano_state.key_to_frequency['s'] = 493;  // B4
    piano_state.key_to_frequency['d'] = 523;  // C5
    piano_state.key_to_frequency['f'] = 554;  // C#5
    piano_state.key_to_frequency['g'] = 587;  // D5
    piano_state.key_to_frequency['h'] = 622;  // D#5
    piano_state.key_to_frequency['j'] = 659;  // E5
    piano_state.key_to_frequency['k'] = 698;  // F5
    piano_state.key_to_frequency['l'] = 740;  // F#5
    
    piano_state.key_to_frequency['z'] = 783;  // G5
    piano_state.key_to_frequency['x'] = 831;  // G#5
    piano_state.key_to_frequency['c'] = 880;  // A5  
    piano_state.key_to_frequency['v'] = 932;  // A#5
    piano_state.key_to_frequency['b'] = 988;  // B5
    piano_state.key_to_frequency['n'] = 1047; // C6
    piano_state.key_to_frequency['m'] = 1109; // C#6
    
    // Default audio modes (all start as frequency-based)
    const char* keys = "qwertyuiopasdfghjklzxcvbnm";
    for (int i = 0; keys[i]; i++) {
        piano_state.key_to_audio_mode[keys[i]] = 0;  // 0 = frequency mode
        piano_state.key_to_wave_type[keys[i]] = 0;   // 0 = sine wave
    }*/
    
    // Initialize systems
    init_graphics();
    init_audio();
    
    cout << "WAV files available: " << get_sound_count() << endl;
    if (get_sound_count() > 0) {
        list_available_sounds();
    }
    
    cout << "Enhanced Piano System Ready!" << endl;
    cout << "Frequency support: (unlimited frequencies)" << endl;
    cout << "WAV file support: " << (get_sound_count() > 0 ? "Yup" : "No") 
         << " (" << get_sound_count() << " files)" << endl;
    cout << "Color support:  (RGB332 format)" << endl;
    cout << "Key mapping:  (26 keys available)" << endl;
}


void playLetterSound(char letter) {
    if ((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z')) {
        if (letter >= 'a' && letter <= 'z')
            letter -= 32; // make uppercase

        string path = "sounds/alphabet/";
        path += letter;
        path += ".wav";

        cout << "[AutoSound] Attempting to play: " << path << endl;

        cout << "[AutoSound] Found file: " << path << endl;
        PlaySound(path.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    } else {
        cout << "[AutoSound] Ignored key: " << (int)letter << endl;
    }
}


void check_keyboard_input() {
    char key = get_key_input();
    if (key != 0) {
        piano_state.last_key_pressed = key;
        piano_state.key_available = true;
        
        // Visual feedback
        piano_state.key_highlight_timers[key] = 30;
        piano_state.currently_highlighted.insert(key);
        set_key_pressed(key, true);
        set_key_color(key, 255, 255, 255);
        
        cout << "Key '" << key << "' pressed (ASCII " << (int)key << ")" << endl;

        // FOR ALPHABET KEYBOARD
        
        //playLetterSound(key);
    }
}

void update_piano_state() {
    piano_state.current_time++;
    
    // Update key highlight timers
    for (auto it = piano_state.key_highlight_timers.begin(); 
         it != piano_state.key_highlight_timers.end();) {
        
        it->second--;
        
        if (it->second <= 0) {
            char key = it->first;
            set_key_pressed(key, false);
            piano_state.currently_highlighted.erase(key);
            it = piano_state.key_highlight_timers.erase(it);
        } else {
            ++it;
        }
    }
}

// 🪄 Wrapper hook that watches for key events and plays alphabet sounds
void monitor_keyboard_for_letters() {
    // If a key has just been pressed, play its alphabet sound
    if (piano_state.key_available) {
        char letter = piano_state.last_key_pressed;
        playLetterSound(letter);
        piano_state.key_available = false; // mark as handled
    }
}


// Enhanced TeenyAT bus read callback
void piano_bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay) {
    *delay = 0;
    
    switch(addr) {
        case GET_KEY:
            check_keyboard_input();
            if (piano_state.key_available) {
                data->u = piano_state.last_key_pressed;
                piano_state.key_available = false;
                cout << "Assembly read key: '" << (char)data->u << "'" << endl;
            } else {
                data->u = 0;
            }
            break;
            
        case GET_WAV_COUNT:
            data->u = get_sound_count();
            cout << "Assembly read WAV count: " << data->u << endl;
            break;
            
        default:
            data->u = 0;
            break;
    }
}

// Enhanced TeenyAT bus write callback  
void piano_bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay) {
    *delay = 0;
    
    switch(addr) {
        case PLAY_FREQUENCY:
            {
                // Format: [wave_type:4][duration:4][frequency:8]
                int frequency_code = data.u & 0xFF;           // Bottom 8 bits
                int duration_code = (data.u >> 8) & 0xF;      // Next 4 bits  
                int wave_type = (data.u >> 12) & 0xF;         // Top 4 bits
                
                // Map frequency code to actual frequency
                int actual_freq = 220 + (frequency_code * 10); // 220Hz to 2770Hz range
                float duration = 0.1f + (duration_code * 0.1f); // 0.1s to 1.6s
                
                cout << "PLAY_FREQUENCY: " << actual_freq << "Hz, " 
                     << duration << "s, wave_type=" << wave_type << endl;
                
                play_tone_with_type(actual_freq, wave_type, duration);
            }
            break;
            
        case PLAY_WAV_ID:
            {
                int wav_id = data.u;
                cout << "PLAY_WAV_ID: " << wav_id;
                if (wav_id < get_sound_count()) {
                    cout << " (" << get_sound_name_by_id(wav_id) << ")" << endl;
                    play_wav_file_by_id(wav_id);
                } else {
                    cout << " (INVALID - max " << (get_sound_count()-1) << ")" << endl;
                    play_beep(440); // Error beep
                }
            }
            break;
            
        case SHOW_KEY:
            {
                char key = (char)data.u;
                cout << "SHOW_KEY: '" << key << "'" << endl;
                
                // Visual feedback
                piano_state.key_highlight_timers[key] = 50;
                piano_state.currently_highlighted.insert(key);
                set_key_pressed(key, true);
                
                // Apply custom color if set
                int r = 255, g = 255, b = 255;
                const char* color_name = "White";
                
                if (piano_state.key_to_color.find(key) != piano_state.key_to_color.end()) {
                    uint32_t color_val = piano_state.key_to_color[key];
                    r = ((color_val >> 5) & 0x07) * 36;  // Extract red (3 bits)
                    g = ((color_val >> 2) & 0x07) * 36;  // Extract green (3 bits)  
                    b = (color_val & 0x03) * 85;         // Extract blue (2 bits)
                    color_name = "Custom";
                }
                
                set_key_color(key, r, g, b);
                cout << "Color: " << color_name << " RGB(" << r << "," << g << "," << b << ")" << endl;
                
                // Play sound based on key's audio mode
                int audio_mode = 0; // Default to frequency
                if (piano_state.key_to_audio_mode.find(key) != piano_state.key_to_audio_mode.end()) {
                    audio_mode = piano_state.key_to_audio_mode[key];
                }
                
                switch(audio_mode) {
                    case 0: // Frequency mode
                        if (piano_state.key_to_frequency.find(key) != piano_state.key_to_frequency.end()) {
                            int freq = piano_state.key_to_frequency[key];
                            int wave_type = piano_state.key_to_wave_type[key];
                            cout << "Playing frequency: " << freq << "Hz (wave_type=" << wave_type << ")" << endl;
                            play_tone_with_type(freq, wave_type, 0.3f);
                        }
                        break;
                        
                    case 1: // WAV mode
                        if (piano_state.key_to_wav_id.find(key) != piano_state.key_to_wav_id.end()) {
                            int wav_id = piano_state.key_to_wav_id[key];
                            cout << "Playing WAV: " << get_sound_name_by_id(wav_id) << endl;
                            play_wav_file_by_id(wav_id);
                        }
                        break;
                        
                    case 2: // Both frequency + WAV
                        if (piano_state.key_to_frequency.find(key) != piano_state.key_to_frequency.end() &&
                            piano_state.key_to_wav_id.find(key) != piano_state.key_to_wav_id.end()) {
                            int freq = piano_state.key_to_frequency[key];
                            int wav_id = piano_state.key_to_wav_id[key];
                            cout << "Playing BOTH: " << freq << "Hz + " << get_sound_name_by_id(wav_id) << endl;
                            play_sound_mixed(freq, wav_id, 0.3f);
                        }
                        break;
                }
            }
            break;
            
        case SET_KEY_FREQ:
            // First call selects key, second call sets frequency
            if (piano_state.current_key_for_setup == 0) {
                piano_state.current_key_for_setup = (char)data.u;
                cout << "Selected key '" << piano_state.current_key_for_setup << "' for configuration" << endl;
            } else {
                piano_state.key_to_frequency[piano_state.current_key_for_setup] = data.u;
                cout << "Set key '" << piano_state.current_key_for_setup 
                     << "' frequency to " << data.u << "Hz" << endl;
                piano_state.current_key_for_setup = 0; // Reset selection
            }
            break;
            
        case SET_KEY_WAV:
            if (piano_state.current_key_for_setup != 0) {
                if (data.u < get_sound_count()) {
                    piano_state.key_to_wav_id[piano_state.current_key_for_setup] = data.u;
                    cout << "Set key '" << piano_state.current_key_for_setup 
                         << "' WAV to " << data.u << " (" << get_sound_name_by_id(data.u) << ")" << endl;
                } else {
                    cout << "Invalid WAV ID " << data.u << " for key '" 
                         << piano_state.current_key_for_setup << "'" << endl;
                }
                piano_state.current_key_for_setup = 0; // Reset selection
            } else {
                cout << "No key selected for WAV mapping" << endl;
            }
            break;
            
        case SET_KEY_COLOR:
            if (piano_state.current_key_for_setup != 0) {
                piano_state.key_to_color[piano_state.current_key_for_setup] = data.u & 0xFF;
                
                int r = ((data.u >> 5) & 0x07) * 36;
                int g = ((data.u >> 2) & 0x07) * 36;  
                int b = (data.u & 0x03) * 85;
                
                cout << "Set key '" << piano_state.current_key_for_setup 
                     << "' color to RGB(" << r << "," << g << "," << b << ") [0x" 
                     << hex << (data.u & 0xFF) << dec << "]" << endl;
                piano_state.current_key_for_setup = 0; // Reset selection
            } else {
                cout << "No key selected for color mapping" << endl;
            }
            break;
            
        case SET_KEY_MODE:
            // Format: [key:8][mode:4][wave_type:4]
            {
                char key = (char)(data.u & 0xFF);
                int mode = (data.u >> 8) & 0xF;
                int wave_type = (data.u >> 12) & 0xF;
                
                piano_state.current_key_for_setup = key; // Auto-select key
                piano_state.key_to_audio_mode[key] = mode;
                piano_state.key_to_wave_type[key] = wave_type;
                
                const char* mode_names[] = {"Frequency", "WAV", "Both", "Reserved"};
                cout << "Set key '" << key << "' mode to " << mode_names[mode % 4] 
                     << " (wave_type=" << wave_type << ")" << endl;
            }
            break;
            
        case PLAY_COMBINED:
            // Format: [wav_id:8][frequency_code:8]
            {
                int frequency_code = data.u & 0xFF;
                int wav_id = (data.u >> 8) & 0xFF;
                int actual_freq = 220 + (frequency_code * 10);
                
                cout << "PLAY_COMBINED: " << actual_freq << "Hz + WAV " << wav_id << endl;
                play_sound_mixed(actual_freq, wav_id, 0.5f);
            }
            break;
            
        case LIST_WAVS:
            cout << "=== Assembly requested WAV list ===" << endl;
            list_available_sounds();
            cout << "=== End of WAV list ===" << endl;
            break;
            
        default:
            cout << "Unknown I/O address: 0x" << hex << addr << dec << endl;
            break;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Enhanced Dual-Audio Piano System for TeenyAT" << endl;
        cout << "Usage: " << argv[0] << " <assembly_program.bin>" << endl;
        cout << endl;
        cout << "Enhanced I/O Ports:" << endl;
        cout << "  0x9000 - GET_KEY (read keyboard input)" << endl;
        cout << "  0x9001 - PLAY_FREQUENCY (play frequency/beep with wave type)" << endl;
        cout << "  0x9002 - PLAY_WAV_ID (play WAV file by ID)" << endl;
        cout << "  0x9003 - SHOW_KEY (visual + audio feedback)" << endl;
        cout << "  0x9004 - SET_KEY_FREQ (map key to frequency)" << endl;
        cout << "  0x9005 - SET_KEY_WAV (map key to WAV file)" << endl;
        cout << "  0x9006 - SET_KEY_COLOR (set key color RGB332)" << endl;
        cout << "  0x9007 - GET_WAV_COUNT (read number of WAV files)" << endl;
        cout << "  0x9008 - LIST_WAVS (display available WAV files)" << endl;
        cout << "  0x9009 - PLAY_COMBINED (play frequency + WAV together)" << endl;
        cout << "  0x900A - SET_KEY_MODE (set key audio mode)" << endl;
        return 1;
    }
    
    // Open binary file
    FILE *bin_file = fopen(argv[1], "rb");
    if (!bin_file) {
        cout << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }

    // Initialize enhanced system
    init_enhanced_piano_system();

    // Initialize TeenyAT
    teenyat t;
    bool init_success = tny_init_from_file(&t, bin_file, piano_bus_read, piano_bus_write);
    fclose(bin_file);
    
    if (!init_success) {
        cout << "Error: Failed to initialize TeenyAT" << endl;
        return 1;
    }

    cout << "Starting Enhanced Dual-Audio Piano with " << argv[1] << endl;
    cout << "Assembly programmers can now use frequencies AND WAV files!" << endl << endl;

    // Main execution loop
    while (graphics_active()) {
        tny_clock(&t);
        update_piano_state();
        update_graphics();

        // monitor_keyboard_for_letters();
        
        
        for (volatile int i = 0; i < 1000; i++);
    }

    cleanup_graphics();
    cleanup_audio();
    cout << "Enhanced Dual-Audio Piano System stopped." << endl;
    return 0;
}