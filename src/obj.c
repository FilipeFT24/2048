#include "../include/obj.h"

// 'Element' structure function(s).
static E* CreateE(int i, int j, int v){
	// Allocate node (or element).
	E* N = (E*)malloc(sizeof(E));
	// Assign fields.
	if(N != NULL){
		N->i    = i;
		N->j    = j;
		N->v    = v;
		N->prev = NULL;
		N->next = NULL;
	}
	return N;
}
static void InsertE(M* X, int i, int j, int v){
	// Create node (or element).
	E* N = CreateE(i,j,v);
	// Check...
	assert(N->i >= 0 && N->i < X->m);
	assert(N->j >= 0 && N->j < X->n);
	// If empty...
	if(X->head == NULL){
		X->head = N;
		X->tail = N;
	}
	// Else..
	else{
		E* A = X->head;
		E* B = NULL;
		while(A != NULL && X->n*A->i+A->j <= X->n*N->i+N->j){
			B = A;
			A = A->next;
		}
		if(B != NULL && (B->i == N->i && B->j == N->j)){
			exit(1);
		}
		N->prev = B; B != NULL ? B->next = N : (X->head = N);
		N->next = A; A != NULL ? A->prev = N : (X->tail = N);
	}
}
// 'Matrix' structure function(s).
void CreateM(M* X, int MValue, int m, int n){
	// Assign fields.
	for(register int j = 0; j < length(X->VMoves); j++){
		X->VMoves[j] = false;
	}
	X->MValue = MValue; // Doubly linked list (DLL) fields.
	X->MCount = 0;
	X->Score  = 0;
	X->m      = m;
	X->n      = n;
	X->head   = NULL;
	X->tail   = NULL;
	X->sdlR   = NULL;   // Graphics fields.
	X->sdlW   = NULL;
}
/*
static void Print(M X){
	E* A = X.head;
	for(register int i = 0; i < X.m; i++){
		for(register int j = 0; j < X.n; j++){
			if(A != NULL && (A->i == i && A->j == j)){
				printf("%4d|",A->v);
				if(A->next != NULL){
					A = A->next;
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
*/
// Check whether a given move is valid.
static void ValidN(M* X){
	if(X->head == NULL){
		exit(1);
	}
	register int i = 0;
	register int j = 0;
	E* A = NULL;
	E* B = NULL;
	for(j = 0; j < X->n; j++){
		i = 0;
		for(A = X->head; A != NULL && A->j != j; A = A->next){}; if(A == NULL) continue;
		for( ; ; ){
			B = A;
			A = A->next;
			while(A != NULL && A->j != j){
				A = A->next;
			}
			if(A != NULL){
				if(A->v == B->v || A->i > i+1){
					X->VMoves[0] = true;
					return;
				}
				i++;
			}
			else{
				if(B->i > i){
					X->VMoves[0] = true;
					return;
				}
				break;
			}
		}
	}
}
static void ValidS(M* X){
	if(X->head == NULL){
		exit(1);
	}
	register int i = X->m-1;
	register int j = X->n-1;
	E* A = NULL;
	E* B = NULL;
	for(j = X->n-1; j >= 0; j--){
		i = X->m-1;
		for(A = X->tail; A != NULL && A->j != j; A = A->prev){}; if(A == NULL) continue;
		for( ; ; ){
			B = A;
			A = A->prev;
			while(A != NULL && A->j != j){
				A = A->prev;
			}
			if(A != NULL){
				if(A->v == B->v || A->i < i-1){
					X->VMoves[1] = true;
					return;
				}
				i--;
			}
			else{
				if(B->i < i){
					X->VMoves[1] = true;
					return;
				}
				break;
			}
		}
	}
}
static void ValidE(M* X){
	if(X->head == NULL){
		exit(1);
	}
	register int i = X->m-1;
	register int j = X->n-1;
	for(E* A = X->tail; A != NULL; ){
		if(A->i == i){
			if(A->j < j || (A->prev != NULL && (A->prev->i == i && A->v == A->prev->v))){
				X->VMoves[2] = true;
				return;
			}
			A = A->prev;
			j--;
		}
		else{
			i--;
			j = X->n-1;
		}
	}
}
static void ValidW(M* X){
	if(X->head == NULL){
		exit(1);
	}
	register int i = 0;
	register int j = 0;
	for(E* A = X->head; A != NULL; ){
		if(A->i == i){
			if(A->j > j || (A->next != NULL && (A->next->i == i && A->v == A->next->v))){
				X->VMoves[3] = true;
				return;
			}
			A = A->next;
			j++;
		}
		else{
			i++;
			j = 0;
		}
	}
}
// Sliding functions.
static void AddH(M** X, E** A, E** B){
	// Increment *B.v.
	(*B)->v            = (*A)->v+(*B)->v;
	// Detach and delete *A.
	(*A)->prev != NULL ? (*A)->prev->next = (*A)->next : ((*X)->head = (*A)->next);
	(*A)->next != NULL ? (*A)->next->prev = (*A)->prev : ((*X)->tail = (*A)->prev); free(*A); *A = (*B)->next;
}
static void AddN(M** X, E** A, E** B, int j){
	// Repeat (routine)...
	AddH(X,A,B);
	// Set to...
	while(*A != NULL && (*A)->j != j){
		*A = (*A)->next;
	}
}
static void AddS(M** X, E** A, E** B, int j){
	// Increment *B.v.
	(*B)->v            = (*A)->v+(*B)->v;
	// Detach and delete *A.
	(*A)->prev != NULL ? (*A)->prev->next = (*A)->next : ((*X)->head = (*A)->next);
	(*A)->next != NULL ? (*A)->next->prev = (*A)->prev : ((*X)->tail = (*A)->prev); free(*A); *A = (*B)->prev;
	// Set to...
	while(*A != NULL && (*A)->j != j){
		*A = (*A)->prev;
	}
}
static void ShiftN(M** X, E** A, E** B, int i){
	if(*B != NULL && (*X)->n*(*B)->i+(*B)->j > (*X)->n*i+(*A)->j){
		// Detach *A.
		(*A)->prev != NULL ? (*A)->prev->next = (*A)->next : ((*X)->head = (*A)->next); // Redundant else statement.
		(*A)->next != NULL ? (*A)->next->prev = (*A)->prev : ((*X)->tail = (*A)->prev);
		// Re-attach *A (new *B.prev).
		(*A)->prev         = (*B)->prev;
		(*A)->next         = (*B);
		(*B)->prev != NULL ? (*B)->prev->next = (*A)       : ((*X)->head = (*A));
		(*B)->prev         = (*A);
	}
	// Set to...
	(*A)->i = i;
	(*B)    = NULL;
}
static void ShiftS(M** X, E** A, E** B, int i){
	if(*B != NULL && (*X)->n*(*B)->i+(*B)->j < (*X)->n*i+(*A)->j){
		// Detach *A.
		(*A)->prev != NULL ? (*A)->prev->next = (*A)->next : ((*X)->head = (*A)->next);
		(*A)->next != NULL ? (*A)->next->prev = (*A)->prev : ((*X)->tail = (*A)->prev); // Redundant else statement.
		// Re-attach *A (new *B.next).
		(*A)->prev         = (*B);
		(*A)->next         = (*B)->next;
		(*B)->next != NULL ? (*B)->next->prev = (*A)       : ((*X)->tail = (*A));
		(*B)->next         = (*A);
	}
	// Set to...
	(*A)->i = i;
	(*B)    = NULL;
}
static void SlideN(M* X){
	if(X->head == NULL){
		exit(1);
	}
	register int i = 0;
	register int j = 0;
	E* A = NULL;
	E* B = NULL;
	E* C = NULL;
	for(j = 0; j < X->n; j++){
		i = 0;
		for(A = X->head; A != NULL && A->j != j; A = A->next){}; if(A == NULL) continue;
		for( ; ; ){
			B = A;
			A = A->next;
			while(A != NULL && A->j != j){
				A = A->next;
			}
			if(A != NULL && A->v == B->v){
				AddN(&X,&A,&B,j);
			}
			if(B->i > i){
				if(B->prev != NULL){
					for(C = B->prev; C->prev != NULL && X->n*C->prev->i+C->prev->j > X->n*i+j; C = C->prev){};
				}
				ShiftN(&X,&B,&C,i);
			}
			i++;
			if(A == NULL){
				break;
			}
		}
	}
}
static void SlideS(M* X){
	if(X->head == NULL){
		exit(1);
	}
	register int i = X->m-1;
	register int j = X->n-1;
	E* A = NULL;
	E* B = NULL;
	E* C = NULL;
	for(j = X->n-1; j >= 0; j--){
		i = X->m-1;
		for(A = X->tail; A != NULL && A->j != j; A = A->prev){}; if(A == NULL) continue;
		for( ; ; ){
			B = A;
			A = A->prev;
			while(A != NULL && A->j != j){
				A = A->prev;
			}
			if(A != NULL && A->v == B->v){
				AddS(&X,&A,&B,j);
			}
			if(B->i < i){
				if(B->next != NULL){
					for(C = B->next; C->next != NULL && X->n*C->next->i+C->next->j < X->n*i+j; C = C->next){};
				}
				ShiftS(&X,&B,&C,i);
			}
			i--;
			if(A == NULL){
				break;
			}
		}
	}
}
static void SlideE(M* X){
	if(X->head == NULL){
		exit(1);
	}
	if(X->head != X->tail){
		E* A = X->head;
		E* B = NULL;
		while(A->next != NULL){
			B = A;
			A = A->next;
			if(A->i == B->i && A->v == B->v){
				AddH(&X,&A,&B);
			}
			if(A == NULL){
				break;
			}
		}
	}
	register int i = X->m-1;
	register int j = X->n-1;
	for(E* C = X->tail; C != NULL; ){
		if(C->i == i){
			if(C->j < j){
				C->j = j;
			}
			C = C->prev;
			j--;
		}
		else{
			i--;
			j = X->n-1;
		}
	}
}
static void SlideW(M* X){
	if(X->head == NULL){
		exit(1);
	}
	if(X->head != X->tail){
		E* A = X->head;
		E* B = NULL;
		while(A->next != NULL){
			B = A;
			A = A->next;
			if(A->i == B->i && A->v == B->v){
				AddH(&X,&A,&B);
			}
			if(A == NULL){
				break;
			}
		}
	}
	register int i = 0;
	register int j = 0;
	for(E* C = X->head; C != NULL; ){
		if(C->i == i){
			if(C->j > j){
				C->j = j;
			}
			C = C->next;
			j++;
		}
		else{
			i++;
			j = 0;
		}
	}
}
static bool gLoop(M* X){
	// Initialise...
	register int i = 0, j = 0, k = 0, l = 0; register bool bflag = false; E* A = NULL;
	register int c = 0, n = 0;
	if(X->MCount == 0){
		n = 2; // Start (or beggining) of the game.
	}
	else{
		n = 1; // Otherwise.
	}
	for(A = X->head; A != NULL; A = A->next){
		c++;
	}
	for(i = 0; i < n; i++){
		if(c < NumberOfCols*NumberOfRows){
			A = X->head;
			j = 0;
			k = 0;
			l = rand()%(NumberOfCols*NumberOfRows-c);
			while(j <= l){
				if(A != NULL && k == X->n*A->i+A->j){
					A = A->next;
				}
				else{
					j++;
				}
				k++;
			}
			InsertE(X,(k-1)/NumberOfCols,(k-1)%NumberOfCols,rand()%2 ? 2:4);
		}
		c++;
	}
	// Check valid moves...
	for(i = 0; i < length(X->VMoves); i++){
		X->VMoves[i] = false;
	}
	ValidN(X);
	ValidS(X);
	ValidE(X);
	ValidW(X);
	for(i = 0; i < length(X->VMoves); i++){
		if(X->VMoves[i]){
			bflag = true;
			break;
		}
	}
	return bflag;
}
static void Slide(M* X, int C){
	switch(C){
		case 0: SlideN(X); // Up.
			break;
		case 1: SlideS(X); // Down.
			break;
		case 2: SlideE(X); // To the right.
			break;
		case 3: SlideW(X); // To the left.
			break;
		default:
			return;
	}
}

