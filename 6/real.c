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

int draw_texture = 1;
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
double r = 0.8;

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

#define TEXTURE_SIDE (16)
GLubyte texture[TEXTURE_SIDE][TEXTURE_SIDE][4];
void generateTexture() {
	for (int i = 0; i < TEXTURE_SIDE; i++) {
		for (int j = 0; j < TEXTURE_SIDE; j++) {
			int gap = rand() % 255;
			//printf("%d\n", gap);
			texture[i][j][0] = gap;
			
			gap = rand() % 255;
			//printf("%d\n", gap);
			texture[i][j][1] = gap;
			gap = rand() % 255;
			//printf("%d\n", gap);
			texture[i][j][2] = gap;
			texture[i][j][3] = 255;
		}
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

float reflection[] = {1.0, 1.0, 1.0, 1.0};

void draw_cube() {
	float cube_reflection[] = {0.0, 0.0, 0.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cube_reflection);
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
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, reflection);
}

void draw() {
	draw_cube();
	glColor3d(1.0, 1.0, 1.0);
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
				glTexCoord2d(1.0, 0.0);
				glVertex3d(c[i][j][0]+position[0], c[i][j][1] + position[1], c[i][j][2]+position[2]);
				
				glTexCoord2d(0.0, 0.0);
				glVertex3d(c[i-1][j][0]+position[0], c[i-1][j][1] + position[1], c[i-1][j][2]+position[2]);
				
				glTexCoord2d(0.0, 1.0);
				glVertex3d(c[i][(j+1)%vCount][0] + position[0], 
						   c[i][(j+1)%vCount][1] + position[1], 
						   c[i][(j+1)%vCount][2] + position[2]);
				
				glTexCoord2d(1.0, 1.0);
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

float diffuse[] = {1.0, 1.0, 1.0, 1.0};
float ambient[] = {1.0, 1.0, 0.0, 1.0};

#define TUBE_SCALE (0.33)

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	if (draw_texture)
		glEnable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	//glDisable(GL_LIGHTING);
//	float spec[] = {1.0, 1.0, 1.0, 1.0};
	//glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, reflection);
	
	
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
	glScaled(TUBE_SCALE, TUBE_SCALE, TUBE_SCALE);
	glRotated(90, 0,1,0);
	
	draw();
	
	
	// second view
	glViewport (w/2, h/2, w/2, h/2); 
	glLoadIdentity();
	glScaled(TUBE_SCALE, TUBE_SCALE, TUBE_SCALE);
	glRotated(90, 1,0,0);
	
	draw();
	
	// third
	glViewport (0, 0, w/2, h/2); 
	glLoadIdentity();
	//glRotated(90, 0,0,1);
	glScaled(TUBE_SCALE, TUBE_SCALE, TUBE_SCALE);
	draw();
	
	if (draw_texture)
		glDisable(GL_TEXTURE_2D);
		
	glFlush();
}

float light_pos[] = {4, 4, 4, 1};
float light_dir[] = {-1, -1, -1};

int textureID;

int init() {                                      
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
	glPointSize(2.0);
	glColor3f(0.1f, 0.4f, 0.4f);
	generateTexture();
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_dir);
	
	glGenTextures(1, &textureID);
	
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(
		GL_TEXTURE_2D, 0, 
		GL_RGBA,
		TEXTURE_SIDE, TEXTURE_SIDE, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE,
		texture
	);


                  
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glLineWidth(1.0);
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

	position[0] += direction[0] * wind;
	position[1] += direction[1] * wind;
	position[2] += direction[2] * wind;
	glutTimerFunc( 10, timf, 0 );
}


void input() {
	FILE *sc = fopen("scene", "r");
	fscanf(sc, "%d", &phi);
	fscanf(sc, "%d", &psi);
	
	fscanf(sc, "%lf", &wind);
	
	fscanf(sc, "%lf", position);
	fscanf(sc, "%lf", position + 1);
	fscanf(sc, "%lf", position + 2);
	
	fscanf(sc, "%lf", direction);
	fscanf(sc, "%lf", direction + 1);
	fscanf(sc, "%lf", direction + 2);
	
	float *(floatParams[3]) = {diffuse, ambient, reflection};
	for (int i = 0; i < 3; i++) {
		fscanf(sc, "%f", floatParams[i]);
		fscanf(sc, "%f", floatParams[i] + 1);
		fscanf(sc, "%f", floatParams[i] + 2);
		fscanf(sc, "%f", floatParams[i] + 3);
	}
	
	fscanf(sc, "%d", &draw_texture);
	
	fclose(sc);
}

void output() {
	FILE *sc = fopen("scene", "w");
	fprintf(sc, "%d\n", phi);
	fprintf(sc, "%d\n", psi);
	
	fprintf(sc, "%lf\n", wind);
	
	fprintf(sc, "%lf\n", position[0]);
	fprintf(sc, "%lf\n", position[1]);
	fprintf(sc, "%lf\n", position[2]);
	
	fprintf(sc, "%lf\n", direction[0]);
	fprintf(sc, "%lf\n", direction[1]);
	fprintf(sc, "%lf\n", direction[2]);
	
	float *(floatParams[3]) = {diffuse, ambient, reflection};
	for (int i = 0; i < 3; i++) {
		fprintf(sc, "%f\n", floatParams[i][0]);
		fprintf(sc, "%f\n", floatParams[i][1]);
		fprintf(sc, "%f\n", floatParams[i][2]);
		fprintf(sc, "%f\n", floatParams[i][3]);
	}
	
	fprintf(sc, "%d\n",  draw_texture);
	
	fclose(sc);
	glutPostRedisplay();
}


void pressKey(unsigned char key, int x, int y) {
	switch (key) {
		case 'r':
			if (ambient[1] > 0.1) ambient[1] -= 0.1;
			ambient[0] = 2.0 - ambient[1];
			break;
		case 'y':
			if (ambient[1] < 0.9) ambient[1] += 0.1;
			ambient[0] = 2.0 - ambient[1];
			break;
		
		case 'g':
			if (reflection[0] > 0.1) {
				reflection[0] -= 0.1;
				reflection[1] -= 0.1;
				reflection[2] -= 0.1;
			}
			break;
		case 'b':
			if (reflection[0] < 0.9) {
				reflection[0] += 0.1;
				reflection[1] += 0.1;
				reflection[2] += 0.1;
			}
			break;
		case 'm':
			wind += WIND_STEP;
			break;
		case 'n':
			wind -= WIND_STEP;
			break;
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
		case 'f':
			draw_texture = 1 - draw_texture;
			if (draw_texture)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case 'i':
			input();
			break;
		case 'o':
			output();
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
	glutTimerFunc(10, timf, 0); // Set up timer for 40ms, about 25 fps
	
	init();
	glutMainLoop();
	return 0;
}
