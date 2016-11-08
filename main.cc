#include <stdlib.h>
#include "aggregator.h"

int main(int argc, char *argv[])
{
    // argv[1]: Maximum of in-memory records at any time
    // argv[2]: Input file
    int max = 10; // Default maximum is 10 records
    char *fname = NULL;
    if (argc >= 2) {
        max = atoi(argv[1]);
    }
    if (argc == 3) {
        fname = argv[2];
    }

    Aggregator *a = new Aggregator(max);
    if (fname) {
        a->injest(fname);
    } else {
        a->injest("input.txt");
    }
    return 0;
}
