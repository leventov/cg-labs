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

#define WIND_STEP (0.001)
GLdouble wind = WIND_STEP;
GLdouble direction[3] = {0.4, 0.3, 0.5};
GLdouble position[3] = {0.0, 0.0, 0.0};

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

#define BOUND_SIDE (4.0)

GLdouble cube[8][3] = {
	{-BOUND_SIDE/2, -BOUND_SIDE/2, -BOUND_SIDE/2},
	{-BOUND_SIDE/2, -BOUND_SIDE/2, BOUND_SIDE/2},
	
	{BOUND_SIDE/2, -BOUND_SIDE/2, -BOUND_SIDE/2},
	{BOUND_SIDE/2, -BOUND_SIDE/2, BOUND_SIDE/2},
	
	{BOUND_SIDE/2, BOUND_SIDE/2, -BOUND_SIDE/2},
	{BOUND_SIDE/2, BOUND_SIDE/2, BOUND_SIDE/2},
	
	{-BOUND_SIDE/2, BOUND_SIDE/2, -BOUND_SIDE/2},
	{-BOUND_SIDE/2, BOUND_SIDE/2, BOUND_SIDE/2}
};

void draw_cube() {
	//glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_LINES);
	for (int i = 0; i < 4; i++) {
		glVertex3dv(cube[i*2]);
		glVertex3dv(cube[i*2+1]);
	}
	for (int i = 0; i < 4; i++) {
		glVertex3dv(cube[i*2]);
		glVertex3dv(cube[(i*2+2)%8]);
		glVertex3dv(cube[i*2+1]);
		glVertex3dv(cube[(i*2+3)%8]);
	}
	glEnd();
	//glColor()
}

void draw() {
	draw_cube();
	for (int i = 0; i < lCount; i++) {
		//glBegin(GL_LINE_LOOP);
		//for (int j = 0; j < vCount; j++) {
		//	glVertex3dv(c[i][j]);
		//	//printf("%f, %f, %f\n", c[i][j][0], c[i][j][1], c[i][j][2]);
		//} glEnd();
		int reflected = 0;
		if (i > 0) {
		glBegin(GL_QUAD_STRIP);
			for (int j = 0; j < vCount; j++) {
				glVertex3d(c[i][j][0]+position[0], c[i][j][1] + position[1], c[i][j][2]+position[2]);
				glVertex3d(c[i-1][j][0]+position[0], c[i-1][j][1] + position[1], c[i-1][j][2]+position[2]);
				
				glVertex3d(c[i][(j+1)%vCount][0] + position[0], 
						   c[i][(j+1)%vCount][1] + position[1], 
						   c[i][(j+1)%vCount][2] + position[2]);
				
				glVertex3d(c[i-1][(j+1)%vCount][0] + position[0], 
						   c[i-1][(j+1)%vCount][1] + position[1], 
						   c[i-1][(j+1)%vCount][2] + position[2]);
				if (reflected == 0) {
					if (abs(position[0]+c[i][j][0]) >= 2.0) {
						if (direction[0] * wind * (position[0]+c[i][j][0]) > 0) {
							direction[0] = -direction[0];
							reflected = 1;
						}
					}
					else if (abs(position[1]+c[i][j][1]) >= 2.0) {
						if (direction[1] * wind * (position[1]+c[i][j][1]) > 0) {
							direction[1] = -direction[1];
							reflected = 1;
						}
					}
					else if (abs(position[2]+c[i][j][2]) >= 2.0) {
						if (direction[2] * wind * (position[2]+c[i][j][2]) > 0) {
							direction[2] = -direction[2];
							reflected = 1;
						}
					}
				}
				
				//glVertex3dv(c[i-1][j]);
				//glVertex3dv(c[i][(j+1)%vCount]);
				
				
				//glVertex3dv(c[i-1][(j+1)%vCount]);
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

float diffuse[] = {1.0, 1.0, 1.0,1.0};
float ambient[] = {0.5, 0.5, 0.5, 1};

void display() {
	glPushMatrix();
		glTranslated(7.0, 7.0, 7.0);//координаты лампочки
		
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.9);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.9);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.9); //рассеивание устанавливается квадратичное
		
		GLfloat light_pos[] = { 0.0, 0.0, 0.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos); //вектор источника света, источник - лампочка
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		
		//glDisable(GL_LIGHTING);
		
		//рисуем источник света
		//printf("%d %d %d\n", red, green, blue);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	//	glutSolidSphere(0.1, 10, 10);
		
		//glEnable(GL_LIGHTING);
	glPopMatrix();
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
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
	glScaled(0.3,0.3,0.3);
	glRotated(90, 0,1,0);
	
	draw();
	
	
	// second view
	glViewport (w/2, h/2, w/2, h/2); 
	glLoadIdentity();
	glScaled(0.3,0.3,0.3);
	glRotated(90, 1,0,0);
	
	draw();
	
	// third
	glViewport (0, 0, w/2, h/2); 
	glLoadIdentity();
	//glRotated(90, 0,0,1);
	glScaled(0.3,0.3,0.3);
	draw();
	
	glFlush();
}

int init() {                                      
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
	glPointSize(2.0);
	glColor3f(0.1f, 0.4f, 0.4f);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

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



void timf(int v) {
	glutPostRedisplay();
	/*
	//offset_a += 190;
	
	
	offset_a = offset_a +  1 * direct * PI / 360;
	
	printf("%f", offset_a);
	*/
	position[0] += direction[0] * wind;
	position[1] += direction[1] * wind;
	position[2] += direction[2] * wind;
	glutTimerFunc( 10, timf, 0 );
}


void pressKey(unsigned char key, int x, int y) {
	switch (key) {
		case 'r':
			//diffuse[0] = (diffuse[0] + 1) % 10;
			break;
		case 'g':
			//diffuse[1] = (diffuse[1] + 1) % 10;
			break;
		case 'b':
			//diffuse[2] = (diffuse[2] + 1) % 10;
			break;
		case 'm':
			wind += WIND_STEP;
			break;
		case 'n':
			wind -= WIND_STEP;
			break;
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
	glutTimerFunc(10, timf, 0); // Set up timer for 40ms, about 25 fps
	
	init();
	glutMainLoop();
	return 0;
}