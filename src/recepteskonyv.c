#include "recepteskonyv.h"
#include "files.h"

#include <string.h>
#include <time.h>

#include "debugmalloc.h"

Recepteskonyv recepteskonyv_new()
{
    Recepteskonyv konyv;
    konyv.receptek = malloc(1);
    konyv.receptek_szama = 0;

    return konyv;
}

void recepteskonyv_free(Recepteskonyv konyv)
{
    for (int i = 0; i < konyv.receptek_szama; ++i)
    {
        recept_free(konyv.receptek[i]);
    }

    free(konyv.receptek);
}

void recept_torles(Recept *recept, Recepteskonyv *konyv)
{
    // fajlbol torles
    recept_fajl_torles(recept->cim);
    
    // a receptekbol torles
    int recept_index = recept - konyv->receptek;
    recept_free(konyv->receptek[recept_index]);

    for (int i = recept_index; i < konyv->receptek_szama - 1; ++i)
    {
        konyv->receptek[i] = konyv->receptek[i + 1];
    }
    konyv->receptek_szama--;

}

void recept_kiir(Recept *recept, Recepteskonyv *konyv)
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
    printf("[0] Vissza a fomenube\t[d] Recept torlese\n");
    // itt meg kell torles funkcio
    char menu_bemenet;

    // csak a '0' es a 'd' ertekre ugrik vissza a fomenube
    while (true)
    {
        scanf("%c", &menu_bemenet);
        getchar(); // a bemeneten valo entert-t torli

        switch (menu_bemenet)
        {
        case '0':
            return;
        case 'd':
            recept_torles(recept, konyv);
            return;
        default:
            break;
        }
    }
}

void osszes_recept(Recepteskonyv *konyv)
{
    printf("[0] <- Vissza\n");
    for (int i = 0; i < konyv->receptek_szama; ++i)
    {
        printf("[%d] %s\n", i + 1, konyv->receptek[i].cim);
    }
    int recept_sorszam;
    scanf("%d", &recept_sorszam);
    getchar();

    if (recept_sorszam == 0)
        return;

    Recept *recept = &konyv->receptek[recept_sorszam - 1];
    recept_kiir(recept, konyv);
}

// uj recept beolvasasa a konzolrol
Recept recept_interaktiv_beolvas()
{
    Recept recept = recept_new();
    recept.cim = malloc(sizeof(char) * MAX_CIM_HOSSZ);
    recept.hozzavalok = malloc(sizeof(Hozzavalo) * MAX_HOZZAVALOK);
    recept.leiras = malloc(sizeof(char) * MAX_LEIRAS_HOSSZ);

    for (int i = 0; i < MAX_HOZZAVALOK; ++i)
    {
        recept.hozzavalok[i] = hozzavalo_new();
    }

    printf("Adja meg a recept cimet: \n");
    fgets(recept.cim, MAX_CIM_HOSSZ, stdin);
    // fgets entert rak a vegere, ezt lecsereljuk lezaro nullara
    recept.cim[strlen(recept.cim) - 1] = '\0';

    while (true)
    {
        Hozzavalo *hozzavalo = &recept.hozzavalok[recept.hozzavalok_szama];
        hozzavalo->nev = malloc(sizeof(char) * MAX_HOZZAVALO_NEV_HOSSZ);
        hozzavalo->mennyiseg = malloc(sizeof(char) * MAX_HOZZAVALO_MENNYISEG_HOSSZ);

        printf("Hozzavalo neve (hagyja uresen ha nincs tobb hozzavalo): ");
        fgets(hozzavalo->nev, MAX_HOZZAVALO_NEV_HOSSZ, stdin);
        // enter nem kell a vegere
        hozzavalo->nev[strlen(hozzavalo->nev) - 1] = '\0';

        // ures nev, nincs tobb hozzavalo
        if (strlen(hozzavalo->nev) == 0)
        {
            hozzavalo_free(*hozzavalo);
            break;
        }

        printf("Hozzavalo mennyisege: ");
        fgets(hozzavalo->mennyiseg, MAX_HOZZAVALO_MENNYISEG_HOSSZ, stdin);
        // enter nem kell a vegere
        hozzavalo->mennyiseg[strlen(hozzavalo->mennyiseg) - 1] = '\0';

        recept.hozzavalok_szama++;
    }

    printf("Recept leirasa:\n");

    char *kurzor = recept.leiras;
    while (true)
    {
        fgets(kurzor, 80, stdin);
        kurzor += strlen(kurzor);

        // ket ures sorra kilepunk
        if (*(kurzor - 1) == '\n' && *(kurzor - 2) == '\n')
        {
            *(kurzor - 2) = '\0';
            break;
        }
    }

    return recept;
}


void uj_recept(Recepteskonyv *konyv)
{
    Recept recept = recept_interaktiv_beolvas();

    konyv->receptek_szama++;
    konyv->receptek = realloc(konyv->receptek, konyv->receptek_szama * sizeof(Recept));

    konyv->receptek[konyv->receptek_szama - 1] = recept;
    recept_mentes_fajlba(&recept);
}

void keres(Recepteskonyv *konyv)
{
    char recept_nev[MAX_CIM_HOSSZ];
    bool volt = false;
    do
    {
        printf("Adja meg a recept nevet: \n");
        fgets(recept_nev, MAX_CIM_HOSSZ, stdin);
        // enter nem kell a vegere
        recept_nev[strlen(recept_nev)-1] = '\0';
        for (int i = 0; i < konyv->receptek_szama; ++i)
        {
            if (strcmp(konyv->receptek[i].cim, recept_nev) == 0)
            {
                recept_kiir(&konyv->receptek[i], konyv);
                volt = true;
                return;
            }
        }
        printf("Nincs ilyen recept.\n");
    } while (!volt);
}

