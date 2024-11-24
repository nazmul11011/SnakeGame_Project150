// <---------------------Note--------------------->
// This is my first game project for my university course SWE-0610-1250.
// Library: SDL2(https://www.libsdl.org/)
// WiKi: https://wiki.libsdl.org/SDL2/FrontPage
// Developer: MD. NAZMUL ALAM (Tuhin)
// Credits: Snake texture: http://rembound.com/, Audio: http://zapsplat.com/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

// Screen dimension constants
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 600;

// Game loading speed FPS
const int SCREEN_FPS = 15;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

int bonusTimer=0;
Uint32 bonusStart=0;
Uint32 bonusDuration=3000;

void capFrameRate(Uint32 startTicks) {
    Uint32 frameTicks = SDL_GetTicks() - startTicks;
    if (frameTicks < SCREEN_TICK_PER_FRAME) {
        SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
    }
}

// Snake game constants(resources like background, snake texture, homepage images, high scores images)
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
const char* LEFT_DOWN = "resources/leftDown.png";
const char* RIGHT_DOWN = "resources/rightDown.png";
const char* RIGHT_UP = "resources/rightUp.png";
const char* LEFT_UP = "resources/leftUp.png";
const char* BACKGROUND_HIGHSCORE = "resources/highBG.png";

// Rendering functions for initialize texture, font, game and load scores from txt file
int initSDL(SDL_Window **window, SDL_Renderer **renderer);
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *filePath);
SDL_Texture* renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, SDL_Rect *rect);
void renderInstructions(SDL_Renderer *renderer, SDL_Texture *helpTexture);
//void runSnakeGame(SDL_Renderer *renderer);
//void displayHighscore(SDL_Renderer *renderer, TTF_Font *font);
void loadHighScore(const char *filePath);
void saveHighScore(const char *filePath);

// Snake game structures
typedef struct {
    int x, y;
    SDL_Texture* texture;
    SDL_Rect rect;
} Food;

typedef struct {
    int x, y;
    SDL_Texture* texture;
    SDL_Rect rect;
} bonusFood;

typedef struct {
    int x, y;
    int dx, dy;
    SDL_Texture* headTexture;
    SDL_Texture* bodyTexture;
    SDL_Texture* tailTexture;
    SDL_Rect segments[100];
    int directions[100];
    int length;
    int score;
} Snake;

// Home page items
int highScore = 0;
int selectedMenuItem = 0;  // 0 for START, 1 for INSTRUCTIONS, 2 for HIGH SCORE, 3 for EXIT (START Selected default)

// Function to initialize the snake game
void initSnake(Snake* snake, SDL_Renderer* renderer);
void updateSnake(Snake* snake, Food* food);
void renderSnake(Snake* snake, SDL_Renderer* renderer);
void handleSnakeEvents(SDL_Event* e, Snake* snake, SDL_Renderer* renderer);
int isGameOver(Snake* snake);
void generateFood(Food* food);
void generateBonusFood(bonusFood* bonus);
int checkCollision(Snake* snake, Food* food);
int checkBonusFoodCollision(Snake* snake, bonusFood* bonus);
void renderFood(Food* food, SDL_Renderer* renderer);
void renderBonusFood(bonusFood* bonus, SDL_Renderer* renderer) ;

