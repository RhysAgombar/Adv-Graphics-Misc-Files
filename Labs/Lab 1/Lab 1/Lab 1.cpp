// Lab 1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <FreeImage.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <glut.h>

#define WIDTH 512
#define HEIGHT 512

unsigned char image[WIDTH][HEIGHT][3];
bool flip = false;

void makeImage() {
	int i;
	int	j;
	for	(j = 0; j<HEIGHT; j++) {
		for	(i = 0; i<WIDTH; i++) {
			image[j][i][0] = j % 256;
			image[j][i][1] = 0;
			image[j][i][2] = 0;
		}
	}
}

void saveImage(char	*filename) {
	int	i;
	int	j;
	FIBITMAP *bitmap;
	BYTE *bits;
	bitmap = FreeImage_Allocate(WIDTH, HEIGHT, 24);
	for (j = 0; j<HEIGHT; j++) {
		bits = FreeImage_GetScanLine(bitmap, j);
		for (i = 0; i<WIDTH; i++) {
			bits[FI_RGBA_RED] = image[j][i][0];
			bits[FI_RGBA_GREEN] = image[j][i][1];
			bits[FI_RGBA_BLUE] = image[j][i][2];
			bits += 3;
		}
	}

	FreeImage_Save(FIF_PNG, bitmap, filename, PNG_DEFAULT);
}

void loadImage(char *filename) {
	int i, j;
	FIBITMAP *bitmap;
	BYTE *bits;
	bitmap = FreeImage_Load(FIF_PNG, filename, PNG_DEFAULT);
	for (j = 0; j<HEIGHT; j++) {
		bits = FreeImage_GetScanLine(bitmap, j);
		for (i = 0; i<WIDTH; i++) {
			image[j][i][0] = bits[FI_RGBA_RED];
			image[j][i][1] = bits[FI_RGBA_GREEN];
			image[j][i][2] = bits[FI_RGBA_BLUE];
			bits += 3;
		}
	}
	FreeImage_Unload(bitmap);
}

void displayFunc() {
	glLoadIdentity();
	glViewport(0, 0, 511, 511);
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (flip) {
		glRasterPos2i(0, 511);
		glPixelZoom(1.0f, -1.0f);
	} else {
		glRasterPos2i(0, 0);
		glPixelZoom(1.0, 1.0);
	}

	glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, image);
	glFlush();
}

void keyboardFunc(unsigned char k, int, int) {
	switch (k) {
		case 'f':
			flip = !flip;
			break;
		case 's':
			saveImage("image.jpg");
			break;
		case 'l':
			loadImage("image_2.jpg");
			break;
	}
	glutPostRedisplay();
}

void main(int argc,	char **argv) {

	makeImage();

	glutInit(&argc, argv);
	glutInitWindowSize((GLsizei)WIDTH, (GLsizei)HEIGHT);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Image Display");

	glutDisplayFunc(displayFunc);
	glutKeyboardFunc(keyboardFunc);

	glutMainLoop();

}

