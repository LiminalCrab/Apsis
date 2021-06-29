#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h> 
#include "lib/apsisnw.h"

SDL_Window *pWindow = NULL;
SDL_Renderer *pRenderer = NULL;
SDL_Texture *pTexture = NULL;


#define WIDTH 640
#define HEIGHT 480
#define PI 3.14159265

int quit = 0;
int xCenter = WIDTH / 2; 
int yCenter = HEIGHT / 2;

/* GNU Style guide will be followed. Please see C Violence by Sigrid here.
 * https://ftrv.se/3
 */

/* Drawing */

void 
draw_RotPhasor(SDL_Renderer *pRenderer, 
              double oX, /* x coords start point */
              double oY, /* y coords start point */
              double pX, /* x coords end point */
              double pY) /* y coords end point */
  {  

    /* convert to degrees */

    double radians = 0.0;
    double angle = radians * PI / 180;
    printf("Degrees: %f\n", angle);
    
    /* rotate around origin point */
    double dX = (cos(angle) * (pX - oX)) - (sin(angle) * (pY - oY)) + oX;
    printf("deltaX: %f\n", dX);    
    double dY = (sin(angle) * (pX - oX)) + (cos(angle) * (pY - oY)) + oY;
    printf("deltaY: %f\n", dY);
    pX += dX;
    pY += dY;
    printf("translate pX: %f\n", pX);
    printf("translate pY: %f\n", pY);

    SDL_RenderDrawLine(pRenderer, oX, oY, pX, pY); 

  }


void 
draw_UnfilledCircle(SDL_Renderer *pRenderer, 
                                  int centerx,
                                  int centery, 
                                  int radius)
{
  // Draws an empty circle with the given position and radius

  int diameter = (radius * 2);

  int x = radius - 1;
  int y = 0;
  int tx = 1;
  int ty = 1;
  int error = tx - diameter;

  while (x >= y)
  {
    // Each renders an octant of the circle
    SDL_RenderDrawPoint(pRenderer, centerx + x, centery + y);
    SDL_RenderDrawPoint(pRenderer, centerx + x, centery - y);
    SDL_RenderDrawPoint(pRenderer, centerx - x, centery + y);
    SDL_RenderDrawPoint(pRenderer, centerx - x, centery - y);
    SDL_RenderDrawPoint(pRenderer, centerx + y, centery - x);
    SDL_RenderDrawPoint(pRenderer, centerx + y, centery + x);
    SDL_RenderDrawPoint(pRenderer, centerx - y, centery - x);
    SDL_RenderDrawPoint(pRenderer, centerx - y, centery + x);

    if (error <= 0)
    {
      ++y;
      error += ty;
      ty += 2;
    }

    if (error > 0)
    {
      --x;
      tx += 2;
      error += (tx - diameter);
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
    SDL_Quit();
}

void 
testRender(void)
{
    int x = 320;
    int y = 100;

    SDL_RenderClear(pRenderer);
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
    draw_UnfilledCircle(pRenderer, xCenter, yCenter, 160 );
    while(x < 400 )
    {
        x++;
        y++;
        printf("Phasor x actual: %d\n", x);
        printf("Phasor y actual: %d\n", y);
        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
        draw_RotPhasor(pRenderer, xCenter, yCenter, x, y );
    }
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

    pTexture = SDL_CreateTexture(pRenderer, 
                                SDL_PIXELFORMAT_ARGB8888, 
                                SDL_TEXTUREACCESS_STATIC, 
                                WIDTH, 
                                HEIGHT);
    
    if (pTexture == NULL)
    {
        printf("SDL_CreateTexture:%s\n", SDL_GetError());
    }

    testRender();

    return 1; 
}


int
main(void)
{
    if (!init())
    {
        printf("Init failed.");
    }


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
