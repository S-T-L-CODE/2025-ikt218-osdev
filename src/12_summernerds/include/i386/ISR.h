#pragma once //Gjør at denne filen blir ikke behandlet før programmet compiler for at den skal inkluderes bare en gang.
#include <stdint.h>

#define MAX_INTERRUPTS 256 //Antall interrups x86 processoren  vil kunne støtte i IDT-en.
#define MAX_LISTENERS_PER_ISR 4 //begrenser antall funksjoner (listener4s) som kan respondere til et interupt til 4.

typedef struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp;
    uint32_t ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

typedef void (*interrupt_listener_t)(registers_t* regs);

void isr_init();
void isr_handler(registers_t *r);
void subscribe_interrupt(uint8_t interrupt_number, interrupt_listener_t handler);
void subscribe_global(interrupt_listener_t handler);
void isr_dispatch(registers_t* regs);

// ISR stubs fra isr.asm
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
