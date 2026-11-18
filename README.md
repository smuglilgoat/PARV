# PARV

Travaux pratiques de rendu 3D en C++ et OpenGL : bras articulé, cinématique inverse et simulation de particules.

## Contenu

| Dossier | Sujet |
| --- | --- |
| `Partie_C_Gentil/TP1` | Bras articulé, caméra et rotations (GLM) |
| `Partie_C_Gentil/TP2_Cinematic_Inverse` | Cinématique inverse par pseudo-inverse du jacobien, avec cible et contrôle à la souris |
| `Partie_C_Gentil/TP3_Particules` | 2 500 particules animées via VBO dynamiques et shader Phong |
| `Partie_J_Savelli` | Modèle NetLogo proies/prédateurs (`Predation.nlogo`) et automate (`Automate.drawio`) |

Le programme principal de chaque TP est `main.cpp` et ouvre une fenêtre GLUT.

## Compilation

Chaque dossier de TP contient son `Makefile` :

```bash
cd Partie_C_Gentil/TP3_Particules
make
./main
```

Dépendances : OpenGL, GLU, GLEW, freeglut et GLM ; le TP2 utilise Armadillo.

## Notes

- `TP3_Particules` charge ses shaders depuis `PhongShader.vert` et `PhongShader.frag`.
- Le `make` du TP1 lance directement l'exécutable à la fin de la compilation.
- Le modèle NetLogo s'ouvre avec NetLogo ; le schéma d'automate avec draw.io.
