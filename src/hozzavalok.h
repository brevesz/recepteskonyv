#ifndef HOZZAVALOK_H
#define HOZZAVALOK_H

#define MAX_HOZZAVALO_NEV_HOSSZ 50
#define MAX_HOZZAVALO_MENNYISEG_HOSSZ 50

typedef struct
{
    char *nev, *mennyiseg;
} Hozzavalo;

Hozzavalo hozzavalo_new();
void hozzavalo_free(Hozzavalo hozzavalo);

#endif