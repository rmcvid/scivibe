# Projet
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