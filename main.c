#include <SDL2/SDL.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

SDL_Window    *gWin = NULL;
SDL_Renderer  *gRen = NULL;
SDL_Texture   *gTxr = NULL;

#define HR 32
#define VR 16
#define PD 2
#define PI 3.14159265

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;

int WIDTH = 32 * HR + PD * 8 * 2;
int HEIGHT = 32 * (VR + 2) + PD * 8 * 2;

Uint32 *pixels;

int ql = 0; /* Quit loop */

typedef struct Clr
{

 Uint8 r, g, b, a;

} Clr;

Clr set_clr(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  Clr clr;

  clr.r = r;
  clr.g = g;
  clr.b = b;
  clr.a = a;

  return clr;
}

Uint8 symbol[][8] = {   
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* NUL */ 
    { 0x3C, 0x62, 0x52, 0x4A, 0x46, 0x3C, 0x00, 0x00 }, /* 0 */
    { 0x44, 0x42, 0x7E, 0x40, 0x40, 0x00, 0x00, 0x00 }, /* 1 */
    { 0x64, 0x52, 0x52, 0x52, 0x52, 0x4C, 0x00, 0x00 }, /* 2 */
    { 0x24, 0x42, 0x42, 0x4A, 0x4A, 0x34, 0x00, 0x00 }, /* 3 */
    { 0x30, 0x28, 0x24, 0x7E, 0x20, 0x20, 0x00, 0x00 }, /* 4 */
    { 0x2E, 0x4A, 0x4A, 0x4A, 0x4A, 0x32, 0x00, 0x00 }, /* 5 */
    { 0x3C, 0x4A, 0x4A, 0x4A, 0x4A, 0x30, 0x00, 0x00 }, /* 6 */
    { 0x02, 0x02, 0x62, 0x12, 0x0A, 0x06, 0x00, 0x00 }, /* 7 */
    { 0x34, 0x4A, 0x4A, 0x4A, 0x4A, 0x34, 0x00, 0x00 }, /* 8 */
    { 0x0C, 0x52, 0x52, 0x52, 0x52, 0x3C, 0x00, 0x00 }, /* 9 */
    { 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x00 }, /* : */
    { 0x7C, 0x12, 0x12, 0x12, 0x12, 0x7C, 0x00, 0x00 }, /* A */
    { 0x00, 0x00, 0x3c, 0x02, 0x3e, 0x42, 0x3e, 0x00 }, /* B */
    { 0x3C, 0x42, 0x42, 0x42, 0x42, 0x24, 0x00, 0x00 }, /* C */
    { 0x7E, 0x4A, 0x4A, 0x4A, 0x4A, 0x42, 0x00, 0x00 }, /* E */
    { 0x7E, 0x08, 0x08, 0x14, 0x22, 0x40, 0x00, 0x00 }, /* K */
    { 0x7E, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00 }, /* L */
    { 0x7E, 0x04, 0x08, 0x08, 0x04, 0x7E, 0x00, 0x00 }, /* M */
    { 0x7E, 0x04, 0x08, 0x10, 0x20, 0x7E, 0x00, 0x00 }, /* N */
    { 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 }, /* O */ 
    { 0x7E, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, 0x00 }, /* P */
    { 0x7E, 0x12, 0x12, 0x12, 0x32, 0x4C, 0x00, 0x00 }, /* R */
    { 0x02, 0x02, 0x02, 0x7E, 0x02, 0x02, 0x02, 0x00 }, /* T */
    { 0x3E, 0x40, 0x20, 0x20, 0x40, 0x3E, 0x00, 0x00 }, /* W */
    { 0x02, 0x04, 0x08, 0x70, 0x08, 0x04, 0x02, 0x00 }, /* Y */
};

/* Routines  */

/* Exit the program */
void quit(void)
{
    printf("Quiting.\n");
    SDL_DestroyTexture(gTxr);
    gTxr = NULL;
    SDL_DestroyRenderer(gRen);
    gRen = NULL;
    SDL_DestroyWindow(gWin);
    gWin = NULL;
    SDL_Quit();
    exit(0);
}

