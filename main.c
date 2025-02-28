#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL_rect.h>

#include "gameState.h"
#include "render.h"

void handleInput(bool* quit, GameState* gameState) {

	SDL_Event event;

	int mouseX, mouseY;

	// IN GAME EVENT HANDLING
	if (gameState->menuState == IN_GAME) {
		while (SDL_PollEvent(&event)) { //Event Handling

			switch (event.type) {
			case SDL_QUIT: *quit = true; break;//If the X button is pressed we will close the game.

			case SDL_KEYDOWN: //Key pressed
				switch (event.key.keysym.sym) {

				case SDLK_UP:
					rotateFallingBlock(gameState);
					break;

				case SDLK_DOWN:
					gameState->fallingBlock.isFastFalling = true;
					break;

				case SDLK_LEFT:
					moveFallingBlock(gameState, -1, 0);
					break;
				case SDLK_RIGHT:
					moveFallingBlock(gameState, 1, 0);
					break;
				}
				break;

			case SDL_KEYUP: //Key released
				switch (event.key.keysym.sym) {
				case SDLK_UP:
					break;

				case SDLK_DOWN:
					gameState->fallingBlock.isFastFalling = false;
					break;

				case SDLK_LEFT:
					break;

				case SDLK_RIGHT:
					break;

				}

			}
		}
	}
	//TITLE SCREEN EVENT HANDLING
	if (gameState->menuState == TITLE_SCREEN) {
		while (SDL_PollEvent(&event)) { //Event Handling

			switch (event.type) {
			case SDL_QUIT: *quit = true; break;//If the X button is pressed we will close the game.

			case SDL_KEYDOWN: //Key pressed
				switch (event.key.keysym.sym) {

				case SDLK_UP:
					gameState->menuState = IN_GAME;
					break;

				case SDLK_DOWN:
					gameState->fallingBlock.isFastFalling = true;
					break;

				case SDLK_LEFT:
					moveFallingBlock(gameState, -1, 0);
					break;
				case SDLK_RIGHT:
					moveFallingBlock(gameState, 1, 0);
					break;
				case SDLK_RETURN:
					gameState->menuState = IN_GAME;
					break;
				}
			
				break;

			case SDL_KEYUP: //Key released
				switch (event.key.keysym.sym) {
				case SDLK_UP:
					break;

				case SDLK_DOWN:
					gameState->fallingBlock.isFastFalling = false;
					break;

				case SDLK_LEFT:
					break;

				case SDLK_RIGHT:
					break;

				}

			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mouseX, &mouseY);
				
				// Button dimensions (same as used in the render function)
				int buttonWidth = 285;
				int buttonHeight = 140;
				int buttonX = (SCREEN_WIDTH - buttonWidth) / 2;  // Centered horizontally
				int buttonY = SCREEN_HEIGHT - buttonHeight - 200; // Near the bottom

				// Check if the mouse click is inside the button
				if (mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
					mouseY >= buttonY && mouseY <= buttonY + buttonHeight) {
					// Button was clicked, change to the IN_GAME state
					gameState->menuState = IN_GAME;
				}
				break;
			}
		}
	}
}

int main(int argc, char* args[]) {

	renderInit(SCREEN_WIDTH, SCREEN_HEIGHT); //Initialize our renderer

	//Lets setup the game now that initialization is complete.
	bool quit = false;

	GameState gameState;
	initGame(&gameState); //Pass the address of gameState to initGame to set everything up.

	printf("Init Successful\n");
	showToast("Initialization Successful!");


	while (quit == false) { //Main Loop

		handleInput(&quit, &gameState); //Always handle user input
		

		if (gameState.menuState == TITLE_SCREEN){
			
		}

		drawFrame(&gameState); //Always draw to the screen.

		if (gameState.menuState == IN_GAME) {
			//Update all game logic
			tick(&gameState);
		}

		if (gameState.menuState == DEATH_SCREEN) {
			initGame(&gameState);
			gameState.menuState = TITLE_SCREEN; //We do not have a death screen yet, so if we enter that state, just reset and throw us back to the title screen for now.
		}

		clearFrame();	//Set the color to black and then clear the screen.
		SDL_Delay(16);	//About 60 FPS
	}

	//Main loop over, time to clean up and exit.
	quitGame(); //Tell renderer to stop


	return 0;
}