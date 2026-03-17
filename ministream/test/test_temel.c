#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"
#include "../src/hash_map.h"

static int gecen = 0, kalan = 0;

#define ASSERT(kosul, mesaj)                                        \
    do {                                                            \
        if (kosul) { printf("  v %s\n", mesaj); gecen++; }         \
        else       { printf("  X %s  [HATA]\n", mesaj); kalan++; } \
    } while(0)

static void test_sarki_olustur(void) {
    printf("\n[TEST] sarki_olustur\n");
    Sarki* s = sarki_olustur(1, "Bohemian Rhapsody", "A Night at the Opera",
                               "Queen", 354, 1975);
    ASSERT(s != NULL,            "sarki NULL degil");
    ASSERT(s->id == 1,           "id dogru");
    ASSERT(strcmp(s->baslik, "Bohemian Rhapsody") == 0, "baslik dogru");
    ASSERT(s->sure_sn == 354,    "sure_sn dogru");
    ASSERT(s->yil == 1975,       "yil dogru");
    ASSERT(s->ref_sayisi == 0,   "ref_sayisi baslangicta 0");
    ASSERT(izci_malloc_sayisi() >= 1, "1 malloc yapildi");
    sarki_sil(s);
    ASSERT(izci_aktif_byte() <= 0, "SIZINTI yok");
}

static void test_liste_sarki_ekle(void) {
    printf("\n[TEST] liste_sarki_ekle\n");

    Sarki* s1 = sarki_olustur(10, "Sarki1", "Album1", "Sanatci1", 180, 2020);
    Sarki* s2 = sarki_olustur(11, "Sarki2", "Album2", "Sanatci2", 200, 2021);
    CalmaListesi* l = liste_olustur("Test Listesi");

    liste_sarki_ekle(l, s1);
    liste_sarki_ekle(l, s2);
    ASSERT(l->sarki_sayisi == 2,  "2 sarki eklendi");
    ASSERT(s1->ref_sayisi == 1,   "s1 ref_sayisi artti");
    ASSERT(s2->ref_sayisi == 1,   "s2 ref_sayisi artti");
    ASSERT(l->sarkilar[0] == s1,  "pointer kopyalandi");

    liste_temizle(l);
    sarki_sil(s1);
    sarki_sil(s2);
    ASSERT(izci_aktif_byte() <= 0, "SIZINTI yok");
}

static void test_ref_sayisi(void) {
    printf("\n[TEST] ref_sayisi\n");

    Sarki* s  = sarki_olustur(2, "Test Sarki", "Test Album", "Test", 200, 2020);
    CalmaListesi* l1 = liste_olustur("Liste1");
    CalmaListesi* l2 = liste_olustur("Liste2");

    liste_sarki_ekle(l1, s);
    liste_sarki_ekle(l2, s);
    ASSERT(s->ref_sayisi == 2, "2 listede ref_sayisi == 2");

    liste_sarki_cikar(l1, 0);
    ASSERT(s->ref_sayisi == 1, "1 cikarinca ref_sayisi == 1");

    int sonuc = sarki_sil(s);
    ASSERT(sonuc == -1, "ref_sayisi>0 iken silinemez");

    liste_sarki_cikar(l2, 0);
    ASSERT(s->ref_sayisi == 0, "l2 bosaltinca ref_sayisi == 0");

    sonuc = sarki_sil(s);
    ASSERT(sonuc == 0, "ref_sayisi==0 iken silindi");

    izlened_free(l1->sarkilar, l1->kapasite * sizeof(Sarki*));
    izlened_free(l1, sizeof(CalmaListesi));
    izlened_free(l2->sarkilar, l2->kapasite * sizeof(Sarki*));
    izlened_free(l2, sizeof(CalmaListesi));

    ASSERT(izci_aktif_byte() <= 0, "SIZINTI yok");
}

