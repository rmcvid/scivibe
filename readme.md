# Projet

## Compiler et utiliser le moteur

Le moteur est une bibliothèque statique C++17, accessible avec la cible CMake
`scivibe::engine`. GLFW, GLM et miniaudio restent des sous-modules Git. Les sources
générées de GLAD (OpenGL 3.3 core) sont incluses dans `Engine/include/glad`.
`Engine/src/main.cpp` est conservé mais n'est pas compilé.

```sh
git submodule update --init --recursive
cmake -S . -B build
cmake --build build --parallel
```

Avec MinGW sous Windows, configurer avec `cmake -S . -B build -G "MinGW Makefiles"`
depuis un terminal où GCC et mingw32-make sont accessibles. Lancer ensuite
`./build/sandbox/sandbox.exe`. La macro copie les DLL du runtime MinGW disponibles
à côté de l'exécutable. Avec un générateur multi-configuration, l'exécutable se
trouve dans le sous-dossier de la configuration choisie (par exemple `Debug`).

La sandbox utilise simplement :

```cmake
scivibe_add_executable(sandbox test.cpp)
```

Et dans `test.cpp`, un seul en-tête donne accès aux objets et aux dépendances :

```cpp
#include <scivibe.h>

int main() {
    scivibe::print();
    scivibe::Fleche fleche({0.0f, 0.0f}, {100.0f, 0.0f},
                          scivibe::Color(1.0f, 0.0f, 0.0f));
    return fleche.getVertices().empty() ? 1 : 0;
}
```

Pour réutiliser le moteur depuis un autre projet CMake :

```cmake
cmake_minimum_required(VERSION 3.16)
project(mon_application LANGUAGES C CXX)
add_subdirectory(chemin/vers/scivibe/Engine scivibe-engine)
scivibe_add_executable(mon_application main.cpp autres_objets.cpp)
```

Une cible existante peut aussi utiliser
`target_link_libraries(mon_application PRIVATE scivibe::engine)` : les chemins
d'inclusion, C++17 et les dépendances sont transmis automatiquement. Miniaudio
est compilé une seule fois par le moteur ; ne pas définir
`MINIAUDIO_IMPLEMENTATION` dans l'application. L'initialisation d'une fenêtre,
du contexte OpenGL et de GLAD reste à faire par l'application avant tout dessin.

Pour compiler uniquement le moteur :
`cmake -S . -B build -DSCIVIBE_BUILD_SANDBOX=OFF`.

## Lancer depuis VS Code (Windows / MinGW)

Ouvrir le dossier racine `scivibeVScode`, puis utiliser les extensions recommandées
CMake Tools et C/C++. Les presets utilisent MSYS2 UCRT64 dans `C:/msys64/ucrt64`.
La configuration VS Code et les presets sont versionnables avec le projet.

- Sélectionner le configure preset `GCC 15.2.0 x86_64-w64-mingw32 (ucrt64)` et
  le build preset `SciVibe Debug (MinGW)` si CMake Tools les demande.
- Pour les boutons CMake Run/Debug, choisir `sandbox` avec
  `CMake: Set Launch/Debug Target` (ou le sélecteur de cible de lancement).
- Pour F5 / Ctrl+F5, utiliser `Run sandbox (CMake / MinGW)` dans Run and Debug.
  La compilation est effectuée avant le lancement.
- Ctrl+Maj+B compile le projet. La tâche `Run sandbox` compile puis exécute aussi
  l'application.

Enregistrer un `CMakeLists.txt` reconfigure le projet automatiquement ; cela ne
lance pas l'application. Le bouton C/C++ « Run C/C++ File » compile le fichier
actif seul : pour ce projet, utiliser les commandes CMake ou la configuration
Run and Debug ci-dessus, qui lient aussi le moteur et ses dépendances.

Si VS Code était déjà ouvert avec l'ancienne configuration, exécuter
`Developer: Reload Window`, puis `CMake: Configure` et sélectionner `sandbox`.

## To do
Préparer l'audio avec miniaudio, le .h est déja là et est normalement auto suffisant
Lié la fenetre d'openGl à ffmpeg afin de pouvoir enregistrée tout ce qui s'affiche à l'écran
Developper un log system


## fichier utilisé
Glad permet d'utilisé openGL directement ( il faut rester en dessous de 4.3 pour mac)
ffmpeg permet de transformer la fenetre directement en vidéo
??? pour gerer les police d'ecriture, il faut absolument conserver le format latex, pseudo latex peut etre réutilisé mais il devrait être revu et porter en cpp

## Architecture
Lié des listes pour chaque fragment shader afin de minimiser les appels GPU ?
Dans un premier temps les objets auront leu propres .hpp

## très long terme
API pour appeler les fonctions creer sans devoir tout recompiler ?
