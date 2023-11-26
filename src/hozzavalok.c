#include "hozzavalok.h"
#include <stdlib.h>

#include "debugmalloc.h"

Hozzavalo hozzavalo_new()
{
    Hozzavalo h = {.nev = NULL, .mennyiseg = NULL};
    return h;
}

void hozzavalo_free(Hozzavalo hozzavalo)
{
    if (hozzavalo.nev != NULL)
        free(hozzavalo.nev);

    if (hozzavalo.mennyiseg != NULL)
        free(hozzavalo.mennyiseg);
}