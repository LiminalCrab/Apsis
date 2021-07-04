#include "lib/apsislib.h" /* Apsis lib */
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

SDL_Window *gWin = NULL;
SDL_Renderer *gRen = NULL;
SDL_Texture *gTxr = NULL;

#define HR 32
#define VR 16
#define PD 2
#define SZ (HR * VR * 16)
#define MAXSZ (HR * VR)
#define PI 3.14159265

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;

int WIDTH = 32 * HR + PD * 8 * 2;
int HEIGHT = 32 * (VR + 2) + PD * 8 * 2;

Uint32 *px; /* Pixels */

Uint32 theme[] = {
	0x000000,
	0xFFFFFF,
	0xFFFFFF,
	0x000000,
	0xFFFFFF};

Uint8 glyph[][8] = 
{
    {0x00, 0x00, 0x3c, 0x02, 0x3e, 0x42, 0x3e, 0x00}, /* a */
};

/* Routines  */

void quit(void)
{
    printf("Quiting.\n");
    free(px);
    SDL_DestroyTexture(gTxr);
    gTxr = NULL;
    SDL_DestroyRenderer(gRen);
    gRen = NULL;
    SDL_DestroyWindow(gWin);
    gWin = NULL;
    SDL_Quit();
    exit(0);
}

double get_time(void)
{
  struct timespec tp;
  return clock_gettime(CLOCK_MONOTONIC, &tp) == 0 ? (double)tp.tv_sec + (double)tp.tv_nsec/1000000000.0 : 0.0;

}

void setpx(Uint32 *dest, int x, int y, int clr)
{
  if (x >= 0 && x < WIDTH - 8 && y >= 0 && y < HEIGHT - 8)
    dest[(y + PD * 8) * WIDTH + (x + PD * 8)] = theme[clr];
}

void draw_sprite(Uint32 *dest, int x, int y, Uint8 *glyph, int fg, int bg)
{
  int v, h;
  for(v = 0; v < 8; v++)
    for(h = 0; h < 8; h++)
    {
      int clr = (glyph[v] >> (7 - h)) & 0x1;
      setpx(dest, x + h, y + v, clr == 1 ? fg : bg);
    }

}

/* User Interface */

void draw_phasor_line(SDL_Renderer *gRen, double ox,
                    double oy, double radius, double radians)
{

    /* convert to degrees */
    double angle = radians * 180 / PI;
    printf("Degrees: %f\n", angle);

    /* rotate around origin point */
    double dx = ox + cos(angle)*radius;
    double dy = oy + sin(angle)*radius;

    printf("angle:  %f\n", angle);
    SDL_RenderDrawLine(gRen, ox, oy, dx, dy);

 }

void draw_metronome_ring(SDL_Renderer *gRen, int cx, int cy, int radius)
{
  int diam = (radius * 2);

  int x = radius - 1;
  int y = 0;
  int tx = 1;
  int ty = 1;
  int error = tx - diam;

  while (x >= y)
  {
    SDL_RenderDrawPoint(gRen, cx + x, cy + y);
    SDL_RenderDrawPoint(gRen, cx + x, cy - y);
    SDL_RenderDrawPoint(gRen, cx - x, cy + y);
    SDL_RenderDrawPoint(gRen, cx - x, cy - y);
    SDL_RenderDrawPoint(gRen, cx + y, cy - x);
    SDL_RenderDrawPoint(gRen, cx + y, cy + x);
    SDL_RenderDrawPoint(gRen, cx - y, cy - x);
    SDL_RenderDrawPoint(gRen, cx - y, cy + x);

    if (error <= 0)
    {
      ++y;
      error += ty;
      ty += 2;
    }

    if( error > 0)
    {
      --x;
      tx += 2;
      error += (tx - diam);

    }

  }
}

int render_ui(void)
{
  int i;
  int n = 0;
  int BOTTOM = VR * 8 + 8;
  int X_CENTER = WIDTH / 2;
  int Y_CENTER = HEIGHT / 2;

  double speed = 0.1;
  double angle = speed * get_time();

  SDL_RenderClear(gRen);
  SDL_SetRenderDrawColor(gRen, 0xFF, 0xFF, 0xFF, 255);
  draw_metronome_ring(gRen, X_CENTER, Y_CENTER, 160);
  SDL_SetRenderDrawColor(gRen, 0xFF, 0xFF, 0xFF, 255); /* Phasor  */
  draw_phasor_line(gRen, X_CENTER, Y_CENTER, 160, angle);
  SDL_RenderPresent(gRen);

  return 1;
}

/* SDL Initialization and events */

int init(void)
{
  int i, j;
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    return printf("Init has failed. %s\n", SDL_GetError());
  gWin = SDL_CreateWindow("Apsis",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_SHOWN);

  if(gWin == NULL)
    return printf("Window error: %s\n", SDL_GetError());

  gRen = SDL_CreateRenderer(gWin, -1, 0);
  
  if(gRen == NULL)
    return printf("Renderer error: %s\n", SDL_GetError());

  gTxr = SDL_CreateTexture(gRen,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STATIC,
        WIDTH,
        HEIGHT);

  if(gTxr == NULL)
    return printf("Texture error: %s\n", SDL_GetError());
  
  px = (Uint32 *)malloc(WIDTH * HEIGHT * sizeof(Uint32));
  
  if(px == NULL)
    return printf("Pixels failed to allocate memory.");

  for (i = 0; i < HEIGHT; i++)
    for(j = 0; j < WIDTH; j++)
      px[i * WIDTH + j] = theme[0];
  
  render_ui();
  return 1;
}

int main(void)
{
  if(!init())
    return printf("Main(): Init has failed: %s\n", SDL_GetError());
  
  while(1)
  {
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0)
    {
      switch(e.type){
      case SDL_QUIT: quit(); break;
      }
    }
  }

}
