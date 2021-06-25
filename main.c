#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h> 
#include "lib/apsisnw.h"

SDL_Window *pWindow = NULL;
SDL_Renderer *pRenderer = NULL;
SDL_Surface *pSurface = NULL;

#define WIDTH 640
#define HEIGHT 480

int init(void)
{
    pWindow = SDL_CreateWindow("Apsis", SDL_WINDOWPOS_UNDEFINED, 
                                        SDL_WINDOWPOS_UNDEFINED,
                                        WIDTH,
                                        HEIGHT,
                                        SDL_WINDOW_SHOWN);
    if (pWindow == NULL)
    {
        printf("createWindow: %s\n", SDL_GetError());
        return 1;
    }

    unsigned int sTick = SDL_GetTicks();
    while (SDL_GetTicks() - sTick < 5000)
    {
        SDL_PumpEvents();
        SDL_RenderSetLogicalSize(pRenderer, WIDTH, HEIGHT);
        SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
        SDL_RenderClear(pRenderer);
    }


    SDL_DestroyWindow(pWindow);
    SDL_DestroyRenderer(pRenderer);
    SDL_Quit();

    return 0;
}


int main(int argc, char* args[])
{
    init();
    return 0;
}
