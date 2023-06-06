#include "../include/obj.h"

// 'Element' structure functions.
static E* CreateE(int i, int j, int v){
	E* N = (E*)malloc(sizeof(E));
	if(N != NULL){
		N->i    = i;
		N->j    = j;
		N->v    = v;
		N->prev = NULL;
		N->next = NULL;
	}
	assert(N->i >= 0 && N->i < NumberOfRows);
	assert(N->j >= 0 && N->j < NumberOfCols);
	return N;
}
static void InsertE(M* X, int i, int j, int v){
	E* N = CreateE(i,j,v);
	if(X->head == NULL){
		X->head = N;
		X->tail = N;
	}
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
	(*X)->MScore      += (*B)->v;
	// Detach and delete *A.
	(*A)->prev != NULL ? (*A)->prev->next = (*A)->next : ((*X)->head = (*A)->next);
	(*A)->next != NULL ? (*A)->next->prev = (*A)->prev : ((*X)->tail = (*A)->prev); free(*A); *A = (*B)->next;
	// Update (if appropriate).
	if((*X)->MValue < (*B)->v){
		(*X)->MValue = (*B)->v;
	}
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
	(*X)->MScore      += (*B)->v;
	// Detach and delete *A.
	(*A)->prev != NULL ? (*A)->prev->next = (*A)->next : ((*X)->head = (*A)->next);
	(*A)->next != NULL ? (*A)->next->prev = (*A)->prev : ((*X)->tail = (*A)->prev); free(*A); *A = (*B)->prev;
	// Update (if appropriate).
	if((*X)->MValue < (*B)->v){
		(*X)->MValue = (*B)->v;
	}
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
static void SDL_Clear(M* X){
	SDL_SetRenderDrawColor(X->sdlR,Bc[0].r,Bc[0].g,Bc[0].b,Bc[0].a);
    SDL_RenderClear       (X->sdlR);
}
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
					SDL_Clear(X); bflag = true;
				}
			}
		}
	}
	return bflag;
}
static void SDL_Close(M* X){
	TTF_CloseFont    (X->font); X->font = NULL;
	TTF_Quit();
	SDL_DestroyWindow(X->sdlW); X->sdlW = NULL;
	SDL_Quit();
}
static void SDL_Print(M* X){
	// Clear board.
	SDL_Clear(X);
	//


    char str[MAXSIZE];
    int k = 0;

    SDL_Rect R1, R2;



	E* A = X->head;
    for(register int i = 0; i < X->m; i++){
    	for(register int j = 0; j < X->n; j++){
    	    R1.x = SCREEN_PAD+j*(sizeW+SCREEN_PAD);
    	    R1.y = SCREEN_PAD+i*(sizeH+SCREEN_PAD);
    	    R1.w = sizeW;
    	    R1.h = sizeH;



    		if(A != NULL && (A->i == i && A->j == j)){
    			k = log2(A->v);
        	    SDL_SetRenderDrawColor(X->sdlR,TBc[k].r,TBc[k].g,TBc[k].b,TBc[k].a);
        	    SDL_RenderFillRect    (X->sdlR,&R1);


        	    sprintf(str,"%d",A->v);
        	    SDL_Surface* SM = TTF_RenderText_Blended      (X->font,str,TVc[k]);
        	    SDL_Texture*  M = SDL_CreateTextureFromSurface(X->sdlR,SM);
        	    A = A->next;

        	    TTF_SizeText      (X->font,str,&R2.w,&R2.h);

        	    R2.x = R1.x+R1.w/2-R2.w/2;
        	    R2.y = R1.y+R1.h/2-R2.h/2;

        	    SDL_RenderCopy    (X->sdlR,M,NULL,&R2);
        	    SDL_DestroyTexture( M);
        	    SDL_FreeSurface   (SM);
    		}
    		else{
        	    SDL_SetRenderDrawColor(X->sdlR,TBc[0].r,TBc[0].g,TBc[0].b,TBc[0].a);
        	    SDL_RenderFillRect    (X->sdlR,&R1);
    		}
    	}
    }
    SDL_RenderPresent(X->sdlR);
}
static void gLoop(M* X, int n){
	E* A = NULL;
	register int i = 0, j = 0, k = 0, l = 0;
	register int c = 0;
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
	X->Vgloop = false;
	for(i = 0; i < length(X->VMoves); i++){
		X->VMoves[i] = false;
	}
	ValidN(X);
	ValidS(X);
	ValidE(X);
	ValidW(X);
	for(i = 0; i < length(X->VMoves); i++){
		if(X->VMoves[i]){
			X->Vgloop = true;
			break;
		}
	}
	SDL_Print(X);
}
void GLoop(M* X, bool randM){
	if(!SDL_Open(X)){
		exit(0);
	}
	else{
		X->font = TTF_OpenFont("ClearSans-Bold.ttf",50);
	    if(X->font == NULL){
	    	printf("Couldn't initialize SDL TTF: %s\n",SDL_GetError()); exit(1);
	    }
	    else{
	        gLoop(X,2);
			for(E* A = X->head; A != NULL; A = A->next){
				if(A->v > X->MValue){
					X->MValue = A->v;
				}
			}
	        if(!randM){
	        	register  bool Key  = false;
	        	register  bool Quit = !X->Vgloop;
	        	SDL_Event EV;
	        	while(!Quit && X->MValue < MaximumValue){
	        		while(SDL_PollEvent(&EV)){
	        			switch(EV.type){
	        				case SDL_QUIT:
	        					Quit = true;
	        					break;
	        				case SDL_KEYUP:
	        					Key = false;
		        				switch(EV.key.keysym.sym){
		        					case SDLK_UP:
		        	        			Key = X->VMoves[0];
		        	        			if(Key){
		        	        				SlideN(X);
		        	        			}
		        						break;
		        					case SDLK_DOWN:
		        	        			Key = X->VMoves[1];
		        	        			if(Key){
		        	        				SlideS(X);
		        	        			}
		        						break;
		        					case SDLK_RIGHT:
		        	        			Key = X->VMoves[2];
		        	        			if(Key){
		        	        				SlideE(X);
		        	        			}
		        						break;
		        					case SDLK_LEFT:
		        	        			Key = X->VMoves[3];
		        	        			if(Key){
		        	        				SlideW(X);
		        	        			}
		        						break;
		        					default:;
		        				}
			        			if(Key){
			        				gLoop(X,1);
			        				if(!X->Vgloop){
			        					Quit = true;
			        					break;
			        				}
			        			}
	        					break;
	        				default:
	        					break;
	        			}
	        		}
	        	}
	        }
	        else{
	        	register int c = 0;
	        	register int i = 0, j = 0, k = 0;
	        	while(X->Vgloop && X->MValue < MaximumValue){
        			c = 0;
        			i = 0, j = 0, k = 0;
        			for(i = 0; i < length(X->VMoves); i++){
        				if(X->VMoves[i]){
        					c++;
        				}
        			}
        			k = rand()%c;
        			for(i = 0; i < length(X->VMoves) && j < k+1; i++){
        				if(X->VMoves[i]){
        					j++;
        				}
        			}
        			i--;
        			switch(i){
    	        		case 0:
    	        			SlideN(X);
    	        			break;
    	        		case 1:
    	        			SlideS(X);
    	        			break;
    	        		case 2:
    	        			SlideE(X);
    	        			break;
    	        		case 3:
    	        			SlideW(X);
    	        			break;
    	        		default:
    	        			break;
        			}
        			gLoop(X,1); SDL_Delay(0125);
	        	}
	        }
	        SDL_Close(X);
	    }
	}
}
