# Projet

## To do
- Lié la fenetre d'openGl à ffmpeg afin de pouvoir enregistrée tout ce qui s'affiche à l'écran
- Developper un log system


## fichier utilisé
Glad permet d'utilisé openGL directement ( il faut rester en dessous de 4.3 pour mac)
ffmpeg permet de transformer la fenetre directement en vidéo
??? pour gerer les police d'ecriture, il faut absolument conserver le format latex, pseudo latex peut etre réutilisé mais il devrait être revu et porter en cpp

## Architecture
Lié des listes pour chaque fragment shader afin de minimiser les appels GPU ?
Dans un premier temps les objets auront leu propres .hpp

## très long terme
API pour appeler les fonctions creer sans devoir tout recompiler ?

## Compiler et utiliser le moteur

Le moteur est une bibliothèque dynamique C++17, accessible avec la cible CMake
`scivibe::engine`. GLFW, GLM, miniaudio et spdlog restent des sous-modules Git.
Les sources générées de GLAD (OpenGL 3.3 core) sont incluses dans
`Engine/external/glad`.

CMake 3.21 ou plus récent est requis. Le moteur produit une DLL sous Windows,
une `.so` sous Linux et une `.dylib` sous macOS. Les dépendances conservent leur
configuration actuelle ; passer le moteur en `SHARED` n'active pas globalement
`BUILD_SHARED_LIBS`.

`core/core.hpp` définit `SCIVIBE_API`. CMake définit `SCIVIBE_BUILD_DLL` uniquement
pour compiler le moteur : la macro exporte alors ses symboles sous Windows et
les importe dans les clients. Sous GCC/Clang sur Linux/macOS, elle leur donne
une visibilité publique. Pour une classe dont les méthodes sont implémentées
dans un `.cpp` du moteur, écrire `class SCIVIBE_API MaClasse`. Pour une fonction
libre, écrire par exemple `SCIVIBE_API void maFonction();`. Ne pas répéter la
macro sur chaque méthode d'une classe déjà exportée. Les fonctions inline et
templates entièrement définis dans les headers n'ont pas besoin de cette macro.
`CreateApplication()` reste une éventuelle fabrique définie par le client.

Le PCH reste privé au moteur. Les headers publics incluent directement leurs
dépendances. L'interface expose des types C++ et spdlog : moteur et application
doivent utiliser des compilateurs, bibliothèques standard et configurations de
runtime compatibles ; une DLL MinGW n'est pas interchangeable avec une DLL MSVC.

Le code du moteur est regroupé dans `Engine/src` : `scivibe.h`, `log/`, `objet/`
et `shader/`. Les dépendances tierces restent dans `Engine/external` ; il n'y a
plus de dossier `Engine/include`. CMake transmet `Engine/src` comme chemin
d'inclusion aux applications, qui peuvent toujours écrire `#include <scivibe.h>`.

CMake découvre récursivement les `.cpp`, `.hpp` et `.h` de `Engine/src` avec
`CONFIGURE_DEPENDS`. Ajouter, par exemple, `Engine/src/event/event.cpp` et
`event.hpp`, puis lancer une compilation suffit : aucun changement de CMake
n'est nécessaire. Les suppressions sont également détectées. Les `.cpp` sont
compilés ; les en-têtes sont utilisés via les `#include`. Pour exposer une
nouvelle API par l'en-tête unique, ajouter son `#include` dans `src/scivibe.h`.
Les fichiers contenant un `main()` doivent rester dans une application comme
`sandbox`, hors de `Engine/src`.

Les shaders `.vs`, `.fs` et `.glsl` sont recensés comme ressources, sans être
compilés par le compilateur C++. Leur chargement et leur compilation OpenGL
restent à effectuer par l'application. `src/miniaudio.c` est compilé séparément,
une seule fois, dans la cible `scivibe_miniaudio`.

```sh
git submodule update --init --recursive
cmake -S . -B build
cmake --build build --parallel
```

Avec MinGW sous Windows, configurer avec `cmake -S . -B build -G "MinGW Makefiles"`
depuis un terminal où GCC et mingw32-make sont accessibles. Lancer ensuite
`./build/sandbox/sandbox.exe`. La macro copie la DLL du moteur et les DLL de ses
dépendances dynamiques à côté de l'exécutable, ainsi que les DLL du runtime MinGW
disponibles. La copie est vérifiée à chaque construction de l'application, même
si seule la DLL du moteur a changé. Avec un générateur multi-configuration, l'exécutable se
trouve dans le sous-dossier de la configuration choisie (par exemple `Debug`).

La sandbox utilise simplement :

```cmake
scivibe_add_executable(sandbox test.cpp)
```

Et dans `test.cpp`, un seul en-tête donne accès aux objets et aux dépendances :

```cpp
#include <scivibe.h>

int main() {
    scivibe::Log::Init();
    SCIVIBE_INFO("SciVibe prêt");
    scivibe::Fleche fleche({0.0f, 0.0f}, {100.0f, 0.0f},
                          scivibe::Color(1.0f, 0.0f, 0.0f));
    return fleche.getVertices().empty() ? 1 : 0;
}
```

Pour réutiliser le moteur depuis un autre projet CMake :

```cmake
cmake_minimum_required(VERSION 3.21)
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
La copie automatique des DLL sous Windows est fournie par
`scivibe_add_executable` ; si l'on utilise seulement `target_link_libraries`,
il faut aussi rendre ces DLL accessibles à l'exécutable.

Pour compiler uniquement le moteur :
`cmake -S . -B build -DSCIVIBE_BUILD_SANDBOX=OFF`.

Pour vérifier l'API depuis un client sans PCH (méthodes exportées, loggers
partagés, événements et destruction virtuelle d'une application dérivée) :

```sh
cmake -S . -B build -DSCIVIBE_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Avec Visual Studio, ajouter `--config Debug` à la compilation et `-C Debug` à
CTest. Les mécanismes d'export et le code PIC sont configurés pour les autres
plateformes ; leur fonctionnement doit encore être validé sous Linux/macOS.

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
