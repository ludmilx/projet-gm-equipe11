#ifndef TYPES_H
#define TYPES_H

typedef struct {
    char nom[100];
    float duree;
} Examen;

typedef struct {
    char dateDebut[11];
    //char dateFin[11];
    char horaireDebut[6];
    int dureePause;
    int dureeMinParJour;
    int dureeMaxParJour;
} ModalitesExamens;

#endif // TYPES_H

