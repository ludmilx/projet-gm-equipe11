#ifndef MODALITES_H
#define MODALITES_H

#include "types.h"  

void saisirModalitesExamens(ModalitesExamens *modalites);
void planifier_Examens(const char* nomPromo);
void planifierMatieresSpecifiques(const char* nomPromo, const ModalitesExamens modalites);
void chargerExamens(const char *fichier, Examen **examens, int *nbExamens);
int compareDuree(const void *a, const void *b);
void freeExamens(Examen *examens);

#endif // MODALITES_H

