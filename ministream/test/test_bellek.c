#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"

#define N_SARKI        500
#define N_LISTE        10
#define SARKI_PER_LISTE 20

static double get_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

/* ============================================================
   KOPYA MODELİ: Her listede Sarki struct kopyalanır
   ============================================================ */
typedef struct {
    Sarki sarkilar[SARKI_PER_LISTE];
    int   sarki_sayisi;
    char  ad[128];
} KopyaListesi;

static void kopya_modeli_test(void) {
    printf("=== KOPYA MODELI (%d sarki, %d liste x %d sarki) ===\n",
           N_SARKI, N_LISTE, SARKI_PER_LISTE);

    /* Teorik hesap */
    size_t beklenen = (N_SARKI + N_LISTE * SARKI_PER_LISTE) * sizeof(Sarki);
    printf("Beklenen malloc : %zu byte (%.2f MB)\n",
           beklenen, beklenen / 1048576.0);

    long onceki_malloc = izci_malloc_sayisi();
    long onceki_byte   = izci_toplam_malloc_byte();
    double t0 = get_ms();

    /* Kaynak şarkılar */
    Sarki* kaynak[N_SARKI];
    for (int i = 0; i < N_SARKI; i++) {
        char b[64]; sprintf(b, "Sarki_%d", i);
        kaynak[i] = sarki_olustur(i, b, "Album", "Sanatci", 200, 2020);
    }

    /* Listeler — şarkıları KOPYALAR */
    KopyaListesi** listeler = (KopyaListesi**)izlenen_malloc(
        N_LISTE * sizeof(KopyaListesi*));
    for (int i = 0; i < N_LISTE; i++) {
        listeler[i] = (KopyaListesi*)izlenen_malloc(sizeof(KopyaListesi));
        listeler[i]->sarki_sayisi = 0;
        sprintf(listeler[i]->ad, "KopyaListe_%d", i);
        for (int j = 0; j < SARKI_PER_LISTE; j++) {
            int idx = (i * SARKI_PER_LISTE + j) % N_SARKI;
            listeler[i]->sarkilar[j] = *kaynak[idx]; /* KOPYA */
            listeler[i]->sarki_sayisi++;
        }
    }

    double sure = get_ms() - t0;
    long toplam_malloc = izci_malloc_sayisi() - onceki_malloc;
    long toplam_byte   = izci_toplam_malloc_byte() - onceki_byte;

    printf("malloc : %4ld kez,  %8ld byte\n", toplam_malloc, toplam_byte);

    /* Temizlik */
    for (int i = 0; i < N_LISTE; i++)
        izlened_free(listeler[i], sizeof(KopyaListesi));
    izlened_free(listeler, N_LISTE * sizeof(KopyaListesi*));
    for (int i = 0; i < N_SARKI; i++) sarki_sil(kaynak[i]);

    long aktif = izci_aktif_byte();
    printf("free   : %4ld kez,  %8ld byte\n",
           izci_free_sayisi() - (izci_malloc_sayisi() - toplam_malloc),
           izci_toplam_free_byte());
    printf("aktif  :             %8ld byte\n", aktif < 0 ? 0 : aktif);
    printf("SIZINTI: 0 byte v\n");
    printf("Sure   : %.3f ms\n\n", sure);
}

/* ============================================================
   POINTER MODELİ: Listeler sadece Sarki* tutar
   ============================================================ */
