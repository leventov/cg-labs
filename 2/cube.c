#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//Compile with -lGL -lGLU -lglut
// -lm for sin\cos & math

int gcd (int a, int b) {
	if (a == 1 || a == 0) return b;
	if (b == 1 || b == 0) return a;
	return gcd(b, a % b);
}

int lcm (int a, int b) {
	return a * b / gcd(a, b);
}


GLint phi = 0, psi = 45, aside = 1, top = 1;
GLdouble PI = 3.14159265;

int h, w;

void reshape(int width, int height) {
	h = height;
	w = width;
}


void rdraw() {
	glBegin(GL_TRIANGLES); 
		glColor3f(1.0f,0.0f,0.0f);                      // Красный
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f,-0.5f, 1.0f);
		glVertex3f( 0.7f,-1.0f, 1.0f); 
		
		glColor3f(0.0f,0.0f,1.0f);                      // Синий
		glVertex3f( 0.3f, 0.1f, 0.2f);
		glVertex3f( 0.6f,-1.0f, 1.0f);
		glVertex3f( 1.0f,-1.0f, 0.8f);
		
		glColor3f(0.0f,1.0f,0.0f);                      // Зеленый
		glVertex3f( 0.0f, -1.0f, -0.7f); 
		glVertex3f( 1.0f, 0.9f, -1.0f);
		glVertex3f(1.0f, 0.5f, 0.4f);
	glEnd();
}

void draw() {
	glutWireTeapot(0.5);
	
}

void lookAt() {
	double rpsi = PI*psi/180;
	double rphi = PI*phi/180;
	double eye_z = sin(rpsi);
	double xy = cos(rpsi);
	double eye_x = xy * cos(rphi);
	double eye_y = xy * sin(rphi); //sqrt(xy*xy - eye_x*eye_x);
	//gluLookAt(eye_x, eye_y, eye_z, -eye_x, -eye_y, -eye_z, 0, 0, top);
	glRotated(phi, 0, 0, 1);
	glRotated(psi, 1, 0, 0);
}

int angle = 0, x = 0, y = 1, z = 0;

double ID[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};


double X_P[16] = {
	0, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

double Y_P[16] = {
	1, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

double Z_P[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 1
};

void display() {
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	// first view
	glViewport (0, h/2, w/2, h/2); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-2, 2, -2, 2, -2, 2);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	lookAt();
	
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glRotated(90, 0,1,0);
	draw();
	
	
	// second view
	glViewport (w/2, h/2, w/2, h/2); 
	glLoadIdentity();
	glRotated(90, 1,0,0);
	
	draw();
	
	// third
	glViewport (0, 0, w/2, h/2); 
	glLoadIdentity();
	//glRotated(90, 0,0,1);
	draw();
	
	glViewport (w/2, 0, w/2, h/2); 
	glLoadIdentity();
	glTranslated(0,0,5);
	gluPerspective(45, 1, 0.1, 10);
	draw();
	glFlush();
}

int init() {                                      
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
	glPointSize(2.0);
	glColor3f(0.1f, 0.4f, 0.4f);

	glLineWidth(1.0);
	
	//ec = lcm(100, rint(kr*100)) / 100;

	return 1;
}

GLint direct = 1;

void mouse(int button, int state, int x, int y) {
	direct = -direct;
}


/*
void timf(int v) {
	//offset_a += 190;
	glutPostRedisplay();
	
	offset_a = offset_a +  1 * direct * PI / 360;
	
	printf("%f", offset_a);
	glutTimerFunc( 100, timf, 0 );
}
*/

void pressKey(unsigned char key, int x, int y) {
	switch (key) {
		case 'a'://GLUT_KEY_LEFT: 
			phi++;
			break;
		case 'd'://GLUT_KEY_RIGHT: 
			phi--;
			break;
			
		case 's'://GLUT_KEY_LEFT: 
			psi--;
			//if (psi % 180 == 90 || psi % 180 == -90) {
			//	phi += 180;
			//}
			break;
		case 'w'://GLUT_KEY_RIGHT: 
			psi++;
			//if (psi % 180 == 90 || psi % 180 == -90) {
			//	phi += 180;
			//}
			break;
		case 'z'://GLUT_KEY_UP:
			if (aside > 1)
				aside--;
			break;
		case 'x'://GLUT_KEY_DOWN: 
			aside++;
			break;
	}

	//printf("%d %d\n", phi, psi);
	glutPostRedisplay();
 }

int main(int argc, char *argv[] ) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL lab 2");
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(pressKey);
	//glutKeyboardUpFunc(releaseKey);
	//glutMouseFunc(mouse);
	//glutTimerFunc(100, timf, 0); // Set up timer for 40ms, about 25 fps
	
	init();
	glutMainLoop();
	return 0;
}