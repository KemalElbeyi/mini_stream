#ifndef BELLEK_IZCI_H
#define BELLEK_IZCI_H

#include <stdlib.h>

/* ============================================================
   BELLEK İZLEYİCİ
   izlenen_malloc / izlened_free ile tüm heap işlemlerini say.
   ============================================================ */

void* izlenen_malloc(size_t boyut);
void  izlened_free(void* ptr, size_t boyut);
void  bellek_raporu_yazdir(void);

/* Getter fonksiyonları (izci static olduğundan dışarıdan erişim) */
long  izci_malloc_sayisi(void);
long  izci_free_sayisi(void);
long  izci_toplam_malloc_byte(void);
long  izci_toplam_free_byte(void);
long  izci_aktif_byte(void);

#endif /* BELLEK_IZCI_H */