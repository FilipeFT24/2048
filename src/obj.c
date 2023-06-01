#include "../include/obj.h"

// 'Element' structure function(s).
static E* CreateE(int i, int j, int v){
	// Allocate node (or element).
	E* newE = (E*)malloc(sizeof(E));
	// Assign fields.
	if(newE != NULL){
		newE->i    = i;
		newE->j    = j;
		newE->v    = v;
		newE->prev = NULL;
		newE->next = NULL;
	}
	return newE;
}
void InsertE(M* X, int i, int j, int v){
	// Create node (or element).
	E* newE = CreateE(i,j,v);
	// Check...
	assert(newE->i >= 0 && newE->i < X->m);
	assert(newE->j >= 0 && newE->j < X->n);
	// If empty...
	if(X->head == NULL){
		X->head = newE;
		X->tail = newE;
	}
	// Else..
	else{
		E* tmpA = X->head;
		E* tmpB = NULL;
		E* tmpC = X->head;
		while(tmpA != NULL && (X->n*tmpA->i+tmpA->j <= X->n*newE->i+newE->j)){
			tmpC = tmpA;
			tmpB = tmpA;
			tmpA = tmpA->next;
		}
		if(tmpC->i == newE->i && tmpC->j == newE->j){
			tmpC->v = newE->v;
			free(newE);
		}
		else{
			newE->prev = tmpB; tmpB != NULL ? tmpB->next = newE : (X->head = newE);
			newE->next = tmpA; tmpA != NULL ? tmpA->prev = newE : (X->tail = newE);
		}
	}
}

// 'Matrix' structure function(s).
void CreateM(M* X, int MValue, int m, int n){
	// Assign fields.
	for(register int i = 0; i < sizeof(X->VMoves)/sizeof(X->VMoves[0]); i++){
		X->VMoves[i] = false;
	}
	X->MValue = MValue;
	X->MCount = 0;
	X->Score  = 0;
	X->m      = m;
	X->n      = n;
	X->head   = NULL;
	X->tail   = NULL;
}

