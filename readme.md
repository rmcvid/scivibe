# Projet

## To do pour vendredi 9/10
- gerer l'aliasing
- introduire le bluring
- Ajouter l'écriture et la gestion de police à le fentre
- Introduire une interprétation latex pour le rendu de charactère
- Introduire les objets de base :
  - Vecteurs ( flèches)
  - lignes => Repère ( carthésien, polaire, ...) 
  - wrapper de courbe à travers des fonctions
  - Cercle et disques ( plutot passer par une texture sur un quad pour éviter un nombre très élevé de triangle ou alors définir un shader ? )
  - Batterie ( implémenter le blurring pour un effet de lumière, Passer par photoshop ? )
- Ecrire la vidéo sur les vecteurs pour savoir quoi dessiner ?
- wrapper l'introduction de mesh et d'objet
# To do ( plus long terme)
- Pour le moment les drawline ( meme dans pseudo latex) sont fait avec des quad, il serait plus judicieux d'utiliser des shader et d'introduire le même raisonement pour les cercles
- ajouter un profiling pour identifier les bottles neck, notamment pour l'enregistrement vidéo et audio
- Vérifier si les fonctions d'ImGui qui ne sont pas utilisées sont quand même exportées vers le client afin qu'il puisse les utiliser (sinon, faire un fichier `.def` ?)
- Un README qui se respecte
- ajouter une documentation et des exemples pour les étudiants et collaborateur.
  - Si vous êtes dans ce cas là, je vous reconduis (pour le moment) vers ces sources qui permettront d'avoir une solide base ( chacune des sources prend un temps assez considérable pour les terminer):
    - serie de vidéo sur la construction du moteur de jeu Hazel, le moteur a une base similaire: https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT
    - fonctionnement d'OpenGL, les graphismes sont pour le moment uniquement en OpenGL et comprendre son fonctionnement est une réèlle plus value, la programmation graphique est assez différente de ce qu'on ( les physiciens ou ingénieur ) voit à l'université : https://learnopengl.com
    - Pour revoir les bases en cpp : https://www.learncpp.com

## Done récent
- implémenter ffmpeg
- implémented miniaudio
- Batching 2D
  
## Probleme superposition audio vidéo:
Un des problemes qu'on a pendant l'enregistrement est qu'une perte de perfomance momentanée introduit un décalage entre l'audio et l'image qui se cumule. Pour éviter ce problème il existe plusieures posibilitées :
  - Soit on ne construit pas la vidéo durant l'enregistrement de l'audio mais on record uniquement les actions éffectué par le 'joueur'. Le programme peut en post processing reconstruire exactement le play au nombre d'image souhaité et reconstruire la vidéo par après. Le code n'a actuellement pas pris cette direction et l'implémentation du traqueur et l'enregistrement sans affichage ou lié au nombre d'fps voulu doit etre complement construit
  - On peut baisser le nombre d'fps dans la vidéo et à l'affichage
  - On peut introduire un delta entre le nombre d'images réèllement affichée et celles enregistrèes. dans ce cas il faudrait introduire un traqueur qui mettrait des balise afin de pouvoir reconstruire la vidéo en post processing.
## Idea
- Une idée qui pourrait être intéressante serait de relier une caméra à l'ordinateur. En plus d'enregistrer l'affichage, la caméra filmerait la scène, et on pourrait faciliter la synchronisation des deux au montage en notifiant les moments de transition
- 