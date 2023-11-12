#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>

#include "debugmalloc.h"

#define MAX_CIM_HOSSZ 120
#define MAX_LEIRAS_HOSSZ 5000
#define MAX_HOZZAVALO_NEV_HOSSZ 50
#define MAX_HOZZAVALO_MENNYISEG_HOSSZ 50
#define MAX_HOZZAVALOK 30
#define MAX_RECEPTEK_SZAMA 100

typedef struct
{
    char *nev, *mennyiseg;
} Hozzavalo;

typedef struct
{
    char *cim;
    Hozzavalo *hozzavalok; // tomb
    int hozzavalok_szama;
    char *leiras;
} Recept;

Recept recept_new()
{
    Recept r = {.cim = NULL, .hozzavalok = NULL, .hozzavalok_szama = 0, .leiras = NULL};
    return r;
}

Hozzavalo hozzavalo_new()
{
    Hozzavalo h = {.nev = NULL, .mennyiseg = NULL};
    return h;
}

Hozzavalo hozzavalok_vajaskenyer[3] = {{"Kenyer", "1 szelet"},
                                       {"Vaj", "izles szerint"},
                                       {"So", "izles szerint"}};

Hozzavalo hozzavalok_zabkasa[3] = {{"Zab", "1 marek"},
                                   {"Vaj", "izles szerint"},
                                   {"Cukor", "izles szerint"}};

Recept *receptek;
int receptek_szama = 0;

/* const Recept receptek[receptek_szama] = {{.cim = "Vajas kenyer",
                                          .hozzavalok = hozzavalok_vajaskenyer,
                                          .hozzavalok_szama = 3,
                                          .leiras = "Kend meg a kenyeret"},
                                         {.cim = "Zabkasa",
                                          .hozzavalok = hozzavalok_zabkasa,
                                          .hozzavalok_szama = 3,
                                          .leiras = "Csinald meg"}}; */

bool osszes_recept_betolt() // meg kell keresni a receptek mappat
{
    DIR *mappa;
    struct dirent *entry;

    const char *mappanev = "receptek";
    chdir(mappanev);
    mappa = opendir(".");

    if (mappa == NULL)
    {
        perror("Nem sikerult megnyitni a mappa.\n");
        return false;
    }

    // az osszes fajlt be kell tolteni
    while ((entry = readdir(mappa)) != NULL)
    {
        // csak a fajlok erdekelnek
        if (entry->d_type == DT_REG)
        {
            if (!recept_beolvas_fajlbol(entry->d_name, &receptek[receptek_szama++]))
            {
                // hiba beolvasas kozben
                closedir(mappa);
                return false;
            };
        }
    }

    closedir(mappa);
}

void hozzavalo_free(Hozzavalo hozzavalo)
{
    if (hozzavalo.nev != NULL)
        free(hozzavalo.nev);

    if (hozzavalo.mennyiseg != NULL)
        free(hozzavalo.mennyiseg);
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

void recept_kiir(const Recept *recept)
{
    printf("\n");
    printf("%s\n", recept->cim);
    printf("============\n");
    printf("\n");
    printf("Hozzavalok:\n");
    for (int i = 0; i < recept->hozzavalok_szama; ++i)
    {
        Hozzavalo hozzavalo = recept->hozzavalok[i];
        printf("* %s (%s)\n", hozzavalo.nev, hozzavalo.mennyiseg);
    }
    printf("\n");
    printf("%s\n", recept->leiras);
    printf("\n");
    printf("[0] Vissza\t[del] Recept torlese\n");
    // itt még kell a törlés funkció
    int receptnezet_bemenet;
    scanf("%d", &receptnezet_bemenet);
    getchar(); // a bemeneten való entert-t törli
}

void osszes_recept()
{
    printf("[0] <- Vissza\n");
    for (int i = 0; i < receptek_szama; ++i)
    {
        printf("[%d] %s\n", i + 1, receptek[i].cim);
    }
    int recept_sorszam;
    scanf("%d", &recept_sorszam);
    getchar();

    if (recept_sorszam == 0)
        return;

    const Recept *recept = &receptek[recept_sorszam - 1];
    recept_kiir(recept);
}

bool hozzavalo_beolvas(FILE *f, Hozzavalo *hozzavalo)
{
    char sor[101];
    fgets(sor, 100, f);

    if (sor[0] == '\n')
    {
        return false;
    }

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

bool recept_beolvas_fajlbol(char *fajlnev, Recept *recept) // fajlbol a parameterkent adott strukturaba olvassa
{
    FILE *f = fopen(fajlnev, "r");
    if (f == NULL)
    {
        perror("Nem lehet megnyitni a fajlt.");
        return false;
    }

    char puffer[51];

    recept->cim = malloc(MAX_CIM_HOSSZ); // sizeof char?
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

        if (recept->hozzavalok_szama >= MAX_HOZZAVALOK)
        {
            perror("Tul sok hozzavalo");
            fclose(f);
            return false;
        }
    }

    recept->leiras = malloc(MAX_LEIRAS_HOSSZ);
    char *p = recept->leiras;
    while (fgets(puffer, 50, f))
    {
        strncpy(p, puffer, 50);
        p += strlen(puffer);
    }
    *p = '\0';

    fclose(f);
    return true;
}

int pelda_recept_beolvasasara()
{
    Recept recept = recept_new();
    bool siker = recept_beolvas_fajlbol("receptek\\Lencsefozelek", &recept);
    if (!siker)
        return 1;

    recept_kiir(&recept);

    recept_free(recept);
    return 0;
}

Recept *uj_recept()
{
    Recept *recept = &receptek[receptek_szama++];
    recept->cim = malloc(sizeof(char)*MAX_CIM_HOSSZ);
    recept->hozzavalok = malloc(sizeof(Hozzavalo)*MAX_HOZZAVALOK);
    for (int i = 0; i < MAX_HOZZAVALOK; ++i)
    {
        recept->hozzavalok[i] = hozzavalo_new();
    }
    recept->leiras = malloc(sizeof(char)*MAX_LEIRAS_HOSSZ);
    printf("Adja meg a recept cimet: \n");
    gets(recept->cim);

    

    printf("Gepelje be a recept leirasat!\n");
    gets(recept->leiras);

    return recept;
}

bool recept_mentes_fajlba(Recept *ujrecept)
{
    FILE *f = fopen(ujrecept->cim, "w");
    if (f == NULL)
    {
        perror("Nem sikerult megnyitni a fajlt.");
        return false;
    }
    // fajlba iras, formazva
    fprintf(f, "%s", ujrecept->cim);
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
    fprintf(f, "\n");

    fclose(f);
}

int main()
{
    receptek = malloc(sizeof(Recept) * MAX_RECEPTEK_SZAMA);

    for (int i = 0; i < MAX_RECEPTEK_SZAMA; ++i)
    {
        receptek[i] = recept_new();
    }

    if (!osszes_recept_betolt())
        return 1;

    int menu_bemenet;
    printf("%s", "\t\t\t\tReceptes konyv\n\n");

    printf(
        "Valasszon az alabbi pontok kozul a megfelelo szambillentyu "
        "lenyomasaval: \n");

    printf("[1] Osszes recept\n");
    printf("[2] Uj recept\n");
    printf("[3] Kereses\n");
    printf("[4] Nincs otletem\n");
    printf("[5] De ennek egy kis...\n");
    printf("[6] El kell hasznalni\n");
    printf("[0] Kilepes\n");

    scanf("%d", &menu_bemenet);
    getchar();

    printf("A valasztott ertek: %d\n", menu_bemenet);
    if (menu_bemenet == 1)
        osszes_recept();

    return 0;
}
