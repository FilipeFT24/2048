#define  SDL_MAIN_HANDLED
#define  MaximumValue     2048 // Doubly linked list (DLL) variables.
#define  MAXT                2
#define  NumberOfCols        4
#define  NumberOfRows        4
#define  MAXSIZE            15 // Graphics fields.
#define  SCREEN_HEIGHT     500
#define  SCREEN_WIDTH      500
#define  SCREEN_PAD         10
#define  sizeH (SCREEN_WIDTH-SCREEN_PAD)/NumberOfRows-SCREEN_PAD
#define  sizeW (SCREEN_WIDTH-SCREEN_PAD)/NumberOfCols-SCREEN_PAD
#define  length(v) sizeof(v)/sizeof(v[0])
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SDL.h"
#include "SDL_ttf.h"

typedef struct Element{
	int i;
	int j;
	int v;
	struct Element* prev;
	struct Element* next;
}E;
typedef struct Matrix{
	bool VMoves[4];
	int  MValue;
	int  MCount;
	int  Score;
	int  m;
	int  n;
	struct Element* head;
	struct Element* tail;
	SDL_Renderer*   sdlR;
	SDL_Window*     sdlW;
}M;
typedef struct Color{
    char r;
    char g;
    char b;
    char a;
}COLOR;

void CreateM(M*, int, int, int);
void gLoop  (M*);