/* clamp values to a specific range */
int clmp(int val, int min, int max)
{
	return (val >= min) ? (val <= max) ? val : max : min;
}

/* Get time in seconds*/
double get_time(void)
{
  struct timespec tp;
  return clock_gettime(CLOCK_MONOTONIC, &tp) == 0 ? (double)tp.tv_sec \
    + (double)tp.tv_nsec/1000000000.0 : 0.0;

}

/* User Interface */

/* Draw text*/
void draw_symbol(SDL_Renderer *gRen, int px,  int py,
                  Uint8 *symbol, Clr on, Clr off)
{ 
  int x, y;
  for(y = 0; y < 8; y++)
    for(x = 0; x < 8; x++)
    {
      if(symbol[y] & (1 << (7 - x)))
        SDL_SetRenderDrawColor(gRen, on.r, on.g, on.b, on.a);
      else
        SDL_SetRenderDrawColor(gRen, off.r, off.g, off.b, off.a);

      SDL_RenderDrawPoint(gRen, px + x, py + y);
    }
}
/* rotating phasor line 
 * args: renderer, origin x, origin y, radius, and radians*/
void draw_phasor_line(SDL_Renderer *gRen, double ox,
                    double oy, double radius, double radians)
{

    /* convert to degrees */
    double angle = radians * 180 / PI;

    /* rotate around origin point */
    double dx = ox + cos(angle)*radius;
    double dy = oy + sin(angle)*radius;

    SDL_RenderDrawLine(gRen, ox, oy, dx, dy);

 }

/* ring of the metronome, radius 160 by default.
 * args: renderer, center x, center y, radius. */
void draw_metronome_ring(SDL_Renderer *gRen, int cx, 
                      int cy, int radius)
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
  /* Screen width and height centered */
  int X_CENTER = WIDTH / 2;
  int Y_CENTER = HEIGHT / 2;
  

  /*Phasor angle and speed */
  double speed = 0.1;
  double angle = speed * get_time(); 
  
  SDL_RenderClear(gRen);

  SDL_SetRenderDrawColor(gRen, 0xFF, 0xFF, 0xFF, 255);
  draw_metronome_ring(gRen, X_CENTER, Y_CENTER, 160);

  SDL_SetRenderDrawColor(gRen, 0xFF, 0xFF, 0xFF, 255); /* Phasor  */
  draw_phasor_line(gRen, X_CENTER, Y_CENTER, 160, angle);
  SDL_SetRenderDrawColor(gRen, 0x00, 0x00, 0x00, 255);
  
  /* Draw BPM text */
  Clr on = set_clr(0xFF, 0xFF, 0xFF, 255);
  Clr off = set_clr(0x00, 0x00, 0x00, 255);
  draw_symbol(gRen, 100, 550, *symbol, on, off);

  SDL_RenderPresent(gRen);
  
  SDL_DestroyTexture(gTxr);
  gTxr = NULL;

  return 0;
}

/* SDL Initialization and events */

int init(void)
{
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
  
  if (gTxr == NULL)
    return printf("Texture error: %s\n", SDL_GetError());

  return 1;
}

int main(void)
{

  if(!init())
    return printf("Main(): Init has failed: %s\n", SDL_GetError());

  /* Main loop */
  while(!ql)
  {
    SDL_Event e;
    double elapsed, start = SDL_GetPerformanceCounter();
    while(SDL_PollEvent(&e) != 0)
    {
      switch(e.type){
      case SDL_QUIT:
        quit();
        ql = 1;
        break;
      }
    }
    render_ui();
    elapsed = (SDL_GetPerformanceCounter() - start) / (double)SDL_GetPerformanceFrequency() * 1000.0f;
    SDL_Delay(clmp(16.666f - elapsed, 0, 1000));
  }

  quit();
  return 0;
}
