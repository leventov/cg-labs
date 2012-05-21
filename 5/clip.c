#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define LINES (100)

int H = 600, W = 600;

typedef struct {
	double x, y, z;
} vector;

typedef struct {
	vector p1, p2; 
} segment;

int zero(double d) {
	return (fabs(d) < 0.001);
}

double sProduct(vector *a, vector *b) {
	return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

vector sub(vector *a, vector *b) {
	return (vector) {.x = a->x - b->x, .y = a->y - b->y, .z = a->z - b->z};
}

vector add(vector *a, vector *b) {
	return (vector) {.x = a->x + b->x, .y = a->y + b->y, .z = a->z + b->z};
}

vector sk(vector *a, double k) {
	return (vector) {.x = a->x *= k, .y = a->y *= k, .z =  a->z *= k};
}

vector param(segment *s, double k) {
	return (vector) {.x = s->p1.x * (1.0 - k) + s->p2.x * k,
	                 .y = s->p1.y * (1.0 - k) + s->p2.y * k,
	                 .z = s->p1.z * (1.0 - k) + s->p2.z * k
	                };
}

vector *n, *f;
int k;

segment clip(segment *s) {
	double t0 = 0, t1 = 1;
	double Ds, Ws, t;
	vector ww;
	vector d = sub(&(s->p2), &(s->p1));
	for (int i = 0; i < k; i++) {
		ww = sub(&(s->p1), f + i);
		Ds = sProduct(&d, n + i);
		Ws = sProduct(&ww, n + i);
		if (zero(Ds)) goto m2; //2
		t = -Ws/Ds;
		if (Ds > 0.0) goto m1; //1
		if (t < 0.0) goto m4; // 4
		if (t < t1) t1 = t;
		continue;
		m1:
		if (t > 1.0) goto m4; //4
		if (t > t0) t0 = t;
		continue;
		m2:
		if (Ws < 0.0) goto m4; //4
	}
	
	m4:
	printf("clip: %g %g\n", t0, t1);
	return (segment) {param(s, t0), param(s, t1)};
}

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

GLint line_phi = 0, line_psi = 90;
segment init_seg;
segment res_seg;

int h, w;

void recountSeg() {
	double rpsi = PI*line_psi/180;
	double rphi = PI*line_phi/180;
	init_seg.p1 = (vector) {-1, 0, 0};
	init_seg.p2.x = cos(rphi);
	init_seg.p2.y = sin(rphi);
	init_seg.p2.z = cos(rpsi);
	printf("init_end: %g %g %g\n", init_seg.p2.x, init_seg.p2.y, init_seg.p2.z);
	res_seg = clip(&init_seg);
}

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

#define vCount (4)
#define lCount (2)
double r = 1.0;

double c[lCount][vCount][3] = {
	{{-0.5, -0.5, -0.5}, {-0.5, 0.5, -0.5}, 
	{-0.5, 0.5, 0.5}, {-0.5, -0.5, 0.5}},
	{{0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, 
	{0.5, 0.5, 0.5}, {0.5, -0.5, 0.5}}
};
double *center, *rx, *ry; 


void initClip() {
	k = 6;
	n = (vector *) malloc(6 * sizeof(vector));
	f = (vector *) malloc(6 * sizeof(vector));
	for (int i = 0; i  < 6; i++) {
		*(f + i) = i < 3 ? (vector) {-0.5, -0.5, -0.5} : (vector) {0.5, 0.5, 0.5};
	}
	*n = (vector) {1, 0, 0};
	*(n + 1) = (vector){0, 1, 0};
	*(n + 2) = (vector){0, 0, 1};
	*(n + 3) = (vector){0, -1, 0};
	*(n + 4) = (vector){0, 0, -1};
	*(n + 5) = (vector){-1, 0, 0};
}

void draw() {
	for (int i = 0; i < lCount; i++) {
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
	
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3d(init_seg.p1.x, init_seg.p1.y, init_seg.p1.z);
	glVertex3d(res_seg.p1.x, res_seg.p1.y, res_seg.p1.z);
	
	glColor3f(0.1f, 0.4f, 0.4f);
	glVertex3d(res_seg.p1.x, res_seg.p1.y, res_seg.p1.z);
	glVertex3d(res_seg.p2.x, res_seg.p2.y, res_seg.p2.z);
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3d(res_seg.p2.x, res_seg.p2.y, res_seg.p2.z);
	glVertex3d(init_seg.p2.x, init_seg.p2.y, init_seg.p2.z);
	glColor3f(0.1f, 0.4f, 0.4f);
	glEnd();
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
	initClip();

	return 1;
}

GLint direct = 1;

void mouse(int button, int state, int x, int y) {
	direct = -direct;
}

void pressKey(unsigned char key, int x, int y) {
	switch (key) {
		case 'a':
			phi++;
			break;
		case 'd':
			phi--;
			break;
			
		case 's':
			psi--;
			break;
		case 'w':
			psi++;
			break;
		case 'z':
			if (aside > 1)
				aside--;
			break;
		case 'x':
			aside++;
			break;
		case 'f':
			if (polyMode == GL_LINE) polyMode = GL_FILL;
			else polyMode = GL_LINE;
			
		case 'k':
			line_psi--;
			recountSeg();
			break;
		case 'i':
			line_psi++;
			recountSeg();
			break;
	}
	
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

