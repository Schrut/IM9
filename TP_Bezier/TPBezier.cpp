//GIMENEZ Florian
//Code dispo sur : https://github.com/Schrut/IM9
//M1 IS IN IM9, UFR Sciences Marseille
//Projet Final IM9

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

std::deque<double>* curr_deviation;

int casteljau = 0;

int bezier_chaikin = 0;

int control_poly = 1;

int degree_chaikin = 1;

std::deque< deque<point3> > * listControlPoly;

std::deque<point3> * controlPointList;// la structure pour les points de contrôle (sans classe à tous les points de vue)
std::deque<point3> * pts_curve; //La structure où l'on stock les points de la courbe



GLuint leVBO;//pour afficher les points de contrôle
GLuint leVBO2;//pour afficher la courbe

int resolution = 10;
static void CreateVertexBuffer();
void ComputeGeometry();

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

void bezier_casteljau_compute(std::deque<point3>* control_pts, double t)
{
	std::deque<point3>* prev_castel;
	std::deque<point3>* curr_castel;

	listControlPoly = new std::deque< deque<point3> >;

	pts_curve = new std::deque<point3>;

	listControlPoly->push_back(*control_pts);

	for (unsigned int i = 0; i < controlPointList->size() - 1; ++i)
	{
		curr_castel = new std::deque<point3>;
		prev_castel = new std::deque<point3>;

		prev_castel->push_back(listControlPoly->at(i).at(0));
		for (unsigned int k = 0; k < listControlPoly->at(i).size() - 1; ++k)
		{
			curr_castel->push_back(listControlPoly->at(i).at(k) * (1 - t) + listControlPoly->at(i).at(k + 1) * (t));
		}
		listControlPoly->push_back(*curr_castel);
	}
}

//Algorithme de Casteljau
static GLvoid divide_casteljau()
{
	std::deque<point3> list_left;
	std::deque<point3> list_right;

	for (unsigned int k = 0; k < listControlPoly->size(); ++k)
	{
		list_left->push_back(listControlPoly->at(k).front());
		list_right->push_front(listControlPoly->at(k).back());
	}

	

}

point3 made_vector (point3 a, point3 b)
{
	point3 vec = point3();

	vec.x = b.x - a.x;
	vec.y = b.y - a.y;
	vec.z = b.z - a.z;

	return vec;
}

