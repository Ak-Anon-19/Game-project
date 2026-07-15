#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#define HIGHT 880
#define WIDTH 1080
#define GRID_SIZE 20
#define GRID_COLS (WIDTH/GRID_SIZE)
#define GRID_ROWS (HIGHT/GRID_SIZE)
#define MAX_SNAKE_LENGTH (GRID_COLS * GRID_ROWS)
int score = 0;
bool CheckCollisionWithWall(SDL_FRect *head)
{
    if (head->x < 0 || head->y < 0 || head->x >= WIDTH || head->y >= HIGHT)
    {
        
        return true;
    }

    return false;
}

bool CheckSelfCollision(SDL_FRect *body, int length)
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

void DrawSnake(SDL_Renderer *renderer, SDL_FRect *body, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (i == 0)
        {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        SDL_RenderFillRect(renderer, &body[i]);
    }

    // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    // SDL_RenderFillRect(renderer, head);
}

void ScoreCount(SDL_Renderer *renderer, TTF_Font *font)
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

void SpawnFood(SDL_FRect *food,SDL_FRect *body,int snake_length)
{
    bool is_food_on_snake=true;
    while(is_food_on_snake){
        int random_col = rand() % GRID_COLS;
        int random_row = rand() % GRID_ROWS;

        food->x = (float)(random_col * GRID_SIZE);
        food->y = (float)(random_row * GRID_SIZE);
        food->w = GRID_SIZE;
        food->h = GRID_SIZE;
        
        is_food_on_snake=false;

        for(int i=0;i<snake_length;i++){
            if(food->x==body[i].x && food->y == body[i].y){
                is_food_on_snake=true;
                break;
            }
        }
    }



    
}
bool CollisionWithFood(SDL_FRect *head, SDL_FRect *food)
{
    if (head->x == food->x && head->y == food->y)
    {
        return true;
    }
    return false;
}

void DrawGameOverAndLastScore(SDL_Renderer *renderer, TTF_Font *font,int current_score)
{
    if (font == NULL)
    {
        SDL_Log("game Over");
        return;
    }
    SDL_Color Textcolor = {255, 255, 255, 255};
    SDL_Surface *TextSurface = TTF_RenderText_Blended(font, "Game Over", 0, Textcolor);
    if (TextSurface != NULL)
    {
        SDL_Texture *TextTexture = SDL_CreateTextureFromSurface(renderer, TextSurface);
        if (TextTexture != NULL)
        {
            SDL_FRect dstRect;
            dstRect.w = (float)TextSurface->w;
            dstRect.h = (float)TextSurface->h;
            dstRect.x = (WIDTH - dstRect.w) / 2.0f;
            dstRect.y = (HIGHT - dstRect.h) / 2.0f;
            SDL_RenderTexture(renderer, TextTexture, NULL, &dstRect);
            
            char scoreStr[32];
            sprintf(scoreStr,"Total Score: %d",current_score);
            SDL_Surface *scoreSurface=TTF_RenderText_Blended(font,scoreStr,0,Textcolor);
            if(scoreSurface!=NULL){
                SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(renderer,scoreSurface);
                if(scoreTexture!=NULL){
                    SDL_FRect scoreRect;
                    scoreRect.w=(float) scoreSurface->w;
                    scoreRect.h=(float) scoreSurface->h;
                    scoreRect.x=(WIDTH-scoreRect.w)/2.0f;
                    scoreRect.y=dstRect.y+dstRect.h+20.0f;

                    SDL_RenderTexture(renderer,scoreTexture,NULL,&scoreRect);
                    SDL_DestroyTexture(scoreTexture);
                }
                SDL_DestroySurface(scoreSurface);
            }
                        SDL_DestroyTexture(TextTexture);
        }
        SDL_DestroySurface(TextSurface);
    }
}

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    window = SDL_CreateWindow("snake game",
                              WIDTH,
                              HIGHT,
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

    SDL_FRect snake_body[MAX_SNAKE_LENGTH];
    int snake_length = 1;
    snake_body[0].x = 0.0;
    snake_body[0].y = 10 * GRID_SIZE;
    snake_body[0].h = GRID_SIZE;
    snake_body[0].w = GRID_SIZE;

    SDL_FRect food;
    SpawnFood(&food,snake_body,snake_length);

    float dir_x = 0.0f;
    float dir_y = 0.0f;

    bool isrunning = true;      
    bool game_over = false;
    bool is_paused = false;
    SDL_Event event;

    while (isrunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                isrunning = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (game_over)
                {
                    if (event.key.key == SDLK_ESCAPE)
                    {
                        isrunning = false;
                        continue;
                    }
                }
                switch (event.key.key)
                {
                case SDLK_UP:
                    if (dir_y == 0.0f)
                    {
                        dir_x = 0.0f;
                        dir_y = -GRID_SIZE;
                    }
                    break;
                case SDLK_DOWN:
                    if (dir_y == 0.0f)
                    {
                        dir_x = 0.0f;
                        dir_y = GRID_SIZE;
                    }
                    break;
                case SDLK_LEFT:
                    if (dir_x == 0.0f)
                    {
                        dir_x = -GRID_SIZE;
                        dir_y = 0.0f;
                    }
                    break;
                case SDLK_RIGHT:
                    if (dir_x == 0.0f)
                    {
                        dir_x = GRID_SIZE;
                        dir_y = 0.0f;
                    }
                    break;
                }
            }
        }

        if (!game_over)
        {
            for (int i = snake_length - 1; i > 0; i--)
            {
                snake_body[i] = snake_body[i - 1];
            }
            snake_body[0].x += dir_x;
            snake_body[0].y += dir_y;

            if (CollisionWithFood(&snake_body[0], &food))
            {
                score += 10;
                if (snake_length < MAX_SNAKE_LENGTH)
                {
                    snake_body[snake_length] = snake_body[snake_length - 1];
                    snake_body[snake_length].x = -GRID_SIZE;
                    snake_body[snake_length].y = -GRID_SIZE;
                    snake_length++;
                }
                SpawnFood(&food,snake_body,snake_length);
            }
            if (CheckCollisionWithWall(&snake_body[0]) || CheckSelfCollision(snake_body, snake_length))
            {

                game_over = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);

        SDL_RenderClear(renderer);

        if (!game_over)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &food);

            DrawSnake(renderer, &snake_body[0], snake_length);
            ScoreCount(renderer, font);
        }

        else
        {
            DrawGameOverAndLastScore(renderer, font,score);
        }

        // SDL_RenderFillRect(renderer, &snake_head);

        SDL_RenderPresent(renderer);
        SDL_Delay(200);
    }

    if (font != NULL)
    {
        TTF_CloseFont(font);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}