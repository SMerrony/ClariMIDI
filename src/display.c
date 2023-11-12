/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "config.h"
#include "ssd1306.h"

ssd1306_t disp;

void display_setup() {
    i2c_init(i2c0, 200000);
    gpio_set_function(I2C_0, GPIO_FUNC_I2C);
    gpio_set_function(I2C_1, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_0);
    gpio_pull_up(I2C_1);

    disp.external_vcc = false;
    ssd1306_init(&disp, WIDTH, HEIGHT, 0x3C, i2c0);
    ssd1306_clear(&disp);

    ssd1306_draw_string(&disp, 10, 10, 2, APP_NAME);
    ssd1306_draw_string(&disp, 30, 34, 2, APP_VERSION);
    ssd1306_show(&disp);
    sleep_ms(SPLASH_MS);
    ssd1306_clear(&disp);
    ssd1306_show(&disp);
}

void display_clear() {
    ssd1306_clear(&disp);
    ssd1306_show(&disp);
}

void display_error(char msg[], bool fatal) {
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 0, 10, 1, msg);
    ssd1306_show(&disp);
    if (fatal) {
        
    }
    sleep_ms(ERROR_TIMEOUT_MS);
    ssd1306_clear(&disp);
    ssd1306_show(&disp);
}

void display_message(char msg[]) {
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 0, 10, 1, msg);
    ssd1306_show(&disp);
}

void display_string(uint32_t x, uint32_t y, uint32_t scale, const char *s) {
    ssd1306_draw_string(&disp, x, y, scale, s);
    ssd1306_show(&disp);
}