/* Responsible for managing the state of the game.
    Current player state - position, velocity, fuel remaining.
    Menu state - title screen, in game, pause menu, credits, etc.
*/

#include "gameState.h"
#include "render.h"

#include <math.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

// Define the block shapes
int BLOCK_SHAPES[7][4][4] = {
    { // Square (O)
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    { // Line (I)
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    { // L Shape
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    { // Reverse L (J)
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    { // T Shape
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    { // S Shape
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    { // Z Shape
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }
};


// Function to initialize the game state with default values
void initGame(GameState* state) {
    
    state->menuState = TITLE_SCREEN;
    state->score = 0;

    // Initialize playField to all zeros
    memset(&state->playField, 9, sizeof(BlockGrid));

    //Init our "player" (the falling block)

    

    state->fallingBlock.collisionTimer = -1;
    state->fallingBlock.fallTimer = FALL_RATE;
    state->fallingBlock.isFastFalling = false;
    respawnFallingBlock(state);
}

void tick(GameState* state) { //Advance everything in the game by one frame.

    //Fast faling - just reduce the regular fall timer
    if (state->fallingBlock.isFastFalling && state->fallingBlock.fallTimer > FAST_FALL_RATE) {
        state->fallingBlock.fallTimer = FAST_FALL_RATE;
    }

    //Check downward collision.
    bool collision = checkCollision(state->playField, state->fallingBlock.shape, state->fallingBlock.x, state->fallingBlock.y, 0, 1);

    //Move the falling block down if it is time to do so, or just decrement the wait timer.
    if (state->fallingBlock.fallTimer == 0) {

        moveFallingBlock(state, 0, 1);

        state->fallingBlock.fallTimer = FALL_RATE; 

    } else { //Block did not move down this frame
        state->fallingBlock.fallTimer -= 1;
    }

    if (collision) {
        if (state->fallingBlock.collisionTimer == -1) { //If this is the first frame of collision, set the timer.
            state->fallingBlock.collisionTimer = 20;
            }
        if (state->fallingBlock.collisionTimer == 0) { //If the timer is zero, then we need to finalize the block in its current position.
            finalizeBlock(state);

            respawnFallingBlock(state);
        } 
        if (state->fallingBlock.collisionTimer >= 0) { //base case, continue countdown.
            state->fallingBlock.collisionTimer -= 1;
        }
    }
    else { //There was no collision this frame, reset the timer.
        state->fallingBlock.collisionTimer = -1; 
    }
}

bool checkCollision(BlockGrid field, int shape[4][4], int x, int y, int dx, int dy) {

    //int x = state->fallingBlock.x;
    //int y = state->fallingBlock.y;

    x += dx;
    y += dy;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col]) { // If block has a filled square
                int newX = x + col;
                int newY = y + row;

                // Check boundaries
                if (newX < 0 || newX >= GRID_WIDTH || newY >= GRID_HEIGHT) {
                    return true; // Colliding with walls or floor
                }

                // Check if landing on an existing block
                if (field.grid[newY][newX] != 9) {
                    return true;
                }
            }
        }
    }
    return false; // No collision
}

void moveFallingBlock(GameState* state, int dx, int dy) {

    bool collision = checkCollision(state->playField, state->fallingBlock.shape, state->fallingBlock.x, state->fallingBlock.y, dx, dy);

    if (!collision) {
        state->fallingBlock.x += dx;
        state->fallingBlock.y += dy;
    }
}

void rotateFallingBlock(GameState* state) {

    //This is probably a little bit overcomplicated, but I really wanted the game to mimic the original GB Tetris behavior.

    if (state->fallingBlock.shapeid == 0) { return; } //Don't rotate the square, exit the function early.

    int tempShape[4][4] = { 0 }; // Temporary matrix to store the rotated block shape

    //the line only has two possible states, so we are just going to flip the row/col.
    if (state->fallingBlock.shapeid == 1) {
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                tempShape[row][col] = state->fallingBlock.shape[col][row];
            }
        }
    }
    else {
        // Transpose and reverse the rows to rotate the shape counterclockwise, but only a 3x3, as we already dealth with the line
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                tempShape[row][col] = state->fallingBlock.shape[col][2 -row];
            }
        }
    }

    // Check if the new rotated position is valid (no collision)
    if (!checkCollision(state->playField, tempShape, state->fallingBlock.x, state->fallingBlock.y ,0, 0)) {
        // If valid, apply the rotated shape
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                state->fallingBlock.shape[row][col] = tempShape[row][col]; // Update the block shape
            }
        }
    }
}

void finalizeBlock(GameState* state) {
    //Read the coordinates of the falling block and save it to the gamefield array
    int x = state->fallingBlock.x;
    int y = state->fallingBlock.y;


    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (state->fallingBlock.shape[row][col]) { // If block in shape has a filled square
                state->playField.grid[row + y][col + x] = state->fallingBlock.shapeid; //Save it to the play field

                if ((row + y) <= 2) { state->menuState = DEATH_SCREEN; } //lose condition

                if (checkForLine(state, (row + y))) { //Check to see if we made a line
                    deleteRow(state, (row+y));
                }
            }
        }
    }
   
}

bool checkForLine(GameState* state, int row){
    for (int col = 0; col < GRID_WIDTH; col++) { //Check every block in line
        if (state->playField.grid[row][col] == 9) {
            return false; 
        }
    }
    return true;
}

void respawnFallingBlock(GameState* state) {
    state->fallingBlock.shapeid = rand() % 7;
    memcpy(state->fallingBlock.shape, BLOCK_SHAPES[state->fallingBlock.shapeid], sizeof(state->fallingBlock.shape)); // make it a fucking square
    state->fallingBlock.x = 3;
    state->fallingBlock.y = 0;
}

void deleteRow(GameState* state, int row) {
    // Step 1: Clear the deleted row
    for (int col = 0; col < GRID_WIDTH; col++) {
        state->playField.grid[row][col] = 9;
    }

    // Step 2: Shift all rows above the deleted row down by one
    for (int r = row; r > 0; r--) {
        for (int col = 0; col < GRID_WIDTH; col++) {
            state->playField.grid[r][col] = state->playField.grid[r - 1][col];
        }
    }

    // Step 3: Clear the topmost row (row 0)
    for (int col = 0; col < GRID_WIDTH; col++) {
        state->playField.grid[0][col] = 9;
    }
}