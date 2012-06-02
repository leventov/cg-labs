#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define LINES (100)

int H = 600, W = 600;

int smooth, filled;

struct P {
	unsigned char red, green, blue;
};
typedef struct P Pixel;

typedef struct {
	int red, green, blue;
} FullPixel;

void add(FullPixel *p1, Pixel *p2, int k) {
	p1->red += ((int)p2->red) * k;
	p1->green += ((int)p2->green) * k;
	p1->blue += ((int)p2->blue) * k;
}

void multiply(FullPixel *p, int k) {
	p->red *= k;
	p->green *= k;
	p->blue *= k;
}

void divide(FullPixel *p, double d) {
	p->red /= d;
	p->green /= d;
	p->blue /= d;
}

void toPixel(FullPixel *p, Pixel *g) {
	g->red = (unsigned char)p->red;
	g->green = (unsigned char)p->green;
	g->blue = (unsigned char)p->blue;
}


struct M {
	Pixel *pixels;
} rawBuffer, filteredBuffer;

typedef struct M Matrix;

void putPixel(Matrix *m, int x, int y, Pixel *p) {
	m->pixels[y*W+x] = *p;
}

Pixel DEFAULT_COLOR = {26, 102, 102};

void putRaw(int x, int y) {
	putPixel(&rawBuffer, x, y, &DEFAULT_COLOR);
}

int vCount, fx, fy, lx, ly, lly, llx;

struct Edges {
	int *lines;
	int *sizes;
} edges;


void edgeInsert(int x, int line /* y */, int vertex) {

	int imin = 0, imax = edges.sizes[line];

	if (!imax) {

		edges.lines[line*LINES + 0] = x;

		edges.sizes[line]++;
		return;
	}

	imax--;

	while (imax > imin) {
		int imid = (imin + imax) / 2;
		if (edges.lines[line*LINES + imid] < x)
			imin = imid + 1;
		else
			imax = imid;
	}
	printf("1\n");
	if (vertex && imax == imin && edges.lines[line*LINES + imin] == x)
		return; // unique points
	else {
		for (int pos = edges.sizes[line]; pos > imin + 1; pos--)
			edges.lines[line*LINES + pos] = edges.lines[line*LINES + pos - 1];
		
		if (edges.lines[line*LINES + imin] <= x)
			edges.lines[line*LINES + imin + 1] = x;
		else {
			edges.lines[line*LINES + imin + 1] = edges.lines[line*LINES + imin];
			edges.lines[line*LINES + imin] = x;
		}
		edges.sizes[line]++;
	}
	printf("1\n");
}

// Brezenham
void drawLine(int x1, int y1, int x2, int y2) {
	const int deltaX = abs(x2 - x1);
	const int deltaY = abs(y2 - y1);
	const int signX = x1 < x2 ? 1 : -1;
	const int signY = y1 < y2 ? 1 : -1;
	//
	int error = deltaX - deltaY;
	//
	int prevY = y2;
	int v = 0;
	if (!((y2 < y1 && lly < y1) || (y2 > y1 && lly > y1))) {
		//addVertex(x1, y1);
		v = 1;
		//printf("%d %d %d\n", lly, y1, y2);
	}
	
	edgeInsert(x2, y2, 0);
	putRaw(x2, y2);
	edgeInsert(x1, y1, v);
	putRaw(x1, y1);
	int y11 = y1;
	while(x1 != x2 || y1 != y2) {
		if (prevY != y1 && y1 != y2 && y1 != y11) {
			edgeInsert(x1, y1,0);
			prevY =y1;
		}
		putRaw(x1, y1);
		
		const int error2 = error * 2;
		//
		if(error2 > -deltaY) {
			error -= deltaY;
			x1 += signX;
		}
		if(error2 < deltaX) {
			error += deltaX;
			y1 += signY;
		}
	}
}

void addEdgeTo(int x, int y) {
	if (!vCount) {
		fx = x;
		fy = y;
	} else {
		// Не добавлять горизонтальные ребра в построчный скан
		if (ly == y && lx != x) {
			int minx, maxx;
			if (x < lx) minx = x, maxx = lx;
			else minx = lx, maxx = x;
			for (int i = minx; i < maxx; i++)
				putRaw(i, y);
		} else
			drawLine(lx, ly, x, y);
	}
	llx = lx;
	lly = ly;
	lx = x;
	ly = y;
	vCount++;
}


