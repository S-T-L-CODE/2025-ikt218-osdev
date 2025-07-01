#include "matrix_effect/matrix.h"
#include <libc/stdint.h>
#include "common.h"
#include "random.h"
#include "i386/monitor.h"

#define WIDTH 80  // Screen width
#define HEIGHT 25 // Screen height

// Column state
typedef struct
{
    int y_pos; // Position
    int speed; // Speed
    int color; // Color
} ColumnState;

static ColumnState columns[WIDTH]; // Columns array

// Color options
static uint8_t color_palette[] = {0x02, 0x04, 0x01, 0x0E};

// Init columns
void init_matrix()
{
    setupRNG(947); // Init random

    for (int i = 0; i < WIDTH; i++)
    {
        columns[i].y_pos = randint(100) % HEIGHT;            // Start position
        columns[i].speed = 1 + randint(100) % 3;             // Random speed
        columns[i].color = color_palette[randint(1000) % 4]; // Random color
    }
}

// Draw one frame
void draw_matrix_frame()
{
    monitor_clear(); // Clear screen

    for (int x = 0; x < WIDTH; x++)
    {
        int y_max = columns[x].y_pos; // Max Y

        for (int y = 0; y < y_max; y++)
        {
            char ch = 33 + randint(94); // Random char

            if (y < HEIGHT)
            {
                volatile char *cell = (char *)0xB8000 + 2 * (y * WIDTH + x);
                cell[0] = ch;               // Set char
                cell[1] = columns[x].color; // Set color
            }
        }

        columns[x].y_pos += columns[x].speed; // Move down

        if (columns[x].y_pos >= HEIGHT + 3)
        {
            columns[x].y_pos = 0;                                // Reset
            columns[x].speed = 1 + randint(3);                   // New speed
            columns[x].color = color_palette[randint(1000) % 4]; // New color
        }
    }
}
