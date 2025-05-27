// Pong created by pbogre on github, but modified to work
// Link: https://github.com/pbogre/pong.c

#include <libc/stdio.h>
#include <libc/stdbool.h>
#include "common.h"
#include "kernel/pit.h"
#include "i386/keyboard.h"
#include "i386/monitor.h"
#include "game/game.h"

#define LNLIMIT 100

int abs(int number)
{
    if (number < 0)
        return -number;
    return number;
}
// Simple exponential approximation using Taylor series
double exp_taylor(double x, int terms)
{
    double result = 1.0;
    double term = 1.0;
    for (int i = 1; i < terms; ++i)
    {
        term *= x / i;
        result += term;
    }
    return result;
}
// Natural log using Newton-Raphson
double ln(double x)
{
    if (x <= 0)
        return -1e9; // error approximation

    double y = 1.0; // initial guess
    for (int i = 0; i < 20; ++i)
    {
        double e_y = exp_taylor(y, 20);
        y = y - 1 + x / e_y;
    }
    return y;
}
// Log base 10 using ln
double log10(double x)
{
    return ln(x) / ln(10.0);
}
int floor(float num)
{
    return (int)num;
}

typedef uint16_t uint;

// global variables, does not use #define because they can be changed with arguments
uint ROWS = 12,
     COLS = 80,

     PADDLE_X = 2,
     PADDLE_Y = 3,

     BALL_SIZE = 1,

     BOT_FOW = 12,        // bot fog of war. higher means easier opponent
    UPDATE_FREQUENCY = 5, // game status updates every X frames (default)
                          // affects: bots, ball
    _UPDATE_FREQUENCY;    // changed throughout game

unsigned long long updates = 0;

bool pvp = false,
     eve = false,
     debug = false;

// structures
struct paddle
{
    uint x, y;
    unsigned long long score;
} player[2];

struct _ball
{
    uint x, y;
    int8_t dx, dy;
} ball;

void generate_grid(uint grid[ROWS][COLS])
{
    for (uint y = 0; y < ROWS; y++)
    {
        for (uint x = 0; x < COLS; x++)
        {
            if (y == 0 || y == ROWS - 1)
                grid[y][x] = 1;
            else
                grid[y][x] = 0;
        }
    }
}

void draw_game(uint grid[ROWS][COLS])
{
    printf("\033[H");

    // insert players
    for (uint i = 0; i < 2; i++)
    {
        for (uint y = player[i].y; y < player[i].y + PADDLE_Y; y++)
        {
            for (uint x = player[i].x; x < player[i].x + PADDLE_X; x++)
            {
                grid[y][x] = 2;
            }
        }
    }

    // insert ball
    for (uint y = ball.y; y < ball.y + BALL_SIZE; y++)
    {
        for (uint x = ball.x; x < ball.x + BALL_SIZE; x++)
        {
            grid[y][x] = 3;
        }
    }

    // draw values
    for (uint y = 0; y < ROWS; y++)
    {
        for (uint x = 0; x < COLS; x++)
        {
            switch (grid[y][x])
            {
            case 0:
                printf(" "); // background
                break;

            case 1:
                printf("-"); // top/bottom limits
                break;

            case 2:
                printf("|"); // paddle
                break;

            case 3:
                printf("#"); // ball
                break;

            default:
                printf("?");
            }
        }
        printf("\n");
    }

    // draw scores in the middle
    uint score1_length = floor(log10(player[0].score));
    for (int i = 1; i < (COLS / 2) - (score1_length + 1); i++)
    {
        printf(" ");
    }
    printf("%d | %d", (int)player[0].score, (int)player[1].score);

    // debug stuff
    if (debug)
    {
        printf("\n");
        printf("\n Player 1: (%hu, %hu)    |    Player 2: (%hu, %hu)               ", player[0].x, player[0].y, player[1].x, player[1].y);
        printf("\n Ball: (%hu (%d), %hu (%d))                         ", ball.x, ball.dx, ball.y, ball.dy);
        printf("\n Updates: %llu  |  Game Update Frequency: %hu                     ", updates, _UPDATE_FREQUENCY);
    }
}