// Game main function
int main(int argc, char* args[]) {
    Uint32 startTicks;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* backgroundTexture = NULL;
    SDL_Texture* gameOverTexture = NULL;
    SDL_Texture* snakeBigTexture = NULL;
    SDL_Texture* snakeTreeTexture = NULL;
    SDL_Texture* startTexture = NULL;
    SDL_Texture* instructionsTexture = NULL;
    SDL_Texture* highscoreTexture = NULL;
    SDL_Texture* exitTexture = NULL;
    SDL_Texture* helpTexture = NULL;
    TTF_Font* font = NULL;
    TTF_Font* gothicFont = NULL;
    TTF_Font* gothicFontLarge = NULL;
    TTF_Font* largeFont = NULL;


    // ** Music for intro and game(not working due to memory allocation)
    // // Mix_Music pointers
    // Mix_Music *menuMusic = NULL;
    // Mix_Music *gameMusic = NULL;

    // // Initializing SDL_mixer
    // if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
    //     printf("SDL_mixer initialization failed! Error: %s\n", Mix_GetError());
    //     return -1;
    // }

    // // Load music
    // menuMusic = Mix_LoadMUS("resources/intro.mp3");
    // if (!menuMusic){
    //     printf("Failed to load menu music! Error: %s\n", Mix_GetError());
    // }

    // gameMusic = Mix_LoadMUS("resources/game.mp3");
    // if (!gameMusic){
    //     printf("Failed to load game music! Error: %s\n", Mix_GetError());
    // }

    // // music by default
    // if (Mix_PlayMusic(menuMusic, -1) == -1){
    //     printf("Mix_PlayMusic failed: %s\n", Mix_GetError());
    // }


    // Initializing SDL
    if (initSDL(&window, &renderer) != 0) {
        return 1;
    }

    // Load the high score txt file
    loadHighScore("resources/highscore.txt");

    // Load textures gameover and front page
    gameOverTexture = IMG_LoadTexture(renderer, "resources/overBG.png");
    backgroundTexture = loadTexture(renderer, "resources/bg.png");
    snakeBigTexture = loadTexture(renderer, "resources/snakeBig1.png");
    snakeTreeTexture = loadTexture(renderer, "resources/snakeTree.png");
    helpTexture = loadTexture(renderer, "resources/help.png");
    if (backgroundTexture == NULL || snakeBigTexture == NULL || snakeTreeTexture == NULL || helpTexture == NULL || gameOverTexture == NULL) {
        printf("Failed to load texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load font
    largeFont = TTF_OpenFont("resources/SuperMario.ttf", 80); // larger font for "SNAKE GAME" in home page
    gothicFont = TTF_OpenFont("resources/gothic.ttf", 22);
    gothicFontLarge = TTF_OpenFont("resources/gothic.ttf", 40);
    font = TTF_OpenFont("resources/grobold.ttf", 40);
    if (font == NULL || largeFont == NULL || gothicFont == NULL || gothicFontLarge == NULL) {
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
    SDL_Color textColorWhite = {255, 255, 255}; // white color
    SDL_Color textColorGreen = {121, 175, 107}; // green color
    SDL_Color textColorRed = {255, 0, 0, 255}; // red color
    SDL_Color textColorBlue = {93, 93, 173}; // blue color

    // Render "SNAKE GAME" text in front page
    SDL_Rect snakeGameRect = {396, 51, 0, 0}; // Adjust position as needed,  currently at right side
    SDL_Texture* snakeGameTexture = renderText(renderer, largeFont, "SNAKE GAME", textColorBlue, &snakeGameRect);
    // Options
    SDL_Rect startRect = {SCREEN_WIDTH, 140, 0, 0};
    startTexture = renderText(renderer, font, "START", textColorGreen, &startRect);

    SDL_Rect instructionsRect = {SCREEN_WIDTH, 220, 0, 0};
    instructionsTexture = renderText(renderer, font, "INSTRUCTIONS", textColorGreen, &instructionsRect);

    SDL_Rect highscoreRect = {SCREEN_WIDTH, 300, 0, 0};
    highscoreTexture = renderText(renderer, font, "HIGHSCORE", textColorGreen, &highscoreRect);

    SDL_Rect exitRect = {SCREEN_WIDTH, 380, 0, 0};
    exitTexture = renderText(renderer, font, "EXIT", textColorGreen, &exitRect);

    // Main page loop flags
    int quit = 0;
    int showInstructions = 0;
    int showSnakeGame = 0;
    int showHighscore = 0;
    int snakeBigPosX = -SCREEN_WIDTH;  // Initial position outside left edge ** loading from left side
    int snakeTreePosX = -SCREEN_WIDTH; // Initial position outside left edge
    int gameOver = 0;  // Flag to control game over state

    // Event handler
    SDL_Event e;

    // **Main Snake Game Starts here**
    // Snake game state
    Snake snake;
    initSnake(&snake, renderer);

    int bonusActive=0;

    // Initialize Food
    Food food;
    food.texture = loadTexture(renderer, "resources/food.png");
    food.rect.w = 15;  // Food initial position ** Horizontal position (left to right 15px)
    food.rect.h = 15;  // Food initial position ** Vertical position (top to bottom 15px)
    generateFood(&food);  // Generate normal food

    bonusFood bonus;
    bonus.rect.w = rand() % SCREEN_WIDTH;  // Random grid position respect to width
    bonus.rect.h = rand() % SCREEN_HEIGHT; // Random grid position respect to height
    bonus.texture = loadTexture(renderer, "resources/bonusFood.png");
    generateBonusFood(&bonus);

    // While loop to run full the game
    while (!quit) {
        startTicks = SDL_GetTicks();
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit, pressed ESCAPE
            if (e.type == SDL_QUIT) {
                quit = 1;
            }

            // Handle key events for menu selection
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:  // Move selection up
                        selectedMenuItem--;
                        if (selectedMenuItem < 0) {
                            selectedMenuItem = 3;  // LIMIT:Wrap around to the last item
                        }
                        break;
                    case SDLK_DOWN:  // Move selection down
                        selectedMenuItem++;
                        if (selectedMenuItem > 3) {
                            selectedMenuItem = 0;  // LIMIT:Wrap around to the first item
                        }
                        break;
                    case SDLK_RETURN:  // Enter key to select current item
                        switch (selectedMenuItem) {
                            case 0:  // START selected
                                showSnakeGame = 1;
                                // Mix_HaltMusic(); // Stop menu music
                                // if (Mix_PlayMusic(gameMusic, -1) == -1)
                                // {
                                //     printf("Failed to play game music: %s\n", Mix_GetError());
                                // }
                                initSnake(&snake, renderer);
                                generateFood(&food);
                                break;
                            case 1:  // INSTRUCTIONS selected
                                showInstructions = 1;
                                break;
                            case 2:  // HIGH SCORE selected
                                showHighscore = 1;
                                break;
                            case 3:  // EXIT selected
                                quit = 1;
                                break;
                            default:
                                break;
                        }
                        break;
                    // case SDLK_ESCAPE:
                    //     if (showSnakeGame)
                    //     {
                    //         showSnakeGame = 0;
                    //         Mix_HaltMusic(); // Stop game music
                    //         if (Mix_PlayMusic(menuMusic, -1) == -1)
                    //         {
                    //             printf("Failed to play menu music: %s\n", Mix_GetError());
                    //         }
                    //     }
                    //     break;
                    default:
                        break;
                }
            }

            // Handle key events in main game
            if (showSnakeGame) {
                handleSnakeEvents(&e, &snake, renderer);
            }

            // Handle ESC key to exit from sub-menus
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
                selectedMenuItem = 0;
            }
        }

        // Clear screen to go deeper
        SDL_RenderClear(renderer);

        // Condition to start functions
        if (showInstructions) {
            renderInstructions(renderer, helpTexture);
        }
        else if (showSnakeGame && !gameOver){
            // Check for collision with food
            if (showSnakeGame && checkCollision(&snake, &food)) {
                snake.length += 2;   // Increase snake's length
                snake.score += 1;    // Increase score when snake eats food
                generateFood(&food); // Generate new food
            }

            // Check for collision with bonus food
            if (showSnakeGame && bonusActive && checkBonusFoodCollision(&snake, &bonus)) {
               // snake.length += 3; // Uncomment to implement length increment
                snake.score += 3;
                bonusActive = 0;
            }

            // Generate bonus food at a difference
            if (!bonusActive && snake.score % 10 == 0 && snake.score > 0) //10 point difference
            {
                bonusActive = 1;
                bonusStart = SDL_GetTicks();
            }

            //Bonus timer
            if (bonusActive) {
                Uint32 currentTime = SDL_GetTicks();
                if (currentTime - bonusStart > bonusDuration)
                {
                    bonusActive = 0; // Deactivate bonus food
                }
            }

            // Update snake position and state
            updateSnake(&snake, &food);

            // Render game background
            SDL_Texture *gameBackground = loadTexture(renderer, BACKGROUND_GAME);
            SDL_RenderCopy(renderer, gameBackground, NULL, NULL);
            SDL_DestroyTexture(gameBackground);

            // Render snake, food, and bonus food (if active)
            renderSnake(&snake, renderer);
            renderFood(&food, renderer);
            if (bonusActive){
                renderBonusFood(&bonus, renderer);
            }

            // Render score at the bottom
            char scoreText[50];
            sprintf(scoreText, "Score: %d", snake.score);
            SDL_Rect scoreRect = {10, 565, 0, 0}; // Adjust position as needed
            SDL_Texture *scoreTexture = renderText(renderer, gothicFont, scoreText, textColorRed, &scoreRect);
            SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
            SDL_DestroyTexture(scoreTexture);

            // Render high score
            char highscoreText[50];
            sprintf(highscoreText, "Highscore: %d", highScore);
            SDL_Rect highscoreTextRect = {800, 565, 0, 0}; // Example position
            SDL_Texture *highscoreTextTexture = renderText(renderer, gothicFont, highscoreText, textColorRed, &highscoreTextRect);
            SDL_RenderCopy(renderer, highscoreTextTexture, NULL, &highscoreTextRect);
            SDL_DestroyTexture(highscoreTextTexture);
        }
        else if (showHighscore) {
            // Render highscore background
            SDL_Texture* highscoreBackground = loadTexture(renderer, BACKGROUND_HIGHSCORE);
            SDL_RenderCopy(renderer, highscoreBackground, NULL, NULL);
            SDL_DestroyTexture(highscoreBackground);

            // Render highscore text
            char highscoreText[50];
            sprintf(highscoreText, "Highscore: %d", highScore);
            SDL_Rect highscoreTextRect = {350, 280, 0, 0}; // Example position
            SDL_Texture* highscoreTextTexture = renderText(renderer, font, highscoreText, textColorRed, &highscoreTextRect);
            SDL_RenderCopy(renderer, highscoreTextTexture, NULL, &highscoreTextRect);
            SDL_DestroyTexture(highscoreTextTexture);
        }
        else {
            // Render main menu
            SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

            // Render menu items
            SDL_Rect menuRect = {SCREEN_WIDTH, 140, 0, 0};
            SDL_Texture* menuTextures[4] = {startTexture, instructionsTexture, highscoreTexture, exitTexture};

            for (int i = 0; i < 4; ++i) {
                menuRect.y = 140 + i * 80;  // Adjust vertical spacing as needed
                if (i == selectedMenuItem) {
                    // Highlight selected item
                    SDL_SetTextureColorMod(menuTextures[i], 85, 104, 42);  // Green color for selected item
                    SDL_SetTextureAlphaMod(menuTextures[i], 255);  // Full opacity for selected item
                } else {
                    // Normal color for other items
                    SDL_SetTextureColorMod(menuTextures[i], 121, 130, 59);  // Light green color
                    SDL_SetTextureAlphaMod(menuTextures[i], 128);  // Semi-transparent for other items
                }
                SDL_RenderCopy(renderer, menuTextures[i], NULL, &menuRect);
            }

            // Render snakeBig texture gradually
            if (snakeBigPosX < 0) {
                snakeBigPosX += 42; // Adjust speed as needed, 2px ahead beacause of 1st item
            }
            SDL_Rect snakeBigRect = {snakeBigPosX, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, snakeBigTexture, NULL, &snakeBigRect);

            // Render snakeTree texture gradually
            if (snakeTreePosX < 0) {
            snakeTreePosX += 40;  // Adjust speed as needed
            }
            SDL_Rect snakeTreeRect = {snakeTreePosX, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, snakeTreeTexture, NULL, &snakeTreeRect);

            // Move and render texts sequentially
            if (startRect.x > 622) {
            startRect.x -= 37;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, startTexture, NULL, &startRect);

            if (instructionsRect.x > 550) {
            instructionsRect.x -= 36;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, instructionsTexture, NULL, &instructionsRect);

            if (highscoreRect.x > 570) {
            highscoreRect.x -= 35;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, highscoreTexture, NULL, &highscoreRect);

            if (exitRect.x > 640) {
            exitRect.x -= 34;  // Adjust speed as needed
            }
            SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);

            // Render big text "SNAKE GAME" on top of "START" option
            SDL_RenderCopy(renderer, snakeGameTexture, NULL, &snakeGameRect);
        }

        // Update screen
        SDL_RenderPresent(renderer);

        // Cap for frame rate
        capFrameRate(startTicks);

        // Check game over condition
        if (showSnakeGame && isGameOver(&snake)) {
            gameOver = 1;

            if (snake.score > highScore) {
                highScore = snake.score;
                saveHighScore("resources/highscore.txt");
            }

            printf("Game Over! Length of snake: %d\n", snake.length);
            printf("Your score: %d\n", snake.score);  // Print final score in terminal

            SDL_Delay(1000); // Game over screen loading 1sec delay, multiply it for to increase seconds

            // Remove snake game from screen
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, gameOverTexture, NULL, NULL);

            // Render the score on the game over screen
            char scoreText[50];
            sprintf(scoreText, "Score: %d", snake.score);
            SDL_Rect gameOverScoreRect = {390, 255, 0, 0}; // Adjust position as needed
            SDL_Texture* gameOverScoreTexture = renderText(renderer, gothicFontLarge, scoreText, textColorWhite, &gameOverScoreRect);
            SDL_RenderCopy(renderer, gameOverScoreTexture, NULL, &gameOverScoreRect);
            SDL_DestroyTexture(gameOverScoreTexture);

            SDL_RenderPresent(renderer);

            // Enter or Esc key to restart the game
            int gameOverHandled = 0;
            while (!gameOverHandled) {
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) {
                        quit = 1;
                        gameOverHandled = 1;
                    }
                    if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_ESCAPE) {
                            gameOverHandled = 1;
                            gameOver = 0;
                            // Reset the snake for a new game
                            initSnake(&snake, renderer);
                            generateFood(&food); // Generate new food -> the next game
                            SDL_Delay(100); // Delay 0.1sec before restarting
                        }
                    }
                }
            }
        }

    }

    // Free resources and close SDL
    // Mix_FreeMusic(menuMusic);
    // Mix_FreeMusic(gameMusic);
    // Mix_CloseAudio();
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(snakeBigTexture);
    SDL_DestroyTexture(snakeTreeTexture);
    SDL_DestroyTexture(startTexture);
    SDL_DestroyTexture(instructionsTexture);
    SDL_DestroyTexture(highscoreTexture);
    SDL_DestroyTexture(exitTexture);
    SDL_DestroyTexture(helpTexture);
    SDL_DestroyTexture(snakeGameTexture);
    TTF_CloseFont(largeFont);
    TTF_CloseFont(font);
    TTF_CloseFont(gothicFont);
    TTF_CloseFont(gothicFontLarge);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
