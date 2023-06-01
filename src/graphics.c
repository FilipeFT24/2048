#include "../include/graphics.h"

COLOR g_bg       = {211,204,201,255};
COLOR g_fg       = { 80, 80, 80,255};


// Basic operations.
bool SDL_Open(SDL_Renderer** gRenderer, SDL_Window** GAMEWindow){
	bool bflag = false;
	if(TTF_Init() < 0){
		printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
	}
	else{
		if(SDL_Init(SDL_INIT_VIDEO) < 0){
			printf("Couldn't initialize SDL: %s\n",SDL_GetError());
		}
		else{
			*GAMEWindow = SDL_CreateWindow("2048",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
			if(GAMEWindow == NULL){
				printf("Failed to open %d x %d window: %s\n",SCREEN_WIDTH,SCREEN_HEIGHT,SDL_GetError());
			}
			else{
				*gRenderer = SDL_CreateRenderer(*GAMEWindow,-1,SDL_RENDERER_ACCELERATED);
				if(gRenderer == NULL){
					printf("Failed to create renderer: %s\n",SDL_GetError());
				}
				else{
					SDL_SetRenderDrawColor(*gRenderer,g_bg.r,g_bg.g,g_bg.b,g_bg.a);
					bflag = true;
				}
			}
		}
	}
	return bflag;
}
void SDL_Clear(SDL_Renderer* gRenderer){
	SDL_SetRenderDrawColor(gRenderer,g_bg.r,g_bg.g,g_bg.b,g_bg.a);
	SDL_RenderClear       (gRenderer);
}
void SDL_Close(SDL_Window* GAMEWindow){
	SDL_DestroyWindow(GAMEWindow); GAMEWindow = NULL;
    TTF_Quit();
	SDL_Quit();
}




//

void draw_text(SDL_Renderer* gRenderer,TTF_Font* font,const char* text, SDL_Rect rect, SDL_Color color){
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);
    SDL_Rect message_rect;

    TTF_SizeText(font, text, &message_rect.w, &message_rect.h);
    message_rect.x = rect.x+rect.w/2-message_rect.w/2;
    message_rect.y = rect.y+rect.h/2-message_rect.h/2;

    SDL_RenderCopy(gRenderer, Message, NULL, &message_rect);
    SDL_DestroyTexture(Message);
    SDL_FreeSurface(surfaceMessage);
}



void LoadingScreen(SDL_Renderer* gRenderer, TTF_Font* Font, const char* Text){
    // Clear.
    SDL_SetRenderDrawColor(gRenderer,g_bg.r,g_bg.g,g_bg.b,g_bg.a);
	SDL_RenderClear       (gRenderer);
    // SDL_Rect: defines a rectangular area of the screen. It is used by SDL_BlitSurface() to define blitting regions and by several other video functions.
    SDL_Rect sdl_rect;
    sdl_rect.x = 0;
    sdl_rect.y = 0;
    sdl_rect.w = SCREEN_WIDTH-2*SCREEN_PAD;
    sdl_rect.h = SCREEN_HEIGHT/2;
    // Draw...
    SDL_Color black = {g_fg.r,g_fg.g, g_fg.b};



    draw_text(gRenderer,Font,Text,sdl_rect,black);


    SDL_RenderPresent(gRenderer);
    SDL_Delay(0000);

}











