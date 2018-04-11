//R. RAFFIN, IUT Aix-Marseille, département Informatique, site d'Arles
//M1 IS IN IM9, UFR Sciences Marseille
//romain.raffin[AT]univ-amu.fr
//Squelette d'une gestion de courbe(s)) de Bézier : 1 structure pour stocker les points de contrôles, affiché sous forme de polygone
 
//compilation par : g++ -Wall TPBezier.cpp -lGLEW -lGLU -lGL -lglut -o TPBezier

#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/freeglut.h>

#include <iostream>
#include <deque>

#include "point3.h"

#include "math.h"

#define KEY_ESC 27

using namespace std;

float angle=0.0f;

float ax = 0.0f, ay = 0.0f;

//variables pour la gestion des paramètres de la fenêtre
float windowRatio = 1.0f;
int windowHeight = 500;
int windowWidth = 500;

float mouseAngleX = 0.0;
float mouseAngleY = 0.0;
int oldMouseX = windowHeight/2;
int oldMouseY = windowWidth/2;

int casteljau = 0;

std::deque<point3> * controlPointList;// la structure pour les points de contrôle (sans classe à tous les points de vue)
std::deque<point3> * pts_curve; //La structure où l'on stock les points de la courbe

GLuint leVBO;//pour afficher les points de contrôle
GLuint leVBO2;//pour afficher la courbe

int resolution = 10;

//Factoriel
int factorial(int n)
{
	if (n > 1)
		return n * factorial(n - 1);
	else
		return 1;
}

//Polynôme de Bernstein
double bernstein_polynome(int i, int n, double t)
{
	return double((factorial(n) / (factorial(i) * factorial(n - i))) * pow(t, i) * pow(1 - t, n - i));
}

//Compute Bezier
point3 bezier_classic_compute(std::deque<point3>* control_pts, double t)
{
	point3 tmp = point3();
	point3 p = point3();

	for (unsigned int i = 0 ; i < control_pts->size(); i++)
	{
		tmp = control_pts->at(i) * bernstein_polynome(i, control_pts->size()-1, t);
		p = p + tmp;
	}
	return p;
}

//Affichage courbe de Bézier avec la formule classique
static GLvoid bezier_classic_render()
{
	pts_curve = new std::deque<point3>();

	for (int i = 0; i <= resolution; i++)
	{
		pts_curve->push_back(bezier_classic_compute(controlPointList, i/double(resolution)));
	}
}

point3 bezier_casteljau_compute(std::deque<point3>* control_pts, double t)
{
	std::deque<point3>* prev_castel;
	std::deque<point3>* curr_castel;
	
	curr_castel = new std::deque<point3>;

	for (unsigned int k = 0; k < control_pts->size(); ++k)
	{
		curr_castel->push_back(control_pts->at(k));
	}

	for (unsigned int i = 0; i < controlPointList->size() - 2; ++i)
	{

		prev_castel = new std::deque<point3>;
		for (unsigned int k = 0; k < curr_castel->size(); ++k)
		{
			prev_castel->push_back(curr_castel->at(k));
		}

		curr_castel = new std::deque<point3>;

		for (unsigned int k = 0; k < prev_castel->size() - 1; ++k)
		{
			curr_castel->push_back(prev_castel->at(k)*(1-t) + prev_castel->at(k+1)*(t));
		}
	}

	return curr_castel->at(0) * (1 - t) + curr_castel->at(1) * (t);
}

//Algorithme de Casteljau
static GLvoid bezier_casteljau_render()
{

	pts_curve = new std::deque<point3>();
	for (int i = 0; i <= resolution; i++)
	{
		pts_curve->push_back(bezier_casteljau_compute(controlPointList, i / double(resolution)));
	}
}

	void drawAxis()
	{
		glBegin(GL_LINES);
		//Ox, rouge
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(8.0, 0.0, 0.0);

		//Oy, vert
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 8.0, 0.0);

		//Oz, bleu
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 8.0);

	glEnd();
}

static void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Modification de la matrice de projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); //remise à 0 (identité)
	//gluPerspective(130.0, windowRatio, 0.1, 100); //définition d'une perspective (angle d'ouverture 130°,rapport L/H=1.0, near=0.1, far=100)
	glOrtho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);

	//Modification de la matrice de modélisation de la scène
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Définition de la position de l'observateur
	//gluLookAt(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //paramètres position(5.0, 5.0, 5.0), point visé(0.0, 0.0, 0.0), upVector - verticale (0.0, 1.0, 0.0)

	//rotation due aux mouvements de la souris
	glRotatef(mouseAngleX, 1.0, 0.0, 0.0);
	glRotatef(mouseAngleY, 0.0, 1.0, 0.0);

	//dessin des axes du repère
	//drawAxis();

	//dessin du polygone de contrôle
	glColor3f(0.4, 0.4, 0.4);

		//Liaison avec le buffer de vertex
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, leVBO);
		glVertexPointer(3, GL_FLOAT, 0, 0); //description des données pointées

	glDrawArrays(GL_LINE_STRIP, 0, controlPointList -> size()); //les éléments à utiliser pour le dessin

	//dessin des points de contrôle
	//avec le même tableau de donnes (le VBO)
	glColor3f(0.8, 0.8, 0.3);
		//on modifie la taille d'un point pour plus de "joliesse"
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, controlPointList -> size()); //les éléments à utiliser pour le dessin

	//dessin du polygone de contrôle
	glColor3f(0.9, 0.4, 0.4);

		//Liaison avec le buffer de vertex
		glBindBuffer(GL_ARRAY_BUFFER, leVBO2);
		glVertexPointer(3, GL_DOUBLE, 0, 0); //description des données pointées

	glDrawArrays(GL_LINE_STRIP, 0, pts_curve -> size()); //les éléments à utiliser pour le dessin
	
	glDisableClientState(GL_VERTEX_ARRAY);

	glutSwapBuffers();
}

