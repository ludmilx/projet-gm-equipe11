# ***Projet de Planification d'Examens***

Ce projet a pour objectif de planifier les examens pour une promotion d'étudiants en fonction de différentes modalités et contraintes.

## Fonctionnalités

- Chargement des examens à partir de fichiers CSV.
- Planification des examens communs et spécifiques.
- Gestion des salles en fonction du nombre d'étudiants et des capacités des salles.
- Génération de fichiers de planning avec les horaires et les salles assignées.

## Structure du Projet

Le projet est composé des fichiers suivants :

- `main.c` : Le fichier principal qui initialise le programme.
- `admin.c` : Contient les fonctions de gestion des utilisateurs et des données administratives.
- `examens.c` : Contient les fonctions de gestion des examens.
- `modalites.c` : Contient les fonctions de gestion des modalités des examens.
- `ING1.csv` : Fichier CSV contenant la liste des étudiants de la promotion ING1.

## Utilisation

### Prérequis

- Un compilateur C (par exemple, `gcc`).
- Les fichiers source (`main.c`, `admin.c`, `examens.c`, `modalites.c`).
- Le fichier CSV `ING1.csv` contenant les informations des étudiants.

### Compilation

Pour compiler le projet, exécutez la commande suivante :

```
make
```

#### Exécution

Pour exécuter le projet, lancez la commande suivante : 
```
./main
```


