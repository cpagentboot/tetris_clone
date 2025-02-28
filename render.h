#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "gameState.h"

int renderInit(int windowWidth, int windowHeight);

void showToast(char* text);

void drawFrame(GameState* state);

void quitGame();

void drawBlock(SDL_Renderer* renderer, int x, int y, int width, int height, int r, int g, int b);

void clearFrame();

void drawPlayField(SDL_Renderer* renderer, GameState* state);

void resolveColor(int *r, int* g, int* b, uint8_t color);

void drawFallingBlock(SDL_Renderer* renderer, GameState* state);

void drawTitleScreen(SDL_Renderer* renderer);

SDL_Texture* createHorizontalGradient(SDL_Renderer* renderer, int width, int height, SDL_Color colorStart, SDL_Color colorEnd);

#endif