// ** Main game ends here **

// Functions for full game is here
// Function to initialize SDL and create window and renderer
int initSDL(SDL_Window **window, SDL_Renderer **renderer) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Function:initSDL, SDL_init failed,Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        printf("Function:initSDL, Window creation failed,Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create renderer for window
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        printf("Function:initSDL, SDL_createRenderer failed,Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("Function:initSDL, SDL_image failed,Error: %s\n", IMG_GetError());
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("Function:initSDL, SDL_ttf failed,Error: %s\n", TTF_GetError());
        return 1;
    }

    return 0;
}

// Function to load texture from file
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *filePath) {
    // Load image
    SDL_Surface* surface = IMG_Load(filePath);
    if (surface == NULL) {
        printf("Function:loadTexture, Surface image loading failed %s! Error: %s\n", filePath, IMG_GetError());
        return NULL;
    }

    // Create texture from surface pixels
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        printf("Function:loadTexture, Texture creation failed %s! Error: %s\n", filePath, SDL_GetError());
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
        printf("Function:renderText, Surface creation failed,Error: %s\n", TTF_GetError());
        return NULL;
    }

    // Create texture from surface pixels
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        printf("Function:renderText, texture creation failed, Error: %s\n", SDL_GetError());
    }

    // Get width and height of surface
    rect->w = surface->w;
    rect->h = surface->h;

    // Free surface
    SDL_FreeSurface(surface);

    return texture;
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
    snake->length = 10;
    snake->score = 0;

    // Load textures
    snake->headTexture = loadTexture(renderer, HEAD_RIGHT);
    snake->bodyTexture = loadTexture(renderer, BODY_HORIZONTAL);
    snake->tailTexture = loadTexture(renderer, TAIL_RIGHT);

    // Initialize segments
    for (int i = 0; i < snake->length; ++i) {
        snake->segments[i].x = snake->x - i * 15;
        snake->segments[i].y = snake->y;
        snake->segments[i].w = 15;
        snake->segments[i].h = 13;
        snake->directions[i] = 0; // Initialize all directions to 0
    }
}

