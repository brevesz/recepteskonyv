#ifndef FILES_H
#define FILES_H

#include "hozzavalok.h"
#include "recept.h"
#include "recepteskonyv.h"

#include <stdio.h>
#include <stdbool.h>


void recept_fajl_torles(char *cim);
bool hozzavalo_beolvas(FILE *f, Hozzavalo *hozzavalo);
bool recept_beolvas_fajlbol(const char *fajlnev, Recept *recept);
bool osszes_recept_betolt(Recepteskonyv *konyv);
bool recept_mentes_fajlba(const Recept *recept);


#endif