#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"
#include "../src/hash_map.h"

#define N_SORGU 1000

static double ms_cevir(struct timespec* bas, struct timespec* son) {
    return (son->tv_sec - bas->tv_sec) * 1000.0
         + (son->tv_nsec - bas->tv_nsec) / 1e6;
}

static void benchmark_arama(int n_sarki) {
    /* Şarkıları dizide tut */
    Sarki** dizi = (Sarki**)malloc(n_sarki * sizeof(Sarki*));
    for (int i = 0; i < n_sarki; i++) {
        char b[64]; sprintf(b, "Sarki_%d", i);
        dizi[i] = sarki_olustur(i, b, "Album", "X", 180, 2020);
        dizi[i]->sonraki = NULL;
    }

    /* Rastgele arama ID'leri */
    int arama_idleri[N_SORGU];
    srand(42);
    for (int i = 0; i < N_SORGU; i++)
        arama_idleri[i] = rand() % n_sarki;

    /* --- LinkedList kur --- */
    for (int i = 0; i < n_sarki - 1; i++)
        dizi[i]->sonraki = dizi[i + 1];
    dizi[n_sarki - 1]->sonraki = NULL;
    Sarki* ll_bas = dizi[0];

    /* LinkedList zamanlama */
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    volatile int bulunan = 0;
    for (int i = 0; i < N_SORGU; i++) {
        Sarki* s = sarki_ara_liste(ll_bas, arama_idleri[i]);
        if (s) bulunan++;
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double ll_ms = ms_cevir(&t0, &t1);

    /* --- LinkedList bağını kopar, HashMap kur --- */
    for (int i = 0; i < n_sarki; i++)
        dizi[i]->sonraki = NULL;

    HashMap* map = hashmap_olustur();
    for (int i = 0; i < n_sarki; i++)
        hashmap_ekle(map, dizi[i]);

    /* HashMap zamanlama */
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N_SORGU; i++) {
        Sarki* s = sarki_ara_map(map, arama_idleri[i]);
        if (s) bulunan++;
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double hm_ms = ms_cevir(&t0, &t1);

    double fark = (hm_ms > 0) ? ll_ms / hm_ms : 0;
    printf("| %7d | %10.3f ms | %10.3f ms | %8.1fx |\n",
           n_sarki, ll_ms, hm_ms, fark);

    /* Temizlik */
    hashmap_temizle(map);
    for (int i = 0; i < n_sarki; i++) {
        dizi[i]->sonraki = NULL;
        sarki_sil(dizi[i]);
    }
    free(dizi);
}

int main(void) {
    printf("\n=== ARAMA BENCHMARK (%d sorgu) ===\n", N_SORGU);
    printf("| %7s | %12s | %12s | %9s |\n",
           "N sarki", "LinkedList", "HashMap", "Fark");
    printf("|---------|--------------|--------------|----------|\n");

    int boyutlar[] = {100, 1000, 10000, 100000};
    for (int i = 0; i < 4; i++)
        benchmark_arama(boyutlar[i]);

    printf("\n");
    bellek_raporu_yazdir();
    return 0;
}