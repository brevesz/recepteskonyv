#ifndef RECEPT_H
#define RECEPT_H

#include "hozzavalok.h"

#define MAX_CIM_HOSSZ 120
#define MAX_LEIRAS_HOSSZ 5000
#define MAX_HOZZAVALOK 50

typedef struct
{
    char *cim;
    Hozzavalo *hozzavalok; // tomb
    int hozzavalok_szama;
    char *leiras;
} Recept;

Recept recept_new();

void recept_free(Recept recept);

#endif