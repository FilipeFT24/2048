#define  SDL_MAIN_HANDLED
#define  SCREEN_HEIGHT 500
#define  SCREEN_WIDTH  500
#define  SCREEN_PAD     10
#define  FONTSIZE_CELL  40
#include <stdbool.h>
#include <string.h>
#include "SDL.h"
#include "SDL_ttf.h"
//#include "../include/obj.h"

typedef struct Color{
    char a;
    char b;
    char g;
    char r;
}COLOR;

bool SDL_Open     (SDL_Renderer**, SDL_Window**);
void SDL_Close    (                SDL_Window* );
void SDL_Clear    (SDL_Renderer*);
void LoadingScreen(SDL_Renderer* , TTF_Font*, const char*);
void draw_text    (SDL_Renderer* , TTF_Font*, const char* , SDL_Rect, SDL_Color);
