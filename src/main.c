#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "debugmalloc.h"

#define MAX_CIM_HOSSZ 120
#define MAX_LEIRAS_HOSSZ 5000
#define MAX_HOZZAVALO_NEV_HOSSZ 50
#define MAX_HOZZAVALO_MENNYISEG_HOSSZ 50
#define MAX_HOZZAVALOK 30

const char *RECEPTEK_MAPPA = "receptek";

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

Recept *receptek;
int receptek_szama = 0;

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

void recept_torles(Recept *recept)
{
    int recept_index = recept - receptek;
    recept_free(receptek[recept_index]);

    for (int i = recept_index; i < receptek_szama - 1; ++i)
    {
        receptek[i] = receptek[i + 1];
    }
    receptek_szama--;
    printf("Recept torolve.\n");
}

void recept_kiir(Recept *recept)
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
            recept_torles(recept);
            return;
        default:
            break;
        }
    }
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

    Recept *recept = &receptek[recept_sorszam - 1];
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

bool osszes_recept_betolt()
{
    DIR *mappa;
    struct dirent *elem;

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
            receptek_szama++;
            receptek = realloc(receptek, receptek_szama * sizeof(Recept));
            receptek[receptek_szama - 1] = recept_new();
            if (!recept_beolvas_fajlbol(fajlnev, &receptek[receptek_szama - 1]))
            {
                // hiba beolvasas kozben
                closedir(mappa);
                chdir("..");
                return false;
            };
        }
    }

    closedir(mappa);
    chdir("..");
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
    // enter nem kell a vegere
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

void uj_recept()
{
    Recept recept = recept_interaktiv_beolvas();

    receptek_szama++;
    receptek = realloc(receptek, receptek_szama * sizeof(Recept));

    receptek[receptek_szama - 1] = recept;
    recept_mentes_fajlba(&recept);
}

void takaritas()
{
    for (int i = 0; i < receptek_szama; ++i)
    {
        recept_free(receptek[i]);
    }

    free(receptek);
}

void keres()
{
    char recept_nev[MAX_CIM_HOSSZ];
    bool volt = false;
    do
    {
        printf("Adja meg a recept nevet: \n");
        scanf("%s", recept_nev);
        getchar();
        for (int i = 0; i < receptek_szama; ++i)
        {
            if (strcmp(receptek[i].cim, recept_nev) == 0)
            {
                recept_kiir(&receptek[i]);
                volt = true;
                return;
            }
        }
        printf("Nincs ilyen recept.\n");
    } while (!volt);
}

Recept *sorsol()
{
    int random_index = rand() % receptek_szama;
    return &receptek[random_index];
}

void nincs_otletem()
{
    int menu_bemenet;
    bool kilepes = false;
    srand(time(0));
    while (!kilepes)
    {
        Recept *sorsolt_recept = sorsol(); // random struktura

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
            recept_kiir(sorsolt_recept);
            kilepes = true;
            break;
        case 2:
            sorsolt_recept = sorsol();
            break;
        }
    }
}

void de_ennek_egy_kis()
{
    printf("[0] <- Vissza\n");
    char bekert_hozzavalo_nev[MAX_HOZZAVALO_NEV_HOSSZ];
    int menu_bemenet;
    int recept_indexek[100];
    int talalatok_szama = 0;
    printf("De ennek egy kis...\n");
    printf("Osszetevo: \n");
    fgets(bekert_hozzavalo_nev, MAX_HOZZAVALO_NEV_HOSSZ, stdin);
    // a vege a lezaro nulla, mert az fgets entert rak a vegere
    bekert_hozzavalo_nev[strlen(bekert_hozzavalo_nev) - 1] = '\0';
    printf("Valasszon egy receptet!\n");
    for (int i = 0; i < receptek_szama; ++i)
    {
        for (int j = 0; j < receptek[i].hozzavalok_szama; ++j)
        {
            if (strcmp(receptek[i].hozzavalok[j].nev, bekert_hozzavalo_nev) == 0)
            {
                talalatok_szama++;
                printf("[%d] %s\n", talalatok_szama, receptek[i].cim);
                recept_indexek[talalatok_szama - 1] = i;
            }
        }
    }
    if (talalatok_szama == 0)
    {
        printf("Nincs talalat!\n");
    }
    do
    {
        scanf("%d", &menu_bemenet);
        getchar();
    } while (menu_bemenet > talalatok_szama);

    if (menu_bemenet != 0)
        recept_kiir(&receptek[recept_indexek[menu_bemenet - 1]]);
}

