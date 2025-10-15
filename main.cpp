#include <iostream>
#include <cstdlib>
#include <map>
#include <set>
#include "../teenyat.h"
#include "audio.h"
#include "graphics.h"

using namespace std;

// Port addresses as specified in your design
const tny_uword GET_KEY = 0x9000;      // Read key pressed
const tny_uword PLAY_NOTE = 0x9001;    // Play note
const tny_uword SHOW_KEY = 0x9002;     // Show keypress visual
const tny_uword SET_KEYNOTE = 0x9003;  // Set/Get keynote
const tny_uword SET_KEYCOLOR = 0x9004; // Set/Get key color
const tny_uword SET_KEYLABEL = 0x9005; // Set/Get key label

// Piano system state
struct PianoSystem {
    char last_key_pressed = 0;
    bool key_available = false;
    char current_key_for_setup = 0;  // Track which key is being configured
    std::map<char, uint16_t> key_to_note;
    std::map<char, uint16_t> key_to_color;
    std::map<char, uint16_t> key_to_label;
    std::map<char, int> key_highlight_timers;  // Individual timers for each key
    std::set<char> currently_highlighted;      // Track which keys are highlighted
} piano_system;

void init_piano_system() {
    // Initialize default piano mapping
    piano_system.key_to_note['q'] = 261; // C4
    piano_system.key_to_note['w'] = 277; // C#4
    piano_system.key_to_note['e'] = 293; // D4
    piano_system.key_to_note['r'] = 311; // D#4
    piano_system.key_to_note['t'] = 329; // E4
    piano_system.key_to_note['y'] = 349; // F4
    piano_system.key_to_note['u'] = 369; // F#4
    piano_system.key_to_note['i'] = 391; // G4
    piano_system.key_to_note['o'] = 415; // G#4
    piano_system.key_to_note['p'] = 440; // A4
    piano_system.key_to_note['a'] = 466; // A#4
    piano_system.key_to_note['s'] = 493; // B4
    piano_system.key_to_note['d'] = 523; // C5
    piano_system.key_to_note['f'] = 554; // C#5
    piano_system.key_to_note['g'] = 587; // D5
    piano_system.key_to_note['h'] = 622; // D#5
    piano_system.key_to_note['j'] = 659; // E5
    piano_system.key_to_note['k'] = 698; // F5
    piano_system.key_to_note['l'] = 740; // F#5
    piano_system.key_to_note['z'] = 783; // G5
    piano_system.key_to_note['x'] = 831; // G#5
    piano_system.key_to_note['c'] = 880; // A5
    piano_system.key_to_note['v'] = 932; // A#5
    piano_system.key_to_note['b'] = 988; // B5
    piano_system.key_to_note['n'] = 1047; // C6
    
    init_graphics();
    init_audio();
    
    cout << "🎹 Leroy's Piano System Ready!" << endl;
}

void check_keyboard_input() {
    char key = get_key_input();
    if (key != 0) {
        piano_system.last_key_pressed = key;
        piano_system.key_available = true;
        
        // Set individual highlight timer for this key
        piano_system.key_highlight_timers[key] = 30;  // 30 frames highlight
        piano_system.currently_highlighted.insert(key);
        
        set_key_pressed(key, true);
        set_key_color(key, 255, 255, 255); // White highlight
        
        // Keep this useful debug output
        cout << "🎹 Key '" << key << "' pressed (ASCII " << (int)key << ")" << endl;
    }
}

void update_key_highlights() {
    // Update all key highlight timers
    for (auto it = piano_system.key_highlight_timers.begin(); 
         it != piano_system.key_highlight_timers.end();) {
        
        it->second--;  // Decrease timer
        
        if (it->second <= 0) {
            // Timer expired - turn off highlight
            char key = it->first;
            set_key_pressed(key, false);
            piano_system.currently_highlighted.erase(key);
            
            it = piano_system.key_highlight_timers.erase(it);  // Remove timer
        } else {
            ++it;
        }
    }
}

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay) {
    switch(addr) {
        case GET_KEY:
            check_keyboard_input();
            if (piano_system.key_available) {
                data->u = piano_system.last_key_pressed;
                piano_system.key_available = false;
            } else {
                data->u = 0;
            }
            break;
            
        case SET_KEYNOTE:
            if (piano_system.current_key_for_setup && 
                piano_system.key_to_note.find(piano_system.current_key_for_setup) != piano_system.key_to_note.end()) {
                data->u = piano_system.key_to_note[piano_system.current_key_for_setup];
            } else {
                data->u = 0;
            }
            break;
            
        case SET_KEYCOLOR:
            if (piano_system.current_key_for_setup && 
                piano_system.key_to_color.find(piano_system.current_key_for_setup) != piano_system.key_to_color.end()) {
                data->u = piano_system.key_to_color[piano_system.current_key_for_setup];
            } else {
                data->u = 0xFFFF;
            }
            break;
            
        case SET_KEYLABEL:
            data->u = piano_system.current_key_for_setup;
            break;
            
        default:
            data->u = 0;
            break;
    }
    *delay = 1;
}

