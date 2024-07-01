#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>

// Function declarations
SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *file);
TTF_Font *loadFont(const char *file, int size);
Mix_Chunk *loadSound(const char *file);
Mix_Music *loadMusic(const char *file);

void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int x, int y) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = { x, y, surface->w, surface->h };
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
}

SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *file) {
SDL_Surface *surface = IMG_Load(file);
SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
SDL_FreeSurface(surface);
return texture;
}

TTF_Font *loadFont(const char *file, int size) {
    return TTF_OpenFont(file, size);
}

Mix_Chunk *loadSound(const char *file) {
    return Mix_LoadWAV(file);
}

Mix_Music *loadMusic(const char *file) {
    return Mix_LoadMUS(file);
}
int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        return -1;
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! Mix_Error: %s\n", Mix_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return -1;
    }

    // Load textures and fonts
    // SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *file) {
    // SDL_Surface *surface = IMG_Load(file);
    // SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_FreeSurface(surface);
    // return texture;
    // }

    // TTF_Font *loadFont(const char *file, int size) {
    // return TTF_OpenFont(file, size);
    // }

    // Load Textures
    SDL_Texture *Tbg = loadTexture(renderer, "resources/bgS.png");
    SDL_Texture *TheadRight = loadTexture(renderer, "resources/headRight.png");
    SDL_Texture *TheadLeft = loadTexture(renderer, "resources/headLeft.png");
    SDL_Texture *TheadUp = loadTexture(renderer, "resources/headUp.png");
    SDL_Texture *TheadDown = loadTexture(renderer, "resources/headDown.png");
    SDL_Texture *TbodyHorr = loadTexture(renderer, "resources/bodyHorr.png");
    SDL_Texture *TbodyVert = loadTexture(renderer, "resources/bodyVert.png");
    SDL_Texture *TtailUp = loadTexture(renderer, "resources/tailUp.png");
    SDL_Texture *TtailDown = loadTexture(renderer, "resources/tailDown.png");
    SDL_Texture *TtailRight = loadTexture(renderer, "resources/tailRight.png");
    SDL_Texture *TtailLeft = loadTexture(renderer, "resources/tailLeft.png");
    SDL_Texture *TleftUp = loadTexture(renderer, "resources/leftUp.png");
    SDL_Texture *TleftDown = loadTexture(renderer, "resources/leftDown.png");
    SDL_Texture *TrightUp = loadTexture(renderer, "resources/rightUp.png");
    SDL_Texture *TrightDown = loadTexture(renderer, "resources/rightDown.png");
    SDL_Texture *Tfood = loadTexture(renderer, "resources/food.png");
    SDL_Texture *TbonusFood = loadTexture(renderer, "resources/bonusFood.png");
    SDL_Texture *ThaaUp = loadTexture(renderer, "resources/haaUp.png");
    SDL_Texture *ThaaDown = loadTexture(renderer, "resources/haaDown.png");
    SDL_Texture *ThaaLeft = loadTexture(renderer, "resources/haaLeft.png");
    SDL_Texture *ThaaRight = loadTexture(renderer, "resources/haaRight.png");
    SDL_Texture *TmenuBG = loadTexture(renderer, "resources/bg.png");
    SDL_Texture *TsnakeTree = loadTexture(renderer, "resources/snakeTree.png");
    SDL_Texture *TsnakeCute = loadTexture(renderer, "resources/snakeCute.png");
    SDL_Texture *TsnakeBig = loadTexture(renderer, "resources/snakeBig.png");
    SDL_Texture *TmenuTitle = loadTexture(renderer, "resources/title.png");
    SDL_Texture *ToverBG = loadTexture(renderer, "resources/overBG.png");
    SDL_Texture *TbeatBG = loadTexture(renderer, "resources/overBeatBG.png");
    SDL_Texture *ThelpBG = loadTexture(renderer, "resources/help.png");
    SDL_Texture *ThighBG = loadTexture(renderer, "resources/highBG.png");

    // Load Fonts
    TTF_Font *gothic = loadFont("resources/gothic.ttf", 25);
    TTF_Font *grobold = loadFont("resources/grobold.ttf", 40);


    // PASTE THE TEXTURE AND FONT LOADING CODE HERE

    // Load sounds

    // Mix_Chunk *loadSound(const char *file) {
    // return Mix_LoadWAV(file);
    // }

    // Mix_Music *loadMusic(const char *file) {
    // return Mix_LoadMUS(file);
    // }

    // Load Sound Effects
    Mix_Chunk *SBeat = loadSound("resources/eat.wav");
    Mix_Chunk *SBdie = loadSound("resources/die.wav");
    Mix_Chunk *SBselect = loadSound("resources/select.wav");
    Mix_Chunk *SBselected = loadSound("resources/selected.wav");
    Mix_Chunk *SBbonusAppear = loadSound("resources/pop.wav");
    Mix_Chunk *SBbonusTime = loadSound("resources/bonusTime.wav");

    // Load Music
    Mix_Music *menuMusic = loadMusic("resources/intro.mp3");
    Mix_Music *gameMusic = loadMusic("resources/game.mp3");
    Mix_Music *overMusic = loadMusic("resources/over.mp3");

    // PASTE THE SOUND LOADING CODE HERE

    bool running = true;
    int dir = -1; // Direction of the snake, -1 for no direction
    int gameState = 0; // 0: Menu, 1: Playing, 2: Game Over, 3: Instructions, 4: High Scores
    int menuSelection = 0;
    bool music = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    // PASTE EVENT HANDLING CODE HERE
                    while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
        running = false;
    } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
            case SDLK_w:
                if (dir != 2 && dir != 0) dir = 0;
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                if (dir != 3 && dir != 1) dir = 1;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                if (dir != 0 && dir != 2) dir = 2;
                break;
            case SDLK_LEFT:
            case SDLK_a:
                if (dir != 1 && dir != 3) dir = 3;
                break;
            case SDLK_m:
                if (music) {
                    if (Mix_PlayingMusic()) {
                        Mix_PauseMusic();
                        music = false;
                    } else {
                        Mix_ResumeMusic();
                        music = true;
                    }
                }
                break;
            case SDLK_RETURN:
                if (gameState == 0) {
                    switch (menuSelection % 4) {
                        case 0:
                            gameState = 1;
                            Mix_HaltMusic();
                            break;
                        case 1:
                            gameState = 3;
                            break;
                        case 2:
                            gameState = 4;
                            break;
                        case 3:
                            running = false;
                            break;
                        }
                    }
                    break;
                }
            }
        }

                }
            }
        }

        SDL_RenderClear(renderer);

        // Render game objects, texts, etc.
        if (gameState == 0) {
            // Render menu
            SDL_Color white = { 255, 255, 255, 255 };
            renderText(renderer, grobold, "START", white, 350, 140);
            renderText(renderer, grobold, "INSTRUCTIONS", white, 300, 220);
            renderText(renderer, grobold, "HIGHSCORE", white, 320, 300);
            renderText(renderer, grobold, "EXIT", white, 360, 380);
            renderText(renderer, gothic, "Developed By KhanShaheb", white, 350, 560);
        } else if (gameState == 1) {
            // Render game playing state
        } else if (gameState == 2) {
            // Render game over state
        } else if (gameState == 3) {
            // Render instructions
        } else if (gameState == 4) {
            // Render high scores
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    // PASTE THE CLEANUP CODE HERE
    // Free textures
    SDL_DestroyTexture(Tbg);
    SDL_DestroyTexture(TheadRight);
    SDL_DestroyTexture(TheadLeft);
    SDL_DestroyTexture(TheadUp);
    SDL_DestroyTexture(TheadDown);
    SDL_DestroyTexture(TbodyHorr);
    SDL_DestroyTexture(TbodyVert);
    SDL_DestroyTexture(TtailUp);
    SDL_DestroyTexture(TtailDown);
    SDL_DestroyTexture(TtailRight);
    SDL_DestroyTexture(TtailLeft);
    SDL_DestroyTexture(TleftUp);
    SDL_DestroyTexture(TleftDown);
    SDL_DestroyTexture(TrightUp);
    SDL_DestroyTexture(TrightDown);
    SDL_DestroyTexture(Tfood);
    SDL_DestroyTexture(TbonusFood);
    SDL_DestroyTexture(ThaaUp);
    SDL_DestroyTexture(ThaaDown);
    SDL_DestroyTexture(ThaaLeft);
    SDL_DestroyTexture(ThaaRight);
    SDL_DestroyTexture(TmenuBG);
    SDL_DestroyTexture(TsnakeTree);
    SDL_DestroyTexture(TsnakeCute);
    SDL_DestroyTexture(TsnakeBig);
    SDL_DestroyTexture(TmenuTitle);
    SDL_DestroyTexture(ToverBG);
    SDL_DestroyTexture(TbeatBG);
    SDL_DestroyTexture(ThelpBG);
    SDL_DestroyTexture(ThighBG);

    // Free fonts
    TTF_CloseFont(gothic);
    TTF_CloseFont(grobold);

    // Free sound effects
    Mix_FreeChunk(SBeat);
    Mix_FreeChunk(SBdie);
    Mix_FreeChunk(SBselect);
    Mix_FreeChunk(SBselected);
    Mix_FreeChunk(SBbonusAppear);
    Mix_FreeChunk(SBbonusTime);

    // Free music
    Mix_FreeMusic(menuMusic);
    Mix_FreeMusic(gameMusic);
    Mix_FreeMusic(overMusic);

    // Quit SDL subsystems
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();


    return 0;
}