//A lier à la glutIdleFunc
static GLvoid callback_Idle()
{
	//angle += 1.0f;
	glutPostRedisplay();

}


//fonction de call-back pour le redimensionnement de la fenêtre
static GLvoid callback_Window(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);

	windowWidth = width;
	windowHeight = height;

	windowRatio = float(windowWidth) / float(windowHeight);

	//cout << "callback_Window - " << "new width " << windowWidth << " new height " << windowHeight << endl;
}

static GLvoid callback_Mouse(int wx, int wy) {
//rotation de la scene suivant les mouvements de la souris
	int dx = oldMouseX - wx;
	int dy = oldMouseY - wy;

	oldMouseX = wx;
	oldMouseY = wy;

	mouseAngleX += dy;
	mouseAngleY += dx;

	//cout << "callback_Mouse - " << "mouseAngleX " << mouseAngleX << " mouseAngleY " << mouseAngleY << endl;
}



static void InitializeGL() {

	//Z Buffer pour la suppression des parties cachées
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

static void CreateVertexBuffer()
{
	float vertices[controlPointList -> size()*3]; //sommets à 3 coordonnées x,y,z par point

	unsigned n = 0;

	for (std::deque<point3>::iterator it = controlPointList -> begin(); it != controlPointList-> end(); ++it) {
		vertices[n] = (*it).x;
		vertices[n+1] = (*it).y;
		vertices[n+2] = (*it).z;
		n += 3;
	}

 	glGenBuffers(1, &leVBO); //génération d'une référence de buffer object
	glBindBuffer(GL_ARRAY_BUFFER, leVBO); //liaison du buffer avec un type tableau de données
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*controlPointList -> size() * 3, vertices, GL_STATIC_DRAW); //création et initialisation du container de données (size() sommets -> 3*size() floats)

	double vertices2[pts_curve->size() * 3]; //sommets à 3 coordonnées x,y,z par point


	n = 0;

	for (std::deque<point3>::iterator it = pts_curve->begin(); it != pts_curve->end(); ++it)
	{
		vertices2[n] = (*it).x;
		vertices2[n + 1] = (*it).y;
		vertices2[n + 2] = (*it).z;
		n += 3;
	}

	glGenBuffers(1, &leVBO2);																																				//génération d'une référence de buffer object
	glBindBuffer(GL_ARRAY_BUFFER, leVBO2);																													//liaison du buffer avec un type tableau de données
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * pts_curve->size() * 3, vertices2, GL_STATIC_DRAW); //création et initialisation du container de données (size() sommets -> 3*size() floats)
}


void InitializeGeometry() {
	controlPointList = new std::deque<point3>();
	//d'après le sujet
	//P_0(-2,-2,0) P_1=(-1,1,0) P_2=(1,1,0) P_3=(2,-2,0)

	controlPointList -> push_back(point3(-2.0, -2.0, 0.0));
	controlPointList -> push_back(point3(-1.0, 1.0, 0.0));
	controlPointList -> push_back(point3(1.0, 1.0, 0.0));
	controlPointList -> push_back(point3(2.0, -2.0, 0.0));

	if(casteljau)
		bezier_casteljau_render();
	else
		bezier_classic_render();

	CreateVertexBuffer();
}

// fonction de call-back pour la gestion des evenements clavier
static GLvoid callback_Keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case KEY_ESC:
	cout << "callback_Keyboard - " << "sortie de la boucle de rendu" << endl;
		glutLeaveMainLoop ( ); //retour au main()
	break;

	case 'b' : 
	casteljau = !casteljau;

	if (casteljau)
		cout << "Courbe de bezier par De Casteljau" << endl;
	else
		cout << "Courbe de Bezier classique" << endl;

	InitializeGeometry();
	break;

  default:
    	cerr << "callback_Keyboard - touche " << key << " non active." << endl;
    break;
  }
}

static void InitializeGlutCallbacks()
{
	//quelle fonction est appelée au rendu ?
	glutDisplayFunc(RenderScene);

	//quelle fonction est appelée quand le GPU est en attente ?
	glutIdleFunc(callback_Idle);

	//quelle fonction est appelée pour traiter les événements du clavier (classique) ?
	glutKeyboardFunc(callback_Keyboard);

	//quelle fonction est appelée pour traiter les événements souris ?
	glutMotionFunc(callback_Mouse);

	//quelle fonction est appelée pour traiter les événements sur la fenêtre ?
	glutReshapeFunc(callback_Window);

	//quelle fonction est appelée pour traiter les touches spéciales du clavier ?
	//glutSpecialFunc(&callback_SpecialKeys);

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("TP : courbe(s) de Bezier");


	//toujours après l'initialisation de glut
	GLenum res = glewInit();

	if (res != GLEW_OK) {
		cerr << "Error: " << glewGetErrorString(res) << endl;
		return (EXIT_FAILURE);
	}

	InitializeGlutCallbacks();
	InitializeGL();
	InitializeGeometry();

	glutMainLoop();

	//désallocation de la liste de points de contôle
	controlPointList -> clear();
	delete controlPointList;

	return (EXIT_SUCCESS);
}
