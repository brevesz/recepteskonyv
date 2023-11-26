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

// parameter: string tomb nullpointerrel a vegen, recept const struct pointer
bool recept_mindet_tartalmazza(const char **keresett_hozzavalok, const Recept *recept)
{
    for (const char **k = keresett_hozzavalok; *k != NULL; ++k)
    {
        // addig ismetli, amig mindegyik osszetevot megtalalta, kulonben kilep false-al
        bool tartalmazza = false;

        for (int j = 0; j < recept->hozzavalok_szama; ++j)
        {
            if (strcmp(recept->hozzavalok[j].nev, *k) == 0)
            {
                tartalmazza = true;
                break;
            }
        }

        // ha az egyik osszetevo nincs benne, akkor mar nem tartalmazza az osszeset
        if (!tartalmazza)
        {
            return false;
        }
    }

    return true;
}