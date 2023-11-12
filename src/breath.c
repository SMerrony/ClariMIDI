/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */

#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "breath.h"
#include "config.h"

static uint8_t cutoff = DEFAULT_BREATH_CUTOFF;

void breath_setup() {
    adc_init();
    adc_set_clkdiv(2400); // sample at ~20KHz
    adc_gpio_init(BREATH_PIN);
    adc_select_input(BREATH_PIN - 26);
}

uint8_t breath_read() {
    uint8_t reading;
    // uint16_t adc = adc_read();
    // printf("DEBUG: Raw ADC value: %d, >>5: %d\n", adc, adc>>5);
    reading = 127 - (uint8_t) (adc_read() >> 5);
    if (reading < cutoff) reading = 0;
    return reading;
}