void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay) {
    switch(addr) {
        case PLAY_NOTE:
            {
                // Fixed frequency calculation!
                // Assembly sends frequency multiplier, we calculate actual frequency
                int freq_mult = data.u & 0x0F;  // Get low 4 bits for frequency
                int sound_type = (data.u >> 4) & 0x0F;  // Get high 4 bits for sound type
                int frequency = 220 + (freq_mult * 55);  // Calculate actual frequency
                
                cout << "🎵 Playing " << frequency << "Hz (type " << sound_type << ", raw: " << data.u << ")" << endl;
                play_note_sound(frequency);  // Play the calculated frequency, not raw value!
            }
            break;
            
        case SHOW_KEY:
            {
                char key = (char)data.u;  // Simple: assembly just sends the key character
                
                // Set individual highlight timer for this key
                piano_system.key_highlight_timers[key] = 50;  // 50 frames highlight
                piano_system.currently_highlighted.insert(key);
                set_key_pressed(key, true);
                
                // Check if this key has a programmed color
                int r = 255, g = 255, b = 255;  // Default white
                const char* color_name = "White";
                
                if (piano_system.key_to_color.find(key) != piano_system.key_to_color.end()) {
                    uint16_t color_val = piano_system.key_to_color[key];
                    // Extract RGB from packed byte (RGB332 format)
                    r = ((color_val >> 5) & 0x07) * 36;  // 3 bits for red
                    g = ((color_val >> 2) & 0x07) * 36;  // 3 bits for green  
                    b = (color_val & 0x03) * 85;         // 2 bits for blue
                    color_name = "Programmed";
                }
                
                set_key_color(key, r, g, b);
                cout << "🎨 Highlighting '" << key << "' in " << color_name << endl;
            }
            break;
            
        case SET_KEYNOTE:
            // If this is a character (< 256), it's selecting which key to configure
            if (data.u < 256) {
                piano_system.current_key_for_setup = (char)data.u;
                cout << "🎼 Selected key '" << piano_system.current_key_for_setup << "' for configuration" << endl;
            } else {
                // It's setting the frequency for the selected key
                if (piano_system.current_key_for_setup) {
                    piano_system.key_to_note[piano_system.current_key_for_setup] = data.u;
                    cout << "🎼 Set key '" << piano_system.current_key_for_setup << "' to " << data.u << "Hz" << endl;
                }
            }
            break;
            
        case SET_KEYCOLOR:
            if (piano_system.current_key_for_setup) {
                piano_system.key_to_color[piano_system.current_key_for_setup] = data.u;
                
                // Extract RGB from packed byte
                int r = ((data.u >> 5) & 0x07) * 36;  // 3 bits for red (0-7 -> 0-252)
                int g = ((data.u >> 2) & 0x07) * 36;  // 3 bits for green
                int b = (data.u & 0x03) * 85;         // 2 bits for blue (0-3 -> 0-255)
                
                cout << "🎨 Set key '" << piano_system.current_key_for_setup 
                     << "' color to RGB(" << r << "," << g << "," << b << ")" << endl;
            }
            break;
            
        case SET_KEYLABEL:
            if (piano_system.current_key_for_setup) {
                piano_system.key_to_label[piano_system.current_key_for_setup] = data.u;
                cout << "🏷️ Set key '" << piano_system.current_key_for_setup 
                     << "' label to '" << (char)data.u << "'" << endl;
            }
            break;
            
        default:
            break;
    }
    *delay = 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "🎹 Leroy's Piano System" << endl;
        cout << "Usage: " << argv[0] << " <assembly_program.bin>" << endl;
        return 1;
    }
    
    FILE *bin_file = fopen(argv[1], "rb");
    if (!bin_file) {
        cout << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }

    // Initialize the complete piano system
    init_piano_system();

    teenyat t;
    tny_init_from_file(&t, bin_file, bus_read, bus_write);
    fclose(bin_file);

    cout << "🎵 Starting piano with " << argv[1] << endl;

    // Main execution loop
    while (graphics_active()) {
        // Execute TeenyAT instruction
        tny_clock(&t);
        
        // Update individual key highlight timers
        update_key_highlights();
        
        update_graphics();
        
        // Small delay
        for (volatile int i = 0; i < 5000; i++);
    }

    cleanup_graphics();
    cleanup_audio();
    cout << "🎹 Leroy's Piano System Stopped." << endl;
    return 0;
}