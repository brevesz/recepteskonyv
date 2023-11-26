#include "recepteskonyv.h"
#include "files.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "debugmalloc.h"

int main()
{
    Recepteskonyv konyv = recepteskonyv_new();

    // A receptek betöltése
    if (!osszes_recept_betolt(&konyv))
    {
        perror("Hiba a receptek betoltese kozben");
        recepteskonyv_free(konyv);
        return 1;
    }

    int menu_bemenet;
    bool kilepes = false;

    printf("%s", "\t\t\t===Receptes konyv===\n\n");

    while (!kilepes)
    {
        printf("==Fomenu=========\n");
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
            osszes_recept(&konyv);
            break;
        case 2:
            uj_recept(&konyv);
            break;
        case 3:
            keres(&konyv);
            break;
        case 4:
            nincs_otletem(&konyv);
            break;
        case 5:
            de_ennek_egy_kis(&konyv);
            break;
        case 6:
            el_kell_hasznalni(&konyv);
            break;
        default:
            break;
        }
    }

    // kilepes
    recepteskonyv_free(konyv);

    return 0;
}
