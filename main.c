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

void _start() {

    // LED Matrix
    int current_x = 7;
    int current_y = 12;

    // HEX colors
    int green = 0x00FF00;
    int black = 0x000000;
    int white = 0xFFFFFF;

    SetBorders(white);

    // initial position
    int start_offset = (current_y * LED_MATRIX_WIDTH + current_x) * PIXEL_SIZE_BYTES;
    *((volatile int *)((char *)LED_PTR + start_offset)) = green;

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
            int next_x = current_x + dx;
            int next_y = current_y + dy;

            // Calculate Address of next pixel
            int next_offset = (next_y * LED_MATRIX_WIDTH + next_x) * PIXEL_SIZE_BYTES;
            volatile int * next_ptr = (volatile int * )((char*)LED_PTR + next_offset);

            // Check Colition
            if(*next_ptr != white){

                // Erease trail
                int old_offset = (current_y * LED_MATRIX_WIDTH + current_x) * PIXEL_SIZE_BYTES;
                *((volatile int *)((char *)LED_PTR + old_offset)) = black;

                // Update coordinate
                current_x = next_x;
                current_y = next_y;

                // Draw character
                *next_ptr = green;
            }

            // Debouncing
            while(*DPAD_RIGHT || *DPAD_LEFT || *DPAD_UP || *DPAD_DOWN) {}
        }

    }
}
