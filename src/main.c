#include "../include/graphics.h"
#include "../include/obj.h"

void gLoop(M*, SDL_Renderer*, SDL_Window*);
void Print(M );
COLOR g_colors[] = {
    {230,227,232,255},
    {255,127, 89,255},
    {224, 74, 69,255},
    {237,207,114,255},
    { 65,216,127,255},
    { 54, 63,135,255},
    { 78, 89,178,255},
    {109,118,191,255},
    { 84, 47,132,255},
    {125, 77,188,255},
    {163, 77,188,255},
    {176,109,196,255},
    {  0,102,204,255},
    {  0,153,255,255},
    { 51,153,255,255},
    {153,204,255,255},
    {102,255,102,255}
};

int main(void){
	// Initialise SDL and GUI variables.
	SDL_Renderer* GAMERenderer = NULL;
	SDL_Window*   GAMEWindow   = NULL;
    if(!SDL_Open(&GAMERenderer,&GAMEWindow)){
    	exit(0);
    }




    M X;
    CreateM (&X,MaximumValue,NumberRows,NumberColumns);
    InsertE (&X,1,0,4);
    InsertE (&X,1,2,4);
    InsertE (&X,1,3,2);
    Print   ( X);

    gLoop(&X,GAMERenderer,GAMEWindow);



    SDL_Close(GAMEWindow);

}

void gLoop(M* X, SDL_Renderer* GAMERenderer, SDL_Window* GAMEWindow){
	// Loading screen.
	TTF_Font* Font = TTF_OpenFont("UbuntuMono-R.ttf",50);
    if(Font == NULL){
    	printf("Couldn't initialize SDL TTF: %s\n",SDL_GetError()); exit(1);
    }
    LoadingScreen(GAMERenderer,Font,"2048");


    int size = (SCREEN_WIDTH-2*SCREEN_PAD)/X->n-SCREEN_PAD;


    SDL_Clear(GAMERenderer);

	E* tmpA = X->head;
    for(register int i = 0; i < X->m; i++){
    	for(register int j = 0; j < X->n; j++){

    		// Fill rectangle.
    	    //SDL_Rect sdl_rect;
    	    //sdl_rect.x = SCREEN_PAD+i*(size+SCREEN_PAD);
    	    //sdl_rect.y = SCREEN_PAD+j*(size+SCREEN_PAD);
    	    //sdl_rect.w = size;
    	    //sdl_rect.h = size;
    		SDL_Rect sdl_rect = { SCREEN_PAD+j*(size+SCREEN_PAD), SCREEN_PAD+i*(size+SCREEN_PAD),size,size};

    	    COLOR s = g_colors[0];

    	    SDL_SetRenderDrawColor(GAMERenderer,s.r,s.g,s.b,s.a);
    	    SDL_RenderFillRect    (GAMERenderer,&sdl_rect);


    		if(tmpA != NULL && (tmpA->i == i && tmpA->j == j)){
    			char str[15]; // 15 chars is enough for 2^16
    			sprintf(str,"%d",tmpA->v);


    			printf("(%d,%d),%s\n",tmpA->i,tmpA->j,str);
    			tmpA = tmpA->next;

        	    SDL_Color White = { 80, 80, 80,255};
        	    draw_text(GAMERenderer,Font,str,sdl_rect,White);
    		}



    	}
    }






    SDL_RenderPresent(GAMERenderer);

    SDL_Delay(2500);





	// Initialise game.


	TTF_CloseFont(Font);
	Font = NULL;
}







void Print(M X){
	E* tmp = X.head;
	for(register int i = 0; i < X.m; i++){
		for(register int j = 0; j < X.n; j++){
			if(tmp != NULL && (tmp->i == i && tmp->j == j)){
				printf("%4d|",tmp->v);
				if(tmp->next != NULL){
					tmp = tmp->next;
				}
			}
			else{
				printf("%4c|",' ');
			}
		}
		printf("\n");
	}
	printf("\n");
}
