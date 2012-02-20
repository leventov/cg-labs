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


GLdouble r1, r2, c1x, c1y, offset_a, kr = 0.5, k = 1.0;
GLint ec;
GLdouble PI = 3.14159265;

void reshape(int width, int height) {
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D(  0.0, (GLdouble)width, 0.0, (GLdouble)height );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	int min = width;
	if (height < min) min = height;
	r1 = 0.45 * min;
	r2 = kr * r1;
	c1x = width * 0.5;
	c1y = height * 0.5;
}

void cycles_drawing() {
	glBegin(GL_LINE_LOOP);
		for(GLdouble a = 0; a < 2 * PI; a += PI / 360) {
			GLdouble x = c1x + r1 * cos(a);
			GLdouble y = c1y + r1 * sin(a);
			glVertex2d(x, y);
		}
	glEnd();

	glBegin(GL_LINE_LOOP);
		for(GLdouble a = 0; a < ec * 2 * PI * r1 / r2; a += PI / 360) {
			GLdouble a1 = offset_a + a * kr;
			GLdouble c2x = c1x + (r1 - r2) * cos(a1);
			GLdouble c2y = c1y - (r1 - r2) * sin(a1);

			GLdouble x = c2x + k * r2 * cos(a);
			GLdouble y = c2y + k * r2 * sin(a);
			glVertex2d(x, y);
		}
	glEnd();
}

void display() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	cycles_drawing();
	glFlush();
	glutSwapBuffers();
}

int init() {                                      
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
	glPointSize(2.0);
	glColor3f(0.1f, 0.4f, 0.4f);

	glLineWidth(5.0);
	
	ec = lcm(100, rint(kr*100)) / 100;

	return 1;
}

GLint direct = 1;

void mouse(int button, int state, int x, int y) {
	direct = -direct;
}

void timf(int v) {
	//offset_a += 190;
	glutPostRedisplay();
	
	offset_a = offset_a +  1 * direct * PI / 360;
	
	printf("%f", offset_a);
	glutTimerFunc( 100, timf, 0 );
}

int main(int argc, char *argv[] ) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL lab 1");
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutTimerFunc(100, timf, 0); // Set up timer for 40ms, about 25 fps
	
	init();
	glutMainLoop();
	
	return 0;
}