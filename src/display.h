/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-4-Clause-Shortened
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

void display_setup();
void display_clear();
void display_error(char msg[], bool fatal);
void display_message(char msg[]);
void display_string(uint32_t x, uint32_t y, uint32_t scale, const char *s);

#endif