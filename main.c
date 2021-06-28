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
    SDL_DestroyRenderer(pRenderer);
    SDL_Quit();
}

int
init(void)
{
    is_running = 1;

    pWindow = SDL_CreateWindow("Apsis", SDL_WINDOWPOS_UNDEFINED, 
                                        SDL_WINDOWPOS_UNDEFINED,
                                        WIDTH,
                                        HEIGHT,
                                        SDL_WINDOW_SHOWN);
    if (pWindow == NULL)
    {
        printf("createWindow: %s\n", SDL_GetError());
        is_running = 1;
    }

    unsigned int sTick = SDL_GetTicks();
    pRenderer = SDL_CreateRenderer ( pWindow, -1, 0);
    while (is_running == 1)
    {
        SDL_RenderSetLogicalSize(pRenderer, WIDTH, HEIGHT);
        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pRenderer);
        SDL_RenderPresent(pRenderer);
    }

    if (is_running == 0)
    {
        apsis_quit();
    }

    return 0;
}


int 
main(int argc, char* argv[])
{
    int opt;
    while((opt = getopt(argc, argv, "isq")) != -1)
    {
        switch(opt)
        {
            case 'i':
                init();
                break;
            case 's':
                printf("Server option is WIP.");
                exit(1);
                break;
            case 'q':
                is_running = 0;
                apsis_quit();
                break;
        }
    }
    return 0;
}
