// *** Spiegelungen mit Stencil Buffer simulieren

#include <math.h>
#include <GL/freeglut.h>	

GLfloat viewPos[3] = {0.0f, 2.0f, 2.0f};

#define PI 3.141592f

#define ROTATE 1
#define MOVE 2

int width = 600;
int height = 600;

float theta = PI / 2.0f - 0.01f;
float phi = 0.0f;
float distance = 2.5f;
float oldX, oldY;
int motionState;

GLfloat lightPos[4] = {3, 3, 3, 1};
GLfloat mirrorColor[4] = {1.0f, 0.2f, 0.2f, 0.8f};
GLfloat teapotColor[4] = {0.8f, 0.8f, 0.2f, 1.0f};


// Szene zeichnen: Eine Teekanne
void drawScene()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, teapotColor);
	glPushMatrix();
	glTranslatef(0,0.37f,0);
	glutSolidTeapot(0.5f);
	glPopMatrix();
}

void drawScene2()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, teapotColor);
	glPushMatrix();
	glTranslatef(0, -0.37f, 0);
	glRotatef(180, 1, 0, 0);
	glutSolidTeapot(0.5f);
	glPopMatrix();
}

// Spiegel zeichen: Ein Viereck
void drawMirror()
{
	glBegin(GL_QUADS);
	glVertex3f(1,0,1);
	glVertex3f(1,0,-1);
	glVertex3f(-1,0,-1);
	glVertex3f(-1,0,1);
	glEnd();
}

void display(void)	
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //set background color to white
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glLoadIdentity();
	float x = distance * sin(theta) * cos(phi);
	float y = distance * cos(theta);
	float z = distance * sin(theta) * sin(phi);

	gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Szene normal zeichnen (ohne Spiegelobjekt)
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	drawScene();

	

	// *** Spiegel zeichnen, so dass Spiegelobjekt im Stencil Buffer eingetragen wird
	// *** Framebuffer dabei auf Read-Only setzen, Depth Buffer deaktivieren, Stencil Test aktivieren
	
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	
	//Maske erstellen
	glColorMask(0,0,0,0);                                //alle Farbkomponenten deaktiviert -> es wird nichts gezeichnet
	glStencilFunc(GL_ALWAYS, 1, 1);                      //test immer erfolgreich
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);           //wenn Test bestanden, mit  Masken/Referenz-Wert ersetzt werden
	drawMirror();
	glColorMask(1,1,1,1);                               // alle Farbkomponenten aktiviert -> wird wieder gezeichnet
	
	
	

	// *** Gespiegelte Szene zeichnen, Stencil Buffer so einstellen, dass nur bei
	// *** einem Eintrag 1 im Stencil Buffer das entsprechende Pixel im Framebuffer 
	// *** gezeichnet wird, der Inhalt vom Stencil Buffer soll unveraendert bleiben 
	// *** Depth Buffer wieder anmachen, Framebuffer Maskierung deaktivieren
	// *** Was macht man mit der Lichtquelle ?

	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_EQUAL,1,1);                         //nur dort gezeichnet, wo im Stencil Buffer 1 steht
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);				 //Inhalt im Stencil Buffer nicht mehr ver�ndert
	

	//Gespiegelte Szene 
	glPushMatrix();
	glScalef(1.f, -1.f, 1.f);	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos); 
	drawScene();                                 
	glPopMatrix();


	// *** Stencil Test deaktivieren, Spiegelung der Szene rueckgaengig machen
	// *** Spiegelobjekt mit diffuser Farbe mirrorColor zeichen
	// *** Blending aktivieren und ueber Alpha-Kanal mit Spiegelbild zusammenrechnen

	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mirrorColor);
	drawMirror();
	glDisable(GL_BLEND);

	glutSwapBuffers();	
}

void mouseMotion(int x, int y)
{
	float deltaX = x - oldX;
	float deltaY = y - oldY;
	
	if (motionState == ROTATE) {
		theta -= 0.01f * deltaY;

		if (theta < 0.01f) theta = 0.01f;
		else if (theta > PI/2.0f - 0.01f) theta = PI/2.0f - 0.01f;

		phi += 0.01f * deltaX;	
		if (phi < 0) phi += 2*PI;
		else if (phi > 2*PI) phi -= 2*PI;
	}
	else if (motionState == MOVE) {
		distance += 0.01f * deltaY;
	}

	oldX = (float)x;
	oldY = (float)y;

	glutPostRedisplay();

}

void mouse(int button, int state, int x, int y)
{
	oldX = (float)x;
	oldY = (float)y;

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			motionState = ROTATE;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			motionState = MOVE;
		}
	}
}


void idle(void)
{
	glutPostRedisplay();
}


int main(int argc, char **argv)
{
	GLfloat fogColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat lightPos[4] = {3, 3, 3, 1};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(width, height);
	glutCreateWindow("Teapot im Spiegel");

	glutDisplayFunc(display);
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouse);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);

	glViewport(0,0,width,height);					
	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();								

	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutMainLoop();
	return 0;
}
