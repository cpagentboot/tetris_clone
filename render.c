/*
Responsible for rendering everything to the screen, I think.
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#include <stdio.h>
#include <math.h>
#include <string.h>

#include "render.h"
#include "gameState.h"



SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* blockTexture;
SDL_Texture* playButtonTexture;
SDL_Texture* quitButtonTexture;
SDL_Texture* textTexture;
SDL_Texture* titleTexture;
SDL_Texture* gradientTexture;
SDL_Texture* dylanTexture;

//Text rendering stuff
TTF_Font* font;
TTF_Font* pixelFont;


SDL_Rect textRect;
SDL_Rect titleRect;
SDL_Rect dylanRect;
char* renderText;
int textFade; //Keep track of the alpha value for the toasts
int titleAnimation; //Frame counter for title screen animations.


int renderInit(int windowWidth, int windowHeight) {
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    //Create window
    window = SDL_CreateWindow("Block Stacker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    //Initialize renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Load all of the assets into memory.

    blockTexture = IMG_LoadTexture(renderer, "assets/block.png");
    playButtonTexture = IMG_LoadTexture(renderer, "assets/PlayButton.png");
    quitButtonTexture = IMG_LoadTexture(renderer, "assets/ExitButton.png");
    titleTexture = IMG_LoadTexture(renderer, "assets/Title.png");

    // Check if the assets loaded successfully
    if (!blockTexture) {
        printf("Failed to load texture: %s\n", IMG_GetError());
        return -1;
    }

    TTF_Init(); //Init truetype for text rendering.
    font = TTF_OpenFont("assets/Sono-Regular.ttf", 16);
    pixelFont = TTF_OpenFont("assets/pixelFont.ttf", 30);
    if (!font) {
        printf("Failed to load font: %s", TTF_GetError());
        return -1;
    }

    // Render title texture
    SDL_Color textColor = { 255, 255, 255, 255 }; //White color
    SDL_Surface* titleSurface = TTF_RenderText_Solid(pixelFont, "Block Stacker", textColor); //render truetype font to surface
    //Create a texture from that surface
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_SetTextureBlendMode(titleTexture, SDL_BLENDMODE_MOD); //enable blending
    //Define where to render that text
    titleRect.w = titleSurface->w;
    titleRect.h = titleSurface->h;
    titleRect.x = (SCREEN_WIDTH - titleSurface->w) / 2;
    titleRect.y = 100;


    //Render "Dylan Industries" texture
    SDL_Color dylantextColor = { 170, 170, 170, 255 }; //Gray color
    SDL_Surface* dylanSurface = TTF_RenderText_Solid(pixelFont, "DYLAN INDUSTRIES", dylantextColor); //render truetype font to surface
    dylanTexture = SDL_CreateTextureFromSurface(renderer, dylanSurface);
    dylanRect.w = titleSurface->w;
    dylanRect.h = titleSurface->h;
    dylanRect.x = (SCREEN_WIDTH - titleSurface->w) / 2;
    dylanRect.y = SCREEN_HEIGHT - 100;

    //Free surface
    SDL_FreeSurface(titleSurface);


    //Apply gradient 
    SDL_Color colorStart = { 134, 83, 201,255 };  // White start color
    SDL_Color colorEnd = { 196, 17, 17,255 };        // Blue end color
    gradientTexture = createHorizontalGradient(renderer, titleRect.w, titleRect.h, colorStart, colorEnd);



    //Set animation timer
    titleAnimation = 0;


    return 0;
}

void drawText(SDL_Renderer* renderer, int x, int y) {
    //Insert some more generic text drawing function here
}

void drawToast(SDL_Renderer* renderer) {
    if (textFade > 0) {
        drawText(renderer, 5, 5); //Draw it 5px diag downleft
        SDL_SetTextureAlphaMod(textTexture, textFade);
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        textFade -= 1; //decrease alpha by one each frame until it is zero.
    }

    // Ensure the texture is destroyed after rendering.
    if (textFade <= 0 && textTexture != NULL) {
        // If textFade is zero, clean up the texture if it exists.
        SDL_DestroyTexture(textTexture);
        textTexture = NULL;  // Reset to avoid potential use after free.
    }
}

void showToast(char* text) {
    if ((renderText == NULL)||(strcmp(renderText,text)!= 0)){ //make sure we haven't already made this texture

        // Destroy the old texture if it exists
        if (textTexture != NULL) {
            SDL_DestroyTexture(textTexture);
            textTexture = NULL;  // Reset the texture to avoid using it after free
        }

        renderText = text;

        SDL_Color textColor = {255, 165, 0, 255}; //Orange color
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, renderText, textColor); //render a hello truetype surface

        //Define where to render that text
        textRect.x = 5;
        textRect.y = 5;
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;

        //Create a texture from that surface
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        //Free the surface as we no longer need it after creating the texture
        SDL_FreeSurface(textSurface);
    }

    textFade = 255; //In any case, refresh the timer.
}

void drawBlock(SDL_Renderer* renderer, int x, int y, int width, int height, int r, int g, int b) {

    SDL_Rect spriteRect;

    //Define where to render the sprite
    spriteRect.x = x;
    spriteRect.y = y;
    spriteRect.w = width;
    spriteRect.h = height;

    SDL_SetTextureColorMod(blockTexture, r, g, b); //Tint


    SDL_RenderCopy(renderer, blockTexture, NULL, &spriteRect);

    

}

void drawFrame(GameState* state) {

    if (state->menuState == TITLE_SCREEN) {
        drawTitleScreen(renderer);
    }

    if (state->menuState == IN_GAME) {
        //Draw play field, logo, stuff, right here
        drawPlayField(renderer, state);

        //Draw the currently falling block
        drawFallingBlock(renderer, state);

        
    }

    if (state->menuState == DEATH_SCREEN) {

    }


    //Draw a toast if it is active, no matter what menu we are in.
    drawToast(renderer);

    SDL_RenderPresent(renderer); //Copy everything to the framebuffer (i think)
}

void resolveColor(int* r, int* g, int* b, uint8_t color) { //Change this to an array of int triplets and use the uint8 as the index - BIG BRAIN
    switch (color) {
        case 9: *r = 50;    *g = 50;    *b = 50;    break;  //  Empty block is gray
        case 0: *r = 255;   *g = 255;   *b = 0;     break;  //  Square is yellow
        case 1: *r = 0;     *g = 255;   *b = 255;   break;  //  I is cyan
        case 2: *r = 255;   *g = 135;   *b = 0;     break;  //  L is orange
        case 3: *r = 0;     *g = 0;     *b = 255;   break;  //  J is blue
        case 4: *r = 128;   *g = 0;     *b = 128;   break;  //  T is purple
        case 5: *r = 0;     *g = 255;   *b = 0;     break;  //  S is green
        case 6: *r = 255;   *g = 0;     *b = 0;     break;  //  Z is red
    }
}

void drawFallingBlock(SDL_Renderer* renderer, GameState* state) { //Draw the currently falling block to the screen.

    int r = 0; int g = 0; int b = 0;

    resolveColor(&r,&g,&b,state->fallingBlock.shapeid); //shapeid is the same as color id :)

    static int xpadding = (SCREEN_WIDTH - (GRID_WIDTH * BLOCK_SIZE)) / 2;
    static int ypadding = (SCREEN_HEIGHT - (GRID_HEIGHT * BLOCK_SIZE)) / 2;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (state->fallingBlock.shape[row][col]) { // If this cell in the 4x4 is a block

                drawBlock(renderer, ((state->fallingBlock.x+col) * BLOCK_SIZE) + xpadding, ((state->fallingBlock.y+row) * BLOCK_SIZE) + ypadding, BLOCK_SIZE, BLOCK_SIZE, r, g, b);

            }
        }
    }


    
}

void drawPlayField(SDL_Renderer* renderer, GameState* state) {

    int r = 0;  int g = 0;  int b = 0;

    static int xpadding = (SCREEN_WIDTH - (GRID_WIDTH*BLOCK_SIZE)) / 2;
    static int ypadding = (SCREEN_HEIGHT - (GRID_HEIGHT * BLOCK_SIZE)) / 2;


    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            
           /* Make checkerboard for debugging purposes*
            if (x % 2 == 0) { state->playField.grid[y][x] = (y % 2 == 0); }
            if (x % 2 == 1) { state->playField.grid[y][x] = (y % 2 == 1); }*/

            resolveColor(&r, &g, &b, state->playField.grid[y][x]); //I feel dirty

            drawBlock(renderer, (x * BLOCK_SIZE) + xpadding, (y * BLOCK_SIZE) + ypadding, BLOCK_SIZE, BLOCK_SIZE, r, g, b);
            

        }
        //printf("\n"); // Newline after each row
    }
    //printf("\n");


}

