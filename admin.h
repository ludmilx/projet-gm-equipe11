#ifndef ADMIN_H
#define ADMIN_H

#include <stdio.h>  // Inclure pour le type FILE

// Structure pour stocker les informations de l'administrateur
typedef struct {
    char username[100];
    char password[100];
} Admin;

// Prototypes des fonctions pour la gestion des administrateurs
void registerAdmin(Admin* admin);
int loginAdmin(Admin* admin);

// Fonctions pour gérer les données principales du système
void ajouterPromotion();
void gererPromotionExistante();
void managePromotion(char* promoName);
void ajouterSalle();
void logoutAdmin();

// Helper functions for internal checks
int isStudentIdUnique(FILE* file, int studentId);
int classNameExists(FILE* file, const char* className);

#endif // ADMIN_H