// Graphics function(s).
SDL_Color Bc [] = {
	{187,173,160,255}
};
SDL_Color TVc[] = {
	{205,193,180,255}, // Empty.
	{119,110,101,255}, //     2.
	{119,110,101,255}, //     4.
	{249,246,242,255}, //     8.
	{249,246,242,255}, //    16.
	{249,246,242,255}, //    32.
	{249,246,242,255}, //    64.
	{249,246,242,255}, //   128.
	{249,246,242,255}, //   256.
	{249,246,242,255}, //   512.
	{249,246,242,255}, //  1024.
	{249,246,242,255}  //  2048.
};
COLOR TBc[] = {
	{205,193,180,255}, // Empty.
	{238,228,218,255}, //     2.
	{238,225,201,255}, //     4.
	{243,178,122,255}, //     8.
	{246,150,100,255}, //    16.
	{247,124, 95,255}, //    32.
	{247, 95, 59,255}, //    64.
	{237,208,115,255}, //   128.
	{237,204, 98,255}, //   256.
	{237,201, 80,255}, //   512.
	{237,197, 63,255}, //  1024.
	{237,194, 46,255}  //  2048.
};
static bool SDL_Open(M* X){
	bool bflag = false;
	if(TTF_Init() < 0){
		printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
	}
	else{
		if(SDL_Init(SDL_INIT_VIDEO) < 0){
			printf("Couldn't initialize SDL: %s\n",SDL_GetError());
		}
		else{
			X->sdlW = SDL_CreateWindow("2048",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
			if(X->sdlW == NULL){
				printf("Failed to open %d x %d window: %s\n",SCREEN_WIDTH,SCREEN_HEIGHT,SDL_GetError());
			}
			else{
				X->sdlR = SDL_CreateRenderer(X->sdlW,-1,SDL_RENDERER_ACCELERATED);
				if(X->sdlR == NULL){
					printf("Failed to create renderer: %s\n",SDL_GetError());
				}
				else{
					SDL_SetRenderDrawColor(X->sdlR,Bc[0].r,Bc[0].g,Bc[0].b,Bc[0].a);
					bflag = true;
				}
			}
		}
	}
	return bflag;
}
static void SDL_Close(M* X){
	SDL_DestroyWindow(X->sdlW);X->sdlW = NULL;
    TTF_Quit();
	SDL_Quit();
}
static void SDL_Fill(M* X, TTF_Font* Font, const char* Text, SDL_Rect r, SDL_Color Color){
    SDL_Surface* SurfMessage = TTF_RenderText_Blended      (Font,Text,Color);
    SDL_Texture*     Message = SDL_CreateTextureFromSurface(X->sdlR,SurfMessage);
    SDL_Rect MessageR;
    TTF_SizeText      (Font,Text,&MessageR.w,&MessageR.h);

    MessageR.x = r.x+r.w/2-MessageR.w/2;
    MessageR.y = r.y+r.h/2-MessageR.h/2;

    SDL_RenderCopy    (X->sdlR,Message,NULL,&MessageR);
    SDL_DestroyTexture(    Message);
    SDL_FreeSurface   (SurfMessage);
}

static void SDL_Print(M* X, TTF_Font* Font){
    char str[MAXSIZE];
    int k = 0;

    SDL_Rect sq;

	E* A = X->head;
    for(int i = 0; i < X->m; i++){
    	for(int j = 0; j < X->n; j++){
    		// Square dimensions.
    	    sq.x = SCREEN_PAD+j*(sizeW+SCREEN_PAD);
    	    sq.y = SCREEN_PAD+i*(sizeH+SCREEN_PAD);
    	    sq.w = sizeW;
    	    sq.h = sizeH;
    	    // Render square.
    		if(A != NULL && (A->i == i && A->j == j)){
    			k = log2(A->v);
    			sprintf(str,"%d",A->v);

        	    SDL_SetRenderDrawColor(X->sdlR,TBc[k].r,TBc[k].g,TBc[k].b,TBc[k].a);
        	    SDL_RenderFillRect    (X->sdlR,&sq);
    			SDL_Fill              (X,Font,str,sq,TVc[k]);

    			A = A->next;
    		}
    		else{
        	    SDL_SetRenderDrawColor(X->sdlR,TBc[0].r,TBc[0].g,TBc[0].b,TBc[0].a);
        	    SDL_RenderFillRect    (X->sdlR,&sq);
    		}
    	}
    }
    SDL_RenderPresent(X->sdlR);
    SDL_Delay(2000);
}
void GLoop(M* X){
	if(!SDL_Open(X)){
		exit(0);
	}
	else{
		TTF_Font* Font = TTF_OpenFont("ClearSans-Bold.ttf",65);
	    if(Font == NULL){
	    	printf("Couldn't initialize SDL TTF: %s\n",SDL_GetError()); exit(1);
	    }
	    else{
	    	// Clear.
	        SDL_SetRenderDrawColor(X->sdlR,Bc[0].r,Bc[0].g,Bc[0].b,Bc[0].a);
	        SDL_RenderClear       (X->sdlR);


	        SDL_Event E;
	        register bool Quit = false;

	        /*
			if(gLoop(X)){
				Slide(X,1);
				X->MCount++;
			}
			else{
				break;
			}
			SDL_Print(X,Font);
			break;
			*/


	        while(!Quit){
		        while(!SDL_PollEvent(&E)){
		        	if(E.type == SDL_QUIT){
		        		Quit = true;
		        	}
		        	else{
		        		if(E.type == SDL_KEYUP){
		        			//make move com as setas

		        		}
		        		if(E.type == SDL_MOUSEBUTTONUP){
		        			// fazer a translação
		        		}
		        	}
		        }
	        }
	        TTF_CloseFont(Font); SDL_Close(X);
	    }
	}
}
