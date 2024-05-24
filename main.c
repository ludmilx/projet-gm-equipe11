#include "admin.h"
#include "examens.h"
#include "modalites.h"
#include <stdio.h>

int main() {
    Admin admin;
    int loggedIn = 0;

    while (!loggedIn) {
        printf("1. S'inscrire\n2. Se connecter\nChoix: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Veuillez entrer un choix valide.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                registerAdmin(&admin);
                break;
            case 2:
                loggedIn = loginAdmin(&admin);
                break;
            default:
                printf("Choix invalide. Veuillez choisir 1 pour s'inscrire ou 2 pour se connecter.\n");
                break;
        }
    }

    // Ajouter les options de gestion ici après connexion réussie
    int choix;
    do {
        printf("\n1. Ajouter une nouvelle promotion\n");
        printf("2. Ajouter des salles\n");
        printf("3. Planifier des examens\n");
        printf("4. Se déconnecter\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                ajouterPromotion();
                break;
            case 2:
                ajouterSalle();
                break;
            case 3:
                planifier();
                break;
            case 4:
                printf("Déconnexion réussie.\n");
                break;
            default:
                printf("Choix invalide ! Veuillez entrer un nombre entre 1 et 4.\n");
                while (getchar() != '\n');
                break;
        }
    } while (choix != 4);

    return 0;
}

