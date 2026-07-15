#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define HEIGHT 880
#define WIDTH 1080
#define GRID_SIZE 20
#define GRID_COLS (WIDTH / GRID_SIZE)
#define GRID_ROWS (HEIGHT / GRID_SIZE)
#define MAX_SNAKE_LENGTH (GRID_COLS * GRID_ROWS)

int score = 0;

bool checkCollisionWithWall(SDL_FRect *head)
{
    if (head->x < 0 || head->y < 0 || head->x >= WIDTH || head->y >= HEIGHT)
    {
        return true;
    }
    return false;
}

bool checkSelfCollision(SDL_FRect *body, int length)
{
    for (int i = 1; i < length; i++)
    {
        if (body[0].x == body[i].x && body[0].y == body[i].y)
        {
            return true;
        }
    }
    return false;
}

void drawSnake(SDL_Renderer *renderer, SDL_FRect *body, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (i == 0)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        SDL_RenderFillRect(renderer, &body[i]);
    }
}

void scoreCount(SDL_Renderer *renderer, TTF_Font *font)
{
    if (font == NULL)
        return;

    char scoreStr[32];
    sprintf(scoreStr, "Score: %d", score);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, scoreStr, 0, textColor);

    if (textSurface != NULL)
    {
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture != NULL)
        {
            SDL_FRect dstRect;
            dstRect.w = (float)textSurface->w;
            dstRect.h = (float)textSurface->h;
            dstRect.x = 20.0f;
            dstRect.y = 20.0f;

            SDL_RenderTexture(renderer, textTexture, NULL, &dstRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_DestroySurface(textSurface);
    }
}

void spawnFood(SDL_FRect *food, SDL_FRect *body, int snakeLength)
{
    bool isFoodOnSnake = true;
    while (isFoodOnSnake)
    {
        int randomCol = rand() % GRID_COLS;
        int randomRow = rand() % GRID_ROWS;

        food->x = (float)(randomCol * GRID_SIZE);
        food->y = (float)(randomRow * GRID_SIZE);
        food->w = GRID_SIZE;
        food->h = GRID_SIZE;

        isFoodOnSnake = false;

        for (int i = 0; i < snakeLength; i++)
        {
            if (food->x == body[i].x && food->y == body[i].y)
            {
                isFoodOnSnake = true;
                break;
            }
        }
    }
}

bool collisionWithFood(SDL_FRect *head, SDL_FRect *food)
{
    if (head->x == food->x && head->y == food->y)
    {
        return true;
    }
    return false;
}

void drawGameOverAndLastScore(SDL_Renderer *renderer, TTF_Font *font, int currentScore)
{
    if (font == NULL)
    {
        SDL_Log("game Over");
        return;
    }
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, "Game Over", 0, textColor);
    if (textSurface != NULL)
    {
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture != NULL)
        {
            SDL_FRect dstRect;
            dstRect.w = (float)textSurface->w;
            dstRect.h = (float)textSurface->h;
            dstRect.x = (WIDTH - dstRect.w) / 2.0f;
            dstRect.y = (HEIGHT - dstRect.h) / 2.0f;
            SDL_RenderTexture(renderer, textTexture, NULL, &dstRect);

            char scoreStr[32];
            sprintf(scoreStr, "Total Score: %d", currentScore);
            SDL_Surface *scoreSurface = TTF_RenderText_Blended(font, scoreStr, 0, textColor);
            if (scoreSurface != NULL)
            {
                SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                if (scoreTexture != NULL)
                {
                    SDL_FRect scoreRect;
                    scoreRect.w = (float)scoreSurface->w;
                    scoreRect.h = (float)scoreSurface->h;
                    scoreRect.x = (WIDTH - scoreRect.w) / 2.0f;
                    scoreRect.y = dstRect.y + dstRect.h + 20.0f;

                    SDL_RenderTexture(renderer, scoreTexture, NULL, &scoreRect);
                    SDL_DestroyTexture(scoreTexture);
                }
                SDL_DestroySurface(scoreSurface);
            }
            SDL_DestroyTexture(textTexture);
        }
        SDL_DestroySurface(textSurface);
    }
}

int countPoint=0;
int delay=125;


