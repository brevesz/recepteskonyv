#ifndef RECEPTESKONYV_H
#define RECEPTESKONYV_H

#include "recept.h"
#include <stdbool.h>
#define MAX_TALALATOK_SZAMA 200

typedef struct
{
    Recept *receptek;
    int receptek_szama;
} Recepteskonyv;

Recepteskonyv recepteskonyv_new();
void recepteskonyv_free(Recepteskonyv konyv);
void recept_torles(Recept *recept, Recepteskonyv *konyv);
void recept_kiir(Recept *recept, Recepteskonyv *konyv);
void osszes_recept(Recepteskonyv *konyv);
Recept recept_interaktiv_beolvas();
void uj_recept(Recepteskonyv *konyv);
void keres(Recepteskonyv *konyv);
Recept *sorsol(Recepteskonyv *konyv);
void nincs_otletem(Recepteskonyv *konyv);
void de_ennek_egy_kis(Recepteskonyv *konyv);
void el_kell_hasznalni(Recepteskonyv *konyv);

#endif