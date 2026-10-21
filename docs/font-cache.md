# Cache des polices SDF

Les atlas produits par `Font`, notamment ceux du prechargement PseudoLaTeX,
sont conserves dans `.cache/fonts/` a la racine du projet. Le chemin est fixe
par CMake et ne depend pas du dossier depuis lequel le programme est lance.
Ce dossier est ignore par Git.

Au premier chargement, FreeType genere les glyphes puis le moteur enregistre
un fichier `.sdfcache`. Les lancements suivants chargent ce fichier et envoient
les pixels a OpenGL, sans recalculer les distances SDF. Garder l'appel existant
a `PreloadMicroTeXFonts()` : il charge maintenant les fichiers mis en cache.
Les autres polices utilisees plus tard dans une formule sont aussi mises en cache.

Le fichier contient le canal alpha de l'atlas (environ 4 Mio pour 2048 x 2048),
les tailles, bearings, avances, UV des glyphes et la hauteur de ligne. Le RGB
blanc est reconstruit au chargement. Ce fichier binaire n'est pas un PNG.

La cle depend du contenu de la police, de la taille de rasterisation, de la
version FreeType, des parametres de rendu et de la version du format. Un changement
de taille ou de police genere donc automatiquement une nouvelle entree. Les
anciens fichiers peuvent etre supprimes en effacant `.cache/fonts/` ; le prochain
lancement les reconstruira. Un cache tronque, corrompu ou incompatible est aussi
regenere. Une erreur d'ecriture du cache n'empeche pas le rendu.

Lors d'un changement de selection des caracteres, de packing ou de parametres SDF
non inclus dans la cle, incrementer `Version` dans `fontAtlasCache.cpp`.

Ce cache concerne les atlas, pas les objets OpenGL ni la mise en page des formules.
Le chargement MicroTeX, la lecture disque et le transfert GPU restent necessaires.
La liste des polices prechargees reste celle de `PreloadMicroTeXFonts()`.

## Verification

```sh
cmake -S . -B build -DSCIVIBE_BUILD_TESTS=ON
cmake --build build --target font_cache_tests --parallel 8
ctest --test-dir build -R font_cache --output-on-failure
```

Le test utilise les vraies polices MicroTeX et FreeType, avec une texture CPU
pour comparer exactement les pixels et metriques sans ouvrir de fenetre OpenGL.
Son cache est isole dans `build/tests/font-cache-data/`.
