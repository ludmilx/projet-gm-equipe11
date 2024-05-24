#define _GNU_SOURCE // Pour s'assurer que strdup est disponible

#include "modalites.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

typedef struct {
    char date[11]; // Format JJ/MM/AAAA
    char heureDebut[6]; // Format HH:MM
    char heureFin[6]; // Format HH:MM
    char filiere[100];
    char examenNom[100];
    char salles[256];
} JourPlanifie;

typedef struct {
    char nom[100];
    int capacite;
    int capaciteUtilisee;
} Salle;

void chargerExamens(const char* nomFichier, Examen** examens, int* nombre);
void planifierExamensCommuns(Examen* examens, int nombre, const ModalitesExamens modalites, const char* nomFichier, int* jour, int* heureDebut, int* finJournee, struct tm* dateDebut, Salle* salles, int nombreSalles, int nombreEtudiants);
void planifierExamensSpecifiques(Examen* examens, int nombre, const ModalitesExamens modalites, const char* nomFichier, int* jour, int heureDebut, const char* filiere, struct tm* dateDebut, Salle* salles, int nombreSalles, const char* nomPromotion);
int compareDuree(const void *a, const void *b);
void freeExamens(Examen* examens);
char* trim(char* str);
void afficherPlanningFinal(const char* nomPromotion, const struct tm* dateDebut);
void chargerSalles(const char* nomFichier, Salle** salles, int* nombre);
void assignerSalles(char* sallesAssignées, Salle* salles, int nombreSalles, int nombreEtudiants);
int compterEtudiants(const char* nomPromotion);
int compterEtudiantsFiliere(const char* nomPromotion, const char* filiere);

void planifierExamens(const char* nomPromotion) {
    int jour = 0;
    int dernierJourCommun = 0;

    Examen* examensCommuns = NULL;
    int nbExamensCommuns = 0;
    char fichier[256];

    sprintf(fichier, "%s_matieres_communes.csv", nomPromotion);
    chargerExamens(fichier, &examensCommuns, &nbExamensCommuns);

    ModalitesExamens modalites;
    saisirModalitesExamens(&modalites);

    // Date de début des examens
    struct tm dateDebut = {0};
    strptime(modalites.dateDebut, "%d/%m/%Y", &dateDebut);

    char horaireDebutCpy[6];
    strncpy(horaireDebutCpy, modalites.horaireDebut, sizeof(horaireDebutCpy));
    int heureDebut = atoi(strtok(horaireDebutCpy, ":")) * 60 + atoi(strtok(NULL, ":")); // Convert HH:MM to minutes
    int finJournee = 18 * 60 + 30; // Limite de fin de journée à 18:30

    Salle* salles = NULL;
    int nombreSalles = 0;
    chargerSalles("salles.csv", &salles, &nombreSalles);

    int nombreEtudiants = compterEtudiants(nomPromotion);

    if (examensCommuns && nbExamensCommuns > 0) {
        qsort(examensCommuns, nbExamensCommuns, sizeof(Examen), compareDuree);
        sprintf(fichier, "%s_planning_communs.csv", nomPromotion);
        planifierExamensCommuns(examensCommuns, nbExamensCommuns, modalites, fichier, &jour, &heureDebut, &finJournee, &dateDebut, salles, nombreSalles, nombreEtudiants);
        dernierJourCommun = jour;
        freeExamens(examensCommuns);
    }

    // Planifier les examens spécifiques par filière
    char promoFilePath[256];
    sprintf(promoFilePath, "%s.csv", nomPromotion);
    FILE* promoFile = fopen(promoFilePath, "r");
    if (!promoFile) {
        printf("Erreur lors de l'ouverture du fichier de promotion %s.csv.\n", nomPromotion);
        return;
    }

    char line[1024];
    char prevClasse[100] = "";
    while (fgets(line, sizeof(line), promoFile)) {
        char* nomClasse = strtok(line, ",");
        if (nomClasse != NULL && strcmp(nomClasse, prevClasse) != 0) {
            strcpy(prevClasse, nomClasse);
            nomClasse = trim(nomClasse); // Assurer l'absence d'espaces superflus
            sprintf(fichier, "%s_matieres_specifiques.csv", nomClasse);
            Examen* examensSpecifiques = NULL;
            int nbExamensSpecifiques = 0;
            chargerExamens(fichier, &examensSpecifiques, &nbExamensSpecifiques);
            if (examensSpecifiques && nbExamensSpecifiques > 0) {
                qsort(examensSpecifiques, nbExamensSpecifiques, sizeof(Examen), compareDuree);
                // Reprendre à partir du lendemain du dernier examen commun pour chaque filière
                int jourSpecifique = dernierJourCommun + 1;
                int heureDebutSpecifique = atoi(strtok(strdup(modalites.horaireDebut), ":")) * 60 + atoi(strtok(NULL, ":"));
                sprintf(fichier, "%s_planning_%s.csv", nomPromotion, nomClasse);
                planifierExamensSpecifiques(examensSpecifiques, nbExamensSpecifiques, modalites, fichier, &jourSpecifique, heureDebutSpecifique, nomClasse, &dateDebut, salles, nombreSalles, nomPromotion);
                freeExamens(examensSpecifiques);
            } else {
                printf("Erreur lors de l'ouverture du fichier %s\n", fichier);
            }
        }
    }
    fclose(promoFile);

    // Afficher le planning final des examens en lisant les fichiers intermédiaires
    printf("\nPlanning final des examens :\n");
    afficherPlanningFinal(nomPromotion, &dateDebut);
}

