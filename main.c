#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h> 
#include <time.h>
#include "lib/apsisnw.h"

SDL_Window    *pWindow = NULL;
SDL_Renderer  *pRenderer = NULL;
SDL_Texture   *pTexture = NULL;
SDL_Texture   *btuiTexture = NULL;

#define WIDTH 640
#define HEIGHT 480
#define PI 3.14159265

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;

int xCenter = WIDTH / 2; 
int yCenter = HEIGHT / 2;
int quit = 0;

typedef struct
{
    Uint8 r, g, b, a;
} Color;

Color create_Clr(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  Color clr;
  clr.r = r;
  clr.g = g;
  clr.b = b;
  clr.a = a;
  return clr;
}

int apsis_Quit(void)
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

/* Routines  */
void putpixel(Uint16 x, Uint16 y, Uint8 color)
{
}

/* Time */
double get_Time(void)
{
  struct timespec tp;
  return clock_gettime(CLOCK_MONOTONIC, &tp) == 0 ? (double)tp.tv_sec + (double)tp.tv_nsec/1000000000.0 : 0.0; 
}

/* 8x8 bitmap of pixel values*/

unsigned char glyph[][8] = 
{
    {0x00, 0x00, 0x3c, 0x02, 0x3e, 0x42, 0x3e, 0x00}, /* a */
};

void draw_Glyph(SDL_Renderer *pRenderer, unsigned char *glyph, Color on_color, Color off_color)
{
  for (int y = 0; y < 8; y++)
    for(int x = 0; x < 8; x++)
  {
    if (glyph[y] & (1 << ( 7 - x)))
      SDL_SetRenderDrawColor(pRenderer, on_color.r, on_color.g, on_color.b, on_color.a);
    else
      SDL_SetRenderDrawColor(pRenderer, off_color.r, off_color.g, off_color.b, off_color.a);

    SDL_RenderDrawPoint(pRenderer, x, y);
  }
}

void draw_Phasor(SDL_Renderer *pRenderer, double originX, 
                    double originY, double radius, double radians)  
{  

    /* convert to degrees */
    double angle = radians * 180 / PI;
    printf("Degrees: %f\n", angle);
    
    /* rotate around origin point */
    double deltaX = originX + cos(angle)*radius;
    double deltaY = originY + sin(angle)*radius;
   
    printf("angle:  %f\n", angle);
    SDL_RenderDrawLine(pRenderer, originX, originY, deltaX, deltaY); 

 }


void draw_UnfilledCircle(SDL_Renderer *pRenderer, int centerx, int centery, int radius)
{

  int diameter = (radius * 2);

  int x = radius - 1;
  int y = 0;
  int tx = 1;
  int ty = 1;
  int error = tx - diameter;

  while (x >= y)
  {
   
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

int render_UI(void)
{

  while (!quit)
  {
    /* Metronome */
    double speed = 0.1;
    double angle = speed * get_Time();
    
    SDL_RenderClear(pRenderer);
   
    SDL_SetRenderDrawColor(pRenderer, 0xFF, 0xFF, 0xFF, 255); /* Circle  */
    draw_UnfilledCircle(pRenderer, xCenter, yCenter, 160);
    
    SDL_SetRenderDrawColor(pRenderer, 0xFF, 0xFF, 0xFF, 255); /* Phasor  */ 
    draw_Phasor(pRenderer, xCenter, yCenter, 160, angle);
    
    SDL_SetRenderDrawColor(pRenderer, 0x00, 0x00, 0x00, 255); /* Background */
    
    /*UI Text */
    Color on_clr = create_Clr(0xFF, 0xFF, 0xFF, 255); /* WHTIE */
    Color off_clr = create_Clr(0x00, 0x00, 0x00, 255); /* BLACK */
    draw_Glyph(pRenderer, *glyph, on_clr, off_clr);

    SDL_RenderPresent(pRenderer);
  
  }
  quit = 1;
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

    render_UI();

    return 1; 
}


int main(void)
{
   
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
          apsis_Quit();
          printf("Apsis quit.\n");
        }
      }
    }
    
    printf("Apsis quit final.\n");
    return 0;
}