Recept *sorsol(Recepteskonyv *konyv)
{
    int random_index = rand() % konyv->receptek_szama;
    return &konyv->receptek[random_index];
}

void nincs_otletem(Recepteskonyv *konyv)
{
    int menu_bemenet;
    bool kilepes = false;
    srand(time(0));
    while (!kilepes)
    {
        Recept *sorsolt_recept = sorsol(konyv); // random struktura

        printf("A sorsolt recept: %s\n", sorsolt_recept->cim);
        printf("[0] <- Megse\n");
        printf("[1] Recept megjelenitese\n");
        printf("[2] Uj recept sorsolasa\n");
        scanf("%d", &menu_bemenet);
        getchar();
        switch (menu_bemenet)
        {
        case 0:
            kilepes = true;
            break;
        case 1:
            recept_kiir(sorsolt_recept, konyv);
            kilepes = true;
            break;
        case 2:
            sorsolt_recept = sorsol(konyv);
            break;
        }
    }
}

void de_ennek_egy_kis(Recepteskonyv *konyv)
{
    printf("[0] <- Vissza\n");
    char bekert_hozzavalo_nev[MAX_HOZZAVALO_NEV_HOSSZ];
    int menu_bemenet;
    int talalatok[MAX_TALALATOK_SZAMA];
    int talalatok_szama = 0;
    printf("De ennek egy kis...\n");
    printf("Osszetevo: \n");
    fgets(bekert_hozzavalo_nev, MAX_HOZZAVALO_NEV_HOSSZ, stdin);
    // a vege a lezaro nulla, mert az fgets entert rak a vegere
    bekert_hozzavalo_nev[strlen(bekert_hozzavalo_nev) - 1] = '\0';

    for (int i = 0; i < konyv->receptek_szama; ++i)
    {
        const Recept *recept = &konyv->receptek[i];
        for (int j = 0; j < recept->hozzavalok_szama; ++j)
        {
            if (strcmp(recept->hozzavalok[j].nev, bekert_hozzavalo_nev) == 0)
            {
                talalatok_szama++;
                talalatok[talalatok_szama - 1] = i;
            }
        }
    }

    if (talalatok_szama == 0)
    {
        printf("Nincs talalat!\n");
        return;
    }

    printf("Valasszon egy receptet!\n");
    printf("[0] Vissza a fomenube\n");
    for (int i = 0; i < talalatok_szama; ++i)
    {
        printf("[%d] %s\n", i+1, konyv->receptek[talalatok[i]].cim);
    }

    do
    {
        scanf("%d", &menu_bemenet);
        getchar();
    } while (menu_bemenet > talalatok_szama);

    if (menu_bemenet != 0)
    {
        int valasztott_index = talalatok[menu_bemenet - 1];
        Recept *valasztott_recept = &konyv->receptek[valasztott_index];
        recept_kiir(valasztott_recept, konyv);
    }
}

// parameter: string tomb nullpointerrel a vegen, recept const struct pointer
bool osszes_hozzavalot_tartalmazza(const char **keresett_hozzavalok, const Recept *recept)
{
    for (const char **keresett_hozzavalo = keresett_hozzavalok; *keresett_hozzavalo != NULL; ++keresett_hozzavalo)
    {
        bool tartalmazza = false;

        for (int j = 0; j < recept->hozzavalok_szama; ++j)
        {
            if (strcmp(recept->hozzavalok[j].nev, *keresett_hozzavalo) == 0)
            {
                tartalmazza = true;
                break;
            }
        }

        if (!tartalmazza)
        {
            return false;
        }
    }

    return true;
}

void el_kell_hasznalni(Recepteskonyv *konyv)
{
    char *keresett_hozzavalok[50] = {0};
    int keresett_hozzavalok_index = 0;
    char buffer[100];
    char *token;
    int talalatok[100];
    int talalatok_szama = 0;
    // fgets, mivel itt meg lehet adni a meretet, es nem tud kiirni a bufferbol
    printf("Adjon meg hozzavalokat, vesszovel elvalasztva! (Pl. alma, rizs)\n");
    printf("El kell hasznalni: \n");
    fgets(buffer, 100, stdin);
    buffer[strlen(buffer) - 1] = '\0';

    token = strtok(buffer, ",");

    while (token != NULL)
    {
        if (token[0] == ' ')
        {
            token = &token[1];
        }
        keresett_hozzavalok[keresett_hozzavalok_index++] = token;
        token = strtok(NULL, ",");
    }

    for (int i = 0; i < konyv->receptek_szama; ++i)
    {
        const Recept *recept = &konyv->receptek[i];
        if (osszes_hozzavalot_tartalmazza((const char **)&keresett_hozzavalok, recept))
        {
            talalatok_szama++;
            talalatok[talalatok_szama - 1] = i;
        }
    }

    if (talalatok_szama == 0)
    {
        printf("Nincs talalat!\n");
        return;
    }

    printf("Valasszon egy receptet!\n");
    printf("[0] Vissza a fomenube\n");

    for (int i = 0; i < talalatok_szama; ++i)
    {
        printf("[%d] %s\n", i+1, konyv->receptek[talalatok[i]].cim);
    }

    int menu_bemenet;
    do
    {
        scanf("%d", &menu_bemenet);
        getchar();
    } while (menu_bemenet > talalatok_szama);

    if (menu_bemenet != 0)
    {
        int valasztott_index = talalatok[menu_bemenet - 1];
        Recept *valasztott_recept = &konyv->receptek[valasztott_index];
        recept_kiir(valasztott_recept, konyv);
    }
}