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
		N->prev = B; B != NULL ? B->next = N : (X->head = N);
		N->next = A; A != NULL ? A->prev = N : (X->tail = N);
		/*
		E* A = X->head;
		E* B = NULL;
		E* C = X->head;
		while(A != NULL && X->n*A->i+A->j <= X->n*N->i+N->j){
			C = A;
			B = A;
			A = A->next;
		}
		if(C->i == N->i && C->j == N->j){
			C->v = N->v;
			free(N);
		}
		else{
			N->prev = B; B != NULL ? B->next = N : (X->head = N);
			N->next = A; A != NULL ? A->prev = N : (X->tail = N);
		}
		*/
	}
}
// 'Matrix' structure function(s).
void CreateM(M* X, int MValue, int m, int n){
	// Assign fields.
	for(int j = 0; j < length(X->VMoves); j++){
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
*/
static void InitMove(M* X, int n){

	srand(time(NULL));

	int i = 0, j = 0, k = 0; E* B = NULL;
	int c = 0;
	int L = 0;
	for(E* A = X->head; A != NULL; A = A->next){
		c++;
	}
	for(; i < n; i++){
		if(c < NumberOfCols*NumberOfRows){
			L = rand()%(NumberOfCols*NumberOfRows-c);
			B = X->head;
			j = 0;
			k = 0;
			while(j <= L){
				if(B != NULL && k == X->n*B->i+B->j){
					B = B->next;
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
}
// Sliding function(s).
static void AddH(M** X, E** A, E** B){
	// Increment *B.v.
	(*B)->v            = (*A)->v+(*B)->v;
	// Detach and delete *A.
	(*A)->prev != NULL ? (*A)->prev->next = (*A)->next : ((*X)->head = (*A)->next);
	(*A)->next != NULL ? (*A)->next->prev = (*A)->prev : ((*X)->tail = (*A)->prev); free(*A); *A = (*B)->next;
}
static void AddV(M** X, E** A, E** B){
	// Increment *B.v.
	(*B)->v            = (*A)->v+(*B)->v;
	// Detach and delete *A.
	(*A)->prev != NULL ? (*A)->prev->next = (*A)->next : ((*X)->head = (*A)->next);
	(*A)->next != NULL ? (*A)->next->prev = (*A)->prev : ((*X)->tail = (*A)->prev); free(*A); *A = *B; *B = NULL;
}
static void ShiftN(M** X, E** A, E** B){
	// Detach *A.
	(*A)->prev != NULL ? (*A)->prev->next = (*A)->next : ((*X)->head = (*A)->next); // Redundant else statement.
	(*A)->next != NULL ? (*A)->next->prev = (*A)->prev : ((*X)->tail = (*A)->prev);
	// Re-attach *A (new *B.prev).
	(*A)->prev         = (*B)->prev;
	(*A)->next         = (*B);
	(*B)->prev != NULL ? (*B)->prev->next = (*A)       : ((*X)->head = (*A));
	(*B)->prev         = (*A);
}
static void ShiftS(M** X, E** A, E** B){
	// Detach *A.
	(*A)->prev != NULL ? (*A)->prev->next = (*A)->next : ((*X)->head = (*A)->next);
	(*A)->next != NULL ? (*A)->next->prev = (*A)->prev : ((*X)->tail = (*A)->prev); // Redundant else statement.
	// Re-attach *A (new *B.next).
	(*A)->prev         = (*B);
	(*A)->next         = (*B)->next;
	(*B)->next != NULL ? (*B)->next->prev = (*A)       : ((*X)->tail = (*A));
	(*B)->next         = (*A);
}
static void SlideN(M* X){
	if(X->head == NULL){
		exit(1);
	}
	E* A = X->head;
	E* B = NULL;
	E* C = NULL;
	E* D = NULL;
	// For each column....
	for(int j = 0; j < X->n; j++){
		// Set *A to the first element (or node) of column j.
		A = X->head;
		while(A != NULL && A->j != j){
			A = A->next;
		}
		// If empty (column), continue...
		if(A == NULL){
			continue;
		}
		B = A;
		C = NULL;
		D = NULL;
		for(int i = 0; i < X->m; i++){
			// Set *B to the "next" element (or node) of column j.
			while(B->next != NULL && !(B->i >= i && B->j == j)){
				B = B->next;
			}
			// If *B.j=j...
			if(B->j == j){
				if(C != NULL && B->v == C->v){
					if(B != C){
						AddV(&X,&B,&C);
						i--;
					}
				}
				else{
					D = B;
					while(D->prev != NULL && X->n*D->prev->i+D->prev->j > X->n*i+j){
						D = D->prev;
					}
					if(B != D){
						ShiftN(&X,&B,&D);
					}
					B->i = i;
					C    = B;
					D    = NULL;
				}
			}
			// Else, break...
			else{
				break;
			}
		}
	}
}
static void SlideS(M* X){
	if(X->head == NULL){
		exit(1);
	}
	E* A = X->tail;
	E* B = NULL;
	E* C = NULL;
	E* D = NULL;
	// For each column....
	for(int j = 0; j < X->n; j++){
		// Set *A to the last element (or node) of column j.
		A = X->tail;
		while(A != NULL && A->j != j){
			A = A->prev;
		}
		// If empty (column), continue...
		if(A == NULL){
			continue;
		}
		B = A;
		C = NULL;
		D = NULL;
		for(int i = 0; i < X->m; i++){
			// Set *B to the "previous" element (or node) of column j.
			while(B->prev != NULL && !(B->i <= X->m-1-i && B->j == j)){
				B = B->prev;
			}
			// If *B.j=j...
			if(B->j == j){
				if(C != NULL && B->v == C->v){
					if(B != C){
						AddV(&X,&B,&C);
						i--;
					}
				}
				else{
					D = B;
					while(D->next != NULL && X->n*D->next->i+D->next->j < X->n*(X->m-1-i)+j){
						D = D->next;
					}
					if(B != D){
						ShiftS(&X,&B,&D);
					}
					B->i = X->m-1-i;
					C    = B;
					D    = NULL;
				}
			}
			// Else, break...
			else{
				break;
			}
		}
	}
}
static void SlideE(M* X){
	if(X->head == NULL){
		exit(1);
	}
	if(X->head->next != NULL){
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
	int i = X->m-1;
	int j = X->n-1;
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
	if(X->head->next != NULL){
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
	int i = 0;
	int j = 0;
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
    SDL_Delay(0100);
}
void gLoop(M* X){
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
	    	// Verify...
	        assert(MAXT >= 0 && MAXT < NumberOfRows*NumberOfCols);



	        int k = 0;
	        for(k = 0; k < 25; k++){
	        	if(k == 0){
	        		InitMove(X,2);
	        	}
	        	else{
	        		InitMove(X,1);
	        	}
	        	SDL_Print(X,Font);
	        	Slide    (X,rand()%4);
	        	SDL_Print(X,Font);
	       }
	        TTF_CloseFont    (Font);
	        Font = NULL;
	        SDL_Close(X);
	    }
	}
}
