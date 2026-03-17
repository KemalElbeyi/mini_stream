#include "ministream.h"
#include "bellek_izci.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
   GÜN 1-2: Veri Modeli & Bellek İzleyici
   ============================================================ */

/* Heap'te yeni Sarki oluştur */
Sarki* sarki_olustur(int id, const char* baslik, const char* album,
                     const char* sanatci, int sure_sn, int yil) {
    Sarki* s = (Sarki*)izlenen_malloc(sizeof(Sarki));
    if (!s) return NULL;

    s->id        = id;
    s->sure_sn   = sure_sn;
    s->yil       = yil;
    s->ref_sayisi = 0;
    s->sonraki   = NULL;

    strncpy(s->baslik,  baslik,  MAX_BASLIK  - 1); s->baslik[MAX_BASLIK-1]  = '\0';
    strncpy(s->album,   album,   MAX_ALBUM   - 1); s->album[MAX_ALBUM-1]   = '\0';
    strncpy(s->sanatci, sanatci, MAX_SANATCI - 1); s->sanatci[MAX_SANATCI-1] = '\0';

    return s;
}

/* ref_sayisi == 0 ise sil, değilse uyar */
int sarki_sil(Sarki* sarki) {
    if (!sarki) return -1;
    if (sarki->ref_sayisi > 0) {
        return -1;
    }
    izlened_free(sarki, sizeof(Sarki));
    return 0;
}

/* ============================================================
   CalmaListesi işlemleri
   ============================================================ */

CalmaListesi* liste_olustur(const char* ad) {
    CalmaListesi* l = (CalmaListesi*)izlenen_malloc(sizeof(CalmaListesi));
    if (!l) return NULL;

    l->sarkilar = (Sarki**)izlenen_malloc(BASLANGIC_KAPASITE * sizeof(Sarki*));
    if (!l->sarkilar) {
        izlened_free(l, sizeof(CalmaListesi));
        return NULL;
    }
    l->sarki_sayisi = 0;
    l->kapasite     = BASLANGIC_KAPASITE;
    strncpy(l->ad, ad, 127); l->ad[127] = '\0';
    return l;
}

/* Listeyi ve pointer dizisini temizle (Sarki nesneleri silinmez) */
int liste_temizle(CalmaListesi* liste) {
    if (!liste) return -1;
    /* ref_sayisi'nı düşür */
    for (int i = 0; i < liste->sarki_sayisi; i++) {
        if (liste->sarkilar[i])
            liste->sarkilar[i]->ref_sayisi--;
    }
    izlened_free(liste->sarkilar, liste->kapasite * sizeof(Sarki*));
    izlened_free(liste, sizeof(CalmaListesi));
    return 0;
}

/* Şarkı ekle — gerekirse realloc ile büyüt */
int liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki) {
    if (!liste || !sarki) return -1;
    if (liste->sarki_sayisi >= liste->kapasite) {
        int yeni_kap = liste->kapasite * 2;
        /* realloc bellek izleyici tarafından sayılmaz — beklenen durum */
        Sarki** yeni = (Sarki**)realloc(liste->sarkilar,
                                         yeni_kap * sizeof(Sarki*));
        if (!yeni) return -1;
        liste->sarkilar = yeni;
        liste->kapasite = yeni_kap;
    }
    liste->sarkilar[liste->sarki_sayisi++] = sarki;
    sarki->ref_sayisi++;
    return 0;
}

/* İndeksteki şarkıyı listeden çıkar (son eleman ile yer değiştir) */
int liste_sarki_cikar(CalmaListesi* liste, int idx) {
    if (!liste || idx < 0 || idx >= liste->sarki_sayisi) return -1;
    liste->sarkilar[idx]->ref_sayisi--;          /* önce azalt */
    liste->sarkilar[idx] = liste->sarkilar[--liste->sarki_sayisi];
    return 0;
}

/* ============================================================
   GÜN 1-2: CSV Okuma
   Kaggle Spotify veri seti kolonları:
   id, name, album, album_id, artists, artist_ids,
   track_number, disc_number, explicit, danceability,
   energy, key, loudness, mode, speechiness, acousticness,
   instrumentalness, liveness, valence, tempo,
   duration_ms, time_signature, year, release_date
   ============================================================ */