// Snake game update logic
void updateSnake(Snake* snake, Food* food) {
    // Update the position of each segment
    for (int i = snake->length - 1; i > 0; --i) {
        snake->segments[i] = snake->segments[i - 1];
        snake->directions[i] = snake->directions[i - 1]; // Update direction
    }
    // Update head position based on direction
    snake->segments[0].x += snake->dx;
    snake->segments[0].y += snake->dy;

    if (snake->dx > 0) {
        snake->directions[0] = 1; // Moving right
    } else if (snake->dx < 0) {
        snake->directions[0] = 2; // Moving left
    } else if (snake->dy > 0) {
        snake->directions[0] = 3; // Moving down
    } else if (snake->dy < 0) {
        snake->directions[0] = 4; // Moving up
    }

    // Check if snake eats food
    if (checkCollision(snake, food)) {
        snake->length += 1;  // Increase snake's length
        snake->score += 1;
        generateFood(food);  // Generate new food
    }
}

// ** Caution: COMPLEX LOGIC ** ANY CHANGE WILL COST SEVERE DAMAGE TO THE SNAKE BODY :)
// Snake game rendering logic
void renderSnake(Snake* snake, SDL_Renderer* renderer) {
    // Render snake head
    SDL_Rect headRect = {snake->segments[0].x, snake->segments[0].y, 15, 13};
    SDL_RenderCopy(renderer, snake->headTexture, NULL, &headRect);

    // Render snake body segments with joints
    for (int i = 1; i < snake->length - 1; ++i) {
        SDL_Rect segmentRect = {snake->segments[i].x, snake->segments[i].y, 15, 13};

        // Determine texture based on direction changes
        if ((snake->segments[i].y > snake->segments[i - 1].y && snake->segments[i].x == snake->segments[i - 1].x && snake->segments[i].x > snake->segments[i + 1].x && snake->segments[i].y == snake->segments[i + 1].y)
            || (snake->segments[i].y == snake->segments[i - 1].y && snake->segments[i].x > snake->segments[i - 1].x && snake->segments[i].x == snake->segments[i + 1].x && snake->segments[i].y > snake->segments[i + 1].y)) {
            SDL_Texture* jointTexture = loadTexture(renderer, RIGHT_UP);
            SDL_RenderCopy(renderer, jointTexture, NULL, &segmentRect);
            SDL_DestroyTexture(jointTexture);
        } else if ((snake->segments[i].y > snake->segments[i - 1].y && snake->segments[i].x == snake->segments[i - 1].x && snake->segments[i].x < snake->segments[i + 1].x && snake->segments[i].y == snake->segments[i + 1].y)
            || (snake->segments[i].y == snake->segments[i - 1].y && snake->segments[i].x < snake->segments[i - 1].x && snake->segments[i].x == snake->segments[i + 1].x && snake->segments[i].y > snake->segments[i + 1].y)) {
            SDL_Texture* jointTexture = loadTexture(renderer, LEFT_UP);
            SDL_RenderCopy(renderer, jointTexture, NULL, &segmentRect);
            SDL_DestroyTexture(jointTexture);
        } else if ((snake->segments[i].y < snake->segments[i - 1].y && snake->segments[i].x == snake->segments[i - 1].x && snake->segments[i].x > snake->segments[i + 1].x && snake->segments[i].y == snake->segments[i + 1].y)
            || (snake->segments[i].y == snake->segments[i - 1].y && snake->segments[i].x > snake->segments[i - 1].x && snake->segments[i].x == snake->segments[i + 1].x && snake->segments[i].y < snake->segments[i + 1].y)) {
            SDL_Texture* jointTexture = loadTexture(renderer, RIGHT_DOWN);
            SDL_RenderCopy(renderer, jointTexture, NULL, &segmentRect);
            SDL_DestroyTexture(jointTexture);
        } else if ((snake->segments[i].y < snake->segments[i - 1].y && snake->segments[i].x == snake->segments[i - 1].x && snake->segments[i].x < snake->segments[i + 1].x && snake->segments[i].y == snake->segments[i + 1].y)
            || (snake->segments[i].y == snake->segments[i - 1].y && snake->segments[i].x < snake->segments[i - 1].x && snake->segments[i].x == snake->segments[i + 1].x && snake->segments[i].y < snake->segments[i + 1].y)) {
            SDL_Texture* jointTexture = loadTexture(renderer, LEFT_DOWN);
            SDL_RenderCopy(renderer, jointTexture, NULL, &segmentRect);
            SDL_DestroyTexture(jointTexture);
        } else {
            // Render snake body segments
            SDL_Rect bodyRect = {snake->segments[i].x, snake->segments[i].y, 15, 13}; // Adjust size as needed
            if (snake->segments[i].x == snake->segments[i - 1].x) {
                SDL_RenderCopy(renderer, loadTexture(renderer, BODY_VERTICAL), NULL, &bodyRect);
            } else {
                SDL_RenderCopy(renderer, loadTexture(renderer, BODY_HORIZONTAL), NULL, &bodyRect);
            }
        }
    }

    SDL_Texture* tailTexture = NULL;
    // Determine and render the tail texture based on the direction of the last segment
    int tailIdx = snake->length - 1;
    if (snake->segments[tailIdx].x > snake->segments[tailIdx - 1].x) {
        snake->tailTexture = loadTexture(renderer, TAIL_LEFT);
    } else if (snake->segments[tailIdx].x < snake->segments[tailIdx - 1].x) {
        snake->tailTexture = loadTexture(renderer, TAIL_RIGHT);
    } else if (snake->segments[tailIdx].y > snake->segments[tailIdx - 1].y) {
        snake->tailTexture = loadTexture(renderer, TAIL_UP);
    } else if (snake->segments[tailIdx].y < snake->segments[tailIdx - 1].y) {
        snake->tailTexture = loadTexture(renderer, TAIL_DOWN);
    }

    // Render the tail
    SDL_RenderCopy(renderer, snake->tailTexture, NULL, &snake->segments[tailIdx]);
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
    // Condition 1: hitting screen boundary
    if (snake->segments[0].x < 15 || snake->segments[0].x >= 929 || snake->segments[0].y < 15 || snake->segments[0].y >= 529) {
        return 1;
    }
    // Condition 2: hitting itself (for loop through snake segments)
    for (int i = 1; i < snake->length; ++i) {
        if (snake->segments[0].x == snake->segments[i].x && snake->segments[0].y == snake->segments[i].y) {
            return 1;
        }
    }
    return 0;
}

