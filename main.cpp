#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

// Screen dimension constants
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 600;

// Function prototypes
int initSDL(SDL_Window **window, SDL_Renderer **renderer);
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *filePath);
SDL_Texture* renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, SDL_Rect *rect);
int handleMouseEvent(SDL_Event *e, SDL_Rect *rect);
void renderInstructions(SDL_Renderer *renderer, SDL_Texture *helpTexture);
void runSnakeGame(SDL_Renderer *renderer);

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* backgroundTexture = NULL;
    SDL_Texture* snakeBigTexture = NULL;
    SDL_Texture* snakeTreeTexture = NULL;
    SDL_Texture* startTexture = NULL;
    SDL_Texture* instructionsTexture = NULL;
    SDL_Texture* highscoreTexture = NULL;
    SDL_Texture* exitTexture = NULL;
    SDL_Texture* helpTexture = NULL;
    TTF_Font* font = NULL;

    // Initialize SDL
    if (initSDL(&window, &renderer) != 0) {
        return 1;
    }

    // Load textures
    backgroundTexture = loadTexture(renderer, "resources/bg.png");
    snakeBigTexture = loadTexture(renderer, "resources/snakeBig.png");
    snakeTreeTexture = loadTexture(renderer, "resources/snakeTree.png");
    helpTexture = loadTexture(renderer, "resources/help.png");
    if (backgroundTexture == NULL || snakeBigTexture == NULL || snakeTreeTexture == NULL || helpTexture == NULL) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load font
    font = TTF_OpenFont("resources/grobold.ttf", 40);
    if (font == NULL) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(snakeBigTexture);
        SDL_DestroyTexture(snakeTreeTexture);
        SDL_DestroyTexture(helpTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Render texts
    SDL_Color textColor = {255, 255, 255}; // White color

    SDL_Rect startRect = {SCREEN_WIDTH, 140, 0, 0};
    startTexture = renderText(renderer, font, "START", textColor, &startRect);

    SDL_Rect instructionsRect = {SCREEN_WIDTH, 220, 0, 0};
    instructionsTexture = renderText(renderer, font, "INSTRUCTIONS", textColor, &instructionsRect);

    SDL_Rect highscoreRect = {SCREEN_WIDTH, 300, 0, 0};
    highscoreTexture = renderText(renderer, font, "HIGHSCORE", textColor, &highscoreRect);

    SDL_Rect exitRect = {SCREEN_WIDTH, 380, 0, 0};
    exitTexture = renderText(renderer, font, "EXIT", textColor, &exitRect);

    // Main loop flag
    int quit = 0;
    int showInstructions = 0;
    int snakeBigPosX = -SCREEN_WIDTH;  // Initial position outside left edge
    int snakeTreePosX = -SCREEN_WIDTH; // Initial position outside left edge

    // Event handler
    SDL_Event e;

    // While application is running
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (handleMouseEvent(&e, &exitRect)) {
                    quit = 1;
                }
                if (handleMouseEvent(&e, &instructionsRect)) {
                    showInstructions = 1;
                }
                if (handleMouseEvent(&e, &startRect)) {
                    // Start the snake game
                    runSnakeGame(renderer);
                }
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                if (showInstructions) {
                    showInstructions = 0;
                }
            }
        }

        // Clear screen
        SDL_RenderClear(renderer);

        if (showInstructions) {
            renderInstructions(renderer, helpTexture);
        } else {
            // Render background texture
            SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

            // Render snakeBig texture gradually
            if (snakeBigPosX < 0) {
                snakeBigPosX += 5;  // Adjust speed as needed
            }
            SDL_Rect snakeBigRect = {snakeBigPosX, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, snakeBigTexture, NULL, &snakeBigRect);

            // Render snakeTree texture gradually
            if (snakeTreePosX < 0) {
                snakeTreePosX += 5;  // Adjust speed as needed
            }
            SDL_Rect snakeTreeRect = {snakeTreePosX, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, snakeTreeTexture, NULL, &snakeTreeRect);

            // Move and render texts sequentially
            if (startRect.x > 620) {
                startRect.x -= 5;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, startTexture, NULL, &startRect);

            if (instructionsRect.x > 560) {
                instructionsRect.x -= 6;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, instructionsTexture, NULL, &instructionsRect);

            if (highscoreRect.x > 560) {
                highscoreRect.x -= 7;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, highscoreTexture, NULL, &highscoreRect);

            if (exitRect.x > 635) {
                exitRect.x -= 8;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);
        }

        // Update screen
        SDL_RenderPresent(renderer);

        // Delay to control frame rate
        SDL_Delay(4);  // Adjust as needed for smoother animation
    }

    // Cleanup resources
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(snakeBigTexture);
    SDL_DestroyTexture(snakeTreeTexture);
    SDL_DestroyTexture(startTexture);
    SDL_DestroyTexture(instructionsTexture);
    SDL_DestroyTexture(highscoreTexture);
    SDL_DestroyTexture(exitTexture);
    SDL_DestroyTexture(helpTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

// Function to initialize SDL components
int initSDL(SDL_Window **window, SDL_Renderer **renderer) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image for PNG loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_ttf for TrueType font rendering
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create window
    *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer for window
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    return 0;
}

// Function to load texture from file
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *filePath) {
    SDL_Surface *surface = IMG_Load(filePath);
    if (surface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", filePath, IMG_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == NULL) {
        printf("Unable to create texture from %s! SDL Error: %s\n", filePath, SDL_GetError());
        return NULL;
    }

    return texture;
}

// Function to render text to a texture
SDL_Texture* renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, SDL_Rect *rect) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (surface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == NULL) {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        return NULL;
    }

    // Set width and height of the texture
    SDL_QueryTexture(texture, NULL, NULL, &rect->w, &rect->h);

    return texture;
}

// Function to handle mouse events
int handleMouseEvent(SDL_Event *e, SDL_Rect *rect) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    if (x >= rect->x && x <= rect->x + rect->w && y >= rect->y && y <= rect->y + rect->h) {
        return 1;
    }
    return 0;
}

// Function to render instructions
void renderInstructions(SDL_Renderer *renderer, SDL_Texture *helpTexture) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, helpTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

// Function to run the snake game
void runSnakeGame(SDL_Renderer *renderer) {
    // Placeholder for the snake game logic
    SDL_Delay(2000);  // Simulate a delay for starting the game
}
