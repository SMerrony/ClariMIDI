/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */

#include <limits.h>

#include "bsp/board.h"
// #include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "tusb.h"

#include "breath.h"
#include "config.h"
#include "display.h"
#include "keys.h"
#include "menu.h"
#include "midi.h"

char *int2bin(unsigned n, char *buf) {
    #define BITS (sizeof(n) * CHAR_BIT)

    static char static_buf[BITS + 1];
    int i;

    if (buf == NULL) buf = static_buf;

    for (i = BITS - 1; i >= 0; --i) {
        buf[i] = (n & 1) ? '1' : '0';
        n >>= 1;
    }

    buf[BITS] = '\0';
    return buf;

    #undef BITS
}

int main() {

    uint8_t transpose = DEFAULT_TRANSPOSE;

    board_init();
    
    tud_init(BOARD_TUD_RHPORT);
  
    display_setup();

    keys_setup();

    breath_setup();

    midi_setup(transpose);

    display_error("Setup complete", false);

    uint8_t curr_breath = 0;
    uint8_t last_breath = 0;
    uint8_t midi_note = 0;
    uint8_t last_midi_note = 0;

    uint32_t keys;

    char debug_msg[80]; // just a convenience while debugging
    
    while (true) {

        tud_task();

        drain_midi_input(); // throw away any incoming MIDI

        curr_breath = breath_read();
        if (curr_breath != last_breath) {
            if (curr_breath == 0) { // stop sound if below threshold
                stop_current_note();
            } else {
                if (last_breath == 0) {  // restart the sound if risen above threshold
                    restart_current_note(curr_breath);
                } else {
                    change_breath_cc(curr_breath);
                }
            }
            last_breath = curr_breath;
            sprintf(debug_msg, "Breath set to %d\n", last_breath);
        }
        
        keys = read_keys();
        midi_note = decode_keys(keys);
        switch (midi_note) {
            case ENTER_MENU:
                main_menu();
                break;
            case MENU_DOWN:         // These two are meaningless unless we are inside the menu
            case MENU_SELECT:
            case UNKNOWN_FINGERING:
                sprintf(debug_msg, "Unknown fingering: %s\n", int2bin(keys, NULL));
                display_message(debug_msg);
                break;
            default:
                if (midi_note != last_midi_note) {
                    stop_current_note();
                    // if (last_breath > 0) {
                        start_note(midi_note, last_breath);
                    // }
                    last_midi_note = midi_note;
                    sprintf(debug_msg, "Last_note set to: %d\n", last_midi_note);
                    display_message(debug_msg);
                }
        }
    }

    return -1; // should never happen
}