// Generate random positions for food within the game screen
void generateFood(Food* food) {
    food->x = 16 + rand() % (928 - 16 - food->rect.w);  // Adjusted for x-axis within the specified range
    food->y = 16 + rand() % (528 - 16 - food->rect.h);  // Adjusted for y-axis within the specified range
    // Food generation grid
    food->x -= food->x % 10;
    food->y -= food->y % 10;
}

// Generate random positions for bonus food within the game screen
void generateBonusFood(bonusFood* bonus) {
    bonus->x = 16 + rand() % (928 - 16 - bonus->rect.w); // x-axis range
    bonus->y = 16 + rand() % (528 - 16 - bonus->rect.h); // y-axis range

    // Bonus food generation grid
    bonus->x -= bonus->x % 10;
    bonus->y -= bonus->y % 10;

    bonus->rect.w = 15; // Image width
    bonus->rect.h = 15; // Image height

    // Update the rect position based on the new coordinates
    bonus->rect.x = bonus->x;
    bonus->rect.y = bonus->y;
}

// Check if snake's head collides with food
int checkCollision(Snake* snake, Food* food) {
    if (snake->segments[0].x < food->x + food->rect.w &&
        snake->segments[0].x + snake->segments[0].w > food->x &&
        snake->segments[0].y < food->y + food->rect.h &&
        snake->segments[0].y + snake->segments[0].h > food->y) {
        return 1;
    }
    return 0;
}

