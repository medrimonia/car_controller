Car Controller
==============

# Dépendances

```
sudo apt install cmake libopencv-dev
```

# Contexte

Ce code est utilisé au sein de l'UE *Projets Technologiques* dispensée en L3 à
*l'Université de Bordeaux*. Il permet de contrôler un robot holonome en envoyant
des commandes à l'aide de la bibliothèque *serial*. Le contrôle du robot est
fonction de sa perception stéréoscopique fournie par deux caméras.

# Compiler le code

Le code peut être simplement compiler à l'aide des instructions suivantes:
```
mkdir build
cd build
cmake ..
make
```

Seule une partie du code est compilé pour éviter qu'il soit nécessaire
d'installer la bibliothèque *flycapture*. Afin de compiler l'ensemble du code,
il est nécessaire d'activer l'option *USE_FLYCAP* dans `build/CMakeCache.txt`.

L'utilisation de la bibliothèque *flycapture* n'est pas nécessaire dans le cadre de l'UE
Projets Technologiques.

# Modification du code

Les seuls fichiers à modifier pour les groupes de l'UE *Projets Technologiques*
sont `src/custom_controller.cpp` et `include/custom_controller.h`. Le fichier
qui sera utilisé pour exécuter le comportement du robot est
`examples/autonomous_car_opencv.cpp`.

