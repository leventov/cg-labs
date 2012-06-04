#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define POLYGONS_ON_MOVE 1
#define FILL_ON_MOVE 1
#define MOVE_TRANSLATE 1
#define DISPLAY_LISTS 1

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

#define WIND_STEP (0.01)
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



#define vCount (80)
#define lCount (60)
double r = 0.8;

double ***c;
double *center, *rx, *ry; 
void makeCircle(double **mc) {
	for (int i = 0; i < vCount; i++) {
		double a = i*2*PI/vCount;
		double mx = r*cos(a), my = r*sin(a);
		for (int j = 0; j < 3; j++)
			mc[i][j] = center[j] + rx[j]*mx + ry[j]*my;
	}
}


int less = 0;
#define LESSvCount (20)
#define LESSlCount (15)

double ***LESSc;
void LESSmakeCircle(double **mc) {
	for (int i = 0; i < LESSvCount; i++) {
		double a = i*2*PI/LESSvCount;
		double mx = r*cos(a), my = r*sin(a);
		for (int j = 0; j < 3; j++)
			mc[i][j] = center[j] + rx[j]*mx + ry[j]*my;
	}
}

void initVertices() {
	c = (double ***) malloc(sizeof(double**) * lCount);
	for (int i = 0; i < lCount; i++) {
		c[i] = (double **) malloc(sizeof(double*) * vCount);
		for (int j = 0; j < vCount; j++)
			c[i][j] = (double *) calloc(3, sizeof(double));
	}
	
	LESSc = (double ***) malloc(sizeof(double**) * LESSlCount);
	for (int i = 0; i < LESSlCount; i++) {
		LESSc[i] = (double **) malloc(sizeof(double*) * LESSvCount);
		for (int j = 0; j < LESSvCount; j++)
			LESSc[i][j] = (double *) calloc(3, sizeof(double));
	}
	//printf("%lf\n", c[0][0][0]);
	
	center = (double[]) {0, 0, 0};
	rx = (double[]) {0, 1, 0};
	ry = (double[]) {1, 0, 0};
	double z, x;
	for (int i = 0; i < lCount; i++) {
		double a = i * PI / lCount/5;
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
	
	int lessC1 = lCount / LESSlCount;
	int lessC2 = vCount / LESSvCount;
	for (int i = 0; i < LESSlCount; i++) {
		for (int j = 0; j < LESSvCount; j++) {
			int il = (i + 1 == LESSlCount) ? lCount - 1 : i * lessC1;
			int iv = (j + 1 == LESSvCount) ? vCount - 1 : j * lessC2;
			LESSc[i][j][0] = c[il][iv][0];
			LESSc[i][j][1] = c[il][iv][1];
			LESSc[i][j][2] = c[il][iv][2];
		}
	}
}

#define TEXTURE_SIDE (4)
GLubyte texture[TEXTURE_SIDE][TEXTURE_SIDE][4];
void generateTexture() {
	for (int i = 0; i < TEXTURE_SIDE; i++) {
		for (int j = 0; j < TEXTURE_SIDE; j++) {
			int gap = rand() % 50 - 25;
			texture[i][j][0] = 200 + gap;
			texture[i][j][1] = 200 + gap;
			texture[i][j][2] = 200 + gap;
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

int less_count = 0;
int unless_count = 0;

void checkPosition() {
	for (int i = 0; i < lCount; i++) {
		for (int j = 0; j < vCount; j++) {
			
			if (abs(position[0]+c[i][j][0]) >= 2.0) {
				if (direction[0] * wind * (position[0]+c[i][j][0]) > 0) {
					direction[0] = -direction[0];
					return;
				}
			}
			else if (abs(position[1]+c[i][j][1]) >= 2.0) {
				if (direction[1] * wind * (position[1]+c[i][j][1]) > 0) {
					direction[1] = -direction[1];
					return;
				}
			}
			else if (abs(position[2]+c[i][j][2]) >= 2.0) {
				if (direction[2] * wind * (position[2]+c[i][j][2]) > 0) {
					direction[2] = -direction[2];
					return;
				}
			}
		}
	}
}

int LESSDL = -1;
int DL = -1;

void draw() {
	//printf("1\n");
	draw_cube();

	#if POLYGONS_ON_MOVE
	//printf("1\n");
		double ***lo_c = less ? LESSc : c;
		int lC = less ? LESSlCount : lCount;
		int vC = less ? LESSvCount : vCount;
		if (less) less_count++;
		else unless_count++;
	#else
	
		double ***lo_c = c;
		//printf("%lf\n", c[0][0][0]);
		
		int lC = lCount;
		int vC = vCount;
	#endif
	
	#if FILL_ON_MOVE
	if (less) {
			#if MOVE_TRANSLATE
			glTranslated(position[0], position[1], position[2]);
			#if DISPLAY_LISTS
			if (LESSDL != -1) {
				glCallList(LESSDL);
			} else {
				LESSDL = glGenLists(1);
				glNewList(LESSDL, GL_COMPILE_AND_EXECUTE);
			#endif
				
				
				for (int i = 0; i < lC; i++) {
					int reflected = 0;
					
					if (i > 0) {
					glBegin(GL_QUAD_STRIP);
						for (int j = 0; j < vC; j++) {
							
							glVertex3d(lo_c[i][j][0], lo_c[i][j][1], lo_c[i][j][2]);
							glVertex3d(lo_c[i-1][j][0], lo_c[i-1][j][1], lo_c[i-1][j][2]);
							glVertex3d(lo_c[i][(j+1)%vC][0], 
									   lo_c[i][(j+1)%vC][1], 
									   lo_c[i][(j+1)%vC][2]);
							glVertex3d(lo_c[i-1][(j+1)%vC][0], 
									   lo_c[i-1][(j+1)%vC][1], 
									   lo_c[i-1][(j+1)%vC][2]);
						}
					glEnd();
					}
				}
				
			#if DISPLAY_LISTS
				glEndList();
			}
			#endif

			#else
				for (int i = 0; i < lC; i++) {
				int reflected = 0;
				
				if (i > 0) {
				glBegin(GL_QUAD_STRIP);
					for (int j = 0; j < vC; j++) {
						
						glVertex3d(lo_c[i][j][0]+position[0], lo_c[i][j][1] + position[1], lo_c[i][j][2]+position[2]);
						glVertex3d(lo_c[i-1][j][0]+position[0], lo_c[i-1][j][1] + position[1], lo_c[i-1][j][2]+position[2]);
						glVertex3d(lo_c[i][(j+1)%vC][0] + position[0], 
								   lo_c[i][(j+1)%vC][1] + position[1], 
								   lo_c[i][(j+1)%vC][2] + position[2]);
						glVertex3d(lo_c[i-1][(j+1)%vC][0] + position[0], 
								   lo_c[i-1][(j+1)%vC][1] + position[1], 
								   lo_c[i-1][(j+1)%vC][2] + position[2]);
					}
				glEnd();
				}
			}
			#endif
			

	}
	else {
	#endif
	#if MOVE_TRANSLATE
	
	glTranslated(position[0], position[1], position[2]);
	#if DISPLAY_LISTS
	if (DL != -1) {
		glCallList(DL);
	} else {
		DL = glGenLists(1);
		glNewList(DL, GL_COMPILE_AND_EXECUTE);
	#endif
		
		for (int i = 0; i < lC; i++) {
			int reflected = 0;
			
			if (i > 0) {
			glBegin(GL_QUAD_STRIP);
				for (int j = 0; j < vC; j++) {
					glTexCoord2d(1.0, 0.0);
					
					glVertex3d(lo_c[i][j][0], lo_c[i][j][1], lo_c[i][j][2]);
					
					glTexCoord2d(0.0, 0.0);
					glVertex3d(lo_c[i-1][j][0], lo_c[i-1][j][1], lo_c[i-1][j][2]);
					
					glTexCoord2d(0.0, 1.0);
					glVertex3d(lo_c[i][(j+1)%vC][0], 
							   lo_c[i][(j+1)%vC][1], 
							   lo_c[i][(j+1)%vC][2]);
					
					glTexCoord2d(1.0, 1.0);
					glVertex3d(lo_c[i-1][(j+1)%vC][0], 
							   lo_c[i-1][(j+1)%vC][1], 
							   lo_c[i-1][(j+1)%vC][2]);
				}
			glEnd();
			}
		}
		
	#if DISPLAY_LISTS
		glEndList();
	}
	#endif
		
	#else
	for (int i = 0; i < lC; i++) {
		int reflected = 0;
		
		if (i > 0) {
		glBegin(GL_QUAD_STRIP);
			for (int j = 0; j < vC; j++) {
				glTexCoord2d(1.0, 0.0);
				
				glVertex3d(lo_c[i][j][0]+position[0], lo_c[i][j][1] + position[1], lo_c[i][j][2]+position[2]);
				
				glTexCoord2d(0.0, 0.0);
				glVertex3d(lo_c[i-1][j][0]+position[0], lo_c[i-1][j][1] + position[1], lo_c[i-1][j][2]+position[2]);
				
				glTexCoord2d(0.0, 1.0);
				glVertex3d(lo_c[i][(j+1)%vC][0] + position[0], 
						   lo_c[i][(j+1)%vC][1] + position[1], 
						   lo_c[i][(j+1)%vC][2] + position[2]);
				
				glTexCoord2d(1.0, 1.0);
				glVertex3d(lo_c[i-1][(j+1)%vC][0] + position[0], 
						   lo_c[i-1][(j+1)%vC][1] + position[1], 
						   lo_c[i-1][(j+1)%vC][2] + position[2]);
			}
		glEnd();
		}
	}
	#endif
	#if FILL_ON_MOVE
	}
	#endif
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

double diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks*10)/CLOCKS_PER_SEC;
	return diffms;
}

float diffuse[] = {1.0, 1.0, 1.0, 1.0};
float ambient[] = {1.0, 1.0, 0.0, 1.0};

#define TUBE_SCALE (0.33)

double t_sum = 0.0;
double t_count = 0.0;

void display() {
	clock_t t_st = clock();
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (draw_texture)
		glEnable(GL_TEXTURE_2D);
	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
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
	
	clock_t t_end = clock();
	t_sum += diffclock(t_end, t_st);
	t_count++;
}

float light_pos[] = {4, 4, 4, 1};
float light_dir[] = {-1, -1, -1};

int textureID;

int init() {                                      
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
	glPointSize(2.0);
	glColor3f(0.1f, 0.4f, 0.4f);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_dir);
	
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	gluBuild2DMipmaps(
		GL_TEXTURE_2D,
		GL_RGBA,
		TEXTURE_SIDE, TEXTURE_SIDE,
		GL_RGBA, GL_UNSIGNED_BYTE,
		texture
	);
                  
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

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
	less = 0;
	position[0] += direction[0] * wind;
	position[1] += direction[1] * wind;
	position[2] += direction[2] * wind;
	checkPosition();
	glutTimerFunc( 40, timf, 0 );
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
			less = 1;
			phi++;
			break;
		case 'd':
			less = 1;
			phi--;
			break;
		case 's':
			less = 1;
			psi--;
			break;
		case 'w':
			less = 1;
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
		case 'c':
			printf("%lf %d %d\n", t_sum / t_count, less_count, unless_count);
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
	glutTimerFunc(40, timf, 0); // Set up timer for 40ms, about 25 fps
	
	init();
	
	glutMainLoop();
	return 0;
}
