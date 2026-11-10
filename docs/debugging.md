# Déboguer SciVibe dans VS Code

La configuration utilise GCC et GDB de `C:/msys64/ucrt64/bin`.
Les extensions **C/C++** (`ms-vscode.cpptools`) et **CMake Tools**
(`ms-vscode.cmake-tools`) sont recommandées par le projet.

## Lancer le debugger

1. Ouvrir le dossier racine du projet dans VS Code.
2. Ouvrir **Exécuter et déboguer** (`Ctrl+Shift+D`).
3. Sélectionner **Debug sandbox (GDB / MinGW)**.
4. Placer un point d'arrêt avec `F9` sur une instruction, par exemple dans
   `Sandbox2D::OnUpdate` ou `Scene::OnUpdate`.
5. Appuyer sur `F5`.

F5 configure CMake, compile le projet en **Debug**, puis lance
`build/sandbox/sandbox.exe`. La première compilation Debug reconstruit les
dépendances concernées. Les symboles `-g` sont générés pour le sandbox et
le moteur ; les assertions Debug sont activées.

Un point d'arrêt dans le moteur peut rester en attente jusqu'au chargement
de `libscivibe.dll`.

## Quand le programme est arrêté

- `F10` : exécuter la ligne suivante sans entrer dans la fonction.
- `F11` : entrer dans la fonction appelée.
- `Shift+F11` : sortir de la fonction.
- `F5` : continuer jusqu'au prochain point d'arrêt ou à une erreur.
- `Shift+F5` : arrêter la session.
- **Variables**, **Espion** et **Pile des appels** : inspecter les valeurs
  et comprendre comment le programme est arrivé à cette ligne.

Lors d'un plantage, chercher la première fonction de ton code dans la pile
des appels. La ligne affichée au sommet peut être dans une bibliothèque.

`Ctrl+Shift+B` compile désormais tout le projet avec CMake. Il ne compile
plus uniquement le fichier ouvert.

## Fichiers de configuration

- `CMakePresets.json` : `CMAKE_BUILD_TYPE` vaut `Debug` ; le preset de
  compilation est `mingw-debug`. Ce fichier est actuellement ignoré par Git.
- `.vscode/tasks.json` : enchaînement configuration puis compilation.
- `.vscode/launch.json` : exécutable, répertoire de travail et GDB.

Références : [configuration du debugger C++ dans VS Code](https://code.visualstudio.com/docs/cpp/launch-json-reference)
et [presets CMake](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html).
