/*
 * uretec.c — Kaggle verisi olmadan test için sahte sarkilar.csv üretir.
 * Kullanım: gcc -o uretec src/uretec.c && ./uretec 1000
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char* SANATCILAR[] = {
    "Radiohead", "Portishead", "Massive Attack", "Boards of Canada",
    "Aphex Twin", "The Prodigy", "Bjork", "Sigur Ros", "Mogwai",
    "Godspeed You! Black Emperor", "Four Tet", "Burial", "Bonobo",
    "Caribou", "Floating Points", "Jon Hopkins", "Nils Frahm",
    "Max Richter", "Ólafur Arnalds", "Tycho"
};
#define N_SANATCI 20

static const char* ALBUMLER[] = {
    "OK Computer", "Dummy", "Mezzanine", "Geogaddi",
    "Selected Ambient Works", "Music for the Jilted Generation",
    "Homogenic", "Takk", "Come On Die Young",
    "Lift Your Skinny Fists", "Rounds", "Untrue",
    "Black Sands", "Our Love", "Reflections",
    "Immunity", "Spaces", "Sleep", "re:member", "Dive"
};
#define N_ALBUM 20

int main(int argc, char* argv[]) {
    int n = 1000;
    if (argc > 1) n = atoi(argv[1]);
    if (n <= 0 || n > 1500000) { fprintf(stderr, "1-1500000 arasi\n"); return 1; }

    /* ministream/data/ dizinine yaz */
    FILE* f = fopen("data/sarkilar.csv", "w");
    if (!f) {
        /* Alternatif yol */
        f = fopen("sarkilar.csv", "w");
        if (!f) { perror("dosya acilamadi"); return 1; }
    }

    /* Başlık satırı — Kaggle formatıyla uyumlu */
    fprintf(f, "id,name,album,album_id,artists,artist_ids,track_number,"
               "disc_number,explicit,danceability,energy,key,loudness,mode,"
               "speechiness,acousticness,instrumentalness,liveness,valence,"
               "tempo,duration_ms,time_signature,year,release_date\n");

    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++) {
        int sanatci_idx = rand() % N_SANATCI;
        int album_idx   = rand() % N_ALBUM;
        int sure_ms     = 120000 + rand() % 240000; /* 2-6 dakika */
        int yil         = 1990 + rand() % 35;

        fprintf(f,
            "FAKE%06d,"          /* id */
            "Sarki %d,"          /* name */
            "%s,"                /* album */
            "ALBUM%06d,"         /* album_id */
            "['%s'],"            /* artists */
            "ART%06d,"           /* artist_ids */
            "%d,1,0,"            /* track/disc/explicit */
            "%.3f,%.3f,%d,%.1f,%d," /* danceability..mode */
            "%.4f,%.4f,%.4f,%.3f,%.3f,%.1f," /* speechiness..valence, tempo */
            "%d,4,"              /* duration_ms, time_signature */
            "%d,"                /* year */
            "%d-01-01\n",        /* release_date */
            i, i,
            ALBUMLER[album_idx],
            album_idx,
            SANATCILAR[sanatci_idx],
            sanatci_idx,
            (i % 15) + 1,
            (float)(rand()%1000)/1000.0f,
            (float)(rand()%1000)/1000.0f,
            rand()%12,
            -5.0f - (rand()%200)/10.0f,
            rand()%2,
            (float)(rand()%1000)/1000.0f,
            (float)(rand()%1000)/1000.0f,
            (float)(rand()%1000)/1000.0f,
            (float)(rand()%1000)/1000.0f,
            (float)(rand()%1000)/1000.0f,
            80.0f + (rand()%120),
            sure_ms,
            yil,
            yil
        );
    }

    fclose(f);
    printf("%d sahte sarki uretildi -> data/sarkilar.csv\n", n);
    return 0;
}