#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
// হেডার ফাইল ইনক্লুড করার সঠিক নিয়ম (MSYS2 এর জন্য)
#include <SDL3_ttf/SDL_ttf.h> 

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRID_SIZE 20
#define MAX_SNAKE_LENGTH 1000

// স্ল্যাশ (/) ব্যবহার করে ফন্ট পাথ দিলে কম্পাইলার সহজে খুঁজে পায়
#define FONT_PATH "C:/Windows/Fonts/Arial.ttf" 

typedef struct {
    int x, y;
} Point;

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

// টেক্সট রেন্ডার করার ফাংশন
void RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, float x, float y) {
    if (!font) return;
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_FRect dstRect = { x - (surface->w / 2.0f), y - (surface->h / 2.0f), (float)surface->w, (float)surface->h };
        SDL_RenderTexture(renderer, texture, NULL, &dstRect);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }
    if (!TTF_Init()) {
        SDL_Log("TTF_Init Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL3 Snake Game", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // ফন্ট সাইজ লোড করা
    TTF_Font* font_small = TTF_OpenFont(FONT_PATH, 24);
    TTF_Font* font_large = TTF_OpenFont(FONT_PATH, 48);
    
    if (!font_small || !font_large) {
        SDL_Log("ঝামেলা: ফন্ট লোড হতে পারেনি! পাথ চেক করুন: %s", FONT_PATH);
    }

    Point snake[MAX_SNAKE_LENGTH];
    int snake_length = 3;
    snake[0] = (Point){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
    snake[1] = (Point){ (SCREEN_WIDTH / 2) - GRID_SIZE, SCREEN_HEIGHT / 2 };
    snake[2] = (Point){ (SCREEN_WIDTH / 2) - (2 * GRID_SIZE), SCREEN_HEIGHT / 2 };

    Direction dir = DIR_RIGHT;
    Point food = { (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE,
                   (rand() % (SCREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE };

    int score = 0;
    bool running = true;
    bool game_started = false; 
    bool game_over = false;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color green = {0, 255, 0, 255};

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (!game_started && !game_over) {
                    game_started = true;
                    continue;
                }
                
                if (game_over) {
                    if (event.key.key == SDLK_ESCAPE) running = false;
                    if (event.key.key == SDLK_r) {
                        snake_length = 3;
                        snake[0] = (Point){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
                        snake[1] = (Point){ (SCREEN_WIDTH / 2) - GRID_SIZE, SCREEN_HEIGHT / 2 };
                        snake[2] = (Point){ (SCREEN_WIDTH / 2) - (2 * GRID_SIZE), SCREEN_HEIGHT / 2 };
                        dir = DIR_RIGHT;
                        score = 0;
                        game_over = false;
                        game_started = false;
                    }
                    continue;
                }

                switch (event.key.key) {
                    case SDLK_UP:    if (dir != DIR_DOWN) dir = DIR_UP; break;
                    case SDLK_DOWN:  if (dir != DIR_UP) dir = DIR_DOWN; break;
                    case SDLK_LEFT:  if (dir != DIR_RIGHT) dir = DIR_LEFT; break;
                    case SDLK_RIGHT: if (dir != DIR_LEFT) dir = DIR_RIGHT; break;
                    case SDLK_ESCAPE: running = false; break;
                }
            }
        }

        if (game_started && !game_over) {
            for (int i = snake_length - 1; i > 0; i--) {
                snake[i] = snake[i - 1];
            }

            switch (dir) {
                case DIR_UP:    snake[0].y -= GRID_SIZE; break;
                case DIR_DOWN:  snake[0].y += GRID_SIZE; break;
                case DIR_LEFT:  snake[0].x -= GRID_SIZE; break;
                case DIR_RIGHT: snake[0].x += GRID_SIZE; break;
            }

            if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH || 
                snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT) {
                game_over = true;
            }

            for (int i = 1; i < snake_length; i++) {
                if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                    game_over = true;
                }
            }

            if (snake[0].x == food.x && snake[0].y == food.y) {
                score += 10;
                if (snake_length < MAX_SNAKE_LENGTH) {
                    snake_length++;
                }
                food.x = (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
                food.y = (rand() % (SCREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE;
            }
        }

        // রেন্ডারিং পার্ট
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (!game_started && !game_over) {
            // START SCREEN
            RenderText(renderer, font_large, "SNAKE GAME", green, SCREEN_WIDTH / 2.0f, (SCREEN_HEIGHT / 2.0f) - 50);
            RenderText(renderer, font_small, "Press ANY KEY to Start", white, SCREEN_WIDTH / 2.0f, (SCREEN_HEIGHT / 2.0f) + 20);
        } 
        else if (game_over) {
            // GAME OVER SCREEN
            char score_text[50];
            SDL_snprintf(score_text, sizeof(score_text), "Final Score: %d", score);

            RenderText(renderer, font_large, "GAME OVER", red, SCREEN_WIDTH / 2.0f, (SCREEN_HEIGHT / 2.0f) - 60);
            RenderText(renderer, font_small, score_text, white, SCREEN_WIDTH / 2.0f, (SCREEN_HEIGHT / 2.0f) - 10);
            RenderText(renderer, font_small, "Press 'R' to Restart or 'ESC' to Exit", white, SCREEN_WIDTH / 2.0f, (SCREEN_HEIGHT / 2.0f) + 50);
        } 
        else {
            // GAME RUNNING
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_FRect food_rect = { food.x, food.y, GRID_SIZE - 2, GRID_SIZE - 2 };
            SDL_RenderFillRect(renderer, &food_rect);

            for (int i = 0; i < snake_length; i++) {
                if (i == 0) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                else SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
                SDL_FRect snake_rect = { snake[i].x, snake[i].y, GRID_SIZE - 2, GRID_SIZE - 2 };
                SDL_RenderFillRect(renderer, &snake_rect);
            }

            char live_score[30];
            SDL_snprintf(live_score, sizeof(live_score), "Score: %d", score);
            RenderText(renderer, font_small, live_score, white, 60, 20);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    if (font_small) TTF_CloseFont(font_small);
    if (font_large) TTF_CloseFont(font_large);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
