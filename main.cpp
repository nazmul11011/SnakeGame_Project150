#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

// Screen dimension constants
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 600;

// Snake game constants
const char* BACKGROUND_GAME = "resources/bgS.png";
const char* HEAD_UP = "resources/headUp.png";
const char* HEAD_DOWN = "resources/headDown.png";
const char* HEAD_LEFT = "resources/headLeft.png";
const char* HEAD_RIGHT = "resources/headRight.png";
const char* BODY_HORIZONTAL = "resources/bodyHorr.png";
const char* BODY_VERTICAL = "resources/bodyVert.png";

// Highscore constants
const char* BACKGROUND_HIGHSCORE = "resources/highBG.png";

// Function prototypes
int initSDL(SDL_Window **window, SDL_Renderer **renderer);
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *filePath);
SDL_Texture* renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, SDL_Rect *rect);
int handleMouseEvent(SDL_Event *e, SDL_Rect *rect);
void renderInstructions(SDL_Renderer *renderer, SDL_Texture *helpTexture);
void runSnakeGame(SDL_Renderer *renderer);
void displayHighscore(SDL_Renderer *renderer, TTF_Font *font);

// Snake game state
typedef struct {
    int x, y;
    int dx, dy;
    SDL_Texture* headTexture;
    SDL_Texture* bodyTexture;
    SDL_Rect segments[100];
    int length;
} Snake;

void initSnake(Snake* snake, SDL_Renderer* renderer);
void updateSnake(Snake* snake);
void renderSnake(Snake* snake, SDL_Renderer* renderer);
void handleSnakeEvents(SDL_Event* e, Snake* snake, SDL_Renderer* renderer);

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
    int showSnakeGame = 0;
    int showHighscore = 0;
    int snakeBigPosX = -SCREEN_WIDTH;  // Initial position outside left edge
    int snakeTreePosX = -SCREEN_WIDTH; // Initial position outside left edge

    // Event handler
    SDL_Event e;

    // Snake game state
    Snake snake;

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
                    // Show the snake game
                    showSnakeGame = 1;
                    initSnake(&snake, renderer);
                }
                if (handleMouseEvent(&e, &highscoreRect)) {
                    // Show the highscore
                    showHighscore = 1;
                }
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                if (showInstructions) {
                    showInstructions = 0;
                }
                if (showSnakeGame) {
                    showSnakeGame = 0;
                }
                if (showHighscore) {
                    showHighscore = 0;
                }
            }

            if (showSnakeGame) {
                handleSnakeEvents(&e, &snake, renderer);
            }
        }

        // Clear screen
        SDL_RenderClear(renderer);

        if (showInstructions) {
            renderInstructions(renderer, helpTexture);
        } else if (showSnakeGame) {
            // Render snake game background
            SDL_Texture* gameBackground = loadTexture(renderer, BACKGROUND_GAME);
            SDL_RenderCopy(renderer, gameBackground, NULL, NULL);
            SDL_DestroyTexture(gameBackground);

            updateSnake(&snake);
            renderSnake(&snake, renderer);
        } else if (showHighscore) {
            // Render highscore background
            SDL_Texture* highscoreBackground = loadTexture(renderer, BACKGROUND_HIGHSCORE);
            SDL_RenderCopy(renderer, highscoreBackground, NULL, NULL);
            SDL_DestroyTexture(highscoreBackground);

            // Render highscore text
            SDL_Rect highscoreTextRect = {220, 270, 0, 0}; // Example position
            SDL_Texture* highscoreTextTexture = renderText(renderer, font, "HIGHEST SCORE", textColor, &highscoreTextRect);
            SDL_RenderCopy(renderer, highscoreTextTexture, NULL, &highscoreTextRect);
            SDL_DestroyTexture(highscoreTextTexture);
        } else {
            // Render main menu
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

            if (instructionsRect.x > 550) {
                instructionsRect.x -= 6;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, instructionsTexture, NULL, &instructionsRect);

            if (highscoreRect.x > 570) {
                highscoreRect.x -= 7;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, highscoreTexture, NULL, &highscoreRect);

            if (exitRect.x > 640) {
                exitRect.x -= 8;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);
        }

        // Update screen
        SDL_RenderPresent(renderer);

        // Delay to control frame rate
        SDL_Delay(10);  // Adjust as needed for smoother animation
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

    TTF_CloseFont(font);
    font = NULL;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