// parameter: string tomb nullpointerrel a vegen, recept const struct pointer
bool osszes_hozzavalot_tartalmazza(const char **keresett_hozzavalok, const Recept *recept)
{
    for (const char **keresett_hozzavalo = keresett_hozzavalok; keresett_hozzavalo != NULL; ++keresett_hozzavalo)
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

void el_kell_hasznalni()
{
    char *keresett_hozzavalok[50];
    int keresett_hozzavalok_index = 0;
    char buffer[100];
    const char elvalaszto[3] = ",";
    char *token;
    int recept_indexek[100];
    int talalatok_szama = 0;
    // fgets, mivel itt meg lehet adni a meretet, es nem tud kiirni a bufferbol
    printf("Adjon meg hozzavalokat, vesszovel elvalasztva! (Pl. alma, rizs)\n");
    printf("El kell hasznalni: \n");
    fgets(buffer, 100, stdin);
    buffer[strlen(buffer) - 1] = '\0';

    token = strtok(buffer, elvalaszto);

    while (token != NULL)
    {
        token = strtok(NULL, elvalaszto);
        if (token[0] == ' ')
        {
            token = &token[1];
        }
        keresett_hozzavalok[keresett_hozzavalok_index++] = token;
    }

    for (int i = 0; i < receptek_szama; ++i)
    {
        if (osszes_hozzavalot_tartalmazza((const char **)&keresett_hozzavalok, &receptek[i]))
        {
            talalatok_szama++;
            printf("[%d] %s\n", talalatok_szama, receptek[i].cim);
            recept_indexek[talalatok_szama - 1] = i;
        }
    }

    if (talalatok_szama == 0)
    {
        printf("Nincs talalat!\n");
    }

    printf("Valasszon egy receptet!\n");
    int menu_bemenet;
    do
    {
        scanf("%d", &menu_bemenet);
        getchar();
    } while (menu_bemenet > talalatok_szama);

    if (menu_bemenet != 0)
    {
        recept_kiir(&receptek[recept_indexek[menu_bemenet - 1]]);
    }
}

// Az osszes nezetbe kellene vissza gomb
int main()
{
    receptek = malloc(sizeof(Recept));

    if (!osszes_recept_betolt())
    {
        perror("Hiba a receptek betoltese kozben");
        takaritas();
        return 1;
    }

    int menu_bemenet;
    bool kilepes = false;

    printf("%s", "\t\t\t\tReceptes konyv\n\n");

    while (!kilepes)
    {
        printf("Fomenu\n");
        printf("[1] Osszes recept\n");
        printf("[2] Uj recept\n");
        printf("[3] Kereses\n");
        printf("[4] Nincs otletem\n");
        printf("[5] De ennek egy kis...\n");
        printf("[6] El kell hasznalni\n");
        printf("[0] Kilepes\n");

        scanf("%d", &menu_bemenet);
        getchar();

        switch (menu_bemenet)
        {
        case 0:
            kilepes = true;
            break;
        case 1:
            osszes_recept();
            break;
        case 2:
            uj_recept();
            break;
        case 3:
            keres();
            break;
        case 4:
            nincs_otletem();
            break;
        case 5:
            de_ennek_egy_kis();
            break;
        case 6:
            el_kell_hasznalni();
            break;
        default:
            break;
        }
    }

    // kilepes
    takaritas();

    return 0;
}
