#define  length(v) sizeof(v)/sizeof(v[0])
#define  MaximumValue  2048
#define  NumberColumns    4
#define  NumberRows       4
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
}M;

void InsertE(M*, int, int, int);
void CreateM(M*, int, int, int);
void Slide  (M*, char);
void Print  (M);
