#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "ministream.h"

/* ============================================================
   GÜN 3-4: İki Farklı Arama Yapısı
   ============================================================ */

/* --- Linked List ile O(n) arama --- */
Sarki* sarki_ara_liste(Sarki* bas, int id);

/* --- Hash Map ile O(1) ortalama arama --- */
#define TABLO_BOYUTU 1024

typedef struct {
    Sarki* kovalar[TABLO_BOYUTU];
} HashMap;

int     hash_fonk(int id);
HashMap* hashmap_olustur(void);
void     hashmap_ekle(HashMap* map, Sarki* sarki);
Sarki*   sarki_ara_map(HashMap* map, int id);
void     hashmap_temizle(HashMap* map);

#endif /* HASH_MAP_H */