void clearFrame() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

void quitGame() {

    //Destroy textures and fonts
    SDL_DestroyTexture(blockTexture);
    TTF_CloseFont(font);

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();
}

void drawTitleScreen(SDL_Renderer* renderer) {
    // Set blend mode to combine the gradient with the text texture
    //SDL_SetTextureBlendMode(gradientTexture, SDL_BLENDMODE_MOD);

    

    // Draw the gradient as the background
    SDL_RenderCopy(renderer, gradientTexture, NULL, &titleRect);
    // Draw the text texture on top of the gradient
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    //Draw my name on it because i am a narccisist
    SDL_RenderCopy(renderer, dylanTexture, NULL, &dylanRect);


    // Render the "Start" button
    int buttonWidth = 285;
    int buttonHeight = 140;
    int buttonX = (SCREEN_WIDTH - buttonWidth) / 2;  // Center the button horizontally
    int buttonY = SCREEN_HEIGHT - buttonHeight - 200; // Position the button close to the bottom
    SDL_Rect buttonRect = { buttonX, buttonY, buttonWidth, buttonHeight };
    SDL_RenderCopy(renderer, playButtonTexture, NULL, &buttonRect);




    //cover it up and make it slowly appear because fucc it lmao
    SDL_Rect coverRect = { titleAnimation, 0 , SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); //black
    SDL_RenderFillRect(renderer, &coverRect);

    //Increase framecounter for the intro animation
    if (titleAnimation < SCREEN_WIDTH) {
        titleAnimation += 4;  // Stop after reaching the end
    }
}

