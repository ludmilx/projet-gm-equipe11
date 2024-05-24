#include "admin.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <openssl/evp.h>

void hashPassword(const char* input, char* output) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sha256();
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int length;

    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, input, strlen(input));
    EVP_DigestFinal_ex(mdctx, hash, &length);
    EVP_MD_CTX_free(mdctx);

    for (size_t i = 0; i < length; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[length * 2] = '\0';
}

void registerAdmin(Admin* admin) {
    FILE* file = fopen("admin_credentials.csv", "w");
    char hashedPassword[65];
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        return;
    }

    printf("Entrez le nom d'utilisateur : ");
    scanf("%s", admin->username);
    printf("Entrez le mot de passe : ");
    scanf("%s", admin->password);

    hashPassword(admin->password, hashedPassword);
    fprintf(file, "%s,%s\n", admin->username, hashedPassword);
    fclose(file);
    printf("Administrateur enregistré avec succès.\n");
}

int loginAdmin(Admin* admin) {
    char username[100], password[100], hashedPassword[65];
    FILE* file = fopen("admin_credentials.csv", "r");
    if (file == NULL) {
        printf("Fichier de credentials non trouvé.\n");
        return 0;
    }

    printf("Nom d'utilisateur : ");
    scanf("%s", username);
    printf("Mot de passe : ");
    scanf("%s", password);

    hashPassword(password, hashedPassword);

    while (fscanf(file, "%[^,],%s", admin->username, admin->password) != EOF) {
        if (strcmp(username, admin->username) == 0 && strcmp(hashedPassword, admin->password) == 0) {
            fclose(file);
            printf("Connexion réussie !\n");
            return 1;
        }
    }

    fclose(file);
    printf("Identifiant ou mot de passe incorrect !\n");
    return 0;
}


void ajouterPromotion() {
    char nomPromo[100];
    printf("Entrez le nom de la nouvelle promotion : ");
    scanf("%s", nomPromo);

    char fileName[120];
    sprintf(fileName, "%s.csv", nomPromo);
    FILE* file = fopen(fileName, "r");
    if (file) {
        printf("Une promotion avec ce nom existe déjà.\n");
        fclose(file);
        return;
    }

    file = fopen(fileName, "w");
    if (!file) {
        printf("Erreur lors de la création du fichier pour la promotion.\n");
        return;
    }
    fclose(file);

    printf("Promotion '%s' créée avec succès.\n", nomPromo);
    managePromotion(nomPromo);
}

void managePromotion(char* promoName) {
    char fileName[120];
    sprintf(fileName, "%s.csv", promoName);
    FILE* file = fopen(fileName, "a+");

    if (!file) {
        printf("Erreur lors de l'ouverture ou de la création du fichier %s\n", fileName);
        return;
    }

    char className[100];
    int numStudents, studentId;
    int moreClasses = 1;
   // char line[1024];

    while (moreClasses) {
        printf("Entrez le nom de la filière : ");
        scanf("%s", className);
        if (classNameExists(file, className)) {
            printf("Une filière avec ce nom existe déjà dans cette promotion. Veuillez essayer un autre nom.\n");
            continue;
        }

        printf("Entrez le nombre d'élèves dans %s: ", className);
        scanf("%d", &numStudents);

        for (int i = 0; i < numStudents; i++) {
            while (1) { // Boucle jusqu'à ce qu'un numéro valide et unique soit saisi
                printf("Entrez le numéro étudiant (8 chiffres) pour l'élève %d: ", i + 1);
                scanf("%d", &studentId);
                if (studentId < 10000000 || studentId > 99999999) {
                    printf("Numéro invalide. Le numéro étudiant doit être de 8 chiffres. Réessayez.\n");
                } else if (!isStudentIdUnique(file, studentId)) {
                    printf("Erreur : Le numéro étudiant %d est déjà utilisé. Veuillez entrer un numéro unique.\n", studentId);
                } else {
                    break; // Sortie de la boucle si le numéro est valide et unique
                }
            }
            fprintf(file, "%s, %d\n", className, studentId);
        }

        printf("Voulez-vous ajouter une autre filière à la promotion %s ? (1 pour oui, 0 pour non): ", promoName);
        scanf("%d", &moreClasses);
    }

    fclose(file);
}

int classNameExists(FILE* file, const char* className) {
    rewind(file);
    char line[256], existingClassName[100];

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],", existingClassName);
        if (strcmp(existingClassName, className) == 0) {
            return 1; // Class name exists
        }
    }
    return 0; // Class name does not exist
}

int isStudentIdUnique(FILE* file, int studentId) {
    rewind(file);
    char line[1024];
    int existingId;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%*[^,], %d", &existingId) == 1) {
            if (existingId == studentId) {
                return 0; // Found a match, not unique
            }
        }
    }
    return 1; // Unique
}

void ajouterSalle() {
    FILE *file = fopen("salles.csv", "a+");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier salles.csv\n");
        return;
    }
    
    char nomSalle[100], line[1024];
    int capacite;

    // Demande du nom de la salle
    printf("Entrez le nom de la salle ou de l'amphithéâtre : ");
    scanf("%99s", nomSalle);

    // Vérifier l'unicité du nom de la salle
    while (fgets(line, sizeof(line), file)) {
        char existingName[100];
        sscanf(line, "%99[^,],", existingName); // Extraire le nom de la salle existante
        if (strcmp(nomSalle, existingName) == 0) {
            printf("Une salle avec ce nom existe déjà.\n");
            fclose(file);
            return;
        }
    }

    // Demande de la capacité de la salle avec validation
    while (1) {
        printf("Entrez la capacité de la salle (doit être un nombre entier positif) : ");
        if (scanf("%d", &capacite) == 1 && capacite > 0) {
            break; // Si l'entrée est valide, sortir de la boucle
        } else {
            printf("Erreur : Veuillez entrer un entier positif.\n");
            while (getchar() != '\n'); // Nettoie le buffer d'entrée en cas d'entrée invalide
        }
    }

    // Positionner le curseur à la fin du fichier pour ajouter la nouvelle salle
    fseek(file, 0, SEEK_END);
    fprintf(file, "%s,%d\n", nomSalle, capacite);
    printf("Salle %s avec capacité %d ajoutée avec succès.\n", nomSalle, capacite);
    fclose(file);
}