// Moves the ball, deals with collisions, sets game speed, updates score.
void update_ball(uint grid[ROWS][COLS])
{
    // clear previous ball position
    for (uint y = ball.y; y < ball.y + BALL_SIZE; y++)
    {
        for (uint x = ball.x; x < ball.x + BALL_SIZE; x++)
        {
            grid[y][x] = 0;
        }
    }

    // horizontal collision
    for (int i = 0; i < 2; i++)
    {
        bool near_ball = (player[i].x > COLS / 2) ?                                     // paddle is on the right?
                             (ball.x >= player[i].x - PADDLE_X)                         // right paddle
                                                  : (ball.x <= player[i].x + PADDLE_X); // left paddle

        if (near_ball)
        {
            if (ball.y >= player[i].y && ball.y <= player[i].y + PADDLE_Y)
            { // paddle hit ball
                ball.dx *= -1;
                ball.x += ball.dx;
                if (_UPDATE_FREQUENCY > 1)
                    _UPDATE_FREQUENCY--; // game gets progressively faster after each hit
            }

            else
            { // paddle missed ball
                ball.x = (COLS / 2) - BALL_SIZE;
                ball.y = (ROWS / 2) - BALL_SIZE;
                player[1].score++;
                ball.dx *= -1;

                _UPDATE_FREQUENCY = UPDATE_FREQUENCY; // reset game speed
            }
        }
    }

    // vertical collision
    if (ball.y <= 1 || ball.y + BALL_SIZE >= ROWS - 1)
        ball.dy *= -1;

    // update ball
    ball.y += ball.dy;
    ball.x += ball.dx;
}

void move_player(uint index, bool direction, uint grid[ROWS][COLS])
{
    if (!direction)
    { // move up
        if (player[index].y >= 2)
        {
            player[index].y -= 1;
            // clear grid below
            for (uint x = player[index].x; x < player[index].x + PADDLE_X; x++)
            {
                grid[player[index].y + PADDLE_Y][x] = 0;
            }
        }
        return;
    }
    // move down
    if (player[index].y + PADDLE_Y <= ROWS - 2)
    {
        player[index].y += 1;
        // clear grid above
        for (uint x = player[index].x; x < player[index].x + PADDLE_X; x++)
        {
            grid[player[index].y - 1][x] = 0;
        }
    }
}

void automate_player(unsigned int index, uint grid[ROWS][COLS])
{
    int diff = abs(player[index].x - ball.x);
    if (diff <= COLS / BOT_FOW)
    {
        bool direction = ((ball.y * 2 + BALL_SIZE) / 2 < (player[index].y * 2 + PADDLE_Y) / 2) ? 0 : 1;
        move_player(index, direction, grid);
    }
}

int run_pong()
{
    uint grid[ROWS][COLS];

    // init values
    player[0].x = 1;
    player[0].y = (ROWS / 2) - (PADDLE_Y / 2);
    player[0].score = 0;
    player[1].x = COLS - (PADDLE_X + 1);
    player[1].y = (ROWS / 2) - (PADDLE_Y / 2);
    player[1].score = 0;

    ball.x = COLS / 2 - BALL_SIZE;
    ball.y = ROWS / 2 - BALL_SIZE;
    ball.dx = 2;
    ball.dy = 1;

    _UPDATE_FREQUENCY = UPDATE_FREQUENCY;

    // setup terminal for  game
    printf("\033[?25l\033[2J");
    generate_grid(grid);
    draw_game(grid);
    char m;
    EnableBufferTyping();
    // begin main loop
    while (true)
    {
        monitor_clear();
        m = get_first_buffer();
        if (m)
        {
            reset_key_buffer();
            switch (m)
            {
                // first player (w-s)
            case 'w':
                move_player(0, 0, grid);
                break;
            case 's':
                move_player(0, 1, grid);
                break;
            }
        }

        if ((int)updates % (int)_UPDATE_FREQUENCY == 0)
        {
            update_ball(grid);
            if (!pvp || eve)
                automate_player(1, grid);
            if (eve)
                automate_player(0, grid);
        }
        draw_game(grid);
        updates++;
        if (has_user_pressed_esc())
        {
            return 0;
        }
        sleep_interrupt(50); // minimum 1ms delay
    }
    return 0;
}