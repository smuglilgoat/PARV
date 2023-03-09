/********************************************************/
/*                     cube.cpp                                                 */
/********************************************************/
/*                Affiche a l'ecran un cube en 3D                      */
/********************************************************/

/* inclusion des fichiers d'en-tete freeglut */

#ifdef __APPLE__
#include <GLUT/glut.h> /* Pour Mac OS X */
#else
#include <GL/glut.h> /* Pour les autres systemes */
#endif
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <armadillo>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/io.hpp>
#include <chrono>

using namespace glm;
using namespace std;
using namespace arma;

char presse;
int anglex, angley, x, y, xold, yold;
float baseAngle = -90.0f;
colvec target = {0, 2, 0};

/* Prototype des fonctions */
void affichage();
void clavier(unsigned char touche, int x, int y);
void reshape(int x, int y);
void idle();
void mouse(int bouton, int etat, int x, int y);
void mousemotion(int x, int y);

int main(int argc, char **argv)
{
  /* initialisation de glut et creation
     de la fenetre */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  int wW = glutGet(GLUT_SCREEN_WIDTH);
  int wH = glutGet(GLUT_SCREEN_HEIGHT);
  glutInitWindowPosition((wW - wW / 2) / 2, (wH - wH / 2) / 2);
  glutInitWindowSize(wW / 2, wH / 2);
  glutCreateWindow("PARV - Cinematique Inverse - MOUSSAOUI Ahmed");

  /* Initialisation d'OpenGL */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glColor3f(1.0, 1.0, 1.0);
  glPointSize(10.0);
  glEnable(GL_DEPTH_TEST);

  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mousemotion);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, .1, 30.);

  /* Entree dans la boucle principale glut */
  glutMainLoop();
  return 0;
}

struct Bone
{
  float size;
  float angle;
  glm::quat rotation;

  Bone(float s, float a, glm::quat r)
  {
    size = s;
    angle = a;
    rotation = r;
  }
};

