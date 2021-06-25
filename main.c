#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h> 
#include "lib/apsisnw.h"

/*
 * Parent metronome SDL stuff.
 */

SDL_Window *pWindow = NULL;
SDL_Renderer *pRenderer = NULL;
SDL_Texture *pTexture = NULL;


/* 
 * Parent metronome destroy.
 */

void quit(void)
{
    SDL_DestroyRenderer(pRenderer);
    pRenderer = NULL;
    SDL_Quit();
    exit(0);
}

/* 
 * Parent metronome window creation 
 */

int init(void)
{
    if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
        return printf("SDL_Init has failed: %s\n", SDL_GetError());
   pWindow = SDL_CreateWindow("Apsis",
           SDL_WINDOWPOS_UNDEFINED, 
           SDL_WINDOWPOS_UNDEFINED,
           640, /* Width */
           640, /* Heigth  */
           SDL_WINDOW_SHOWN);

   if(pWindow == NULL)
       return printf("SDL_Window has failed %s\n", SDL_GetError());
   
    pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
   
   if (pRenderer == NULL)
       return printf("SDL_Renderer has failed. %s\n", SDL_GetError());
    
   SDL_Delay(3000);
   SDL_DestroyWindow(pWindow);
    return 1;

}

int main(void)
{
    init();
    return 0;
}
