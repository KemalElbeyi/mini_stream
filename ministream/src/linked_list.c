#include <stdio.h>
#include "linked_list.h"
#include "bellek_izci.h"

Sarki* sarki_ara_liste(Sarki* bas, int id) {
    Sarki* curr = bas;
    while (curr != NULL) {
        if (curr->id == id) {
            return curr;
        }
        curr = curr->sonraki;
    }
    return NULL;
}

Sarki* veri_uret_liste(int n) {
    Sarki* bas = NULL;
    for (int i = n - 1; i >= 0; i--) {
        Sarki* s = sarki_olustur(i, "Test Sarki", "Sanatci", "Album", 180);
        if (s) {
            s->sonraki = bas;
            bas = s;
        }
    }
    return bas;
}

void liste_temizle_hepsi(Sarki* bas) {
    Sarki* curr = bas;
    while (curr != NULL) {
        Sarki* sonraki = curr->sonraki;
        izlened_free(curr, sizeof(Sarki));
        curr = sonraki;
    }
}