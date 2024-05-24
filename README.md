# ***Projet de Planification d'Examens***

Ce projet a pour objectif de planifier les examens pour une promotion d'étudiants en fonction de différentes modalités et contraintes.

## Fonctionnalités
- Inscription et connexion pour l'administrateur.
- Gestion des promotions et de leurs filières ainsi que des salles disponibles pour la période d'examens.
- Chargement des examens à partir de fichiers CSV.
- Planification des examens communs et spécifiques.
- Allocatio, des salles en fonction du nombre d'étudiants et des capacités des salles.
- Génération de fichiers de planning avec les horaires et les salles assignées.

Ce projet respecte avant tout les **contraintes dures** *(non chevauchement des examens, une salle doit être occupée à moitié pour accueillir un examen et les salles doivent être disponibles aux heures programmées pour éviter tout conflit de réservation)* ainsi que les **contraintes molles** *(minimisation des périodes d'attente et équilibrage de la charge de travail)* qui constituent le **cahier des charges** établit par un établissement scolaire et qui a été défini lors du rendu 1 du projet.

## Structure du Projet

Le projet est composé des fichiers suivants :

- `main.c` : Le fichier principal qui initialise le programme.
- `admin.c` : Contient les fonctions de gestion des utilisateurs et des données administratives.
- `examens.c` : Contient les fonctions de gestion des examens.
- `modalites.c` : Contient les fonctions de gestion des modalités des examens.
- `ING1.csv` : Fichier CSV contenant la liste des étudiants de la promotion ING1.

## Utilisation

### Prérequis

- Un compilateur C.
- Les fichiers source (`main.c`, `admin.c`, `examens.c`, `modalites.c`).
- Le fichier CSV `ING1.csv` contenant les informations des étudiants.
- La bibliothèque OpenSSL pour le hachage des mots de passe. Vous pouvez l'installer sur votre système avec la commande suivante :

    **Pour Ubuntu/Debian :**

    ```sh
    sudo apt-get install libssl-dev
    ```

    **Pour macOS avec Homebrew :**

    ```sh
    brew install openssl
    ```

### Compilation

Pour compiler le projet, exécutez la commande suivante :

```
make
```
:warning: La compilation ne fonctionnera pas si la bibliothèque OpenSSL n'est pas préalablement installée !

#### Exécution

Pour exécuter le projet, lancez la commande suivante : 
```
./main
```


