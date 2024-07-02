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
const char* TAIL_UP = "resources/tailUp.png";
const char* TAIL_DOWN = "resources/tailDown.png";
const char* TAIL_LEFT = "resources/tailLeft.png";
const char* TAIL_RIGHT = "resources/tailRight.png";

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
    SDL_Texture* texture;
    SDL_Rect rect;
} Food;

typedef struct {
    int x, y;
    int dx, dy;
    SDL_Texture* headTexture;
    SDL_Texture* bodyTexture;
    SDL_Texture* tailTexture;
    SDL_Rect segments[100];
    int length;
} Snake;


void initSnake(Snake* snake, SDL_Renderer* renderer);
void updateSnake(Snake* snake);
void renderSnake(Snake* snake, SDL_Renderer* renderer);
void handleSnakeEvents(SDL_Event* e, Snake* snake, SDL_Renderer* renderer);
int isGameOver(Snake* snake);
void generateFood(Food* food);
int checkCollision(Snake* snake, Food* food);
void renderFood(Food* food, SDL_Renderer* renderer);

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
    initSnake(&snake, renderer);

    // Initialize Food
    Food food;
    food.texture = loadTexture(renderer, "resources/food.png");  // Adjust path as needed
    food.rect.w = 15;  // Adjust size as needed
    food.rect.h = 15;  // Adjust size as needed
    generateFood(&food);  // Generate initial food position

    // While application is running
    while (!quit) {
    // Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
        // User requests quit
        if (e.type == SDL_QUIT) {
            quit = 1;
        }

        // Handle mouse events
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
                generateFood(&food);  // Generate initial food
            }
            if (handleMouseEvent(&e, &highscoreRect)) {
                // Show the highscore
                showHighscore = 1;
            }
        }

        // Handle key events
        if (showSnakeGame) {
            handleSnakeEvents(&e, &snake, renderer);
        }

        // Handle ESC key to exit sub-menus
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
    }

    // Clear screen
    SDL_RenderClear(renderer);

    // Update game state and render based on current screen state
    if (showInstructions) {
        renderInstructions(renderer, helpTexture);
    } else if (showSnakeGame) {
        // Check for collision with food
        if (checkCollision(&snake, &food)) {
            snake.length += 1;  // Increase snake's length or update score
            generateFood(&food);  // Generate new food
        }

        // Update snake position and state
        updateSnake(&snake);

        // Render snake and food
        SDL_Texture* gameBackground = loadTexture(renderer, BACKGROUND_GAME);
        SDL_RenderCopy(renderer, gameBackground, NULL, NULL);
        SDL_DestroyTexture(gameBackground);

        renderSnake(&snake, renderer);
        renderFood(&food, renderer);
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
            startRect.x -= 8;  // Adjust speed as needed
        }
        SDL_RenderCopy(renderer, startTexture, NULL, &startRect);

        if (instructionsRect.x > 550) {
            instructionsRect.x -= 7;  // Adjust speed as needed
        }
        SDL_RenderCopy(renderer, instructionsTexture, NULL, &instructionsRect);

        if (highscoreRect.x > 570) {
            highscoreRect.x -= 6;  // Adjust speed as needed
        }
        SDL_RenderCopy(renderer, highscoreTexture, NULL, &highscoreRect);

        if (exitRect.x > 640) {
            exitRect.x -= 5;  // Adjust speed as needed
        }
        SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);
    }

    // Update screen
    SDL_RenderPresent(renderer);

    // Delay to control frame rate
    SDL_Delay(10); // Adjust delay as needed

    // Check game over condition
    if (showSnakeGame && isGameOver(&snake)) {
        // Handle game over, e.g., reset or show score
        printf("Game Over! Length of snake: %d\n", snake.length);
        // Reset the snake for a new game
        initSnake(&snake, renderer);
        generateFood(&food);  // Generate new food for the next game
        SDL_Delay(1000); // Delay for 1 second before restarting
    }
    }

    // Free resources and close SDL
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(snakeBigTexture);
    SDL_DestroyTexture(snakeTreeTexture);
    SDL_DestroyTexture(startTexture);
    SDL_DestroyTexture(instructionsTexture);
    SDL_DestroyTexture(highscoreTexture);
    SDL_DestroyTexture(exitTexture);
    SDL_DestroyTexture(helpTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;

}

