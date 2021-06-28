#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <SDL2/SDL.h> 
#include "lib/apsisnw.h"

SDL_Window *pWindow = NULL;
SDL_Renderer *pRenderer = NULL;
SDL_Texture *pTexture = NULL;
SDL_Surface *pSurface = NULL;

#define WIDTH 640
#define HEIGHT 480

/* GNU Style guide will be followed. Please see C Violence by Sigrid here.
 * https://ftrv.se/3
 */




/* Drawing */

void 
draw_circle(SDL_Renderer *pRenderer, int32_t centreX, int32_t centreY, int32_t radius)
{
    int32_t x = (radius - 1);
    int32_t y = 0;

    while (x >= y)
    {
        SDL_RenderDrawPoint(pRenderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(pRenderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(pRenderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(pRenderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(pRenderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(pRenderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(pRenderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(pRenderer, centreX - y, centreY + x);

        int32_t diameter = (radius * 2);
        int32_t tix = 1;
        int32_t tiy = 1;
        int32_t error = (tix - diameter);

        if (error <= 0)
        {
            ++y; 
            error += tiy;
            tiy += 2;
        }
        if (error > 0)
        {
            --x;
            tix += 2;
            error += (tix - diameter);
        }
    }
}

/* Setup */



void 
apsis_quit(void)
{
    SDL_DestroyWindow(pWindow);
    pWindow = NULL;
    SDL_DestroyRenderer(pRenderer);
    pRenderer = NULL;
    SDL_FreeSurface(pSurface);
    pSurface = NULL;
    SDL_Quit();
}

void 
on_render(void)
{

    SDL_RenderClear(pRenderer);
    SDL_SetRenderDrawColor(pRenderer, 255, 87, 51, 255);
    draw_circle(pRenderer, 260, 260, 160);
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
    SDL_RenderPresent(pRenderer);

}

int 
init(void)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Init: %s\n", SDL_GetError());
    }
    
    pWindow = SDL_CreateWindow("Apsis", 
                                SDL_WINDOWPOS_UNDEFINED, 
                                SDL_WINDOWPOS_UNDEFINED, 
                                WIDTH, 
                                HEIGHT, 
                                SDL_WINDOW_SHOWN);
    
    if (pWindow == NULL )
    {
        printf("SDL_CreateWindow: %s\n", SDL_GetError());
    }

    pRenderer = SDL_CreateRenderer(pWindow, -1, 0);

    if (pRenderer == NULL)
    {
        printf("SDL_CreateRenderer:%s\n", SDL_GetError());
    }

    pTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
    
    if (pTexture == NULL)
    {
        printf("SDL_CreateTexture:%s\n", SDL_GetError());
    }

    on_render();

    return 1; 
}


int
main(void)
{
    if (!init())
    {
        printf("Init failed.");
    }

    int quit = 0;

    SDL_Event event_exit;
    
    while (!quit)
    {
        while(SDL_PollEvent( &event_exit ) != 0 )
        {
            if (event_exit.type == SDL_QUIT )
            {
                quit = 1;
                apsis_quit();
            }
        }

    }
    
    apsis_quit();
    return 0;
}
