#pragma once
#include <libc/stdint.h>

void write_to_terminal (const char* str, int line);
void print_where (const char* str, int row, int col);
void clear_the_screen ();
void set_color (uint8_t fg, uint8_t bg);
