#include "../include/obj.h"

int main(void){
	M X;
	X.Vgloop = false;
	for(register int i = 0; i < length(X.VMoves); i++){
		X.VMoves[i] = false;
	}
	X.MValue = 0;
	X.MScore = 0;
	X.m      = NumberOfRows;
	X.n      = NumberOfCols;
	X.head   = NULL;
	X.tail   = NULL;
	X.font   = NULL;
	X.sdlR   = NULL;
	X.sdlW   = NULL;

    srand(time(NULL));

    bool randMovement = false;

    GLoop  (&X,randMovement);
    return 0;
}