/* Yardımcı: CSV satırından N. alanı çıkar (virgül ayraç, tırnak desteği) */
static int csv_alan_cek(const char* satir, int alan_no, char* cikti, int max) {
    int alan = 0;
    int i    = 0;
    int j    = 0;
    int uzunluk = (int)strlen(satir);

    while (i <= uzunluk && alan <= alan_no) {
        if (satir[i] == '"') {
            /* tırnaklı alan */
            i++;
            while (satir[i] && satir[i] != '"') {
                if (alan == alan_no && j < max - 1)
                    cikti[j++] = satir[i];
                i++;
            }
            if (satir[i] == '"') i++;
        } else {
            while (satir[i] && satir[i] != ',') {
                if (alan == alan_no && j < max - 1)
                    cikti[j++] = satir[i];
                i++;
            }
        }
        if (satir[i] == ',') i++;
        if (alan == alan_no) break;
        alan++;
        j = 0;
    }
    cikti[j] = '\0';
    return j;
}

/*
 * CSV yükle — linked list başı döner.
 * Kaggle tracks.csv kolon indeksleri:
 *   0=id(string), 1=name, 2=album, 3=album_id,
 *   4=artists, 5=artist_ids, 6=track_number, 7=disc_number,
 *   8=explicit, 9=danceability, 10=energy, 11=key,
 *   12=loudness, 13=mode, 14=speechiness, 15=acousticness,
 *   16=instrumentalness, 17=liveness, 18=valence, 19=tempo,
 *   20=duration_ms, 21=time_signature, 22=year, 23=release_date
 */
int csv_yukle(const char* dosya_yolu, Sarki** bas, int max_sarki) {
    FILE* f = fopen(dosya_yolu, "r");
    if (!f) {
        fprintf(stderr, "Hata: %s acilamadi\n", dosya_yolu);
        return -1;
    }

    char satir[2048];
    /* başlık satırını atla */
    fgets(satir, sizeof(satir), f);

    *bas = NULL;
    Sarki* son = NULL;
    int sayac  = 0;
    int id     = 0;

    char baslik[MAX_BASLIK], album[MAX_ALBUM], sanatci[MAX_SANATCI];
    char sure_str[32], yil_str[16];

    while (sayac < max_sarki && fgets(satir, sizeof(satir), f)) {
        /* satır sonu temizle */
        int len = (int)strlen(satir);
        while (len > 0 && (satir[len-1] == '\n' || satir[len-1] == '\r'))
            satir[--len] = '\0';

        if (len == 0) continue;

        csv_alan_cek(satir, 1,  baslik,   MAX_BASLIK);
        csv_alan_cek(satir, 2,  album,    MAX_ALBUM);
        csv_alan_cek(satir, 4,  sanatci,  MAX_SANATCI);
        csv_alan_cek(satir, 20, sure_str, 32);
        csv_alan_cek(satir, 22, yil_str,  16);

        /* artists alanı bazen ["Sanatçı"] formatında gelir */
        if (sanatci[0] == '[') {
            /* [ ' ve ' ] karakterlerini temizle */
            char temiz[MAX_SANATCI];
            int ti = 0;
            for (int k = 0; sanatci[k] && ti < MAX_SANATCI-1; k++) {
                char c = sanatci[k];
                if (c != '[' && c != ']' && c != '\'' && c != '"')
                    temiz[ti++] = c;
            }
            temiz[ti] = '\0';
            strncpy(sanatci, temiz, MAX_SANATCI-1);
        }

        int sure_sn = atoi(sure_str) / 1000;
        int yil     = atoi(yil_str);

        Sarki* s = sarki_olustur(id++, baslik, album, sanatci, sure_sn, yil);
        if (!s) continue;

        /* linked list'e ekle */
        if (!*bas) { *bas = s; son = s; }
        else       { son->sonraki = s; son = s; }

        sayac++;
    }

    fclose(f);
    printf("%d sarki yuklendi: %s\n", sayac, dosya_yolu);
    return sayac;
}