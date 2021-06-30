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

int xCenter = WIDTH / 2; 
int yCenter = HEIGHT / 2;
int FPS = 30;

/* Drawing */

void draw_RotPhasor(SDL_Renderer *pRenderer, double originX, 
                    double originY, double pointX, double pointY) 
{  

    /* convert to degrees */

    double radians = 0.0;
    double angle = radians * PI / 180;
    printf("Degrees: %f\n", angle);
    
    /* rotate around origin point */
    double deltaX = (cos(angle) * (pointX - originX)) - (sin(angle) * (pointY - originY)) + originX;
    printf("deltaX: %f\n", deltaX);    
    double deltaY = (sin(angle) * (pointX - originX)) + (cos(angle) * (pointY - originY)) + originY;
    printf("deltaY: %f\n", deltaY);

    pointX += deltaX;
    pointY += deltaY;

    printf("translate pX: %f\n", pointX);
    printf("translate pY: %f\n", pointY);

    SDL_RenderDrawLine(pRenderer, originX, originY, pointX, pointY); 

 }


void draw_UnfilledCircle(SDL_Renderer *pRenderer, int centerx, int centery, int radius)
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

/* Render  */

int render(void)
{

  int x = 320; //temp
  int y = 100; //temp
  
  while ( x > 500 )
  {
    x++;
    y++;

    printf("Phasor X actual: %d\n", x);
    printf("Phasor Y actual: %d\n", y);

    SDL_RenderClear(pRenderer);
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255); //circle
    draw_UnfilledCircle(pRenderer, xCenter, yCenter, 160);
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255); //phasor
    draw_RotPhasor(pRenderer, xCenter, yCenter, x, y);
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255); //background
    SDL_RenderPresent(pRenderer);
  }

  return 1;
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

    render();

    return 1; 
}


int main(void)
{
  int quit = 0;

    
    if (!init())
    {
        printf("Init failed.");
    }
    
    int nTick = 0;

    while (!quit)
    {
      int tick = SDL_GetTicks();
     
      if(tick < nTick)
      {
        SDL_Delay(nTick - tick);
      }

      nTick = tick + (1000 / FPS);
      SDL_Event event;
      printf("tick: %d\n", tick);
      printf("next tick: %d\n", nTick);

      while(SDL_PollEvent( &event ) != 0 )
      {
        switch(event.type)
        {
          case SDL_QUIT:
            quit = 1;
            apsis_quit();
            break;
          case SDL_MOUSEBUTTONDOWN:
            render();
        }
      }
    }
    
    apsis_quit();
    return 0;
}
