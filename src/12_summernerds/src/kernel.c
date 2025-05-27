#include "i386/keyboard.h"
#include "i386/descriptorTables.h"
#include "i386/interruptRegister.h"
#include "i386/monitor.h"
#include "kernel/pit.h"
#include "kernel/memory.h"
#include <libc/stdint.h>
#include <libc/stddef.h>
#include "libc/stdbool.h"
#include "song/song.h"
#include "song/song.h"
#include "common.h"
#include "menu.h"

#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define VGA_MEMORY (volatile uint16_t*)0xB8000
 

extern uint32_t end; // Linker symbol marking the end of kernel

// uint8_t rainbow_colours[4] = {0x4, 0xE, 0x2, 0x9}; // Rød, gul, grønn, blå

struct multiboot_info
{
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};




int main(uint32_t magic, uint32_t mb_info_addr)
{

    // initializing basic systems
    monitor_initialize();

    //-- assignment 2 --
    init_gdt();
    printf("\n\n\nHello world!\n");

    //-- assignment 3 --
    init_idt();
    init_irq();
    testThreeISRs();
    register_irq_handler(IRQ1, irq1_keyboard_handler, 0);

    // assignment 4
    init_kernel_memory(&end); // Initializing the kernel memory manager
    init_paging();            // call function to activate paging
    print_memory_layout();    // print memory layout to screen
    init_pit();               // Initialize PIT (programmable interval timer)
    // Here we test the memory allocation
    void *mem1 = malloc(2345);
    void *mem2 = malloc(4321);
    void *mem3 = malloc(3331);

    free(mem1);
    free(mem2);
    free(mem3);

    // print if we get any problem with allocating memory
    // printf("Allocated memory blocks at: 0x%x, 0x%x, 0x%x\n", mem1, mem2, mem3);

    // beep();

    // beep();

    // EnableTyping(); // Enables free typing
    handle_menu(); // opens the menu

    while (true)
    {
    }

    // Usually shouldnt get here, since it then quits kernel main.
    return 0;
}