int initSDL(SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if (!IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return -1;
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return -1;
    }

    *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *filePath) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(filePath);
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", filePath, IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL_Error: %s\n", filePath, SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

SDL_Texture* renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, SDL_Rect *rect) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
    if (textSurface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return NULL;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL) {
        printf("Unable to create texture from rendered text! SDL_Error: %s\n", SDL_GetError());
    } else {
        rect->w = textSurface->w;
        rect->h = textSurface->h;
    }
    SDL_FreeSurface(textSurface);
    return textTexture;
}

int handleMouseEvent(SDL_Event *e, SDL_Rect *rect) {
    int x, y;
    SDL_GetMouseState(&x, &y);

    if (x >= rect->x && x <= rect->x + rect->w && y >= rect->y && y <= rect->y + rect->h) {
        return 1;
    }
    return 0;
}

void renderInstructions(SDL_Renderer *renderer, SDL_Texture *helpTexture) {
    SDL_RenderCopy(renderer, helpTexture, NULL, NULL);
}

void initSnake(Snake* snake, SDL_Renderer* renderer) {
    snake->x = SCREEN_WIDTH / 2;
    snake->y = SCREEN_HEIGHT / 2;
    snake->dx = 10;
    snake->dy = 0;
    snake->headTexture = loadTexture(renderer, HEAD_RIGHT);
    snake->bodyTexture = loadTexture(renderer, BODY_HORIZONTAL);
    snake->length = 5;

    for (int i = 0; i < snake->length; ++i) {
        snake->segments[i].x = snake->x - i * 10;
        snake->segments[i].y = snake->y;
        snake->segments[i].w = 10;
        snake->segments[i].h = 10;
    }
}

void updateSnake(Snake* snake) {
    for (int i = snake->length - 1; i > 0; --i) {
        snake->segments[i].x = snake->segments[i - 1].x;
        snake->segments[i].y = snake->segments[i - 1].y;
    }

    snake->x += snake->dx;
    snake->y += snake->dy;
    snake->segments[0].x = snake->x;
    snake->segments[0].y = snake->y;
}

void renderSnake(Snake* snake, SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, snake->headTexture, NULL, &snake->segments[0]);

    for (int i = 1; i < snake->length; ++i) {
        SDL_RenderCopy(renderer, snake->bodyTexture, NULL, &snake->segments[i]);
    }
}

void handleSnakeEvents(SDL_Event* e, Snake* snake, SDL_Renderer* renderer) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP:
                if (snake->dy == 0) {
                    snake->dx = 0;
                    snake->dy = -10;
                    SDL_DestroyTexture(snake->headTexture);
                    snake->headTexture = loadTexture(renderer, HEAD_UP);
                }
                break;
            case SDLK_DOWN:
                if (snake->dy == 0) {
                    snake->dx = 0;
                    snake->dy = 10;
                    SDL_DestroyTexture(snake->headTexture);
                    snake->headTexture = loadTexture(renderer, HEAD_DOWN);
                }
                break;
            case SDLK_LEFT:
                if (snake->dx == 0) {
                    snake->dx = -10;
                    snake->dy = 0;
                    SDL_DestroyTexture(snake->headTexture);
                    snake->headTexture = loadTexture(renderer, HEAD_LEFT);
                }
                break;
            case SDLK_RIGHT:
                if (snake->dx == 0) {
                    snake->dx = 10;
                    snake->dy = 0;
                    SDL_DestroyTexture(snake->headTexture);
                    snake->headTexture = loadTexture(renderer, HEAD_RIGHT);
                }
                break;
        }
    }
}

