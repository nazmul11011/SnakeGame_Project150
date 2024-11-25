#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CELL_SIZE 20
#define BORDER_THICKNESS 5
#define SNAKE_MAX_LENGTH 100
#define BORDER_OFFSET 50

typedef struct {
    int x, y;
} Point;

bool checkBorderCollision(Point head) {
    if (head.y == 0 && head.x >= (SCREEN_WIDTH / 2 + BORDER_OFFSET) / CELL_SIZE && head.x < SCREEN_WIDTH / CELL_SIZE) {
        return true;
    }
    if (head.x == (SCREEN_WIDTH - BORDER_THICKNESS - BORDER_OFFSET) / CELL_SIZE && head.y >= (SCREEN_HEIGHT / 2) / CELL_SIZE) {
        return true;
    }
    if (head.y == (SCREEN_HEIGHT - BORDER_THICKNESS - BORDER_OFFSET) / CELL_SIZE && head.x < (SCREEN_WIDTH / 2 - BORDER_OFFSET) / CELL_SIZE) {
        return true;
    }
    if (head.x == BORDER_OFFSET / CELL_SIZE && head.y >= (SCREEN_HEIGHT / 2) / CELL_SIZE) {
        return true;
    }
    return false;
}

void drawBorders(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect topRightBorder = {SCREEN_WIDTH / 2 + BORDER_OFFSET, BORDER_OFFSET, SCREEN_WIDTH / 2 - BORDER_OFFSET, BORDER_THICKNESS};
    SDL_RenderFillRect(renderer, &topRightBorder);
    SDL_Rect rightSideBorder = {SCREEN_WIDTH - BORDER_THICKNESS - BORDER_OFFSET, SCREEN_HEIGHT / 2, BORDER_THICKNESS, SCREEN_HEIGHT / 2 - BORDER_OFFSET};
    SDL_RenderFillRect(renderer, &rightSideBorder);
    SDL_Rect bottomLeftBorder = {BORDER_OFFSET, SCREEN_HEIGHT - BORDER_THICKNESS - BORDER_OFFSET, SCREEN_WIDTH / 2 - BORDER_OFFSET, BORDER_THICKNESS};
    SDL_RenderFillRect(renderer, &bottomLeftBorder);
    SDL_Rect bottomSideBorder = {BORDER_OFFSET, SCREEN_HEIGHT / 2, BORDER_THICKNESS, SCREEN_HEIGHT / 2 - BORDER_OFFSET};
    SDL_RenderFillRect(renderer, &bottomSideBorder);
}

void drawSnake(SDL_Renderer *renderer, Point *snake, int length) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 0; i < length; i++) {
        SDL_Rect rect = {snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void drawFood(SDL_Renderer *renderer, Point food) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect rect = {food.x * CELL_SIZE, food.y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &rect);
}

bool checkCollision(Point a, Point b) {
    return a.x == b.x && a.y == b.y;
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    bool running = true;
    SDL_Event event;
    Point snake[SNAKE_MAX_LENGTH];
    int snakeLength = 3;
    snake[0].x = 10;
    snake[0].y = 10;
    snake[1].x = 9;
    snake[1].y = 10;
    snake[2].x = 8;
    snake[2].y = 10;
    Point food = {15, 10};
    int dx = 1, dy = 0;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP && dy == 0) {
                    dx = 0;
                    dy = -1;
                }
                if (event.key.keysym.sym == SDLK_DOWN && dy == 0) {
                    dx = 0;
                    dy = 1;
                }
                if (event.key.keysym.sym == SDLK_LEFT && dx == 0) {
                    dx = -1;
                    dy = 0;
                }
                if (event.key.keysym.sym == SDLK_RIGHT && dx == 0) {
                    dx = 1;
                    dy = 0;
                }
            }
        }
        for (int i = snakeLength - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }
        snake[0].x += dx;
        snake[0].y += dy;
        if (checkCollision(snake[0], food)) {
            snakeLength++;
            food.x = rand() % (SCREEN_WIDTH / CELL_SIZE - 2) + 1;
            food.y = rand() % (SCREEN_HEIGHT / CELL_SIZE - 2) + 1;
        }
        for (int i = 1; i < snakeLength; i++) {
            if (checkCollision(snake[0], snake[i])) {
                running = false;
            }
        }
        if (snake[0].x < 1 || snake[0].x >= SCREEN_WIDTH / CELL_SIZE - 1 || snake[0].y < 1 || snake[0].y >= SCREEN_HEIGHT / CELL_SIZE - 1) {
            running = false;
        }
        if (checkBorderCollision(snake[0])) {
            running = false;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawBorders(renderer);
        drawSnake(renderer, snake, snakeLength);
        drawFood(renderer, food);
        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}