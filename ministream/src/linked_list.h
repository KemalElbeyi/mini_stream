#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "ministream.h"

/* Linked list üzerinde arama yapar - O(n) */
Sarki* sarki_ara_liste(Sarki* bas, int id);

/* Benchmark için N adet sahte şarkı üretir ve bağlı liste olarak döndürür */
Sarki* veri_uret_liste(int n);

/* Bağlı listedeki tüm şarkıları temizler */
void liste_temizle_hepsi(Sarki* bas);

#endif