char* trim(char* str) {
    char* end;

    // Éliminer les espaces de début
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // Si la chaîne est vide
        return str;

    // Éliminer les espaces de fin
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Écrire le nouveau caractère de terminaison null
    end[1] = '\0';

    return str;
}

void planifierExamensCommuns(Examen* examens, int nombre, const ModalitesExamens modalites, const char* nomFichier, int* jour, int* heureDebut, int* finJournee, struct tm* dateDebut, Salle* salles, int nombreSalles, int nombreEtudiants) {
    int tempsMax = modalites.dureeMaxParJour * 60; // Convertir en minutes
    int tempsTotal = 0;
    int debutJournee = *heureDebut;
    FILE* fichier = fopen(nomFichier, "w");

    int i = 0;
    while (i < nombre) {
        int dureeExam = (int)examens[i].duree;
        int finExamen = debutJournee + dureeExam;

        // Réinitialiser les salles
        for (int j = 0; j < nombreSalles; j++) {
            salles[j].capaciteUtilisee = 0;
        }

        // Vérifier si l'examen peut être planifié sans dépasser la fin de journée ni la durée maximale par jour
        if (finExamen <= *finJournee && tempsTotal + dureeExam <= tempsMax) {
            struct tm date = *dateDebut;
            date.tm_mday += *jour;
            mktime(&date); // Normaliser la structure tm

            char sallesAssignées[256];
            assignerSalles(sallesAssignées, salles, nombreSalles, nombreEtudiants);

            fprintf(fichier, "%d,%02d:%02d à %02d:%02d,%s,Matières communes,%s\n",
                    *jour + 1, debutJournee / 60, debutJournee % 60, finExamen / 60, finExamen % 60, examens[i].nom, sallesAssignées);

            printf("Examen planifié: %s\nJour: %d, Heure de début: %02d:%02d, Heure de fin: %02d:%02d, Filière: Matières communes, Salles: %s\n",
                   examens[i].nom, *jour, debutJournee / 60, debutJournee % 60, finExamen / 60, finExamen % 60, sallesAssignées);

            debutJournee = finExamen + modalites.dureePause;
            tempsTotal += dureeExam;
            i++;
        } else {
            // Passer au jour suivant
            *jour += 1;
            char* horaireDebutDup = strdup(modalites.horaireDebut);
            debutJournee = atoi(strtok(horaireDebutDup, ":")) * 60 + atoi(strtok(NULL, ":")); // Réinitialiser l'heure de début au début de la journée
            free(horaireDebutDup);
            tempsTotal = 0;
        }
    }
    fclose(fichier);
    *heureDebut = debutJournee; // Sauvegarder l'heure de fin du dernier examen
}

