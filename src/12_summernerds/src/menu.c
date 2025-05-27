#include "i386/keyboard.h"
#include "kernel/pit.h"
#include "kernel/memory.h"
#include "libc/stdio.h"
#include "matrix_effect/matrix.h"
#include "song/song.h"
#include "common.h"
#include "menu.h"
#include "game/game.h"
#include "i386/monitor.h"

void reset_key_buffer();

void shutdown()
{
    // a function that clears out the screen
    sleep_interrupt(500);
    monitor_clear();
    outw(0x604, 0x2000);
    outw(0xB004, 0x2000);

    asm volatile("cli");
    while (1)
    {
        asm volatile("hlt");
    }
}

void print_menu()
{
    monitor_clear();
    monitor_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf(" "
           "       __ _.--..--._ _\n"
           "     .-' _/   _/\\_   \\_'-.\n"
           "    |__ /   _/\\__/\\_   \\__|\n"
           "       |___/\\_\\__/  \\___|\n"
           "              \\__/\n"
           "              \\__/\n"
           "               \\__/\n"
           "                \\__/\n"
           "             ____\\__/___\n"
           "       . - '             ' -.\n"
           "      /                      \\ \n");
    monitor_setcolor(VGA_COLOR_LIGHT_BLUE);
    printf("~~~~~~~  ~~~~~ ~~~~~  ~~~ ~~~  ~~~~~\n");

    monitor_setcolor(VGA_COLOR_LIGHT_CYAN);
    printf("Welcome to the os for summernerds!\n"
           "\n"
           " 1. Play Startup Song\n"
           " 2. Matrix Rain Effect\n"
           " 3. Play beep Sound\n"
           " 4. Write text (similar to notepad)\n"
           " 5. Play pong.\n"
           " 6. Exit\n"
           "\n");
    monitor_setcolor(11);
}

void handle_menu()
{
    EnableBufferTyping();
    while (1)
    {
        print_menu();
        char choice = get_key();
        putchar(choice);
        putchar('\n');

        switch (choice)
        {
        case '1':
        {
            printf("Playing startup song...\n");
            Song song = {music_1, sizeof(music_1) / sizeof(Note)};
            play_song(&song);
            break;
        }

        case '2':
        {
            printf("Starting Matrix Rain effect...\n");
            reset_key_buffer();
            init_matrix();
            reset_key_buffer();
            while (1)
            {
                draw_matrix_frame();
                sleep_interrupt(80);
                if (get_first_buffer())
                    break;
            }
            break;
        }

        case '3':
        {
            printf("Beep!\n");
            beep();
            break;
        }

        case '4':
        {
            DisableBufferTyping();
            monitor_clear();
            printf("\nPress 'Esc' to exit typing\n");
            EnableTyping();

            while (true)
            {
                if (has_user_pressed_esc())
                {
                    DisableTyping();
                    break;
                }
            }
            EnableBufferTyping();
            break;
        }
        case '5':
            run_pong();
            break;
        case '6':
        {
            printf("Shutting down...\n");
            shutdown();
            return;
        }

        default:
        {
            printf("Option not acceptable. Please try again...\n");
            break;
        }
        }

        printf("\nPress any key in order to return to get back to menu...");
        wait_for_keypress();
    }
}
