#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    for (int i=0; i<1000000; i++)
    {
        int j = rand() % 230;
        printf("k%d v%d\n", j, j);
    }
}