SDL_Texture* createHorizontalGradient(SDL_Renderer* renderer, int width, int height, SDL_Color colorStart, SDL_Color colorEnd) {
    // Create a surface to draw the gradient on
    SDL_Surface* gradientSurface = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    // Create a pointer to the pixel data of the surface
    Uint32* pixels = (Uint32*)gradientSurface->pixels;

    for (int x = 0; x < width; x++) {
        // Calculate the blending factor based on the x position
        float t = (float)x / width;

        // Interpolate colors
        Uint8 r = colorStart.r + (colorEnd.r - colorStart.r) * t;

        Uint8 g = colorStart.g + (colorEnd.g - colorStart.g) * t;
        Uint8 b = colorStart.b + (colorEnd.b - colorStart.b) * t;
        Uint8 a = colorStart.a + (colorEnd.a - colorStart.a) * t; 
        // Set the pixel color
        Uint32 color = (a << 24) | (r << 16) | (g << 8) | b;  // RGBA color format
        for (int y = 0; y < height; y++) {
            pixels[y * width + x] = color;  // Set pixel color
        }
    }

    // Create a texture from the surface
    SDL_Texture* gradientTexture = SDL_CreateTextureFromSurface(renderer, gradientSurface);

    // Free the surface as it's no longer needed
    SDL_FreeSurface(gradientSurface);

    return gradientTexture;
}
