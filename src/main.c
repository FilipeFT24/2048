#define  MaximumValue  2048
#define  NumberColumns 4
#define  NumberRows    4
#define  length(v) sizeof(v)/sizeof(v[0])
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Element{
	int i;
	int j;
	int v;
	struct Element* prev;
	struct Element* next;
};
struct Matrix{
	bool VMoves[4];
	int  MValue;
	int  MCount;
	int  Score;
	int  m;
	int  n;
	struct Element* head;
	struct Element* tail;
};
void InsertE(struct Matrix*, int, int, int);
void CreateM(struct Matrix*, int, int, int);
void Slide  (struct Matrix*, char);
void Print  (struct Matrix);

int main(void){
	struct Matrix M;
	CreateM(&M,MaximumValue,NumberRows,NumberColumns);

	InsertE(&M,1,0,4);
	InsertE(&M,1,1,2);
	InsertE(&M,1,2,4);
	InsertE(&M,1,3,2);

	Print  ( M);
}

// 'Element' structure function(s).
static struct Element* CreateE(int i, int j, int v){
	// Allocate node (or element).
	struct Element* newE = (struct Element*)malloc(sizeof(struct Element));
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
void InsertE(struct Matrix* M, int i, int j, int v){
	// Create node (or element).
	struct Element* newE = CreateE(i,j,v);
	// Check...
	assert(newE->i >= 0 && newE->i < M->m);
	assert(newE->j >= 0 && newE->j < M->n);
	// If empty...
	if(M->head == NULL){
		M->head = newE;
		M->tail = newE;
	}
	// Else..
	else{
		struct Element* tmpA = M->head,
				      * tmpB = NULL,
                      * tmpC = M->head;
		while(tmpA != NULL && (M->n*tmpA->i+tmpA->j <= M->n*newE->i+newE->j)){
			tmpC = tmpA;
			tmpB = tmpA;
			tmpA = tmpA->next;
		}
		if(tmpC->i == newE->i && tmpC->j == newE->j){
			tmpC->v = newE->v;
			free(newE);
		}
		else{
			newE->prev = tmpB; tmpB != NULL ? tmpB->next = newE : (M->head = newE);
			newE->next = tmpA; tmpA != NULL ? tmpA->prev = newE : (M->tail = newE);
		}
	}
}
// 'Matrix' structure function(s).
void CreateM(struct Matrix* M, int MValue, int m, int n){
	// Assign fields.
	for(register int i = 0; i < sizeof(M->VMoves)/sizeof(M->VMoves[0]); i++){
		M->VMoves[i] = false;
	}
	M->MValue = MValue;
	M->MCount = 0;
	M->Score  = 0;
	M->m      = m;
	M->n      = n;
	M->head   = NULL;
	M->tail   = NULL;
}
// Sliding function(s).
static void AddH  (struct Matrix** M, struct Element** tmpA, struct Element** tmpB){
	// Increment *tmpB.v.
	(*tmpB)->v            = (*tmpA)->v+(*tmpB)->v;
	// Detach and delete *tmpA.
	(*tmpA)->prev != NULL ? (*tmpA)->prev->next = (*tmpA)->next : ((*M)->head = (*tmpA)->next);
	(*tmpA)->next != NULL ? (*tmpA)->next->prev = (*tmpA)->prev : ((*M)->tail = (*tmpA)->prev); free(*tmpA); *tmpA = (*tmpB)->next;
}
static void AddV  (struct Matrix** M, struct Element** tmpA, struct Element** tmpB){
	// Increment *tmpB.v.
	(*tmpB)->v            = (*tmpA)->v+(*tmpB)->v;
	// Detach and delete *tmpA.
	(*tmpA)->prev != NULL ? (*tmpA)->prev->next = (*tmpA)->next : ((*M)->head = (*tmpA)->next);
	(*tmpA)->next != NULL ? (*tmpA)->next->prev = (*tmpA)->prev : ((*M)->tail = (*tmpA)->prev); free(*tmpA); *tmpA = *tmpB;
																											 *tmpB =  NULL;
}
static void ShiftN(struct Matrix** M, struct Element** tmpA, struct Element** tmpB){
	// Detach *tmpA.
	(*tmpA)->prev != NULL ? (*tmpA)->prev->next = (*tmpA)->next : ((*M)->head = (*tmpA)->next); // Redundant else statement.
	(*tmpA)->next != NULL ? (*tmpA)->next->prev = (*tmpA)->prev : ((*M)->tail = (*tmpA)->prev);
	// Re-attach *tmpA (new *tmpB.prev).
	(*tmpA)->prev         = (*tmpB)->prev;
	(*tmpA)->next         = (*tmpB);
	(*tmpB)->prev != NULL ? (*tmpB)->prev->next = (*tmpA)       : ((*M)->head = (*tmpA));
	(*tmpB)->prev         = (*tmpA);
}
static void ShiftS(struct Matrix** M, struct Element** tmpA, struct Element** tmpB){
	// Detach *tmpA.
	(*tmpA)->prev != NULL ? (*tmpA)->prev->next = (*tmpA)->next : ((*M)->head = (*tmpA)->next);
	(*tmpA)->next != NULL ? (*tmpA)->next->prev = (*tmpA)->prev : ((*M)->tail = (*tmpA)->prev); // Redundant else statement.
	// Re-attach *tmpA (new *tmpB.next).
	(*tmpA)->prev         = (*tmpB);
	(*tmpA)->next         = (*tmpB)->next;
	(*tmpB)->next != NULL ? (*tmpB)->next->prev = (*tmpA)       : ((*M)->tail = (*tmpA));
	(*tmpB)->next         = (*tmpA);
}
static void SlideN(struct Matrix* M){
	if(M->head == NULL){
		return;
	}
	register int i = 0,
		         j = 0;
	struct Element* tmpA = M->head,
			      * tmpB = NULL,
			      * tmpC = NULL,
				  * tmpD = NULL;
	// For each column....
	for(; j < M->n; j++){
		// Set *tmpA to the first element (or node) of column j.
		tmpA = M->head;
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
		for(i = 0; i < M->m; i++){
			// Set *tmpB to the "next" element (or node) of column j.
			while(tmpB->next != NULL && !(tmpB->i >= i && tmpB->j == j)){
				tmpB = tmpB->next;
			}
			// If *tmpB.j=j...
			if(tmpB->j == j){
				if(tmpC != NULL && tmpB->v == tmpC->v){
					AddV(&M,&tmpB,&tmpC);
					i--;
				}
				else{
					tmpD = tmpB;
					while(tmpD->prev != NULL && (M->n*tmpD->prev->i+tmpD->prev->j > M->n*i+j)){
						tmpD = tmpD->prev;
					}
					if(tmpB != tmpD){
						ShiftN(&M,&tmpB,&tmpD);
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
static void SlideS(struct Matrix* M){
	if(M->head == NULL){
		return;
	}
	register int i = 0,
		         j = 0;
	struct Element* tmpA = M->tail,
			      * tmpB = NULL,
			      * tmpC = NULL,
				  * tmpD = NULL;
	// For each column....
	for(; j < M->n; j++){
		// Set *tmpA to the last element (or node) of column j.
		tmpA = M->tail;
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
		for(i = 0; i < M->m; i++){
			// Set *tmpB to the "previous" element (or node) of column j.
			while(tmpB->prev != NULL && !(tmpB->i <= M->m-1-i && tmpB->j == j)){
				tmpB = tmpB->prev;
			}
			// If *tmpB.j=j...
			if(tmpB->j == j){
				if(tmpC != NULL && tmpB->v == tmpC->v){
					AddV(&M,&tmpB,&tmpC);
					i--;
				}
				else{
					tmpD = tmpB;
					while(tmpD->next != NULL && (M->n*tmpD->next->i+tmpD->next->j < M->n*(M->m-1-i)+j)){
						tmpD = tmpD->next;
					}
					if(tmpB != tmpD){
						ShiftS(&M,&tmpB,&tmpD);
					}
					tmpB->i = M->m-1-i;
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
static void SlideE(struct Matrix* M){
	if(M->head == NULL){
		return;
	}
	if(M->head->next != NULL){
		struct Element* tmpA = M->head,
			          * tmpB = NULL;
		while(tmpA->next != NULL){
			tmpB = tmpA;
			tmpA = tmpA->next;
			if(tmpA->i == tmpB->i && tmpA->v == tmpB->v){
				AddH(&M,&tmpA,&tmpB);
			}
		}
	}
	register int i = M->m-1,
		         j = M->n-1;
	struct Element* tmpC = M->tail;
	while(tmpC != NULL){
		if(tmpC->i == i){
			if(tmpC->j < j){
				tmpC->j = j;
			}
			tmpC = tmpC->prev;
			j--;
		}
		else{
			i--;
			j = M->n-1;
		}
	}
}
static void SlideW(struct Matrix* M){
	if(M->head == NULL){
		return;
	}
	if(M->head->next != NULL){
		struct Element* tmpA = M->head,
			          * tmpB = NULL;
		while(tmpA->next != NULL){
			tmpB = tmpA;
			tmpA = tmpA->next;
			if(tmpA->i == tmpB->i && tmpA->v == tmpB->v){
				AddH(&M,&tmpA,&tmpB);
			}
		}
	}
	register int i = 0,
		         j = 0;
	struct Element* tmpC = M->head;
	while(tmpC != NULL){
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
void Slide(struct Matrix* M, char D){
	switch(D){
		case 'N': SlideN(M); // Up.
			break;
		case 'S': SlideS(M); // Down.
			break;
		case 'E': SlideE(M); // To the right.
			break;
		case 'W': SlideW(M); // To the left.
			break;
		default:
			return;
	}
}
// Graphics.
void Print(struct Matrix M){
	struct Element* tmp = M.head;
	for(int i = 0; i < M.m; i++){
		for(int j = 0; j < M.n; j++){
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
