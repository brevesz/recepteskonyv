#include "recept.h"
#include <stdlib.h>

#include "debugmalloc.h"

Recept recept_new()
{
    Recept r = {.cim = NULL, .hozzavalok = NULL, .hozzavalok_szama = 0, .leiras = NULL};
    return r;
}

void recept_free(Recept recept)
{
    if (recept.cim != NULL)
        free(recept.cim);

    if (recept.hozzavalok != NULL)
    {

        for (int i = 0; i < recept.hozzavalok_szama; ++i)
        {
            hozzavalo_free(recept.hozzavalok[i]);
        }

        free(recept.hozzavalok);
    }

    if (recept.leiras != NULL)
        free(recept.leiras);
}