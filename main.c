/*
MIT License

Copyright (c) 2021 Preston Smith

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice 
(including the next paragraph) shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

*/

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

int ql = 0; /* Quit loop */

int BPM = 128;
int offset = 0;

/* Font */

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

/* 9x16 VGA Page 437 */

Uint8 symbol[][16] = {   
  { 0x00, 0x00, 0x38, 0x6c, 0xc6, 0xc6, 0xd6, 0xd6, 0xc6, 0xc6, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00 }, /* 0 */
  { 0x00, 0x00, 0x18, 0x38, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00 }, /* 1 */
  { 0x00, 0x00, 0x7c, 0xc6, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00 }, /* 2 */
  { 0x00, 0x00, 0x7c, 0xc6, 0x06, 0x06, 0x3c, 0x06, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00 }, /* 3 */
  { 0x00, 0x00, 0x0c, 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x0c, 0x0c, 0x1e, 0x00, 0x00, 0x00, 0x00 }, /* 4 */
  { 0x00, 0x00, 0xfe, 0xc0, 0xc0, 0xc0, 0xfc, 0x06, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00 }, /* 5 */
  { 0x00, 0x00, 0x38, 0x60, 0xc0, 0xc0, 0xfc, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00 }, /* 6 */
  { 0x00, 0x00, 0xfe, 0xc6, 0x06, 0x06, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00 }, /* 7 */
  { 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00 }, /* 8 */
  { 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x06, 0x06, 0x0c, 0x78, 0x00, 0x00, 0x00, 0x00 }, /* 9 */
  { 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* : */
  { 0x00, 0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00 }, /* A */
  { 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x66, 0x66, 0xfc, 0x00, 0x00, 0x00, 0x00 }, /* B */
  { 0x00, 0x00, 0x3c, 0x66, 0xc2, 0xc0, 0xc0, 0xc0, 0xc0, 0xc2, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00 }, /* C */
  { 0x00, 0x00, 0xf8, 0x6c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00, 0x00, 0x00, 0x00 }, /* D */
  { 0x00, 0x00, 0xfe, 0x66, 0x62, 0x68, 0x78, 0x68, 0x60, 0x62, 0x66, 0xfe, 0x00, 0x00, 0x00, 0x00 }, /* E */
  { 0x00, 0x00, 0xfe, 0x66, 0x62, 0x68, 0x78, 0x68, 0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00 }, /* F */
  { 0x00, 0x00, 0x3c, 0x66, 0xc2, 0xc0, 0xc0, 0xde, 0xc6, 0xc6, 0x66, 0x3a, 0x00, 0x00, 0x00, 0x00 }, /* G */
  { 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00 }, /* H */
  { 0x00, 0x00, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00 }, /* I */
  { 0x00, 0x00, 0x1e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0xcc, 0x78, 0x00, 0x00, 0x00, 0x00 }, /* J */
  { 0x00, 0x00, 0xe6, 0x66, 0x66, 0x6c, 0x78, 0x78, 0x6c, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00 }, /* K */
  { 0x00, 0x00, 0xf0, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00, 0x00, 0x00, 0x00 }, /* L */
  { 0x00, 0x00, 0xc6, 0xee, 0xfe, 0xfe, 0xd6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00 }, /* M */
  { 0x00, 0x00, 0xc6, 0xe6, 0xf6, 0xfe, 0xde, 0xce, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00 }, /* N */
  { 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00 }, /* O */
  { 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00 }, /* P */
  { 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xd6, 0xde, 0x7c, 0x0c, 0x0e, 0x00, 0x00 }, /* Q */
  { 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00 }, /* R */
  { 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0x60, 0x38, 0x0c, 0x06, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00 }, /* S */
  { 0x00, 0x00, 0x7e, 0x7e, 0x5a, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00 }, /* T */
  { 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00 }, /* U */
  { 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00 }, /* V */
  { 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xd6, 0xd6, 0xd6, 0xfe, 0xee, 0x6c, 0x00, 0x00, 0x00, 0x00 }, /* W */
  { 0x00, 0x00, 0xc6, 0xc6, 0x6c, 0x7c, 0x38, 0x38, 0x7c, 0x6c, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00 }, /* X */
  { 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00 }, /* Y */
  { 0x00, 0xfe, 0xc6, 0x86, 0x0c, 0x18, 0x30, 0x60, 0xc2, 0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* Z */
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* NUL */
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

/* BPM Controller 
 * increasesd BPM (tempo) value. */
void bpm_controller(int *temp, int value)
{
  *temp = value;
  if(BPM <= 0)
  {
    BPM = 0;
  }
  if(BPM >= 300)
  {
    BPM = 300;
  }
}


/*phase offset in miliseconds */
void phase_offset(int *os, int value)
{
  *os = value;
  if(offset <= 0)
  {
    offset = 0;
  }
  if(offset >= 10)
  {
    offset = 10;
  }
}

/* key press events */
void keypress(SDL_Event *e)
{
  switch(e->key.keysym.sym)
  {
    case SDLK_j:
     bpm_controller(&BPM, BPM + 1);
     break;
    case SDLK_k:
     bpm_controller(&BPM, BPM - 1);
     break;
    case SDLK_PAGEUP:
     phase_offset(&offset, offset + 1);
     break;
    case SDLK_PAGEDOWN:
     phase_offset(&offset, offset - 1);
     break;
  }

}
/* User Interface */

/* Draw text*/
void draw_symbol(SDL_Renderer *gRen, int px,  int py,
                  Uint8 *symbol, Clr on, Clr off)
{ 
  int x, y;
  for(y = 0; y < 16; y++)
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
  double speed = 0.1 * (double)BPM/100;
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
  draw_symbol(gRen, 100, 550, symbol[12], on, off);
  draw_symbol(gRen, 108, 550, symbol[26], on, off);
  draw_symbol(gRen, 116, 550, symbol[23], on, off);
  draw_symbol(gRen, 124, 550, symbol[10], on, off);
  draw_symbol(gRen, 132, 550, symbol[(BPM/100) % 10], on, off);
  draw_symbol(gRen, 140, 550, symbol[(BPM/10) % 10], on, off);
  draw_symbol(gRen, 148, 550, symbol[BPM % 10], on, off);

  /* Phase offset */
  draw_symbol(gRen, 200, 550, symbol[26], on, off);
  draw_symbol(gRen, 208, 550, symbol[18], on, off);  
  draw_symbol(gRen, 216, 550, symbol[11], on, off);  
  draw_symbol(gRen, 224, 550, symbol[29], on, off);
  draw_symbol(gRen, 232, 550, symbol[15], on, off);  
  draw_symbol(gRen, 240, 550, symbol[38], on, off);
  draw_symbol(gRen, 248, 550, symbol[25], on, off);
  draw_symbol(gRen, 256, 550, symbol[16], on, off);
  draw_symbol(gRen, 264, 550, symbol[16], on, off);  
  draw_symbol(gRen, 272, 550, symbol[29], on, off);
  draw_symbol(gRen, 280, 550, symbol[15], on, off);
  draw_symbol(gRen, 288, 550, symbol[30], on, off);
  draw_symbol(gRen, 296, 550, symbol[10], on, off);
  draw_symbol(gRen, 304, 550, symbol[(offset/10) % 10], on, off);
  draw_symbol(gRen, 312, 550, symbol[offset % 10], on, off);
  
  /* Latency */
  draw_symbol(gRen, 200, 570, symbol[22], on, off);
  draw_symbol(gRen, 208, 570, symbol[11], on, off);  
  draw_symbol(gRen, 216, 570, symbol[30], on, off);  
  draw_symbol(gRen, 224, 570, symbol[15], on, off);
  draw_symbol(gRen, 232, 570, symbol[24], on, off);  
  draw_symbol(gRen, 240, 570, symbol[13], on, off);
  draw_symbol(gRen, 248, 570, symbol[35], on, off);
  draw_symbol(gRen, 256, 570, symbol[37], on, off);
  draw_symbol(gRen, 264, 570, symbol[0], on, off);
  draw_symbol(gRen, 272, 570, symbol[0], on, off);
  
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
      switch(e.type)
      {
      case SDL_QUIT:
        quit();
        ql = 1;
        break;
        case SDL_KEYDOWN:
        keypress(&e);
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
