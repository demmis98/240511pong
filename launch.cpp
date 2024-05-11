#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include <stdio.h>

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 360

#define SHIP_MAX_SPEED 4


using namespace std;

void init(SDL_GameController **controller, SDL_Rect *ball, int *ball_speed_x, int *ball_speed_y, int *score_1, int *score_2);

void update(bool *running, SDL_GameController **controller, SDL_Rect *ball, int *ball_speed_x, int *ball_speed_y, int *score_1, int *score_2);
void update_frame(SDL_GameController **controller, SDL_Rect *p_1, SDL_Rect *p_2, SDL_Rect *ball, int *ball_speed_x, int *ball_speed_y, int *score_1, int *score_2);
void render(SDL_Renderer *renderer, SDL_Rect p_1, SDL_Rect p_2, SDL_Rect ball);

int main(){
    bool running;

    //open window
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("pong", 64, 64, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    uint32_t time_old = SDL_GetTicks(), time_new;

    SDL_GameController *controller = NULL;

    SDL_Rect p_1, p_2;
    p_1.h = 50;
    p_1.w = 10;
    p_1.x = 20;
    p_1.y = (SCREEN_HEIGHT + p_1.h) / 2;
    
    p_2.h = 50;
    p_2.w = 10;
    p_2.x = SCREEN_WIDTH - (p_2.w + 20);
    p_2.y = p_1.y;

    SDL_Rect ball;
    ball.w = 10;
    ball.h = 10;
    int ball_speed_x, ball_speed_y;

    int score_1, score_2;

    init(&controller, &ball, &ball_speed_x, &ball_speed_y, &score_1, &score_2);
    
    running = true;
    while(running){
        time_new = SDL_GetTicks();
        update(&running, &controller, &ball, &ball_speed_x, &ball_speed_y, &score_1, &score_2);
        if(time_new - time_old > 1000/60){
            time_old = SDL_GetTicks();
            update_frame(&controller, &p_1, &p_2, &ball, &ball_speed_x, &ball_speed_y, &score_1, &score_2);
        }
        render(renderer, p_1, p_2, ball);
    }

    return 0;
}


void init(SDL_GameController **controller, SDL_Rect *ball, int *ball_speed_x, int *ball_speed_y, int *score_1, int *score_2){
    if(SDL_NumJoysticks() != 0){
        int c = 0;
        while(!SDL_IsGameController(c) && c < SDL_NumJoysticks()){
            c++;
        }
        *controller = SDL_GameControllerOpen(c);
    }
    else{
        *controller = NULL;
    }

    ball->x = (SCREEN_WIDTH + ball->w) / 2;
    ball->y = (SCREEN_HEIGHT + ball->h) / 2;
    *ball_speed_x = 3;
    *ball_speed_y = 0;

    *score_1 = 0;
    *score_2 = 0;
}


void update(bool *running, SDL_GameController **controller, SDL_Rect *ball, int *ball_speed_x, int *ball_speed_y, int *score_1, int *score_2){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                *running = false;
                break;
            case SDLK_r:
                init(controller, ball, ball_speed_x, ball_speed_y, score_1, score_2);
            }
            break;
        case SDL_QUIT:
            *running = false;
            break;
        }
    }
}

void update_frame(SDL_GameController **controller, SDL_Rect *p_1, SDL_Rect *p_2, SDL_Rect *ball, int *ball_speed_x, int *ball_speed_y, int *score_1, int *score_2){
    ball->x += *ball_speed_x;
    ball->y += *ball_speed_y;

    bool collition;

    if(*ball_speed_x < 0){
        collition = SDL_HasIntersection(p_1, ball);
        if(collition){
            *ball_speed_x -= 1;
            *ball_speed_x *= -1;
            *ball_speed_y = ball->y - (p_1->h / 2) - p_1->y;
            *ball_speed_y /= 4;
        }
    }
    else{
        collition = SDL_HasIntersection(p_2, ball);
        if(collition){
            *ball_speed_x += 1;
            *ball_speed_x *= -1;
            *ball_speed_y = ball->y - (p_2->h / 2) - p_2->y;
            *ball_speed_y /= 4;
        }
    }

    if(ball->x > SCREEN_WIDTH || ball->x < -ball->w){
        if(ball->x < 0){
            *ball_speed_x = 3;
            *score_2 += 1;
        }
        else{
            *ball_speed_x = -3;
            *score_1 += 1;
        }

        ball->x = (SCREEN_WIDTH + ball->w) / 2;
        ball->y = (SCREEN_HEIGHT + ball->h) / 2;
        *ball_speed_y = 0;

        printf("\n%d %d", *score_1, *score_2);
    }
    
    if(ball->y > SCREEN_HEIGHT){
        *ball_speed_y *= -1;
    }
    else if(ball->y < -ball->h){
        *ball_speed_y *= -1;
    }

    if(*controller == NULL){
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_W]){
            p_1->y -= 5;
        }
        else if(state[SDL_SCANCODE_S]){
            p_1->y += 5;
        }

        if(p_1->y < 0){
            p_1->y = 0;
        }
        else if(p_1->y > SCREEN_HEIGHT - p_1->h){
            p_1->y = SCREEN_HEIGHT - p_1->h;
        }
        
        if(state[SDL_SCANCODE_UP]){
            p_2->y -= 5;
        }
        else if(state[SDL_SCANCODE_DOWN]){
            p_2->y += 5;
        }
        
        if(p_2->y < 0){
            p_2->y = 0;
        }
        else if(p_2->y > SCREEN_HEIGHT - p_2->h){
            p_2->y = SCREEN_HEIGHT - p_2->h;
        }
    }
    else{
        p_1->y = (SDL_GameControllerGetAxis(*controller, SDL_CONTROLLER_AXIS_LEFTY) / 200) + (p_1->h / 2) + 130;
        p_2->y = (SDL_GameControllerGetAxis(*controller, SDL_CONTROLLER_AXIS_RIGHTY) / 200) + (p_2->h / 2) + 130;
    }
}

void render(SDL_Renderer *renderer, SDL_Rect p_1, SDL_Rect p_2, SDL_Rect ball){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer); //clear screen

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &p_1);
    SDL_RenderFillRect(renderer, &p_2);
    SDL_RenderFillRect(renderer, &ball);

    SDL_RenderPresent(renderer);
}