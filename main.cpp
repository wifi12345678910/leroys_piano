#include <iostream>
#include <cstdlib>
#include "teenyat.h"
#include "audio.c"
#include "graphics.c"

using namespace std;

const tny_uword STORE = 0x9000;
const tny_uword SETCOLOR = 0x9001;
const tny_uword DISPKEY = 0x9002;
const tny_uword PLAYKEY = 0x9003;
const tny_uword SETTONE = 0x9004;
const tny_uword GETKEY = 0x9006;

tny_uword notes[256];
tny_uword colors[256];

tny_uword curr_color = 0xFFFF;
tny_uword curr_note = 0x041;

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay);
void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay);

int main(int argc, char *argv[]){
    FILE *bin_file = fopen(argv[1], "rb");

    teenyat t;
	tny_init_from_file(&t, bin_file, bus_read, bus_write);

    for(;;) {
        tny_clock(&t);
    }
}


void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay) {
    switch(addr) {
        case STORE:
            notes[int(data.bytes.byte0)] = curr_note;
            colors[int(data.bytes.byte0)] = curr_color;
            break;
        case SETCOLOR:
            curr_color = data.u;
            break;
        case SETTONE:
            curr_note = data.u;
        case DISPKEY:
            light_key(int(data.bytes.byte0));
            break;
        case PLAYKEY:
            play_tone(int(data.bytes.byte0));
            break;
        default:
        break;
    }
}

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay) {
    switch (addr)
    {
    case GETKEY:
        
    }
}