static void test_liste(void) {
    printf("\n[TEST] liste_olustur / liste_temizle\n");

    CalmaListesi* l = liste_olustur("Sabah Listesi");
    ASSERT(l != NULL,                         "liste olusturuldu");
    ASSERT(l->sarki_sayisi == 0,              "basta sarki_sayisi 0");
    ASSERT(l->kapasite == BASLANGIC_KAPASITE, "kapasite BASLANGIC_KAPASITE");

    Sarki* s1 = sarki_olustur(10, "S1", "A1", "S1", 100, 2000);
    Sarki* s2 = sarki_olustur(11, "S2", "A2", "S2", 200, 2001);

    liste_sarki_ekle(l, s1);
    liste_sarki_ekle(l, s2);
    ASSERT(l->sarki_sayisi == 2, "2 sarki eklendi");
    ASSERT(s1->ref_sayisi == 1,  "s1 ref_sayisi == 1");

    liste_temizle(l);
    ASSERT(s1->ref_sayisi == 0, "liste_temizle ref_sayisi'ni dusurdu");

    sarki_sil(s1);
    sarki_sil(s2);
    ASSERT(izci_aktif_byte() <= 0, "SIZINTI yok");
}

static void test_linked_list_arama(void) {
    printf("\n[TEST] linked_list arama\n");

    Sarki* bas = NULL;
    Sarki* son = NULL;
    for (int i = 0; i < 5; i++) {
        char b[32]; sprintf(b, "Sarki_%d", i);
        Sarki* s = sarki_olustur(i, b, "Album", "Sanatci", 180, 2020);
        if (!bas) { bas = s; son = s; }
        else      { son->sonraki = s; son = s; }
    }

    ASSERT(sarki_ara_liste(bas, 0) == bas,  "id=0 bulundu");
    ASSERT(sarki_ara_liste(bas, 4) != NULL, "id=4 bulundu");
    ASSERT(sarki_ara_liste(bas, 9) == NULL, "id=9 bulunamadi (NULL)");

    Sarki* curr = bas;
    while (curr) {
        Sarki* sonraki = curr->sonraki;
        curr->sonraki = NULL;
        sarki_sil(curr);
        curr = sonraki;
    }
    ASSERT(izci_aktif_byte() <= 0, "SIZINTI yok");
}

static void test_hashmap(void) {
    printf("\n[TEST] hashmap arama\n");

    HashMap* map = hashmap_olustur();
    ASSERT(map != NULL, "hashmap olusturuldu");

    Sarki* sarkilar[10];
    for (int i = 0; i < 10; i++) {
        char b[32]; sprintf(b, "MapSarki_%d", i);
        sarkilar[i] = sarki_olustur(i * 100, b, "Album", "Sanatci", 200, 2021);
        sarkilar[i]->sonraki = NULL;
        hashmap_ekle(map, sarkilar[i]);
    }

    ASSERT(sarki_ara_map(map, 0)   != NULL, "id=0   bulundu");
    ASSERT(sarki_ara_map(map, 500) != NULL, "id=500 bulundu");
    ASSERT(sarki_ara_map(map, 900) != NULL, "id=900 bulundu");
    ASSERT(sarki_ara_map(map, 999) == NULL, "id=999 bulunamadi");

    hashmap_temizle(map);
    for (int i = 0; i < 10; i++) {
        sarkilar[i]->sonraki = NULL;
        sarki_sil(sarkilar[i]);
    }
    ASSERT(izci_aktif_byte() <= 0, "SIZINTI yok");
}

static void test_realloc_buyume(void) {
    printf("\n[TEST] CalmaListesi realloc buyume\n");

    CalmaListesi* l = liste_olustur("Buyuk Liste");
    Sarki* sarkilar[20];
    for (int i = 0; i < 20; i++) {
        sarkilar[i] = sarki_olustur(i, "S", "A", "X", 100, 2022);
        liste_sarki_ekle(l, sarkilar[i]);
    }
    ASSERT(l->sarki_sayisi == 20,        "20 sarki eklendi");
    ASSERT(l->kapasite >= 20,            "kapasite buyudu");
    ASSERT(sarkilar[0]->ref_sayisi == 1, "ref_sayisi dogru");

    liste_temizle(l);
    for (int i = 0; i < 20; i++) sarki_sil(sarkilar[i]);
    ASSERT(izci_aktif_byte() <= 0, "SIZINTI yok");
}

int main(void) {
    printf("========================================\n");
    printf("   MiniStream - Temel Testler\n");
    printf("========================================\n");

    test_sarki_olustur();
    test_liste_sarki_ekle();
    test_ref_sayisi();
    test_liste();
    test_linked_list_arama();
    test_hashmap();
    test_realloc_buyume();

    printf("\n========================================\n");
    printf("Sonuc: %d gecti, %d kaldi\n", gecen, kalan);
    printf("========================================\n");

    bellek_raporu_yazdir();
    return kalan == 0 ? 0 : 1;
}