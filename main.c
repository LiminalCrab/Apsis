#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <SDL2/SDL.h> 
#include "lib/apsisnw.h"

SDL_Window *pWindow = NULL;
SDL_Renderer *pRenderer = NULL;
SDL_Surface *pSurface = NULL;

#define WIDTH 640
#define HEIGHT 480
int is_running = 0;

/* GNU Style guide will be followed. Please see C Violence by Sigrid here.
 * https://ftrv.se/3
 */

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

int 
init(void)
{
    int success = 1;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Init: %s\n", SDL_GetError());
        success = 0;
    }
    else 
    {
        pWindow = SDL_CreateWindow("Apsis", 
                                    SDL_WINDOWPOS_UNDEFINED, 
                                    SDL_WINDOWPOS_UNDEFINED, 
                                    WIDTH, 
                                    HEIGHT, 
                                    SDL_WINDOW_SHOWN);
        if (pWindow == NULL )
        {
            printf("SDL_CreateWindow: %s\n", SDL_GetError());
            success = 0;
        }
        else 
            pSurface = SDL_GetWindowSurface( pWindow );

    }

    return success; 
}


int
main(void)
{
    if (!init())
    {
        printf("Main failed to initalize");
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
            }
        }

    }
    
    apsis_quit();
    return 0;
}