int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    window = SDL_CreateWindow("snake game",
                              WIDTH,
                              HEIGHT,
                              SDL_WINDOW_RESIZABLE);

    if (!TTF_Init())
    {
        SDL_Log("TTF Init Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    if (window == NULL)
    {
        SDL_Log("window creating error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL)
    {
        SDL_Log("renderer creating error:%s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    TTF_Font *font = TTF_OpenFont("font.ttf", 24);
    if (font == NULL)
    {
        SDL_Log("Font loading error", SDL_GetError());
    }

    SDL_FRect snakeBody[MAX_SNAKE_LENGTH];
    int snakeLength = 4;
    snakeBody[0].x = 3*GRID_SIZE;
    snakeBody[0].y = HEIGHT / 2;
    snakeBody[0].h = GRID_SIZE;
    snakeBody[0].w = GRID_SIZE;

    snakeBody[1].x = 2*GRID_SIZE;
    snakeBody[1].y = HEIGHT / 2;
    snakeBody[1].h = GRID_SIZE;
    snakeBody[1].w = GRID_SIZE;


    snakeBody[2].x = GRID_SIZE;
    snakeBody[2].y = HEIGHT / 2;
    snakeBody[2].h = GRID_SIZE;
    snakeBody[2].w = GRID_SIZE;

    snakeBody[3].x = 0.0;
    snakeBody[3].y = HEIGHT / 2;
    snakeBody[3].h = GRID_SIZE;
    snakeBody[3].w = GRID_SIZE;

    SDL_FRect food;
    spawnFood(&food, snakeBody, snakeLength);

    float dirX = 0.0f;
    float dirY = 0.0f;

    bool isRunning = true;
    bool gameOver = false;
    bool isPaused = false;
    SDL_Event event;

    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                isRunning = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (gameOver)
                {
                    if (event.key.key == SDLK_ESCAPE)
                    {
                        isRunning = false;
                        continue;
                    }
                }
                if (event.key.key == SDLK_SPACE && !gameOver)
                {
                    isPaused = !isPaused;
                    continue;
                }

                if (!isPaused)
                {
                    switch (event.key.key)
                    {
                    case SDLK_UP:
                        if (dirY == 0.0f)
                        {
                            dirX = 0.0f;
                            dirY = -GRID_SIZE;
                        }
                        break;
                    case SDLK_DOWN:
                        if (dirY == 0.0f)
                        {
                            dirX = 0.0f;
                            dirY = GRID_SIZE;
                        }
                        break;
                    case SDLK_LEFT:
                        if (dirX == 0.0f)
                        {
                            dirX = -GRID_SIZE;
                            dirY = 0.0f;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (dirX == 0.0f)
                        {
                            dirX = GRID_SIZE;
                            dirY = 0.0f;
                        }
                        break;
                    }
                }
            }
        }

        if (!gameOver && !isPaused)
        {
            if (dirX != 0.0f || dirY != 0.0)
            {
                for (int i = snakeLength - 1; i > 0; i--)
                {
                    snakeBody[i] = snakeBody[i - 1];
                }
                snakeBody[0].x += dirX;
                snakeBody[0].y += dirY;
            if(snakeBody[0].x<0){                           //collision delt
                snakeBody[0].x=WIDTH-GRID_SIZE;
            }
            else if(snakeBody[0].x>WIDTH){
                snakeBody[0].x=0;
            } 

            if(snakeBody[0].y<0){
                snakeBody[0].y=HEIGHT-GRID_SIZE;
            }
            else if(snakeBody[0].y>HEIGHT){
                snakeBody[0].y=0;
            }

            }
            if (collisionWithFood(&snakeBody[0], &food))
            {     countPoint+=1;
                score += 10;
                if (snakeLength < MAX_SNAKE_LENGTH)
                {
                    snakeBody[snakeLength] = snakeBody[snakeLength - 1];
                    snakeBody[snakeLength].x = -GRID_SIZE;
                    snakeBody[snakeLength].y = -GRID_SIZE;
                    snakeLength++;
                }
                spawnFood(&food, snakeBody, snakeLength);
                // if(countPoint%3==0){
                //     delay-=50;
                // }
            }
            if ( checkSelfCollision(snakeBody, snakeLength))
            {
                gameOver = true;
            }
            // if ( checkCollisionWithWall(&snakeBody[0]))
            // {
            //     gameOver = true;
            // }
           
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);

        if (!gameOver)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            SDL_RenderFillRect(renderer, &food);

            drawSnake(renderer, &snakeBody[0], snakeLength);
            scoreCount(renderer, font);
        }
        else
        {
            drawGameOverAndLastScore(renderer, font, score);
        }

        SDL_RenderPresent(renderer);

       // if(countPoint%3==0) delay-=20;

        SDL_Delay(delay);
    }

    if (font != NULL)
    {
        TTF_CloseFont(font);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}