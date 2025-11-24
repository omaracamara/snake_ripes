#include "ripes_system.h"

// Constants
#define LED_MATRIX_WIDTH 35
#define LED_MATRIX_HEIGHT 25
#define PIXEL_SIZE_BYTES 4
#define TOTAL_BYTES (LED_MATRIX_WIDTH * LED_MATRIX_HEIGHT * PIXEL_SIZE_BYTES)


// Pointer towards LED
#define LED_PTR     ((volatile int *)0xF0000000)

// Directional Pad
#define DPAD_UP     ((volatile int *)0xF0000DB0)
#define DPAD_DOWN   ((volatile int *)0xF0000DB4)
#define DPAD_LEFT   ((volatile int *)0xF0000DB8)
#define DPAD_RIGHT  ((volatile int *)0xF0000DBC)

// Snake array
int snake_x[100];
int snake_y[100];

void SetBorders(int white){

    int bottom_row_start = (LED_MATRIX_HEIGHT - 1) * LED_MATRIX_WIDTH;

    for (int x = 0; x < LED_MATRIX_WIDTH; x++) {
        // Top Border
        *((volatile int *)((char *)LED_PTR + (x * PIXEL_SIZE_BYTES))) = white;

        // Bottom Border
        *((volatile int *)((char *)LED_PTR + ((bottom_row_start + x) * PIXEL_SIZE_BYTES))) = white;
    }


    for (int y = 1; y < LED_MATRIX_HEIGHT - 1; y++) {
        // Left Border
        int left_offset = (y * LED_MATRIX_WIDTH) * PIXEL_SIZE_BYTES;
        *((volatile int *)((char *)LED_PTR + left_offset)) = white;

        // Right Border
        int right_offset = (y * LED_MATRIX_WIDTH + (LED_MATRIX_WIDTH - 1)) * PIXEL_SIZE_BYTES;
        *((volatile int *)((char *)LED_PTR + right_offset)) = white;
    }
}

void SetApple(int red){

    int x_coordinate = 15;
    int y_coordinate = 15;

    int start_offset = (x_coordinate * LED_MATRIX_WIDTH + y_coordinate) * PIXEL_SIZE_BYTES;
    *((volatile int *)((char *)LED_PTR + start_offset)) = red;
}

void _start() {

    // HEX colors
    int green = 0x00FF00;
    int black = 0x000000;
    int white = 0xFFFFFF;
    int red   = 0xFF0000;

    // Initial snake setup
    int length = 1;
    snake_x[0] = 17; // Head x
    snake_y[0] = 12; // Head y

    // Draw initial position
    int start_offset = (snake_y[0] * LED_MATRIX_WIDTH + snake_x[0]) * PIXEL_SIZE_BYTES;
    *((volatile int *)((char *)LED_PTR + start_offset)) = green;

    SetApple(red);

    // Loop forever
    while(1){
        // change in positions
        int dx = 0;
        int dy = 0;

        // check inputs
        if (*DPAD_RIGHT) dx += 1;
        if (*DPAD_LEFT)  dx -= 1;
        if (*DPAD_DOWN)  dy += 1;
        if (*DPAD_UP)    dy -= 1;

        // If change detected
        if(dx != 0 || dy != 0){

            // Calculate next position
            int next_x = snake_x[0] + dx;
            int next_y = snake_y[0] + dy;

            // Calculate Address of next pixel
            int next_offset = (next_y * LED_MATRIX_WIDTH + next_x) * PIXEL_SIZE_BYTES;
            volatile int * next_ptr = (volatile int * )((char*)LED_PTR + next_offset);

            // Next color
            int next_color = *next_ptr;

            // Check Colition
            if(next_color != white){

                // Check Apple
                int grow = 0;
                if(next_color == red){
                    grow = 1;
                    length++;
                }

                // If didnt grow erease trail
                if(grow == 0){
                    int tail_x = snake_x[length - 1];
                    int tail_y = snake_y[length - 1];

                     // Erease trail
                    int tail_offset = (tail_y * LED_MATRIX_WIDTH + tail_x) * PIXEL_SIZE_BYTES;
                    *((volatile int *)((char *)LED_PTR + tail_offset)) = black;
                }

                // Shift snake
                for(int i = length - 1; i > 0; i--){
                    snake_x[i] = snake_x[i-1];
                    snake_y[i] = snake_y[i-1];
                }


                // Update coordinate
                snake_x[0] = next_x;
                snake_y[0] = next_y;

                // Draw snake
                *next_ptr = green;
            }

            // Debouncing
            while(*DPAD_RIGHT || *DPAD_LEFT || *DPAD_UP || *DPAD_DOWN) {}
        }

    }
}
