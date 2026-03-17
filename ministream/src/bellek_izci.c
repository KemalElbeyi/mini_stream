#include "bellek_izci.h"
#include <stdio.h>

/* ============================================================
   Static izleyici — sadece bu dosyadan erişilebilir.
   Dışarıdan getter fonksiyonlarını kullan!
   ============================================================ */
typedef struct {
    long malloc_sayisi;
    long free_sayisi;
    long toplam_malloc_byte;
    long toplam_free_byte;
} BellekIzci;

static BellekIzci izci = {0, 0, 0, 0};

void* izlenen_malloc(size_t boyut) {
    void* ptr = malloc(boyut);
    if (ptr) {
        izci.malloc_sayisi++;
        izci.toplam_malloc_byte += (long)boyut;
    }
    return ptr;
}

void izlened_free(void* ptr, size_t boyut) {
    if (ptr) {
        izci.free_sayisi++;
        izci.toplam_free_byte += (long)boyut;
        free(ptr);
    }
}

void bellek_raporu_yazdir(void) {
    long aktif   = izci.toplam_malloc_byte - izci.toplam_free_byte;
    long sizinti = aktif; /* henüz free edilmemiş = sızıntı adayı */
    printf("=== BELLEK RAPORU ===\n");
    printf("malloc : %4ld kez,  %8ld byte\n",
           izci.malloc_sayisi, izci.toplam_malloc_byte);
    printf("free   : %4ld kez,  %8ld byte\n",
           izci.free_sayisi,   izci.toplam_free_byte);
    printf("aktif  :             %8ld byte (%.2f MB)\n",
           aktif, aktif / 1048576.0);
    if (sizinti > 0)
        printf("SIZINTI: %ld byte ⚠\n", sizinti);
    else
        printf("SIZINTI: 0 byte ✓\n");
    printf("====================\n");
}

/* --- Getter'lar --- */
long izci_malloc_sayisi(void)       { return izci.malloc_sayisi; }
long izci_free_sayisi(void)         { return izci.free_sayisi; }
long izci_toplam_malloc_byte(void)  { return izci.toplam_malloc_byte; }
long izci_toplam_free_byte(void)    { return izci.toplam_free_byte; }
long izci_aktif_byte(void)          { return izci.toplam_malloc_byte - izci.toplam_free_byte; }