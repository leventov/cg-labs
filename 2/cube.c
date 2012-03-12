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


GLdouble phi = 0, psi = 45, aside = 1;
GLdouble PI = 3.14159265;

int h, w;

void reshape(int width, int height) {
	h = height;
	w = width;
	glViewport(0, 0, h/2, w/2);
}


void draw() {
	glBegin(GL_TRIANGLES); 
		glColor3f(1.0f,0.0f,0.0f);                      // Красный
		glVertex3f( 0.0f, 1.0f, 0.0f);                  // Верх треугольника (Передняя)
		glColor3f(0.0f,1.0f,0.0f);                      // Зеленный
		glVertex3f(-1.0f,-1.0f, 1.0f);                  // Левая точка
		glColor3f(0.0f,0.0f,1.0f);                      // Синий
		glVertex3f( 1.0f,-1.0f, 1.0f); 
		
		glColor3f(1.0f,0.0f,0.0f);                      // Красная
		glVertex3f( 0.0f, 1.0f, 0.0f);                  // Верх треугольника (Правая)
		glColor3f(0.0f,0.0f,1.0f);                      // Синия
		glVertex3f( 1.0f,-1.0f, 1.0f);                  // Лево треугольника (Правая)
		glColor3f(0.0f,1.0f,0.0f);                      // Зеленная
		glVertex3f( 1.0f,-1.0f, -1.0f);                 // Право треугольника (Правая)
		
		glColor3f(1.0f,0.0f,0.0f);                      // Красный
		glVertex3f( 0.0f, 1.0f, 0.0f);                  // Низ треугольника (Сзади)
		glColor3f(0.0f,1.0f,0.0f);                      // Зеленный
		glVertex3f( 1.0f,-1.0f, -1.0f);                 // Лево треугольника (Сзади)
		glColor3f(0.0f,0.0f,1.0f);                      // Синий
		glVertex3f(-1.0f,-1.0f, -1.0f);                 // Право треугольника (Сзади)
		
		glColor3f(1.0f,0.0f,0.0f);                      // Красный
		glVertex3f( 0.0f, 1.0f, 0.0f);                  // Верх треугольника (Лево)
		glColor3f(0.0f,0.0f,1.0f);                      // Синий
		glVertex3f(-1.0f,-1.0f,-1.0f);                  // Лево треугольника (Лево)
		glColor3f(0.0f,1.0f,0.0f);                      // Зеленный
		glVertex3f(-1.0f,-1.0f, 1.0f);                  // Право треугольника (Лево)
	glEnd();                                        // Кончили рисовать пирамиду
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
	0, 0, 1, -1,
	0, 0, 0, 1
};

double Z_P[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 0, -1,
	0, 0, 0, 1
};

void display() {
	//glViewport(0, 0, h/2, w/2);
	//glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	double rpsi = PI*psi/180;
	double rphi = PI*phi/180;
	double eye_z = sin(rpsi);
	double xy = cos(rpsi);
	double eye_x = xy * cos(rphi);
	double eye_y = sqrt(xy*xy - eye_x*eye_x);
	
	gluLookAt(5*eye_x, 5*eye_y, 5*eye_z, 0, 0, 0, 0, 0, 1);
	glOrtho(-20.0,20.0,-20.0,20.0,20.0,-3.0);
	
	//glRotated(angle, x, y, z);
	double scale = 3.0/aside; 
	glScaled(scale, scale, scale);
	
	//glMatrixMode(GL_PROJECTION);
	//glMultTransposeMatrixd(X_P);
	draw();
	glFlush();
	glutSwapBuffers();
	
}

int init() {                                      
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
	glPointSize(2.0);
	glColor3f(0.1f, 0.4f, 0.4f);

	glLineWidth(5.0);
	
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
			break;
		case 'w'://GLUT_KEY_RIGHT: 
			psi++;
			break;
		case 'z'://GLUT_KEY_UP:
			if (aside > 1)
				aside--;
			break;
		case 'x'://GLUT_KEY_DOWN: 
			aside++;
			break;
	}
	printf("%g\n", phi);
	glutPostRedisplay();
 }

int main(int argc, char *argv[] ) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL lab 2");
	
	//glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(pressKey);
	//glutKeyboardUpFunc(releaseKey);
	//glutMouseFunc(mouse);
	//glutTimerFunc(100, timf, 0); // Set up timer for 40ms, about 25 fps
	
	init();
	glutMainLoop();
	return 0;
}