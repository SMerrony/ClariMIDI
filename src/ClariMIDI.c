/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */

#include <limits.h>

#include "bsp/board.h"
// #include "hardware/clocks.h"
// #include "hardware/uart.h"
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

    // gpio_set_function(16, GPIO_FUNC_UART); //TX
    // gpio_set_function(17, GPIO_FUNC_UART); //RX
    board_init();

    
    tud_init(BOARD_TUD_RHPORT);
  
    // stdio_init_all();
    // printf("DEBUG: USB TUD Init done\n");

    display_setup();

    keys_setup();

    breath_setup();

    midi_setup(transpose);

    display_error("Setup complete", false);
    // printf("DEBUG: Setup complete\n");

    uint8_t curr_breath = 0;
    uint8_t last_breath = 0;
    uint8_t midi_note = 0;
    uint8_t last_midi_note = 0;

    uint32_t keys;

    // char debug[10];
    
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
            // printf("DEBUG: Breath set to %d\n", last_breath);
        }
        
        keys = read_keys();
        midi_note = decode_keys(keys);
        switch (midi_note) {
            case ENTER_MENU:
                main_menu();
                break;
            case MENU_DOWN:         // These two are meaningless unless we are inside the menu
            case MENU_SELECT:
                break;
            case UNKNOWN_FINGERING:
                // printf("DEBUG: Unknown fingering: %s\n", int2bin(keys, NULL));
                break;
            default:
                if (midi_note != last_midi_note) {
                    stop_current_note();
                    if (last_breath > 0) {
                        start_note(midi_note, last_breath);
                    }
                    // sprintf(debug, "%d", midi_note);
                    // display_message(debug);
                    last_midi_note = midi_note;
                    // printf("DEBUG: Last_note set to: %d\n", last_midi_note);
                }

        }

    }

    return -1; // should never happen
}
