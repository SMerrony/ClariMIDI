/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */

#ifndef KEYS_H
#define KEYS_H

#include <stdbool.h>

typedef struct {
    int pin;    // GPIO for switches, # for MPR121
    bool touch; // true if a touch key, else a switch
    uint threshold;
} keydef_t;

void keys_setup();

// Read all keys (touches and switches)
uint32_t read_keys(); 

uint8_t decode_keys(uint32_t keys);

#endif
