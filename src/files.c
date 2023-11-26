#include "files.h"

#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "debugmalloc.h"

const char *RECEPTEK_MAPPA = "receptek";

// spacet '_'-re csereli
// nincs a headerbe, mert mashol nem hasznaljuk
void slug(char *nev)
{
    char *c;
    char *kurzor = nev;
    while ((c = strchr(kurzor, ' ')) != NULL)
    {
        *c = '_';
        kurzor = c + 1;
    }
}

void recept_fajl_torles(char *cim)
{
    // fajlnev letrehozasa
    char fajlnev[MAX_CIM_HOSSZ];
    strncpy(fajlnev, cim, MAX_CIM_HOSSZ);
    slug(fajlnev);

    // fajl torlese
    chdir(RECEPTEK_MAPPA);
    if (remove(fajlnev) == 0)
    {
        printf("Recept torolve.\n");
    }
    else
    {
        printf("A recept torlese sikertelen.\n");
    }
    chdir("..");
}

// recept_beolvas_fajlbol seged fuggvenye
bool hozzavalo_beolvas(FILE *f, Hozzavalo *hozzavalo)
{
    char sor[101];
    fgets(sor, 100, f);

    if (sor[0] == '\n')
    {
        return false;
    }

    // lefoglalt teruletek merete a zarojelekkel meghatarozhato
    char *nev = sor + 2;
    char *mennyiseg = strrchr(sor, '(') + 1;
    int nev_hossz = (mennyiseg - nev) - 2;
    int mennyiseg_hossz = strrchr(mennyiseg, ')') - mennyiseg;

    hozzavalo->nev = malloc(nev_hossz + 1);
    hozzavalo->mennyiseg = malloc(mennyiseg_hossz + 1);

    strncpy(hozzavalo->nev, nev, nev_hossz);
    hozzavalo->nev[nev_hossz] = '\0';
    strncpy(hozzavalo->mennyiseg, mennyiseg, mennyiseg_hossz);
    hozzavalo->mennyiseg[mennyiseg_hossz] = '\0';

    return true;
}

// fajlbol a parameterkent adott strukturaba olvassa
bool recept_beolvas_fajlbol(const char *fajlnev, Recept *recept)
{
    FILE *f = fopen(fajlnev, "r");
    if (f == NULL)
    {
        perror("Nem lehet megnyitni a fajlt.");
        return false;
    }

    char puffer[51];

    recept->cim = malloc(MAX_CIM_HOSSZ);
    fgets(recept->cim, MAX_CIM_HOSSZ, f);
    // az \n nem kell a cim vegere
    recept->cim[strlen(recept->cim) - 1] = '\0';

    // egy alahuzas es egy ures sor
    fgets(puffer, 50, f);
    fgets(puffer, 50, f);

    // hozzavalok
    fgets(puffer, 50, f);

    recept->hozzavalok = malloc(sizeof(Hozzavalo) * MAX_HOZZAVALOK);
    for (int i = 0; i < MAX_HOZZAVALOK; ++i)
    {
        recept->hozzavalok[i] = hozzavalo_new();
    }

    while (hozzavalo_beolvas(f, &recept->hozzavalok[recept->hozzavalok_szama]))
    {
        recept->hozzavalok_szama++;

        // szurunk a makro tullepesere
        if (recept->hozzavalok_szama >= MAX_HOZZAVALOK)
        {
            perror("Tul sok hozzavalo");
            fclose(f);
            return false;
        }
    }

    // fgets amig nincs vege a fajlnak
    // minden sor, vagy ha egy sor hosszabb mint 50: 50-nel noveljuk a foglalast
    char *leiras = NULL;
    int leiras_hossz = 0;
    while (fgets(puffer, 50, f))
    {
        leiras = realloc(leiras, (leiras_hossz + 50) * sizeof(char));
        strncpy(leiras + leiras_hossz, puffer, 50);
        leiras_hossz += strlen(puffer);
    }
    leiras[leiras_hossz] = '\0';

    recept->leiras = leiras;

    fclose(f);
    return true;
}

bool osszes_recept_betolt(Recepteskonyv *konyv)
{
    DIR *mappa;
    struct dirent *elem;

    // eljut a receptek mappaba
    chdir(RECEPTEK_MAPPA);
    mappa = opendir(".");

    if (mappa == NULL)
    {
        perror("Nem sikerult megnyitni a mappat.\n");
        chdir("..");
        return false;
    }

    // az osszes fajlt be kell tolteni
    while ((elem = readdir(mappa)) != NULL)
    {
        const char *fajlnev = elem->d_name;

        // csak a fajlok erdekelnek
        struct stat st;
        if (stat(fajlnev, &st) != 0)
        {
            perror("Hibas stat()\n");
            closedir(mappa);
            chdir("..");
            return false;
        }

        bool ez_egy_fajl = (st.st_mode & S_IFMT) == S_IFREG;
        if (ez_egy_fajl)
        {
            konyv->receptek_szama++;
            konyv->receptek = realloc(konyv->receptek, konyv->receptek_szama * sizeof(Recept));

            Recept recept = recept_new();
            if (!recept_beolvas_fajlbol(fajlnev, &recept))
            {
                // hiba beolvasas kozben
                closedir(mappa);
                chdir("..");
                return false;
            };

            konyv->receptek[konyv->receptek_szama - 1] = recept;
        }
    }

    closedir(mappa);
    chdir("..");
    return true;
}

bool recept_mentes_fajlba(const Recept *recept)
{
    char fajlnev[121];
    strncpy(fajlnev, recept->cim, 120);
    slug(fajlnev);

    chdir(RECEPTEK_MAPPA);
    FILE *f = fopen(fajlnev, "w");
    chdir("..");

    if (f == NULL)
    {
        perror("Nem sikerult megnyitni a fajlt.");
        return false;
    }

    fprintf(f, "%s\n", recept->cim);
    fprintf(f, "============\n");
    fprintf(f, "\n");
    fprintf(f, "Hozzavalok:\n");
    for (int i = 0; i < recept->hozzavalok_szama; ++i)
    {
        Hozzavalo hozzavalo = recept->hozzavalok[i];
        fprintf(f, "* %s (%s)\n", hozzavalo.nev, hozzavalo.mennyiseg);
    }
    fprintf(f, "\n");
    fprintf(f, "%s\n", recept->leiras);

    fclose(f);
    return true;
}
