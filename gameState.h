#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//Screen dimension constants
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 800

//Gameplay Constants
#define GRID_HEIGHT 22
#define GRID_WIDTH 10
#define BLOCK_SIZE 30
#define FALL_RATE 60 //in frames
#define FAST_FALL_RATE 2

//Menu States
#define TITLE_SCREEN 0
#define IN_GAME 1
#define DEATH_SCREEN 2

// Define the Tetrimino shapes as a 3D array
extern int BLOCK_SHAPES[7][4][4];

typedef uint8_t Block; //Stores block color state

typedef struct {
	Block grid[GRID_HEIGHT][GRID_WIDTH]; //2d array of blocks, one byte each.
}BlockGrid;

typedef struct {
	uint8_t color;
	int shapeid;
	int shape[4][4];
	int x;
	int y;
	int collisionTimer;
	int fallTimer;
	bool isFastFalling;

}FallingBlock;

typedef struct {
	uint8_t menuState;
	int score;
	BlockGrid playField;
	FallingBlock fallingBlock;

}GameState;

//Function prototypes
void initGame(GameState* state);
void tick(GameState* state);
bool checkCollision(BlockGrid field, int shape[4][4], int x, int y, int dx, int dy);
void finalizeBlock(GameState* state);
void moveFallingBlock(GameState* state, int dx, int dy);
void rotateFallingBlock(GameState* state);
void respawnFallingBlock(GameState* state);
bool checkForLine(GameState* state, int row);
void deleteRow(GameState* state, int row);
#endif