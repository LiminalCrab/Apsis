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

/* GNU Style guide will be followed. Please see C Violence by Sigrid here.
 * https://ftrv.se/3
 */


/* Drawing */

void 
draw_circle(SDL_Renderer *pRenderer, int32_t centerX, int32_t centerY, int32_t radius)
{
    int32_t x = (radius - 1);
    int32_t y = 0;

    while (x >= y)
    {
        SDL_RenderDrawPoint(pRenderer, centerX + x, centerY - y);
        SDL_RenderDrawPoint(pRenderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(pRenderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(pRenderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(pRenderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(pRenderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(pRenderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(pRenderer, centerX - y, centerY + x);
        
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

int 
init(void)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Init: %s\n", SDL_GetError());
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
        }
        else
        {  
            pSurface = SDL_GetWindowSurface( pWindow );
        }

    }

    return 0; 
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
