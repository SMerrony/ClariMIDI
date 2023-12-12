/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */

#include "pico/stdlib.h"

#include "mpr121.h"

#include "config.h"
#include "display.h"
#include "keys.h"

static uint32_t switches_mask = 0;

static struct mpr121_sensor mpr121;
static int touchmap[MPR121_SENSORS]; // map MPR121 sensors to our key numbers

static keydef_t keys[] = {
    [0] = {15, false, 0},   // Register key
    [1] = {2,  true,  0},   // Thumb touchplate
    [2] = {14, false, 0},
    [3] = {13, false, 0},
    [4] = {0,  true,  0},   // L1
    [5] = {4,  true,  0},   // L2
    [6] = {12, false, 0},
    [7] = {6,  true,  0},   // L3
    [8] = {11, false, 0},
    [9] = {20, false, 0},
    [10] = {10, false, 0},
    [11] = {21, false, 0},
    [12] = {8,  true,  0},   // R1
    [13] = {10, true, 0},    // R2
    [14] = {8,  false, 0},
    [15] = {11, true, 0},    // R3
    [16] = {6,  false, 0},
    [17] = {5,  false, 0},
    [18] = {4,  false, 0},
    [19] = {3,  false, 0},
    [20] = {2,  false, 0}    // E-flat                                                                 
};

#define LAST_KEY 20

void keys_setup() {

    // I2C for the MPR121
    i2c_init(MPR121_I2C_PORT, MPR121_I2C_FREQ);
    gpio_set_function(MPR121_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(MPR121_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(MPR121_I2C_SDA);
    gpio_pull_up(MPR121_I2C_SCL);

    // MPR121
    mpr121_init(MPR121_I2C_PORT, MPR121_ADDR, &mpr121);
    mpr121_set_thresholds(MPR121_TOUCH_THRESHOLD, MPR121_RELEASE_THRESHOLD, &mpr121);
    for (int s = 0; s < MPR121_SENSORS; s++){
        touchmap[s] = UNUSED_SENSOR;
    }

    for (int k = 0; k <= LAST_KEY; k++) {
        if (keys[k].touch) {
            touchmap[keys[k].pin] = k;
        } else {
            gpio_init(keys[k].pin);
            gpio_set_dir(keys[k].pin, GPIO_IN);
            gpio_pull_down(keys[k].pin);
            switches_mask |= (1<<k);
        }

    }
}

uint32_t read_touches() {
    uint32_t state = 0;
    uint16_t touched;
    mpr121_touched(&touched, &mpr121);
    for (int s = 0; s < MPR121_SENSORS; s++) {
        if (touchmap[s] != UNUSED_SENSOR) {
            if ((touched & (1 << s)) != 0) {
                state |= (1 << touchmap[s]);
            }
        }
    }
    return state;
}

uint32_t read_switches() {
    uint32_t state = 0;
    // don't use gpio_get_all() in case it interferes with touch sensing
    for (int k = 0; k <= LAST_KEY; k++) {
        if (!keys[k].touch) {
            if (gpio_get(keys[k].pin)) {
                state |= (1<<k);
            }
        }
    }
    return state;
}

uint32_t read_keys() {
    return read_touches() | read_switches();
}

uint8_t decode_keys(uint32_t keys) {
    // N.B. this decode is effectively for 'clarinet in C'
    switch (keys) {
        //     098765432109876543210
        //     EEFFG3 21FBC 3 21 ATR
        //     b  ##    #b      b
        case 0b000000000000000000000: return 67; // G
        case 0b000000000000000000010: return 65; // F
        case 0b000000000000000000011: return 84; // c
        case 0b000000000000000000100: return 69; // A
        case 0b000000000000000000101: return 70; // normal B-flat
        case 0b000000000000000001000: return 68; // A-flat
        case 0b000000000000000001100: return 69; // normal A plus A-flat key
        case 0b000000000000000010000: return 66; // F# also, menu-up
        case 0b000000000000000010010: return 64; // E
        case 0b000000000000000010011: return 83; // b
        case 0b000000000000000110010: return 62; // D
        case 0b000000000000000110011: return 81; // a
        case 0b000000000000001110010: return 63; // # Front & SK1
        case 0b000000000000001110011: return 82; // # Front & SK1
        case 0b000000000000010110010: return 60; // # Middle C
        case 0b000000000000010110011: return 79; // g
        case 0b000000000000110110010: return 61; // C#
        case 0b000000000000110110011: return 80; // g#
        case 0b000000000001000000100: return 72; // # SK4
        case 0b000000000010000000100: return 70; // # SK3
        case 0b000000000100000000010: return 66; // # SK2
        case 0b000000001000010110010: return 58; //
        case 0b000000001000010110011: return 77; //
        case 0b000000010000010110010: return 59; //
        case 0b000000010000010110011: return 78; //
        case 0b000000011000010110010: return 57; //
        case 0b000000011000010110011: return 76; //
        case 0b000000011000010100011: return 85; //
        case 0b000001011000010110010: return 55; //
        case 0b000001011000010110011: return 74; //
        case 0b000011011000010110010: return 56; //
        case 0b000011011000010110011: return 75; //
        case 0b001001011000010110010: return 53; //
        case 0b001001011000010110011: return 72; //
        case 0b010001011000010110010: return 52; //
        case 0b010001011000010110011: return 71; //
        //     098765432109876543210
        //     EEFFG3 21FBC 3 21 ATR
        //     b  ##    #b      b        
        case 0b010010000000000000000: return ENTER_MENU;
        case 0b000000000000000000001: return MENU_SELECT;
        case 0b000000000010000000000: return MENU_DOWN;
        case 0b000000000001000000000: return MENU_UP;
        default: 
            return UNKNOWN_FINGERING;
    }
}