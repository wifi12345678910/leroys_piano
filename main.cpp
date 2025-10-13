#include <iostream>
#include <cstdlib>
#include "teenyat.h"
#include "audio.c"
#include "graphics.c"

using namespace std;

const tny_uword SETNOTE = 0x9000;
const tny_uword SETCOLOR = 0x9001;
const tny_uword DISPKEY = 0x9002;
const tny_uword PLAYKEY = 0x9003;
const tny_uword SETTONE = 0x9004;
const tny_uword GETKEY = 0x9005;



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
        case SETNOTE:
        case SETCOLOR:
        case DISPKEY:
        case PLAYKEY:
        case SETTONE:
    }
}

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay) {
    switch (addr)
    {
    case GETKEY:
    }
}