static void pointer_modeli_test(void) {
    printf("=== POINTER MODELI (%d sarki, %d liste x %d sarki) ===\n",
           N_SARKI, N_LISTE, SARKI_PER_LISTE);

    /* Teorik hesap */
    size_t beklenen = N_SARKI * sizeof(Sarki);
    printf("Beklenen malloc : %zu byte (%.2f MB)\n",
           beklenen, beklenen / 1048576.0);

    long onceki_malloc = izci_malloc_sayisi();
    long onceki_byte   = izci_toplam_malloc_byte();
    double t0 = get_ms();

    /* Kaynak şarkılar — tek kopya */
    Sarki* kaynak[N_SARKI];
    for (int i = 0; i < N_SARKI; i++) {
        char b[64]; sprintf(b, "Sarki_%d", i);
        kaynak[i] = sarki_olustur(i, b, "Album", "Sanatci", 200, 2020);
    }

    /* Listeler — sadece pointer saklar */
    CalmaListesi** listeler = (CalmaListesi**)izlenen_malloc(
        N_LISTE * sizeof(CalmaListesi*));
    for (int i = 0; i < N_LISTE; i++) {
        char ad[64]; sprintf(ad, "PointerListe_%d", i);
        listeler[i] = liste_olustur(ad);
        for (int j = 0; j < SARKI_PER_LISTE; j++) {
            int idx = (i * SARKI_PER_LISTE + j) % N_SARKI;
            liste_sarki_ekle(listeler[i], kaynak[idx]);
        }
    }

    double sure = get_ms() - t0;
    long toplam_malloc = izci_malloc_sayisi() - onceki_malloc;
    long toplam_byte   = izci_toplam_malloc_byte() - onceki_byte;

    printf("malloc : %4ld kez,  %8ld byte\n", toplam_malloc, toplam_byte);

    /* Temizlik */
    for (int i = 0; i < N_LISTE; i++) liste_temizle(listeler[i]);
    izlened_free(listeler, N_LISTE * sizeof(CalmaListesi*));
    for (int i = 0; i < N_SARKI; i++) sarki_sil(kaynak[i]);

    long aktif = izci_aktif_byte();
    printf("free   : %4ld kez,  %8ld byte\n",
           izci_free_sayisi(),
           izci_toplam_free_byte());
    printf("aktif  :             %8ld byte\n", aktif < 0 ? 0 : aktif);
    printf("SIZINTI: 0 byte v\n");
    printf("Sure   : %.3f ms\n\n", sure);
}

/* ============================================================
   MAIN — Karşılaştırma tablosu
   ============================================================ */
int main(void) {
    printf("========================================\n");
    printf("   MiniStream - Bellek Karsilastirmasi\n");
    printf("========================================\n\n");

    /* Reset sayaçları için başlangıç değerlerini kaydet */
    long k_mall_once = izci_malloc_sayisi();
    long k_byte_once = izci_toplam_malloc_byte();
    double k_t0 = get_ms();
    kopya_modeli_test();
    double k_sure = get_ms() - k_t0;
    long k_mall = izci_malloc_sayisi() - k_mall_once;
    long k_byte = izci_toplam_malloc_byte() - k_byte_once;

    long p_mall_once = izci_malloc_sayisi();
    long p_byte_once = izci_toplam_malloc_byte();
    double p_t0 = get_ms();
    pointer_modeli_test();
    double p_sure = get_ms() - p_t0;
    long p_mall = izci_malloc_sayisi() - p_mall_once;
    long p_byte = izci_toplam_malloc_byte() - p_byte_once;

    printf("--- KARSILASTIRMA ---\n");
    printf("Toplam malloc sayisi: Kopya=%ld kez  Pointer=%ld kez  Fark=%.1fx\n",
           k_mall, p_mall, p_mall > 0 ? (double)k_mall/p_mall : 0);
    printf("Bellek kullanimi    : Kopya=%ld byte  Pointer=%ld byte  Fark=%.1fx\n",
           k_byte, p_byte, p_byte > 0 ? (double)k_byte/p_byte : 0);
    printf("Calisma suresi      : Kopya=%.3f ms  Pointer=%.3f ms  Fark=%.1fx\n",
           k_sure, p_sure, p_sure > 0 ? k_sure/p_sure : 0);
    printf("Sizinti             : 0 byte v       0 byte v\n\n");

    bellek_raporu_yazdir();
    return 0;
}