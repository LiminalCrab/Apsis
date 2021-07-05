#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

SDL_Window    *gWin = NULL;
SDL_Renderer  *gRen = NULL;
SDL_Texture   *gTxr, *bpmtxt_Txr = NULL;

SDL_Rect       bpmtxt_Rect;
TTF_Font      *font = NULL;

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

/* Routines  */

/* Exit the program */
void quit(void)
{
    printf("Quiting.\n");
    SDL_DestroyTexture(gTxr);
    gTxr = NULL;
    SDL_DestroyTexture(bpmtxt_Txr);
    bpmtxt_Txr = NULL;
    SDL_DestroyRenderer(gRen);
    gRen = NULL;
    SDL_DestroyWindow(gWin);
    gWin = NULL;
    SDL_Quit();
    exit(0);
}

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
void draw_txt(SDL_Renderer *gRen, int x, int y, 
              char *txt, TTF_Font *font, 
              SDL_Texture **txtTxr, SDL_Rect *txtRect)
{
  int txt_w;
  int txt_h;
  SDL_Surface  *txtSurf;
  SDL_Color     txt_clr = {0xFF, 0xFF, 0xFF, 0};
  
  txtSurf = TTF_RenderText_Solid(font, txt, txt_clr);
  *txtTxr = SDL_CreateTextureFromSurface(gRen, txtSurf);

  if(txtTxr == NULL)
  {
      printf("Text Texture: %19s\n", &txtTxr);
  }

  txt_w = txtSurf->w;
  txt_h = txtSurf->h;
  txtRect->x = x;
  txtRect->y = y;
  txtRect->w = txt_w;
  txtRect->h = txt_h;
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
  
  /* Draw BPM text */
  SDL_SetRenderDrawColor(gRen, 0xFF, 0xFF, 0xFF, 255);
  
  /*draws text "BPM"
   *args: renderer, x coord, y coord, text, texture, rectangle*/
  draw_txt(gRen, 100, 550, "BPM:", font, &bpmtxt_Txr, &bpmtxt_Rect);
  SDL_RenderCopy(gRen, bpmtxt_Txr, NULL, &bpmtxt_Rect);
  
  /* draws text that reads "00" */
  draw_txt(gRen, 130, 550, "00", font, &bpmtxt_Txr, &bpmtxt_Rect);
  SDL_RenderCopy(gRen, bpmtxt_Txr, NULL, &bpmtxt_Rect);

  SDL_SetRenderDrawColor(gRen, 0x00, 0x00, 0x00, 255); /* Background */
  SDL_RenderPresent(gRen);
  
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

  if(gTxr == NULL)
    return printf("Texture error: %s\n", SDL_GetError());
  
  TTF_Init();

  if(!font)
   {
     font = TTF_OpenFont("/home/liminalcrab/Documents/Projects/Apsis/fonts/FreeSans.ttf", 12);
    if (font == NULL)
     {
      printf("Font: %s\n", TTF_GetError());
      quit();
    }
  }
  return 1;
}

int main(void)
{
  Uint8 tick = 0;

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
