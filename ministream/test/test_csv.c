#include <stdio.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"

int main(void) {
    Sarki* bas = NULL;
    int n = csv_yukle("data/sarkilar.csv", &bas, 1000);
    if (n < 0) return 1;

    bellek_raporu_yazdir();

    return 0;
}