void planifierExamensSpecifiques(Examen* examens, int nombre, const ModalitesExamens modalites, const char* nomFichier, int* jour, int heureDebut, const char* filiere, struct tm* dateDebut, Salle* salles, int nombreSalles, const char* nomPromotion) {
    int tempsMax = modalites.dureeMaxParJour * 60; // Convertir en minutes
    int finJournee = 18 * 60 + 30; // Limite de fin de journée à 18:30
    int tempsTotal = 0;
    int debutJournee = heureDebut;
    FILE* fichier = fopen(nomFichier, "w");

    int nombreEtudiants = compterEtudiantsFiliere(nomPromotion, filiere);

    int i = 0;
    while (i < nombre) {
        int dureeExam = (int)examens[i].duree;
        int finExamen = debutJournee + dureeExam;

        // Réinitialiser les salles
        for (int j = 0; j < nombreSalles; j++) {
            salles[j].capaciteUtilisee = 0;
        }

        // Vérifier si l'examen peut être planifié sans dépasser la fin de journée ni la durée maximale par jour
        if (finExamen <= finJournee && tempsTotal + dureeExam <= tempsMax) {
            struct tm date = *dateDebut;
            date.tm_mday += *jour;
            mktime(&date); // Normaliser la structure tm

            char sallesAssignees[256];
            assignerSalles(sallesAssignees, salles, nombreSalles, nombreEtudiants);

            fprintf(fichier, "%d,%02d:%02d à %02d:%02d,%s,Filière %s,%s\n",
                    *jour + 1, debutJournee / 60, debutJournee % 60, finExamen / 60, finExamen % 60, examens[i].nom, filiere, sallesAssignees);

            printf("Examen planifié: %s\nJour: %d, Heure de début: %02d:%02d, Heure de fin: %02d:%02d, Filière: %s, Salles: %s\n",
                   examens[i].nom, *jour, debutJournee / 60, debutJournee % 60, finExamen / 60, finExamen % 60, filiere, sallesAssignees);

            debutJournee = finExamen + modalites.dureePause;
            tempsTotal += dureeExam;
            i++;
        } else {
            // Passer au jour suivant
            *jour += 1;
            char* horaireDebutDup = strdup(modalites.horaireDebut);
            debutJournee = atoi(strtok(horaireDebutDup, ":")) * 60 + atoi(strtok(NULL, ":")); // Réinitialiser l'heure de début au début de la journée
            free(horaireDebutDup);
            tempsTotal = 0;
        }
    }
    fclose(fichier);
}

void afficherPlanningFinal(const char* nomPromotion, const struct tm* dateDebut) {
    char fichier[256];
    char line[256];
    int jourMax = 0;
    struct tm date = *dateDebut;

    // Structure pour stocker les examens par jour
    struct {
        int jour;
        struct tm date;
        char examens[100][256];
        int nbExamens;
    } planning[100] = {0};

    // Lire le fichier des matières communes
    sprintf(fichier, "%s_planning_communs.csv", nomPromotion);
    FILE* fichierCommuns = fopen(fichier, "r");
    if (fichierCommuns) {
        while (fgets(line, sizeof(line), fichierCommuns)) {
            int jourFichier;
            char horaire[20], matiere[100], filiere[100], salles[256];
            sscanf(line, "%d,%[^,],%[^,],%[^,],%[^\n]", &jourFichier, horaire, matiere, filiere, salles);

            jourMax = jourFichier > jourMax ? jourFichier : jourMax;

            sprintf(planning[jourFichier - 1].examens[planning[jourFichier - 1].nbExamens++], "  - Examen : %s\n    Heure de l'examen : %s\n    Filières concernées : %s\n    Salles : %s\n", matiere, horaire, filiere, salles);
            planning[jourFichier - 1].jour = jourFichier;
        }
        fclose(fichierCommuns);
    }

    // Lire les fichiers des matières spécifiques
    sprintf(fichier, "%s.csv", nomPromotion);
    FILE* promoFile = fopen(fichier, "r");
    if (promoFile) {
        char prevClasse[100] = "";
        while (fgets(line, sizeof(line), promoFile)) {
            char* nomClasse = strtok(line, ",");
            if (nomClasse != NULL && strcmp(nomClasse, prevClasse) != 0) {
                strcpy(prevClasse, nomClasse);
                nomClasse = trim(nomClasse);

                sprintf(fichier, "%s_planning_%s.csv", nomPromotion, nomClasse);
                FILE* fichierSpecifiques = fopen(fichier, "r");
                if (fichierSpecifiques) {
                    while (fgets(line, sizeof(line), fichierSpecifiques)) {
                        int jourFichier;
                        char horaire[20], matiere[100], filiere[100], salles[256];
                        sscanf(line, "%d,%[^,],%[^,],%[^,],%[^\n]", &jourFichier, horaire, matiere, filiere, salles);

                        jourMax = jourFichier > jourMax ? jourFichier : jourMax;

                        sprintf(planning[jourFichier - 1].examens[planning[jourFichier - 1].nbExamens++], "  - Examen : %s\n    Heure de l'examen : %s\n    Filières concernées : %s\n    Salles : %s\n", matiere, horaire, filiere, salles);
                        planning[jourFichier - 1].jour = jourFichier;
                    }
                    fclose(fichierSpecifiques);
                }
            }
        }
        fclose(promoFile);
    }

    // Afficher le planning regroupé par jour
    for (int i = 0; i < jourMax; i++) {
        if (planning[i].nbExamens > 0) {
            date.tm_mday = dateDebut->tm_mday + planning[i].jour - 1;
            mktime(&date); // Normaliser la structure tm
            printf("Jour %d (%02d/%02d/%04d) :\n", planning[i].jour, date.tm_mday, date.tm_mon + 1, date.tm_year + 1900);
            for (int j = 0; j < planning[i].nbExamens; j++) {
                printf("%s", planning[i].examens[j]);
            }
            printf("\n");
        }
    }
}