vector<Bone> arm = {
    Bone(1, 0, glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
    Bone(2, -90, glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
    Bone(1, 90, glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
    Bone(1, 0, glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
};

vector<glm::vec3> colors = {
    glm::vec3(1, 0, 1),
    glm::vec3(0, 1, 1),
    glm::vec3(1, 1, 0),
    glm::vec3(0, 1, 1),
};

void drawArm()
{
  arm[1].rotation =
      glm::angleAxis(glm::radians(baseAngle), glm::vec3(0.0f, 0.0f, 1.0f));
  glm::vec3 currPoint(0, 0, 0);
  glm::vec3 direction(1, 0, 0);
  arma::mat pivots = arma::zeros(4, 3);
  for (int i = 0; i < 4; i++)
  {
    glColor3fv(&colors[i][0]);
    glBegin(GL_LINES);
      glVertex3fv(&currPoint[0]);
      direction = arm[i].rotation * glm::vec4(direction, 1.0f);
      currPoint += direction * arm[i].size;
      pivots(i, 0) = currPoint.x;
      pivots(i, 1) = currPoint.y;
      pivots(i, 2) = currPoint.z;
      glVertex3fv(&currPoint[0]);
    glEnd();
  }

  arma::mat Jacobien = arma::zeros(3, 4);
  arma::mat JacInverse = arma::zeros(4, 3);
  arma::vec lambdas = arma::zeros(4);
  arma::vec E = arma::zeros(3);
  E = target - pivots.row(3).t();
  unsigned short cpt = 0;

  arma::vec Thetas;
  Thetas = arma::zeros(4);
  Thetas(0) = glm::pi<float>() / 2;

  while (arma::norm(E) >= 0.01f && cpt < 50)
  {
    for (int i = 0; i < 4; i++)
    {
      Jacobien(0, i) = pivots(i, 1) - pivots(3, 1);
      Jacobien(1, i) = pivots(3, 0) - pivots(i, 0);
    }

    JacInverse = arma::pinv(Jacobien);
    lambdas = JacInverse * E;

    float max_lambda = max(lambdas);

    if (max_lambda > 2 * glm::pi<float>() / 180)
      lambdas *= (2 * glm::pi<float>() / 180) / max_lambda;

    Thetas = Thetas + lambdas;

    {
      for (int i = 0; i < 4; i++)
      {
        arm[i].angle += Thetas(i) / 100.;
        arm[i].rotation = glm::angleAxis(glm::radians(arm[i].angle),
                                         glm::vec3(0.0f, 0.0f, 1.0f));
      }

      glm::vec3 currPoint(0, 0, 0);
      glm::vec3 direction(1, 0, 0);
      arma::mat pivots = arma::zeros(4, 3);
      for (int i = 0; i < 4; i++)
      {
        direction = arm[i].rotation * glm::vec4(direction, 1.0f);
        currPoint += direction * arm[i].size;
        pivots(i, 0) = currPoint.x;
        pivots(i, 1) = currPoint.y;
        pivots(i, 2) = currPoint.z;
      }
    }

    E = target - pivots.row(3).t();

    cpt++;
  }
}

void drawAxis()
{
  // Repère
  // axe x en rouge
  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0, 0, 0.0);
  glVertex3f(1, 0, 0.0);
  glEnd();
  // axe des y en vert
  glBegin(GL_LINES);
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0, 0, 0.0);
  glVertex3f(0, 1, 0.0);
  glEnd();
  // axe des z en bleu
  glBegin(GL_LINES);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0, 0, 0.0);
  glVertex3f(0, 0, 1.0);
  glEnd();
}

void drawTarget()
{
  glBegin(GL_POINTS);
  glColor3f(1.0, 0.0, 1.0);
  glVertex3f(target[0], target[1], target[2]);
  glEnd();
}

void affichage()
{
  int i, j;
  /* effacement de l'image avec la couleur de fond */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glShadeModel(GL_SMOOTH);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0., 0., -5.);
  glRotatef(angley, 1.0, 0.0, 0.0);
  glRotatef(anglex, 0.0, 1.0, 0.0);

  drawAxis();
  drawArm();
  drawTarget();

  glFlush();

  // On echange les buffers
  glutSwapBuffers();
  glutPostRedisplay();
}

void clavier(unsigned char touche, int x, int y)
{
  switch (touche)
  {
  case '1':
    baseAngle -= 5;
    glutPostRedisplay();
    break;
  case '2':
    baseAngle += 5;
    glutPostRedisplay();
    break;

  case 'p': /* affichage du carre plein */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glutPostRedisplay();
    break;
  case 'f': /* affichage en mode fil de fer */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glutPostRedisplay();
    break;
  case 's': /* Affichage en mode sommets seuls */
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glutPostRedisplay();
    break;
  case 'd':
    glEnable(GL_DEPTH_TEST);
    glutPostRedisplay();
    break;
  case 'D':
    glDisable(GL_DEPTH_TEST);
    glutPostRedisplay();
    break;
  case 'q': /*la touche 'q' permet de quitter le programme */
    exit(0);
  }
}

void reshape(int x, int y)
{
  if (x < y)
    glViewport(0, (y - x) / 2, x, x);
  else
    glViewport((x - y) / 2, 0, y, y);
}

void mouse(int button, int state, int x, int y)
{
  /* si on appuie sur le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    presse = 1; /* le booleen presse passe a 1 (vrai) */
    xold = x;   /* on sauvegarde la position de la souris */
    yold = y;
  }
  /* si on relache le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    presse = 0; /* le booleen presse passe a 0 (faux) */
}

void mousemotion(int x, int y)
{
  if (presse) /* si le bouton gauche est presse */
  {
    /* on modifie les angles de rotation de l'objet
 en fonction de la position actuelle de la souris et de la derniere
 position sauvegardee */
    anglex = anglex + (x - xold);
    angley = angley + (y - yold);
    glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
  }
  //    else
  //    {
  //        pointCible.x= ;
  //        pointCible.y= ;
  //    }

  xold = x; /* sauvegarde des valeurs courante de le position de la souris */
  yold = y;
}
