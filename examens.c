#include "examens.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Helper functions
int verifierPromotion(const char *nomPromo);
int verifierClasse(const char *nomPromo, const char *nomClasse);
void saisirMatieresSpecifiques(const char *nomPromo);
void saisirMatieresCommunes(const char *nomPromo);
float saisirDureeExamen();
void clearInputBuffer();
extern void planifierExamens(const char* nomPromo); 

void saisirModalitesExamens(ModalitesExamens *modalites) {
    printf("Entrez la date de début des examens (JJ/MM/AAAA) : ");
    scanf("%s", modalites->dateDebut);
   /* printf("Entrez la date de fin des examens (JJ/MM/AAAA) : ");
    scanf("%s", modalites->dateFin);*/
    printf("Entrez l'horaire de début des examens (HH:MM) : ");
    scanf("%s", modalites->horaireDebut);
    printf("Entrez la durée de la pause entre les examens en minutes : ");
    scanf("%d", &modalites->dureePause);
    printf("Entrez la durée minimale des examens par jour en minutes : ");
    scanf("%d", &modalites->dureeMinParJour);
    printf("Entrez la durée maximale des examens par jour en minutes : ");
    scanf("%d", &modalites->dureeMaxParJour);
}

int verifierPromotion(const char *nomPromo) {
    char fileName[256];
    sprintf(fileName, "%s.csv", nomPromo);
    FILE *file = fopen(fileName, "r");
    if (!file) {
        return 0;
    }
    fclose(file);
    return 1;
}

void planifier() {
    char nomPromo[100];
    do {
        printf("Entrez le nom de la promotion pour laquelle planifier les examens : ");
        scanf("%s", nomPromo);
        clearInputBuffer(); // Nettoyer le buffer d'entrée après la saisie du nom de la promotion
        if (!verifierPromotion(nomPromo)) {
            printf("Cette promotion n'existe pas. Veuillez réessayer.\n");
        }
    } while (!verifierPromotion(nomPromo));

    saisirMatieresCommunes(nomPromo);
    saisirMatieresSpecifiques(nomPromo);
    
    planifierExamens(nomPromo);
}


int verifierClasse(const char *nomPromo, const char *nomClasse) {
    char fileName[256], line[256], className[100];
    sprintf(fileName, "%s.csv", nomPromo);
    FILE *file = fopen(fileName, "r");
    if (!file) return 0;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],", className);
        if (strcmp(className, nomClasse) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

float saisirDureeExamen() {
    float duree;
    printf("Entrez la durée (tiers temps compris) de l'examen en minutes : ");
    scanf("%f", &duree);
    clearInputBuffer(); // Nettoyer le buffer d'entrée
    return duree;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void saisirMatieresSpecifiques(const char *nomPromo) {
    char nomClasse[100], nomMatiere[100], fileName[256];
    FILE *classeFile;
    float duree;

    printf("Saisie des matières spécifiques par filière :\n");
    while (1) {
        printf("Entrez le nom de la filière (ou 'fin' pour terminer) : ");
        scanf("%s", nomClasse);
        clearInputBuffer(); // Nettoyer le buffer d'entrée après la saisie du nom de la classe
        if (strcmp(nomClasse, "fin") == 0) break;

        if (!verifierClasse(nomPromo, nomClasse)) {
            printf("La filière n'existe pas dans la promotion.\n");
            continue;
        }

        sprintf(fileName, "%s_matieres_specifiques.csv", nomClasse);
        classeFile = fopen(fileName, "a+"); // Fichier par classe pour les matières spécifiques
        if (!classeFile) {
            printf("Erreur lors de la création du fichier pour la classe %s.\n", nomClasse);
            continue;
        }

        while (1) {
            printf("Entrez le nom de la matière pour %s (ou 'fin' pour terminer) : ", nomClasse);
            scanf("%s", nomMatiere);
            clearInputBuffer(); // Nettoyer le buffer d'entrée après la saisie du nom de la matière
            if (strcmp(nomMatiere, "fin") == 0) break;

            printf("Entrez la durée (tiers temps compris) de l'examen en minutes pour %s : ", nomMatiere);
            scanf("%f", &duree);
            clearInputBuffer(); // Nettoyer le buffer d'entrée après la saisie de la durée
            fprintf(classeFile, "%s,%.2f\n", nomMatiere, duree);
        }

        fclose(classeFile);
    }
}


void saisirMatieresCommunes(const char *nomPromo) {
    char nomMatiere[100], fileName[120];

    sprintf(fileName, "%s_matieres_communes.csv", nomPromo);
    FILE *file = fopen(fileName, "a+");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier pour les matières communes.\n");
        return;
    }

    printf("Saisie des matières communes à la promotion :\n");
    while (1) {
        printf("Entrez le nom de la matière commune (ou 'fin' pour terminer) : ");
        scanf("%s", nomMatiere);
        clearInputBuffer(); // Nettoyer le buffer d'entrée après la saisie du nom de la matière
        if (strcmp(nomMatiere, "fin") == 0) break;

        float duree = saisirDureeExamen(); // Saisie de la durée de l'examen
        fprintf(file, "%s,%f\n", nomMatiere, duree);
    }
    fclose(file);
}

