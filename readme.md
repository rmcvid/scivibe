# Projet

## To do pour vendredi 9/10
- Faire un batching pour le rendering
- Lier la fenêtre OpenGL à FFmpeg afin de pouvoir enregistrer tout ce qui s'affiche à l'écran
- Ajouter l'écriture et la gestion de police à le fentre
- Introduire une interprétation latex pour le rendu de charactère
- Introduire les objets de base :
  - Vecteurs ( flèches)
  - lignes => Repère ( carthésien, polaire, ...) 
  - wrapper de courbe à travers des fonctions
  - Cercle et disques ( plutot passer par une texture sur un quad pour éviter un nombre très élevé de triangle ou alors définir un shader ? )
  - Batterie ( implémenter le blurring pour un effet de lumière, Passer par photoshop ? )
- Ecrire la vidéo sur les vecteurs pour savoir quoi dessiner ?

# To do ( plus long terme)
- Vérifier si les fonctions d'ImGui qui ne sont pas utilisées sont quand même exportées vers le client afin qu'il puisse les utiliser (sinon, faire un fichier `.def` ?)
- Un README qui se respecte
- ajouter une documentation et des exemples pour les étudiants et collaborateur.
  - Si vous êtes dans ce cas là, je vous reconduis (pour le moment) vers ces sources qui permettront d'avoir une solide base ( chacune des sources prend un temps assez considérable pour les terminer):
    - serie de vidéo sur la construction du moteur de jeu Hazel, le moteur a une base similaire: https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT
    - fonctionnement d'OpenGL, les graphismes sont pour le moment uniquement en OpenGL et comprendre son fonctionnement est une réèlle plus value, la programmation graphique est assez différente de ce qu'on ( les physiciens ou ingénieur ) voit à l'université : https://learnopengl.com
    - Pour revoir les bases en cpp : https://www.learncpp.com
## Idea
- Une idée qui pourrait être intéressante serait de relier une caméra à l'ordinateur. En plus d'enregistrer l'affichage, la caméra filmerait la scène, et on pourrait faciliter la synchronisation des deux au montage en notifiant les moments de transition
- Un des problemes qu'on peut avoir pendant l'enregistrement est une perte de perfomance momentanée car le cpu doit faire une tache ponctuelle. La l'enregistrement des images ne serait plus exactement superposée à l'audio et un décalage se crérait. Pour éviter ca on peut enregistre plus d'image par seconde et reconstruire une vidéo basée sur ces images avec 60fps. On aurait probablement besoin de balise toutes les x secondes