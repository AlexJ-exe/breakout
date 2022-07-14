#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>


typedef struct Paddle
{
    SDL_Rect forma;
    double vel_x;
}Paddle;

typedef struct Brick
{
    SDL_Rect forma;
    int hardness;
}Brick;

typedef struct Ball
{
    SDL_Rect forma;
    int vel_x;
    int vel_y;
    int strenght;
}Ball;


const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;
const int VELOCITY = 20;
    
SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture*  texture = NULL;


void startSDL(){
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024);
    window = SDL_CreateWindow("Atari breakout",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
}


void end(){
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Texture* loadMedia(const char *file, SDL_Renderer *renderer){
    surface = IMG_Load(file);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}


void move();
void explote();
void bounce();
void life();
void points();
void lose();


void darFormaPaleta(Paddle *paddle)
{
    paddle->forma.x=340;
    paddle->forma.y=490;
    paddle->forma.w=40;
    paddle->forma.h=10;
}

void darformaLadrillo(Brick *brick)
{
    brick->forma.x=220;
    brick->forma.y=100;
    brick->forma.w=35;
    brick->forma.h=18;
}

Brick *crearLadrillos(int cantidadLadrillos)
{
    int aux_x = 220;
    Brick *arrayLadrillos;
    arrayLadrillos = malloc(cantidadLadrillos * sizeof(Brick));
    for(int i=0; i<cantidadLadrillos; i++)
    {
        arrayLadrillos[i].forma.x = aux_x;
        arrayLadrillos[i].forma.y = 100;
        arrayLadrillos[i].forma.w = 35;
        arrayLadrillos[i].forma.h = 18;
        aux_x += 40;
    }
    return arrayLadrillos;
}

int main()
{
    startSDL();
    bool closeGame = false;
    bool ballUp = false;
    SDL_Event event;

    SDL_Texture *background = loadMedia("./pxArt.png",renderer);
    SDL_Texture *paddle_texture = loadMedia("./green.png",renderer);
    SDL_Texture *ball_texture = loadMedia("./pokeball.png",renderer);
    SDL_Texture *brick_texture[8];
    for(int i=0; i<8; i++)
    {
        brick_texture[i] = loadMedia("./green.png",renderer);
    }
    //SDL_Texture *brick_texture_1 = loadMedia("./images/green.png",renderer);
    //SDL_Texture *brick_texture_2 = loadMedia("./images/green.png",renderer);

    Paddle paleta;
    darFormaPaleta(&paleta);

    Ball pelota;
    pelota.forma.x = 350;
    pelota.forma.y = 475;
    pelota.forma.w = 15;
    pelota.forma.h = 15;
    pelota.vel_x = 3;
    pelota.vel_y = 3;

    Brick *arrayLadrillos;
    arrayLadrillos = crearLadrillos(8);
    /*Brick ladrillo_1;
    darformaLadrillo(&ladrillo_1);
    Brick ladrillo_2;
    ladrillo_2.forma.x = 260;
    ladrillo_2.forma.y = 100;
    ladrillo_2.forma.w = 35;
    ladrillo_2.forma.h = 18;*/

    Mix_Music *backgroundSong = Mix_LoadMUS("./theme.ogg");
    Mix_Chunk *paddleSong = Mix_LoadWAV("paddle.wav");
    Mix_Chunk *bricksSong = Mix_LoadWAV("bricks.wav");
    Mix_PlayMusic(backgroundSong,-1);

        while (!closeGame)
        {
            while (SDL_PollEvent(&event) != 0)
            {
                if(event.type == SDL_QUIT){
                    closeGame = true;
                }

                if(event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_RIGHT:
                        Mix_PlayChannel(-1,paddleSong,0);
                        if (paleta.forma.x<490)
                        {
                            if(!ballUp)
                            {
                                paleta.forma.x +=VELOCITY;
                                pelota.forma.x +=VELOCITY;
                            }else
                            {
                                paleta.forma.x += VELOCITY;
                            }
                        }
                        break;
                    case SDLK_LEFT:
                        Mix_PlayChannel(-1,paddleSong,0);
                        if(paleta.forma.x>200)
                        {
                            if(!ballUp)
                            {
                                paleta.forma.x -=VELOCITY;
                                pelota.forma.x -=VELOCITY;
                            }else
                            {
                                paleta.forma.x -= VELOCITY;
                            }
                        }
                        break;
                    case SDLK_RETURN:
                        ballUp = true;
                        pelota.forma.x += VELOCITY;
                        pelota.forma.y -= VELOCITY;
                        break;
                    default:
                        break;
                    }//switch teclas
                }//if eventos teclas
            }//while eventos

            if(pelota.forma.x>=500+pelota.forma.w)
            {
                pelota.forma.x = 500+pelota.forma.w;
                pelota.vel_x = -pelota.vel_x;
            }
            if(pelota.forma.x<=202-pelota.forma.w)
            {
                pelota.forma.x = 202-pelota.forma.w;
                pelota.vel_x = -pelota.vel_x;
            }
            if(pelota.forma.y>=510-pelota.forma.h)
            {
                pelota.forma.y = 510-pelota.forma.h;
                pelota.vel_y = -pelota.vel_y;
            }
            if(pelota.forma.y<=40+pelota.forma.h)
            {
                pelota.forma.y = 40+pelota.forma.h;
                pelota.vel_y = -pelota.vel_y;
            }

            if(SDL_HasIntersection(&pelota.forma,&paleta.forma))
            {
                pelota.forma.y = paleta.forma.y-pelota.forma.h;
                pelota.vel_y = -pelota.vel_y;
            }
            for(int i=0; i<8; i++)
            {
                if(SDL_HasIntersection(&pelota.forma,&arrayLadrillos[i].forma))
                {
                    Mix_PlayChannel(-1,bricksSong,0);
                    arrayLadrillos[i].forma.h = 0;
                    arrayLadrillos[i].forma.w = 0;
                    pelota.forma.y = arrayLadrillos[i].forma.y+pelota.forma.h;
                    pelota.vel_y = -pelota.vel_y;
                }
            }

            if(ballUp)
            {
                pelota.forma.x += pelota.vel_x;
                pelota.forma.y -= pelota.vel_y;

            }

            SDL_RenderCopy(renderer,background,NULL,NULL);
            SDL_RenderCopy(renderer,paddle_texture,NULL,&paleta.forma);
            for(int j=0; j<8; j++)
            {
                SDL_RenderCopy(renderer,brick_texture[j],NULL,&arrayLadrillos[j].forma);   
            }
            //SDL_RenderCopy(renderer,brick_texture_1,NULL,&ladrillo_1.forma);
            //SDL_RenderCopy(renderer,brick_texture_2,NULL,&ladrillo_2.forma);
            SDL_RenderCopy(renderer,ball_texture,NULL,&pelota.forma);
            SDL_Delay(1000/60);
            SDL_RenderPresent(renderer);
            
        }//while principal

    end();
    return 0;
}//main

