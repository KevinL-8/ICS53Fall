#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

int main(){
    srand(time(0));
    for(int i=0; i<10; ++i)
    {
        int x = rand() % 10;
        int y = rand() % 10;
        int comb = x * x + y * y;
        int z = ceil(sqrt(comb));
        char stars[z+1];
        for(int a=0; a < z; ++a)
        {
            stars[a] = '*';
        }
        stars[z] = '\0';
        printf("(%d, %d): %d %s\n", x, y, z, stars);
    }
    return 0;
}