// Function to initialize SDL and create window and renderer
int initSDL(SDL_Window **window, SDL_Renderer **renderer) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create renderer for window
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    return 0;
}

// Function to load texture from file
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *filePath) {
    // Load image at specified path
    SDL_Surface* surface = IMG_Load(filePath);
    if (surface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", filePath, IMG_GetError());
        return NULL;
    }

    // Create texture from surface pixels
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        printf("Unable to create texture from %s! SDL Error: %s\n", filePath, SDL_GetError());
    }

    // Free loaded surface
    SDL_FreeSurface(surface);

    return texture;
}

// Function to render text using SDL_ttf
SDL_Texture* renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, SDL_Rect *rect) {
    // Render text surface
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (surface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return NULL;
    }

    // Create texture from surface pixels
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
    }

    // Get width and height of surface
    rect->w = surface->w;
    rect->h = surface->h;

    // Free surface
    SDL_FreeSurface(surface);

    return texture;
}

// Function to handle mouse click events
int handleMouseEvent(SDL_Event *e, SDL_Rect *rect) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    return (mouseX >= rect->x && mouseX <= rect->x + rect->w && mouseY >= rect->y && mouseY <= rect->y + rect->h && e->type == SDL_MOUSEBUTTONDOWN);
}

// Function to render instructions
void renderInstructions(SDL_Renderer *renderer, SDL_Texture *helpTexture) {
    // Clear screen
    SDL_RenderClear(renderer);

    // Render help texture
    SDL_RenderCopy(renderer, helpTexture, NULL, NULL);

    // Update screen
    SDL_RenderPresent(renderer);
}
// Snake game state initialization
void initSnake(Snake* snake, SDL_Renderer* renderer) {
    // Initialize snake starting position and direction
    snake->x = SCREEN_WIDTH/2;
    snake->y = SCREEN_HEIGHT/2;
    snake->dx = 10;
    snake->dy = 0;
    snake->length = 12;

    // Load textures
    snake->headTexture = loadTexture(renderer, HEAD_RIGHT);
    snake->bodyTexture = loadTexture(renderer, BODY_HORIZONTAL);
    snake->tailTexture = loadTexture(renderer, TAIL_RIGHT);

    // Initialize segments
    snake->segments[0].x = snake->x;
    snake->segments[0].y = snake->y;
    snake->segments[0].w = 15; // Adjust size as needed
    snake->segments[0].h = 13; // Adjust size as needed
}

// Snake game update logic
void updateSnake(Snake* snake) {
    // Update snake position based on direction
    for (int i = snake->length - 1; i > 0; --i) {
        snake->segments[i] = snake->segments[i - 1];
    }
    snake->segments[0].x += snake->dx/2;
    snake->segments[0].y += snake->dy/2;
}

// Snake game rendering logic
void renderSnake(Snake* snake, SDL_Renderer* renderer) {
    // Render snake head
    SDL_Rect headRect = {snake->segments[0].x, snake->segments[0].y, 15, 13}; // Adjust size as needed
    SDL_RenderCopy(renderer, snake->headTexture, NULL, &headRect);

    // Render snake body
    for (int i = 1; i < snake->length - 1; ++i) {
        SDL_Rect bodyRect = {snake->segments[i].x, snake->segments[i].y, 15, 13}; // Adjust size as needed
        if (snake->segments[i].x == snake->segments[i - 1].x) {
            SDL_RenderCopy(renderer, snake->bodyTexture, NULL, &bodyRect);
        } else {
            SDL_RenderCopy(renderer, snake->bodyTexture, NULL, &bodyRect);
        }
    }

    // Render snake tail based on direction
    SDL_Rect tailRect = {snake->segments[snake->length - 1].x, snake->segments[snake->length - 1].y, 15, 13}; // Adjust size as needed
    SDL_Texture* tailTexture = NULL;

    if (snake->segments[snake->length - 2].x < snake->segments[snake->length - 1].x) {
        tailTexture = loadTexture(renderer, TAIL_LEFT);
    } else if (snake->segments[snake->length - 2].x > snake->segments[snake->length - 1].x) {
        tailTexture = loadTexture(renderer, TAIL_RIGHT);
    } else if (snake->segments[snake->length - 2].y < snake->segments[snake->length - 1].y) {
        tailTexture = loadTexture(renderer, TAIL_UP);
    } else if (snake->segments[snake->length - 2].y > snake->segments[snake->length - 1].y) {
        tailTexture = loadTexture(renderer, TAIL_DOWN);
    }

    SDL_RenderCopy(renderer, tailTexture, NULL, &tailRect);
    SDL_DestroyTexture(tailTexture);
}

