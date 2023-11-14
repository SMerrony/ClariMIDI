/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */

#include <stdio.h>

#include "tusb.h"

#include "config.h"
#include "display.h"
#include "keys.h"
#include "midi.h"

#include "menu.h"

void menu_wait(uint32_t ms) {
    // must run the USB task 'often'
    tud_task();
    busy_wait_ms(ms);
}

uint8_t get_menu_key() {
    uint8_t key;
    while (true) {
        menu_wait(20);
        key = decode_keys(read_keys());
        switch (key) {
            case ENTER_MENU:
            case MENU_SELECT:
            case MENU_UP:
            case MENU_DOWN:
                return key;
            default:
                menu_wait(10);
        }
    }
}

char *common_transpositions(int8_t txp) {
    switch (txp) {
        case -21: return "Baritone Sax (EEb)";
        case -14: return "Bass Clar/Ten.Sax";
        case -12: return "Double Bass (8vb)";
        case -9: return  "Alto Sax/Clar (Eb)";
        case -7: return  "Basset Horn (F)";
        case -3: return  "A Clarinet";
        case -2: return  "Bb Clar/Sop Sax/Tpt";
        case 0: return   "C Clarinet (None)";
        case 2: return   "D Trumpet";
        case 3: return   "Eb Clarinet";
        case 12: return  "Piccolo (8va)";
        default: return  " ";  // not a common transposition
    }
}

void transpose_adjust() {
    uint8_t cmd;
    int8_t txp;
    char txp_s[4];
    while (true) {
        txp = get_transposition();
        sprintf(txp_s, "%+d", txp);
        display_string(48, 7, 2, txp_s);
        display_string(0, 25, 1, common_transpositions(txp));

        cmd = get_menu_key();

        display_clear();
        switch (cmd) {
            case MENU_UP:
                set_transposition(txp + 1);
                break;
            case MENU_DOWN:
                set_transposition(txp - 1);
                break;
            case MENU_SELECT:
                return;
        }
        menu_wait(MENU_KEY_REPEAT_MS);
    }


}

void transpose_menu() {
    int current_line = 1;
    uint8_t cmd;
    char curr_txp[4];

    while (true) {
        display_string(0, 7,  2, (current_line == 1) ? (">Adjust") : (" Adjust"));
        display_string(0, 25, 2, (current_line == 2) ? (">Reset") : (" Reset"));
        display_string(0, 45, 2, (current_line == 3) ? (">Exit") : (" Exit"));
        sprintf(curr_txp, "%+d", get_transposition());
        display_string(80, 48, 2, curr_txp);

        cmd = get_menu_key();

        display_clear();
        switch(cmd) {
            case MENU_UP:
                if (current_line > 1) current_line--;
                break;
            case MENU_DOWN:
                if (current_line < 3) current_line++;
                break;
            case MENU_SELECT:
                switch (current_line) {
                    case 1: 
                        transpose_adjust();
                        break;
                    case 2:
                        set_transposition(DEFAULT_TRANSPOSE);
                        break;
                    case 3:
                        return;
                }
        }
        menu_wait(MENU_KEY_REPEAT_MS);
    }
}

void main_menu() {
    int current_line = 1;
    uint8_t cmd;

    display_clear();
    while (true) {
        display_string(0, 7,  2, (current_line == 1) ? (">Transpose") : (" Transpose"));
        display_string(0, 25, 2, (current_line == 2) ? (">MIDI") : (" MIDI"));
        display_string(0, 45, 2, (current_line == 3) ? (">Exit") : (" Exit"));

        cmd = get_menu_key();

        display_clear();
        switch(cmd) {
            case MENU_UP:
                if (current_line > 1) current_line--;
                break;
            case MENU_DOWN:
                if (current_line < 3) current_line++;
                break;
            case MENU_SELECT:
                switch (current_line) {
                    case 1: 
                        transpose_menu();
                        break;
                    case 3:
                        return;
                }
        }
        menu_wait(MENU_KEY_REPEAT_MS);
    }

}