/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */

#ifndef MIDI_H
#define MIDI_H

#include "pico/stdlib.h"

// Status bytes - only the left nibble is used,
//                right nibble is channel number
#define NOTE_OFF       0b10000000
#define NOTE_ON        0b10010000
#define CONTROL_CHANGE 0b10110000
#define BREATH_CC      2

void midi_setup(int8_t txpose);
void drain_midi_input();
int8_t get_transposition();
void set_transposition(int8_t txp);
void change_breath_cc(uint8_t new_breath);
void restart_current_note(uint8_t new_breath);
void start_note(uint8_t note, uint8_t breath);
void stop_current_note();

#endif