void chargerExamens(const char* nomFichier, Examen** examens, int* nombre) {
    FILE* fichier = fopen(nomFichier, "r");
    if (!fichier) {
        printf("Erreur lors de l'ouverture du fichier %s\n", nomFichier);
        *examens = NULL;
        *nombre = 0;
        return;
    }

    char ligne[256];
    int lignes = 0;
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (strchr(ligne, ',') != NULL) lignes++;
    }
    rewind(fichier);

    *examens = malloc(lignes * sizeof(Examen));
    if (*examens == NULL) {
        fclose(fichier);
        *nombre = 0;
        return;
    }

    *nombre = lignes;
    int i = 0;
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (sscanf(ligne, "%99[^,],%f", (*examens)[i].nom, &(*examens)[i].duree) == 2) {
            i++;
        }
    }
    fclose(fichier);
}

int compareDuree(const void *a, const void *b) {
    const Examen *examenA = (const Examen *)a;
    const Examen *examenB = (const Examen *)b;
    return (int)(examenB->duree - examenA->duree);
}

void freeExamens(Examen* examens) {
    if (examens != NULL) {
        free(examens);
    }
}

void chargerSalles(const char* nomFichier, Salle** salles, int* nombre) {
    FILE* fichier = fopen(nomFichier, "r");
    if (!fichier) {
        printf("Erreur lors de l'ouverture du fichier %s\n", nomFichier);
        *salles = NULL;
        *nombre = 0;
        return;
    }

    char ligne[256];
    int lignes = 0;
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (strchr(ligne, ',') != NULL) lignes++;
    }
    rewind(fichier);

    *salles = malloc(lignes * sizeof(Salle));
    if (*salles == NULL) {
        fclose(fichier);
        *nombre = 0;
        return;
    }

    *nombre = lignes;
    int i = 0;
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (sscanf(ligne, "%99[^,],%d", (*salles)[i].nom, &(*salles)[i].capacite) == 2) {
            i++;
        }
    }
    fclose(fichier);
}

void assignerSalles(char* sallesAssignées, Salle* salles, int nombreSalles, int nombreEtudiants) {
    int etudiantsRestants = nombreEtudiants;
    sallesAssignées[0] = '\0';

    for (int i = 0; i < nombreSalles && etudiantsRestants > 0; i++) {
        int capaciteUtilisee = (salles[i].capacite / 2 < etudiantsRestants) ? salles[i].capacite / 2 : etudiantsRestants;
        etudiantsRestants -= capaciteUtilisee;
        salles[i].capaciteUtilisee += capaciteUtilisee;

        char salleInfo[150];
        snprintf(salleInfo, sizeof(salleInfo), "%s (%d)", salles[i].nom, capaciteUtilisee);
        if (i > 0) {
            strcat(sallesAssignées, ", ");
        }
        strcat(sallesAssignées, salleInfo);
    }
}

int compterEtudiants(const char* nomPromotion) {
    char fichier[256];
    sprintf(fichier, "%s.csv", nomPromotion);
    FILE* promoFile = fopen(fichier, "r");
    if (!promoFile) {
        printf("Erreur lors de l'ouverture du fichier de promotion %s.csv.\n", nomPromotion);
        return 0;
    }

    int count = 0;
    char line[1024];
    while (fgets(line, sizeof(line), promoFile)) {
        count++;
    }
    fclose(promoFile);
    return count;
}

int compterEtudiantsFiliere(const char* nomPromotion, const char* filiere) {
    char fichier[256];
    sprintf(fichier, "%s.csv", nomPromotion);
    FILE* promoFile = fopen(fichier, "r");
    if (!promoFile) {
        printf("Erreur lors de l'ouverture du fichier de promotion %s.csv.\n", nomPromotion);
        return 0;
    }

    int count = 0;
    char line[1024];
    while (fgets(line, sizeof(line), promoFile)) {
        if (strstr(line, filiere) != NULL) {
            count++;
        }
    }
    fclose(promoFile);
    return count;
}

