#include "hash_map.h"
#include "bellek_izci.h"
#include <stdio.h>

/* ============================================================
   Linked List ile O(n) arama
   ============================================================ */
Sarki* sarki_ara_liste(Sarki* bas, int id) {
    Sarki* curr = bas;
    while (curr != NULL) {
        if (curr->id == id) return curr;
        curr = curr->sonraki;
    }
    return NULL;
}

/* ============================================================
   Hash Map
   ============================================================ */
int hash_fonk(int id) {
    return id % TABLO_BOYUTU;
}

HashMap* hashmap_olustur(void) {
    HashMap* map = (HashMap*)izlenen_malloc(sizeof(HashMap));
    if (!map) return NULL;
    for (int i = 0; i < TABLO_BOYUTU; i++)
        map->kovalar[i] = NULL;
    return map;
}

/* Şarkıyı ilgili kovaya ekle (chaining: sonraki pointer kullanılır) */
void hashmap_ekle(HashMap* map, Sarki* sarki) {
    if (!map || !sarki) return;
    int idx = hash_fonk(sarki->id);
    /* Zincirin başına ekle */
    sarki->sonraki      = map->kovalar[idx];
    map->kovalar[idx]   = sarki;
}

Sarki* sarki_ara_map(HashMap* map, int id) {
    if (!map) return NULL;
    int idx = hash_fonk(id);
    return sarki_ara_liste(map->kovalar[idx], id);
}

/* HashMap struct'ını temizle (Sarki nesnelerine dokunmaz) */
void hashmap_temizle(HashMap* map) {
    if (!map) return;
    izlened_free(map, sizeof(HashMap));
}