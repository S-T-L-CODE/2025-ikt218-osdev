#include "screen.h"
#include <libc/stdint.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY (volatile uint16_t*)0xB8000

uint8_t rainbow_colours[4] = {0x4, 0xE, 0x2, 0x9};
static uint8_t current_color = 0x0F;

void write_to_terminal (const char* str, int line) {
    if (line >= VGA_HEIGHT) return;

    volatile uint16_t* vga = VGA_MEMORY + (VGA_WIDTH * line);
    for (int i = 0; str[i] && i < VGA_WIDTH; i++) {
        vga[i] = (rainbow_colours[i % 4] << 8) | str[i];
    }
}

void print_where (const char* str, int row, int col) {
    if (row >= VGA_HEIGHT || col >= VGA_WIDTH) return;

    volatile uint16_t* vga = VGA_MEMORY + (row * VGA_WIDTH) + col;
    for (int i = 0; str[i] && (col + i) < VGA_WIDTH; i++) {
        vga[i] = (current_color << 8) | str[i];
    }
}

void clear_the_screen () {
    volatile uint16_t* vga = VGA_MEMORY;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = (current_color << 8) | ' ';
    }
}

void set_color (uint8_t fg, uint8_t bg) {
    current_color = (bg << 4) | (fg & 0x0F);
}
