/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */
#ifndef CONFIG_H
#define CONFIG_H

#include "hardware/i2c.h"

#define APP_NAME "ClariMIDI"
#define APP_VERSION "v0.0.4"
#define APP_AUTHOR "SMerrony"

// display
#define I2C_0 0
#define I2C_1 1
#define WIDTH 128
#define HEIGHT 64
#define SPLASH_MS 2000
#define ERROR_TIMEOUT_MS 3000

// MIDI
#define DEFAULT_CHANNEL 0
#define BREATH_CC 2
#define DEFAULT_TRANSPOSE 0 // 'clarinet in C'
#define MAX_NOTE          127
// dummy note values for special 'fingerings'
#define ENTER_MENU        251
#define MENU_SELECT       252
#define MENU_UP           253
#define MENU_DOWN         254 
#define UNKNOWN_FINGERING 255

#define MENU_KEY_REPEAT_MS 333

// breath
#define BREATH_PIN 28
#define DEFAULT_BREATH_CUTOFF 20 // below this we are silent

// touch
// #define TOUCH_SAMPLES 5
// #define TOUCH_TIMEOUT_TICKS 10000
// #define TOUCH_DEFAULT_THRESHOLD 100

// I2C definitions: port and pin numbers
#define I2C_PORT i2c1
#define I2C_SDA 18
#define I2C_SCL 19

// I2C definitions: address and frequency
#define MPR121_ADDR 0x5A
#define MPR121_I2C_FREQ 100000

// Touch and release thresholds
#define MPR121_TOUCH_THRESHOLD 16
#define MPR121_RELEASE_THRESHOLD 10

#define MPR121_SENSORS 12 // max. the board can handle
#define UNUSED_SENSOR  99

#endif