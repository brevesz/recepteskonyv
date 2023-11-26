#ifndef RECEPT_H
#define RECEPT_H

#include "hozzavalok.h"
#include <stdbool.h>

#define MAX_CIM_HOSSZ 120
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
bool recept_mindet_tartalmazza(const char **keresett_hozzavalok, const Recept *recept);
#endif