// Sliding function(s).
static void AddH  (M** X, E** tmpA, E** tmpB){
	// Increment *tmpB.v.
	(*tmpB)->v            = (*tmpA)->v+(*tmpB)->v;
	// Detach and delete *tmpA.
	(*tmpA)->prev != NULL ? (*tmpA)->prev->next = (*tmpA)->next : ((*X)->head = (*tmpA)->next);
	(*tmpA)->next != NULL ? (*tmpA)->next->prev = (*tmpA)->prev : ((*X)->tail = (*tmpA)->prev); free(*tmpA); *tmpA = (*tmpB)->next;
}
static void AddV  (M** X, E** tmpA, E** tmpB){
	// Increment *tmpB.v.
	(*tmpB)->v            = (*tmpA)->v+(*tmpB)->v;
	// Detach and delete *tmpA.
	(*tmpA)->prev != NULL ? (*tmpA)->prev->next = (*tmpA)->next : ((*X)->head = (*tmpA)->next);
	(*tmpA)->next != NULL ? (*tmpA)->next->prev = (*tmpA)->prev : ((*X)->tail = (*tmpA)->prev); free(*tmpA); *tmpA = *tmpB;
																											 *tmpB =  NULL;
}
static void ShiftN(M** X, E** tmpA, E** tmpB){
	// Detach *tmpA.
	(*tmpA)->prev != NULL ? (*tmpA)->prev->next = (*tmpA)->next : ((*X)->head = (*tmpA)->next); // Redundant else statement.
	(*tmpA)->next != NULL ? (*tmpA)->next->prev = (*tmpA)->prev : ((*X)->tail = (*tmpA)->prev);
	// Re-attach *tmpA (new *tmpB.prev).
	(*tmpA)->prev         = (*tmpB)->prev;
	(*tmpA)->next         = (*tmpB);
	(*tmpB)->prev != NULL ? (*tmpB)->prev->next = (*tmpA)       : ((*X)->head = (*tmpA));
	(*tmpB)->prev         = (*tmpA);
}
static void ShiftS(M** X, E** tmpA, E** tmpB){
	// Detach *tmpA.
	(*tmpA)->prev != NULL ? (*tmpA)->prev->next = (*tmpA)->next : ((*X)->head = (*tmpA)->next);
	(*tmpA)->next != NULL ? (*tmpA)->next->prev = (*tmpA)->prev : ((*X)->tail = (*tmpA)->prev); // Redundant else statement.
	// Re-attach *tmpA (new *tmpB.next).
	(*tmpA)->prev         = (*tmpB);
	(*tmpA)->next         = (*tmpB)->next;
	(*tmpB)->next != NULL ? (*tmpB)->next->prev = (*tmpA)       : ((*X)->tail = (*tmpA));
	(*tmpB)->next         = (*tmpA);
}
static void SlideN(M* X){
	if(X->head == NULL){
		return;
	}
	E* tmpA = X->head;
	E* tmpB = NULL;
	E* tmpC = NULL;
	E* tmpD = NULL;
	// For each column....
	for(register int j = 0; j < X->n; j++){
		// Set *tmpA to the first element (or node) of column j.
		tmpA = X->head;
		while(tmpA != NULL && tmpA->j != j){
			tmpA = tmpA->next;
		}
		// If empty (column), continue...
		if(tmpA == NULL){
			continue;
		}
		tmpB = tmpA;
		tmpC = NULL;
		tmpD = NULL;
		for(register int i = 0; i < X->m; i++){
			// Set *tmpB to the "next" element (or node) of column j.
			while(tmpB->next != NULL && !(tmpB->i >= i && tmpB->j == j)){
				tmpB = tmpB->next;
			}
			// If *tmpB.j=j...
			if(tmpB->j == j){
				if(tmpC != NULL && tmpB->v == tmpC->v){
					AddV(&X,&tmpB,&tmpC);
					i--;
				}
				else{
					tmpD = tmpB;
					while(tmpD->prev != NULL && (X->n*tmpD->prev->i+tmpD->prev->j > X->n*i+j)){
						tmpD = tmpD->prev;
					}
					if(tmpB != tmpD){
						ShiftN(&X,&tmpB,&tmpD);
					}
					tmpB->i = i;
					tmpC    = tmpB;
					tmpD    = NULL;
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
		return;
	}
	E* tmpA = X->tail;
	E* tmpB = NULL;
	E* tmpC = NULL;
	E* tmpD = NULL;
	// For each column....
	for(register int j = 0; j < X->n; j++){
		// Set *tmpA to the last element (or node) of column j.
		tmpA = X->tail;
		while(tmpA != NULL && tmpA->j != j){
			tmpA = tmpA->prev;
		}
		// If empty (column), continue...
		if(tmpA == NULL){
			continue;
		}
		tmpB = tmpA;
		tmpC = NULL;
		tmpD = NULL;
		for(register int i = 0; i < X->m; i++){
			// Set *tmpB to the "previous" element (or node) of column j.
			while(tmpB->prev != NULL && !(tmpB->i <= X->m-1-i && tmpB->j == j)){
				tmpB = tmpB->prev;
			}
			// If *tmpB.j=j...
			if(tmpB->j == j){
				if(tmpC != NULL && tmpB->v == tmpC->v){
					AddV(&X,&tmpB,&tmpC);
					i--;
				}
				else{
					tmpD = tmpB;
					while(tmpD->next != NULL && (X->n*tmpD->next->i+tmpD->next->j < X->n*(X->m-1-i)+j)){
						tmpD = tmpD->next;
					}
					if(tmpB != tmpD){
						ShiftS(&X,&tmpB,&tmpD);
					}
					tmpB->i = X->m-1-i;
					tmpC    = tmpB;
					tmpD    = NULL;
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
		return;
	}
	if(X->head->next != NULL){
		E* tmpA = X->head;
		E* tmpB = NULL;
		while(tmpA->next != NULL){
			tmpB = tmpA;
			tmpA = tmpA->next;
			if(tmpA->i == tmpB->i && tmpA->v == tmpB->v){
				AddH(&X,&tmpA,&tmpB);
			}
		}
	}
	register int i = X->m-1;
	register int j = X->n-1;
	for(E* tmpC = X->tail; tmpC != NULL;){
		if(tmpC->i == i){
			if(tmpC->j < j){
				tmpC->j = j;
			}
			tmpC = tmpC->prev;
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
		return;
	}
	if(X->head->next != NULL){
		E* tmpA = X->head;
		E* tmpB = NULL;
		while(tmpA->next != NULL){
			tmpB = tmpA;
			tmpA = tmpA->next;
			if(tmpA->i == tmpB->i && tmpA->v == tmpB->v){
				AddH(&X,&tmpA,&tmpB);
			}
		}
	}
	register int i = 0;
	register int j = 0;
	for(E* tmpC = X->head; tmpC != NULL;){
		if(tmpC->i == i){
			if(tmpC->j > j){
				tmpC->j = j;
			}
			tmpC = tmpC->next;
			j++;
		}
		else{
			i++;
			j = 0;
		}
	}
}
void Slide(M* X, char D){
	switch(D){
		case 'N': SlideN(X); // Up.
			break;
		case 'S': SlideS(X); // Down.
			break;
		case 'E': SlideE(X); // To the right.
			break;
		case 'W': SlideW(X); // To the left.
			break;
		default:
			return;
	}
}
