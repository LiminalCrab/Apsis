#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h> 
#include <time.h>
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

/* Time */
double get_time(void)
{
  struct timespec tp;
  return clock_gettime(CLOCK_MONOTONIC, &tp) == 0 ? (double)tp.tv_sec + (double)tp.tv_nsec/1000000000.0 : 0.0; 
}


int apsis_quit(void)
{
    SDL_DestroyWindow(pWindow);
    pWindow = NULL;
    printf("Window Destroyed.\n");
    SDL_DestroyRenderer(pRenderer);
    pRenderer = NULL;
    printf("Renderer destroyed.\n");
    SDL_DestroyTexture(pTexture);
    pTexture = NULL;
    printf("Texture destroyed.\n");
    SDL_Quit();

    return 0;
}

void draw_phasor(SDL_Renderer *pRenderer, double originX, 
                    double originY, double radius, double radians)  
{  

    /* convert to degrees */
    double angle = radians * 180 / PI;
    printf("Degrees: %f\n", angle);
    
    /* rotate around origin point */
    double deltaX = originX + cos(angle)*radius;
    printf("deltaX: %f\n", deltaX);    
    double deltaY = originY + sin(angle)*radius;
    printf("deltaY: %f\n", deltaY);
   
    printf("angle:  %f\n", angle);
    SDL_RenderDrawLine(pRenderer, originX, originY, deltaX, deltaY); 

 }


void draw_unfilledcircle(SDL_Renderer *pRenderer, int centerx, int centery, int radius)
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
  int count = 0;

  while (count < 500)
  {
    double speed = 0.1;
    double angle = speed * get_time();
    
    SDL_RenderClear(pRenderer);
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255); //circle
    draw_unfilledcircle(pRenderer, xCenter, yCenter, 160);
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255); //phasor
    draw_phasor(pRenderer, xCenter, yCenter, 160, angle);
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255); //background
    SDL_RenderPresent(pRenderer);
  
  }

  return 1;
}



/* Setup */

 
int init(void)
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
    
    while (!quit)
    {
      
      SDL_Event event;

      while(SDL_PollEvent( &event ) != 0 )
      {
        if (event.type == SDL_QUIT)
        {
          quit = 1;
          apsis_quit();
          printf("Apsis quit.\n");
        }
      }
    }
    
    printf("Apsis quit final.\n");
    return 0;
}
