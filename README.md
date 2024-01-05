# Stochastic sudoku solver #

**Name:** Solveur de Sudoku par recuit simulé
**Version:** 0.2

...

## Installation ##

Vous pouvez aussi récupérer la dernière version du projet avec cette commande
```
git clone -b remake https://github.com/Notgard/Stochastic-sudoku-solver.git
```

Pour obtenir les fichiers de tests utiliser dans le répertoire 'ressources', utiliser le script comme ceci pour les télécharger directement :
```
bash ./get_sudoku_files
```

## Utilisation ##

Le Makefile possède différentes options de compilation de 'make', notamment celles pour les différentes versions parallèles :
* 'parallel' : Permet de compiler le programme principal 'sudoku_remake' en utilisant la version modifiée de l'algorithme de recuit simulé avec la parallélisation interne, avec OpenMP.
* 'omp' : Permet de compiler le programme 'sudoku_omp', dans lequel l'algorithme de résolution est lancé sur tous les threads renseignés jusqu'à ce qu'une solution soit trouvée par l'un d'entre eux.
* 'mpi' : Permet de compiler le programme 'sudoku_mpi', dans lequel l'algorithme de résolution est lancé sur tous les processus renseignés jusqu'à ce qu'une solution soit trouvée par l'un d'entre eux.
* 'omp_mpi' : Permet de compiler le programme 'sudoku_mpi' dans lequel la version modifié de l'algorithme de résolution, parallélisé avec OpenMP, est lancé sur tous les processus renseignés jusqu'à ce qu'une solution soit trouvée par l'un d'entre eux.

D'autres options sont présentes, notamment la règle 'show' qui permet d'obtenir une visualisation en temps réel du sudoku, il est recommandé d'utiliser 2 fenêtres du terminal pour exécuter le programme de résolution et de visualisation.

Voici l'ordre d'exécution pour avoir le rendu voulu :
```
make clean && make show
./bin/visualization
```

Et dans une autre fenêtre :
```
./bin/sudoku_remake <sudoku_file> <sudoku_hash>
```

Une utilisation générique du programme est la suivante :
```console
foo@bar:~$ make clean && make

Use: ./bin/sudoku_<remake/mpi/omp> Flags file puzzle
Where :
  file   : The file containing the sudoku puzzles
  puzzle : The hash of the puzzle to solve
Flags :
  -v : Program verbose output

foo@bar:~$ ./bin/sudoku_<remake/mpi/omp> -v "diabolical.txt" "001510809006"
```

Les scripts bash run_sudoku_*** permettent d'effectuer des tests sur les différentes versions du programme :
* La version sequentielle
* Les versions parallèles OMP et MPI

Pour une analyse plus fine du temps d'exécution et de la distribution du coût et du nombre d'essais de l'algorithme de résolution, les tests run_sudoku_tests, run_sudoku_mpi_tests et run_sudoku_omp_tests sont mises en place, permettant de générer automatiquement des graphiques pour visualisation des données.

Les scripts run_sudoku_parallel_test, run_sudoku_mpi_test et run_sudoku_omp_test permettent d'effectuer des comparaisons de temps d'execution avec un sudoku pris au hasard entre la version sequentielle et les versions parallèles avec une génération automatique de graphiques.

Le script run_sudoku_parallel_tests permet de faire ces comparaisons en utilisant plusieurs différents sudokus.

## Configuration ##

De nombreuses options de configurations comme le nombre maximum d'essais de l'algorithme de résolution du sudoku sont disponibles dans le fichier config.h. 

Afin que les modifications faites dans ce fichier soient prises en compte, il est recommandé d'effectuer la commande **make clean** au préalable.

Notamment, une visualisation de l'évolution du coût et de la température lors de l'exécution de l'algorithme est disponible en modifiant le paramètre **GET_STATS** à **true** dans le fichier de configuration config.h.

## Changelog ##

### 0.2 ###
* Version parallèle avec OpenMP et MPI
* Démo avec stats et visualisation du sudoku

### 0.1 ###
* Version séquentielle