int checkBonusFoodCollision(Snake* snake, bonusFood* bonus) {
    // Check if the snake's head collides with the bonus food
    if (snake->segments[0].x < bonus->x + bonus->rect.w &&
        snake->segments[0].x + snake->segments[0].w > bonus->x &&
        snake->segments[0].y < bonus->y + bonus->rect.h &&
        snake->segments[0].y + snake->segments[0].h > bonus->y) {
        return 1; // Collision detected
    }
    return 0; // No collision
}

void renderFood(Food* food, SDL_Renderer* renderer) {
    SDL_Rect destRect = { food->x, food->y, food->rect.w, food->rect.h };
    SDL_RenderCopy(renderer, food->texture, NULL, &destRect);
}
void renderBonusFood(bonusFood* bonus, SDL_Renderer* renderer) {
    // Create a destination rectangle based on the bonus food's position and dimensions
    SDL_Rect destRect = { bonus->x, bonus->y, bonus->rect.w, bonus->rect.h };
    // Render the bonus food texture
    SDL_RenderCopy(renderer, bonus->texture, NULL, &destRect);
}

void loadHighScore(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Failed to load high score file: %s\n", filePath);
        return;
    }
    fscanf(file, "%d", &highScore);
    fclose(file);
}

void saveHighScore(const char *filePath) {
    FILE *file = fopen(filePath, "w");
    if (file == NULL) {
        printf("Failed to write high score file: %s\n", filePath);
        return;
    }
    fprintf(file, "%d", highScore);
    fclose(file);
}