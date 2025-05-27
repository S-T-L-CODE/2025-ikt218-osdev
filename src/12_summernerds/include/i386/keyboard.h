#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "i386/interruptRegister.h"

typedef struct
{
    int x;
    int y;
} vector2D;

#define BUFFER_SIZE 255

void EnableBufferTyping();
void DisableBufferTyping();
void EnableTyping();
void DisableTyping();
char scanCodeToASCII(unsigned char *scanCode);
void irq1_keyboard_handler(registers_t *regs, void *ctx);
int has_user_pressed_esc();

void wait_for_keypress();
void reset_key_buffer();
char get_first_buffer();
char get_key();

static vector2D arrowKeys2D;

#endif