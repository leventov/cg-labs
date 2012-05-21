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

GLenum polyMode; 
GLint phi = 0, psi = 45, aside = 1, top = 1;
GLdouble PI = 3.14159265;

int h, w;

void reshape(int width, int height) {
	h = height;
	w = width;
}

void crossProduct(double v1[3], double v2[3], double vr[3]) {
    vr[0] = (v1[1]*v2[2])-(v2[1]*v1[2]);
    vr[1] = (v1[2]*v2[0])-(v2[2]*v1[0]);
    vr[2] = (v1[0]*v2[1])-(v1[1]*v2[0]);
}

void normalize(double v[3]) {
	double c = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] /= c;
	v[1] /= c;
	v[2] /= c;
}

#define vCount (20)
#define lCount (15)
double r = 1.0;

double c[lCount][vCount][3];
double *center, *rx, *ry; 
void makeCircle(double mc[vCount][3]) {
	for (int i = 0; i < vCount; i++) {
		double a = i*2*PI/vCount;
		double mx = r*cos(a), my = r*sin(a);
		for (int j = 0; j < 3; j++)
			mc[i][j] = center[j] + rx[j]*mx + ry[j]*my;
	}
}

void initVertices() {
	center = (double[]) {0, 0, 0};
	rx = (double[]) {0, 1, 0};
	ry = (double[]) {1, 0, 0};
	double z, x;
	for (int i = 0; i < lCount; i++) {
		double a = i * PI / 90;
		double dz = 1.0/lCount * cos(a);
		double dx = 1.0/lCount * sin(a);
		z += dz;
		x += dz;
		double vv[3] = {dx, 0, dz}, vl[3] = {-dz, 0, dx};
		crossProduct(vv, vl, ry);
		crossProduct(vv, ry, rx);
		normalize(ry);
		normalize(rx);
		center[0] = x;
		center[2] = z;
		makeCircle(c[i]);
	}
}

void draw() {
	for (int i = 0; i < lCount; i++) {
		//glBegin(GL_LINE_LOOP);
		//for (int j = 0; j < vCount; j++) {
		//	glVertex3dv(c[i][j]);
		//	//printf("%f, %f, %f\n", c[i][j][0], c[i][j][1], c[i][j][2]);
		//} glEnd();
		if (i > 0) {
		glBegin(GL_QUAD_STRIP);
			for (int j = 0; j < vCount; j++) {
				glVertex3dv(c[i][j]);
				glVertex3dv(c[i-1][j]);
				glVertex3dv(c[i][(j+1)%vCount]);
				
				
				glVertex3dv(c[i-1][(j+1)%vCount]);
			}
		glEnd();
		}
	}
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
	glPolygonMode(GL_FRONT_AND_BACK, polyMode);
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
	glScaled(0.5,0.5,0.5);
	glRotated(90, 0,1,0);
	
	draw();
	
	
	// second view
	glViewport (w/2, h/2, w/2, h/2); 
	glLoadIdentity();
	glScaled(0.5,0.5,0.5);
	glRotated(90, 1,0,0);
	
	draw();
	
	// third
	glViewport (0, 0, w/2, h/2); 
	glLoadIdentity();
	//glRotated(90, 0,0,1);
	glScaled(0.5,0.5,0.5);
	draw();
	
	glFlush();
}

int init() {                                      
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
	glPointSize(2.0);
	glColor3f(0.1f, 0.4f, 0.4f);

	glLineWidth(1.0);
	polyMode = GL_LINE;
	//ec = lcm(100, rint(kr*100)) / 100;
	initVertices();

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
		case 'f':
			if (polyMode == GL_LINE) polyMode = GL_FILL;
			else polyMode = GL_LINE;
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