double dot(point3 a, point3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

double norm(point3 a)
{
	return sqrt( pow(a.x,2) + pow(a.y,2) + pow(a.z,2) );
}

double deviation(point3 a, point3 b)
{
	return acos(dot(a,b) / (norm(a) * norm(b)))*180.0/M_PI;
}

double mean_deviation()
{
	double sum = 0;
	for (unsigned int k = 0; k < curr_deviation->size(); ++k)
	{
		sum += curr_deviation->at(k);
	}
	return sum/curr_deviation->size();
}

double max_deviation()
{
	double maximum;
	maximum = curr_deviation->at(0);
	for (unsigned int k = 1; k < curr_deviation->size(); ++k)
	{
		if (curr_deviation->at(k) > maximum)
			maximum = curr_deviation->at(k);
	}
	return maximum;
}

double min_deviation()
{
	double minimum;
	minimum = curr_deviation->at(0);
	for (unsigned int k = 1; k < curr_deviation->size(); ++k)
	{
		if (curr_deviation->at(k) < minimum)
			minimum = curr_deviation->at(k);
	}
	return minimum;
}

static GLvoid chaikin_render()
{
	std::deque<point3> *prev_chaikin;

	pts_curve = new std::deque<point3>;

	for (unsigned int k = 0; k < controlPointList->size(); ++k)
	{
		pts_curve->push_back(controlPointList->at(k));
	}

	for(int i = 0; i < degree_chaikin ; i++)
	{
		prev_chaikin = new std::deque<point3>;
		for (unsigned int k = 0; k < pts_curve->size(); ++k)
		{
			prev_chaikin->push_back(pts_curve->at(k));
		}

		pts_curve = new std::deque<point3>;
		for (unsigned int k = 0; k < prev_chaikin->size() - 1 ; ++k)
		{
			pts_curve->push_back(prev_chaikin->at(k) + made_vector(prev_chaikin->at(k), prev_chaikin->at(k + 1)) * (1 / 4.0));
			pts_curve->push_back(prev_chaikin->at(k) + made_vector(prev_chaikin->at(k), prev_chaikin->at(k + 1)) * (3 / 4.0));
		}
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

	//dessin des axes du repère
	//drawAxis();
	glEnableClientState(GL_VERTEX_ARRAY);
	if (control_poly)
	{
		//dessin du polygone de contrôle
		glColor3f(0.4, 0.4, 0.4);

		//Liaison avec le buffer de vertex
		glBindBuffer(GL_ARRAY_BUFFER, leVBO);
		glVertexPointer(3, GL_FLOAT, 0, 0); //description des données pointées

		glDrawArrays(GL_LINE_STRIP, 0, controlPointList->size()); //les éléments à utiliser pour le dessin

		//dessin des points de contrôle
		//avec le même tableau de donnes (le VBO)
		glColor3f(0.8, 0.8, 0.3);
		//on modifie la taille d'un point pour plus de "joliesse"
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, controlPointList->size()); //les éléments à utiliser pour le dessin
	}
	if (pts_curve->size() != 0)
	{
		//dessin de la courbe de contrôle
		glColor3f(0.9, 0.4, 0.4);

		//Liaison avec le buffer de vertex
		glBindBuffer(GL_ARRAY_BUFFER, leVBO2);
		glVertexPointer(3, GL_DOUBLE, 0, 0); //description des données pointées

	glDrawArrays(GL_LINE_STRIP, 0, pts_curve->size()); //les éléments à utiliser pour le dessin
	}
	
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

static GLvoid callback_Mouse(int button, int state, int x, int y) {
//rotation de la scene suivant les mouvements de la souris

	double y_disp = ((windowHeight - y) * (10.0 / windowHeight ) ) - 5 ;
	double x_disp = (x * (10.0 / windowWidth ) ) - 5 ;
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		controlPointList->push_back(point3(x_disp, y_disp, 0.0));
		CreateVertexBuffer();
		if (controlPointList->size() > 3)
		{
			ComputeGeometry();
		}
	}

//cout << "callback_Mouse - " << "mouseAngleX " << mouseAngleX << " mouseAngleY " << mouseAngleY << endl;
}


static void InitializeGL() {

	//Z Buffer pour la suppression des parties cachées
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

static void CreateVertexBuffer()
{

	float vertices[controlPointList->size() * 3]; //sommets à 3 coordonnées x,y,z par point

	unsigned n = 0;

	for (std::deque<point3>::iterator it = controlPointList->begin(); it != controlPointList->end(); ++it)
	{
		vertices[n] = (*it).x;
		vertices[n + 1] = (*it).y;
		vertices[n + 2] = (*it).z;
		n += 3;
	}

	glGenBuffers(1, &leVBO);																																							 //génération d'une référence de buffer object
	glBindBuffer(GL_ARRAY_BUFFER, leVBO);																																	 //liaison du buffer avec un type tableau de données
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * controlPointList->size() * 3, vertices, GL_STATIC_DRAW); //création et initialisation du container de données (size() sommets -> 3*size() floats)


	if (pts_curve->size() != 0)
	{
		double vertices2[pts_curve->size() * 3]; //sommets à 3 coordonnées x,y,z par point

		n = 0;

		for (std::deque<point3>::iterator it = pts_curve->begin(); it != pts_curve->end(); ++it)
		{
			vertices2[n] = (*it).x;
			vertices2[n + 1] = (*it).y;
			vertices2[n + 2] = (*it).z;
			n += 3;
		}

		glGenBuffers(1, &leVBO2);																																					//génération d'une référence de buffer object
		glBindBuffer(GL_ARRAY_BUFFER, leVBO2);																														//liaison du buffer avec un type tableau de données
		glBufferData(GL_ARRAY_BUFFER, sizeof(double) * pts_curve->size() * 3, vertices2, GL_STATIC_DRAW); //création et initialisation du container de données (size() sommets -> 3*size() floats)
	}
}


void ComputeGeometry() {
if(bezier_chaikin)
{
	if(casteljau)
		bezier_casteljau_render();
	else
		bezier_classic_render();
}
else
	chaikin_render();

/*curr_deviation = new std::deque<double>;
for (unsigned int k = 0; k < pts_curve->size() - 2; ++k)
{
	//curr_deviation->push_back(deviation(made_vector(pts_curve->at(k),pts_curve->at(k + 1)),made_vector(pts_curve->at(k+1),pts_curve->at(k + 2))));
}

if (bezier_chaikin)
{
	if (casteljau)
		cout << "Courbe de Bezier par De Casteljau" << endl;
	else
		cout << "Courbe de Bezier" << endl;
	cout << "Resolution : " << resolution << endl;
}
else
{
	cout << "Subdivision de Chaikin" << endl;
	cout << "Nb Chaikin : " << degree_chaikin << endl;
}

cout << "Moyenne de deviation : " << mean_deviation() << endl;
cout << "Max de deviation : " << max_deviation() << endl;
cout << "Min de deviation : " << min_deviation() << endl;
cout << "Nb deviation : " << pts_curve->size() << endl;

cout << "-----------" << endl;*/
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

	case 'c' : 
	casteljau = !casteljau;
	ComputeGeometry();
	break;

	case 'b':
		bezier_chaikin = !bezier_chaikin;
		ComputeGeometry();
		break;

	case '+':
		if(bezier_chaikin)
		{
			resolution += 1;
		}
		else
		{
		degree_chaikin++;
		}
		ComputeGeometry();
		break;

	case '-':
		if (bezier_chaikin)
		{
			resolution -= 1;
		}
		else
		{
			degree_chaikin--;
		}
		ComputeGeometry();
		break;

	case 'p':
		control_poly = !control_poly;
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
	glutMouseFunc(callback_Mouse);

	//quelle fonction est appelée pour traiter les événements sur la fenêtre ?
	glutReshapeFunc(callback_Window);

	//quelle fonction est appelée pour traiter les touches spéciales du clavier ?
	//glutSpecialFunc(&callback_SpecialKeys);

	controlPointList = new std::deque<point3>();
	pts_curve = new std::deque<point3>();

	/*	controlPointList->push_back(point3(-2.0, -2.0, 0.0));
	controlPointList->push_back(point3(-1.0, 1.0, 0.0));
	controlPointList->push_back(point3(1.0, 1.0, 0.0));
	controlPointList->push_back(point3(2.0, -2.0, 0.0));*/


}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("TP IM9");


	//toujours après l'initialisation de glut
	GLenum res = glewInit();

	if (res != GLEW_OK) {
		cerr << "Error: " << glewGetErrorString(res) << endl;
		return (EXIT_FAILURE);
	}

	InitializeGlutCallbacks();
	InitializeGL();

	glutMainLoop();

	//désallocation de la liste de points de contôle
	controlPointList -> clear();
	delete controlPointList;

	return (EXIT_SUCCESS);
}