void fill() {
	addEdgeTo(fx, fy);
	
	for (int y = 0; y < H; y++)
		for (int j = 0; j < edges.sizes[y] - 1; j+=2) {
//			int v1 = 0, v2 = 0;
//			for (int k = 0; k < edges.vsizes[y]; k++) {
//				if (edges.lines[y][j] == edges.vertices[y][k]) {
//					v1 = 1;
//				}
//				if (edges.lines[y][j+1] == edges.vertices[y][k]) {
//					v2 = 1;
//				}
//				if (v1 || v2) break;
//			}
//			if (v1) {
//				j--; // to plus 1 in sum with 
//				continue;
//			}
			for (int x = edges.lines[y*LINES + j];
				x < edges.lines[y*LINES + j + 1]; 
				x++)
				putRaw(x, y);
//			if (v2) j++;
		}
}

void filterBuffer() {
	double vs[] = {16.0, 12.0, 9.0};
	
	for (int i = 0; i < H; i++)
		for (int j = 0; j < W; j++) {
			int mini = -1, minj = -1, maxi = 2, maxj = 2;		
			int vc = 0;
			if (!i) mini = 1, vc++;
			if (!j) minj = 1, vc++;
			if (i == H - 1) maxi = -1, vc++;
			if (j == W - 1) maxj = -1, vc++;
			
			FullPixel a = {0, 0, 0};
			
			for (int di = mini; di < maxi; di += 2)
				for (int dj = minj; dj < maxj; dj += 2)
					add(&a, &rawBuffer.pixels[(i + di)*W + j + dj], 1);

			for (int di = mini; di < maxi; di += 2)
				add(&a, &rawBuffer.pixels[(i + di)*W + j], 2);

			for (int dj = minj; dj < maxj; dj += 2)
				add(&a, &rawBuffer.pixels[(i)*W + j + dj], 2);
			
			add(&a, &rawBuffer.pixels[i*W + j], 4);
			
			//if (i == 2 && j == 2 )printf("%d %d %d ", a.red, a.green, a.blue);
			divide(&a, vs[vc]);
			//if (i == 2 && j == 2 )printf("%d %d %d\n", a.red, a.green, a.blue);
			
			toPixel(&a, &filteredBuffer.pixels[i*W + j]);
		}
}

void draw() {
	if (smooth)
		glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, filteredBuffer.pixels);
	else
		glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, rawBuffer.pixels);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glReadBuffer(GL_FRONT);
	draw();
	glFlush();
}

void init() {
	rawBuffer.pixels = (Pixel*) calloc(H*W*3, 1);
	
	memset(rawBuffer.pixels, 255U, H*W*3);
	
	
	filteredBuffer.pixels = (Pixel*) calloc(H*W*3, 1);
	
	//memset(&edges.lines, 0, H*LINES*4);
	//memset(&edges.vertices, 0, H*LINES*4);
	//memset(&edges.sizes, 0, H*4);
	//memset(&edges.vsizes, 0, H*4);
	
	edges.lines =  (int*) calloc(H, LINES*4);
	edges.sizes = (int*) calloc(H, 4);
	smooth = 0;
	filled = 0;
	vCount = 0;
}

void mouseClicked(int button, int state, int x, int y) {
	if (!filled && state == GLUT_DOWN) {
		addEdgeTo(x, H - y);
		glutPostRedisplay();
	}
}

void pressKey(unsigned char key, int x, int y) {
	switch (key) {
		case 'f':
			if (!filled)
				fill();
			filled = 1;
			break;
		case 's':
			smooth = 1 - smooth;
			if (smooth)
				filterBuffer();
			break;
		case 'c':
			init();
			break;
	}
	glutPostRedisplay();
}

void reshape(int w, int h) {
	H = h; W = w;
	init();
}


int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(W, H);
	
	init();
	
	glutCreateWindow("OpenGL lab 4");
	glutMouseFunc(mouseClicked);
	glutDisplayFunc(display);
	glutKeyboardFunc(pressKey);
	glutReshapeFunc(reshape);
	
	glutMainLoop();
	return 0;
}

