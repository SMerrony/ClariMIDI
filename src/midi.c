/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */

#include "midi.h"

#include "tusb.h"

static int8_t transpose;
static uint8_t  last_note = 67; // G :-)

int8_t get_transposition() { return transpose; }
void set_transposition(int8_t txp) { transpose = txp; }

void midi_setup(int8_t txpose) {
    transpose = txpose;
}

// The MIDI interface always creates input and output port/jack descriptors
// regardless of these being used or not. Therefore incoming traffic should be read
// (possibly just discarded) to avoid the sender blocking in IO
void drain_midi_input() {
    uint8_t packet[4];
    while ( tud_midi_available() ) tud_midi_packet_read(packet);
}

void change_breath_cc(uint8_t new_breath) {
    uint8_t  msg[3];
    msg[0] = CONTROL_CHANGE;
    msg[1] = BREATH_CC;
    msg[2] = new_breath;
    tud_midi_stream_write(0, msg, 3);
}

void restart_current_note(uint8_t new_breath) {
    uint8_t  msg[3];
    msg[0] = NOTE_ON;
    msg[1] = last_note;
    msg[2] = new_breath;
    tud_midi_stream_write(0, msg, 3);
}

void start_note(uint8_t note, uint8_t breath) {
    uint8_t  msg[3];
    msg[0] = NOTE_ON;
    msg[1] = note;
    msg[2] = breath;
    tud_midi_stream_write(0, msg, 3);
    last_note = note;
}

void stop_current_note() {
    uint8_t  msg[3];
    msg[0] = NOTE_OFF;
    msg[1] = last_note;
    msg[2] = 0; // Zero velocity for note off required by some synths
    tud_midi_stream_write(0, msg, 3);
}