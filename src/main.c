#include "../include/obj.h"

int main(void){
    M X;
    CreateM(&X,MaximumValue,NumberOfRows,NumberOfCols);

    srand(time(NULL));

    GLoop  (&X);
    return 0;
}
