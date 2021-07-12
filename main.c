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

Uint16 symbol[][8] = {   
  { 0x0000, 0x0000, 0xfefe, 0x7c7c, 0x3838, 0x1000, 0x0000, 0x0000 },
  { 0x0000, 0x386c, 0xc6c6, 0xd6d6, 0xc6c6, 0x6c38, 0x0000, 0x0000 },
  { 0x0000, 0x1838, 0x7818, 0x1818, 0x1818, 0x187e, 0x0000, 0x0000 },
  { 0x0000, 0x7cc6, 0x060c, 0x1830, 0x60c0, 0xc6fe, 0x0000, 0x0000 },
  { 0x0000, 0x7cc6, 0x0606, 0x3c06, 0x0606, 0xc67c, 0x0000, 0x0000 },
  { 0x0000, 0x0c1c, 0x3c6c, 0xccfe, 0x0c0c, 0x0c1e, 0x0000, 0x0000 },
  { 0x0000, 0xfec0, 0xc0c0, 0xfc06, 0x0606, 0xc67c, 0x0000, 0x0000 },
  { 0x0000, 0x3860, 0xc0c0, 0xfcc6, 0xc6c6, 0xc67c, 0x0000, 0x0000 },
  { 0x0000, 0xfec6, 0x0606, 0x0c18, 0x3030, 0x3030, 0x0000, 0x0000 },
  { 0x0000, 0x0060, 0x3018, 0x0c06, 0x0c18, 0x3060, 0x0000, 0x0000 },
  { 0x0000, 0x7cc6, 0xc60c, 0x1818, 0x1800, 0x1818, 0x0000, 0x0000 },
  { 0x0000, 0x007c, 0xc6c6, 0xdede, 0xdedc, 0xc07c, 0x0000, 0x0000 },
  { 0x0000, 0x1038, 0x6cc6, 0xc6fe, 0xc6c6, 0xc6c6, 0x0000, 0x0000 },
  { 0x0000, 0xfc66, 0x6666, 0x7c66, 0x6666, 0x66fc, 0x0000, 0x0000 },
  { 0x0000, 0x3c66, 0xc2c0, 0xc0c0, 0xc0c2, 0x663c, 0x0000, 0x0000 },
  { 0x0000, 0xf86c, 0x6666, 0x6666, 0x6666, 0x6cf8, 0x0000, 0x0000 },
  { 0x0000, 0xfe66, 0x6268, 0x7868, 0x6062, 0x66fe, 0x0000, 0x0000 },
  { 0x0000, 0xfe66, 0x6268, 0x7868, 0x6060, 0x60f0, 0x0000, 0x0000 },
  { 0x0000, 0x3c66, 0xc2c0, 0xc0de, 0xc6c6, 0x663a, 0x0000, 0x0000 },
  { 0x0000, 0xc6c6, 0xc6c6, 0xfec6, 0xc6c6, 0xc6c6, 0x0000, 0x0000 },
  { 0x0000, 0x3c18, 0x1818, 0x1818, 0x1818, 0x183c, 0x0000, 0x0000 },
  { 0x0000, 0x1e0c, 0x0c0c, 0x0c0c, 0xcccc, 0xcc78, 0x0000, 0x0000 },
  { 0x0000, 0xe666, 0x666c, 0x7878, 0x6c66, 0x66e6, 0x0000, 0x0000 },
  { 0x0000, 0xf060, 0x6060, 0x6060, 0x6062, 0x66fe, 0x0000, 0x0000 },
  { 0x0000, 0xc6ee, 0xfefe, 0xd6c6, 0xc6c6, 0xc6c6, 0x0000, 0x0000 },
  { 0x0000, 0xc6e6, 0xf6fe, 0xdece, 0xc6c6, 0xc6c6, 0x0000, 0x0000 },
  { 0x0000, 0x7cc6, 0xc6c6, 0xc6c6, 0xc6c6, 0xc67c, 0x0000, 0x0000 },
  { 0x0000, 0xfc66, 0x6666, 0x7c60, 0x6060, 0x60f0, 0x0000, 0x0000 },
  { 0x0000, 0x7cc6, 0xc6c6, 0xc6c6, 0xc6d6, 0xde7c, 0x0c0e, 0x0000 },
  { 0x0000, 0xfc66, 0x6666, 0x7c6c, 0x6666, 0x66e6, 0x0000, 0x0000 },
  { 0x0000, 0x7cc6, 0xc660, 0x380c, 0x06c6, 0xc67c, 0x0000, 0x0000 },
  { 0x0000, 0x7e7e, 0x5a18, 0x1818, 0x1818, 0x183c, 0x0000, 0x0000 },
  { 0x0000, 0xc6c6, 0xc6c6, 0xc6c6, 0xc6c6, 0xc67c, 0x0000, 0x0000 },
  { 0x0000, 0xc6c6, 0xc6c6, 0xc6c6, 0xc66c, 0x3810, 0x0000, 0x0000 },
  { 0x0000, 0xc6c6, 0xc6c6, 0xd6d6, 0xd6fe, 0xee6c, 0x0000, 0x0000 },
  { 0x0000, 0xc6c6, 0x6c7c, 0x3838, 0x7c6c, 0xc6c6, 0x0000, 0x0000 },

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
                  Uint16 *symbol, Clr on, Clr off)
{ 
  int x, y;
  for(y = 0; y < 16; y++)
    for(x = 0; x < 8; x++)
    {
      if(symbol[y] & (1 << (16 - x)))
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
    
  /* Ring */
  SDL_SetRenderDrawColor(gRen, 0xFF, 0xFF, 0xFF, 255);
  draw_metronome_ring(gRen, X_CENTER, Y_CENTER, 160);
  
  /* Phasor */
  SDL_SetRenderDrawColor(gRen, 0xFF, 0xFF, 0xFF, 255);
  draw_phasor_line(gRen, X_CENTER, Y_CENTER, 160, angle);

  SDL_SetRenderDrawColor(gRen, 0x00, 0x00, 0x00, 255);
  
  /* BPM */
  Clr on = set_clr(0xFF, 0xFF, 0xFF, 255);
  Clr off = set_clr(0x00, 0x00, 0x00, 255);
  draw_symbol(gRen, 100, 550, symbol[13], on, off);
  draw_symbol(gRen, 108, 550, symbol[28], on, off);
  draw_symbol(gRen, 116, 550, symbol[24], on, off);
  draw_symbol(gRen, 124, 550, symbol[41], on, off);
  draw_symbol(gRen, 132, 550, symbol[1], on, off);
  draw_symbol(gRen, 140, 550, symbol[1], on, off);
  draw_symbol(gRen, 148, 550, symbol[1], on, off);

  /* Phase offset */
  draw_symbol(gRen, 200, 550, symbol[25], on, off);
  draw_symbol(gRen, 208, 550, symbol[17], on, off);  
  draw_symbol(gRen, 216, 550, symbol[10], on, off);  
  draw_symbol(gRen, 224, 550, symbol[28], on, off);
  draw_symbol(gRen, 232, 550, symbol[14], on, off);  
  draw_symbol(gRen, 240, 550, symbol[41], on, off);
  draw_symbol(gRen, 248, 550, symbol[24], on, off);
  draw_symbol(gRen, 256, 550, symbol[15], on, off);
  draw_symbol(gRen, 264, 550, symbol[15], on, off);  
  draw_symbol(gRen, 272, 550, symbol[28], on, off);
  draw_symbol(gRen, 280, 550, symbol[14], on, off);
  draw_symbol(gRen, 288, 550, symbol[29], on, off);
  
  /* Latency */
  draw_symbol(gRen, 200, 570, symbol[21], on, off);
  draw_symbol(gRen, 208, 570, symbol[10], on, off);  
  draw_symbol(gRen, 216, 570, symbol[29], on, off);  
  draw_symbol(gRen, 224, 570, symbol[14], on, off);
  draw_symbol(gRen, 232, 570, symbol[23], on, off);  
  draw_symbol(gRen, 240, 570, symbol[12], on, off);
  draw_symbol(gRen, 248, 570, symbol[34], on, off);
  draw_symbol(gRen, 256, 570, symbol[39], on, off);
  draw_symbol(gRen, 264, 570, symbol[0], on, off);  
  
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
