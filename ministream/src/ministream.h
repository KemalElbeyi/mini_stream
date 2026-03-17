#ifndef MINISTREAM_H
#define MINISTREAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
   VERİ MODELİ
   ============================================================ */

#define MAX_BASLIK   256
#define MAX_ALBUM    256
#define MAX_SANATCI  256
#define BASLANGIC_KAPASITE 8

typedef struct Sarki {
    int    id;
    char   baslik[MAX_BASLIK];
    char   album[MAX_ALBUM];
    char   sanatci[MAX_SANATCI];
    int    sure_sn;   /* milisaniye / 1000 */
    int    yil;
    int    ref_sayisi; /* kaç çalma listesinde kullanılıyor */
    struct Sarki* sonraki; /* linked list için */
} Sarki;

typedef struct {
    Sarki** sarkilar;   /* Sarki* dizisi — heap'te */
    int     sarki_sayisi;
    int     kapasite;
    char    ad[128];
} CalmaListesi;

typedef struct {
    int          id;
    char         kullanici_adi[64];
    CalmaListesi** listeler;
    int          liste_sayisi;
    int          liste_kapasite;
} Kullanici;

/* ============================================================
   FONKSIYON PROTOTİPLERİ
   ============================================================ */

/* Sarki işlemleri */
Sarki*       sarki_olustur(int id, const char* baslik, const char* album,
                            const char* sanatci, int sure_sn, int yil);
int          sarki_sil(Sarki* sarki);

/* CalmaListesi işlemleri */
CalmaListesi* liste_olustur(const char* ad);
int           liste_temizle(CalmaListesi* liste);
int           liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki);
int           liste_sarki_cikar(CalmaListesi* liste, int idx);

/* CSV okuma */
int csv_yukle(const char* dosya_yolu, Sarki** bas, int max_sarki);

#endif /* MINISTREAM_H */