// Handle snake game events
void handleSnakeEvents(SDL_Event* e, Snake* snake, SDL_Renderer* renderer) {
    // Handle key press events for snake direction
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP:
                if (snake->dy == 0) {
                    snake->dx = 0;
                    snake->dy = -10; // Adjust movement speed as needed
                    snake->headTexture = loadTexture(renderer, HEAD_UP);
                    snake->bodyTexture = loadTexture(renderer, BODY_VERTICAL);
                    snake->tailTexture = loadTexture(renderer, TAIL_UP);
                }
                break;
            case SDLK_DOWN:
                if (snake->dy == 0) {
                    snake->dx = 0;
                    snake->dy = 10; // Adjust movement speed as needed
                    snake->headTexture = loadTexture(renderer, HEAD_DOWN);
                    snake->bodyTexture = loadTexture(renderer, BODY_VERTICAL);
                    snake->tailTexture = loadTexture(renderer, TAIL_DOWN);
                }
                break;
            case SDLK_LEFT:
                if (snake->dx == 0) {
                    snake->dx = -10; // Adjust movement speed as needed
                    snake->dy = 0;
                    snake->headTexture = loadTexture(renderer, HEAD_LEFT);
                    snake->bodyTexture = loadTexture(renderer, BODY_HORIZONTAL);
                    snake->tailTexture = loadTexture(renderer, TAIL_LEFT);
                }
                break;
            case SDLK_RIGHT:
                if (snake->dx == 0) {
                    snake->dx = 10; // Adjust movement speed as needed
                    snake->dy = 0;
                    snake->headTexture = loadTexture(renderer, HEAD_RIGHT);
                    snake->bodyTexture = loadTexture(renderer, BODY_HORIZONTAL);
                    snake->tailTexture = loadTexture(renderer, TAIL_RIGHT);
                }
                break;
            default:
                break;
        }
    }
}

// Check if game over (e.g., snake hits boundary or itself)
int isGameOver(Snake* snake) {
    // Implement game over conditions
    // Example: hitting screen boundary
    if (snake->segments[0].x < 15 || snake->segments[0].x >= 929 || snake->segments[0].y < 15 || snake->segments[0].y >= 529) {
        return 1;
    }
    // Example: hitting itself (for loop through snake segments)
    for (int i = 1; i < snake->length; ++i) {
        if (snake->segments[0].x == snake->segments[i].x && snake->segments[0].y == snake->segments[i].y) {
            return 1;
        }
    }
    return 0;
}

void generateFood(Food* food) {
    // Generate random positions for food within the game screen
    food->x = rand() % (SCREEN_WIDTH - food->rect.w);
    food->y = rand() % (SCREEN_HEIGHT - food->rect.h);
    // Adjust to grid for easier handling (optional, depending on game design)
    food->x -= food->x % 10;
    food->y -= food->y % 10;
}

int checkCollision(Snake* snake, Food* food) {
    // Check if snake's head collides with food
    if (snake->segments[0].x < food->x + food->rect.w &&
        snake->segments[0].x + snake->segments[0].w > food->x &&
        snake->segments[0].y < food->y + food->rect.h &&
        snake->segments[0].y + snake->segments[0].h > food->y) {
        return 1;
    }
    return 0;
}

void renderFood(Food* food, SDL_Renderer* renderer) {
    SDL_Rect destRect = { food->x, food->y, food->rect.w, food->rect.h };
    SDL_RenderCopy(renderer, food->texture, NULL, &destRect);
}