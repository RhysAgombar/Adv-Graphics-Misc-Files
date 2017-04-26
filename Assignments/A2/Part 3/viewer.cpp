/************************************************************
*                   CSCI 4110 Lab 2
*
*  Basic OpenGL program that shows how to set up a
*  VAO and some basic shaders.  This program draws
*  a cube or sphere depending upon whether CUBE or
*  SPHERE is defined.
*
*/
#include <Windows.h>
#include <GL/glew.h>
#include <gl/glut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include "Shaders.h"
#include "texture.h"

#include <math.h>
#include "tiny_obj_loader.h"
#include <iostream>

float eyex, eyey, eyez;

double theta, phi;
double r;

GLuint program, programC;

glm::mat4 projection;

GLuint objVAO;
int triangles, trianglesS;

GLuint tbuffer, tbufferS;
GLuint cubeVAO, cubeVBO;
int trianglesC;


GLuint texName1;
GLuint texName2;

GLuint tex1;
GLuint tex2;

float angle, L;

void sphere() {
	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;
	GLint vTex;
	GLfloat *vertices;
	GLfloat *normals;
	GLuint *indices;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	int nv;
	int nn;
	int ni;
	int i;

	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	/*  Load the obj file */

	std::string err = tinyobj::LoadObj(shapes, materials, "sphere.obj", 0);

	if (!err.empty()) {
		std::cerr << err << std::endl;
		return;
	}

	/*  Retrieve the vertex coordinate data */

	nv = shapes[0].mesh.positions.size();
	vertices = new GLfloat[nv];
	for (i = 0; i<nv; i++) {
		vertices[i] = shapes[0].mesh.positions[i];
	}

	/*  Retrieve the vertex normals */

	nn = shapes[0].mesh.normals.size();
	normals = new GLfloat[nn];
	for (i = 0; i<nn; i++) {
		normals[i] = shapes[0].mesh.normals[i];
	}

	/*  Retrieve the triangle indices */

	ni = shapes[0].mesh.indices.size();
	triangles = ni / 3;
	indices = new GLuint[ni];
	for (i = 0; i<ni; i++) {
		indices[i] = shapes[0].mesh.indices[i];
	}

	int verts = nv / 3;
	int nt = 2 * verts;
	int x, y, z;

	GLfloat* tex = new GLfloat[nt];
	for (int i = 0; i < verts; i++) {
		x = vertices[3 * i];
		y = vertices[3 * i + 1];
		z = vertices[3 * i + 2];

		theta = atan2(x, z);
		phi = atan2(y, sqrt(x*x + z*z));

		tex[2 * i] = fabs(theta) / M_PI;
		tex[2 * i + 1] = phi / M_PI;
	}

	/*
	*  load the vertex coordinate data
	*/
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv + nn + nt) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv * sizeof(GLfloat), nn * sizeof(GLfloat), normals);
	glBufferSubData(GL_ARRAY_BUFFER, (nv + nn) * sizeof(GLfloat), nt * sizeof(GLfloat), tex);

	/*
	*  load the vertex indexes
	*/
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni * sizeof(GLuint), indices, GL_STATIC_DRAW);

	/*
	*  link the vertex coordinates to the vPosition
	*  variable in the vertex program.  Do the same
	*  for the normal vectors.
	*/
	glUseProgram(program);
	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nv * sizeof(GLfloat)));
	glEnableVertexAttribArray(vNormal);

}

void init() {

	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	GLfloat vertices[8][4] = {
		{ -10.0, -10.0, -10.0, 1.0 },		//0
		{ -10.0, -10.0, 10.0, 1.0 },		//1
		{ -10.0, 10.0, -10.0, 1.0 },		//2
		{ -10.0, 10.0, 10.0, 1.0 },		//3
		{ 10.0, -10.0, -10.0, 1.0 },		//4
		{ 10.0, -10.0, 10.0, 1.0 },		//5
		{ 10.0, 10.0, -10.0, 1.0 },		//6
		{ 10.0, 10.0, 10.0, 1.0 }			//7
	};

	GLfloat normals[8][3] = {
		{ -1.0, -1.0, -1.0 },			//0
		{ -1.0, -1.0, 1.0 },			//1
		{ -1.0, 1.0, -1.0 },			//2
		{ -1.0, 1.0, 1.0 },				//3
		{ 1.0, -1.0, -1.0 },			//4
		{ 1.0, -1.0, 1.0 },				//5
		{ 1.0, 1.0, -1.0 },				//6
		{ 1.0, 1.0, 1.0 }				//7
	};

	GLuint indexes[36] = { 0, 1, 3, 0, 2, 3,
		0, 4, 5, 0, 1, 5,
		2, 6, 7, 2, 3, 7,
		0, 4, 6, 0, 2, 6,
		1, 5, 7, 1, 3, 7,
		4, 5, 7, 4, 6, 7 };

	triangles = 12;

	glUseProgram(programC);

	Cube* texture1;
	texture1 = loadCube(".");
	glGenTextures(1, &tbuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tbuffer);

	for (int i = 0; i<6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, texture1->width, texture1->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture1->data[i]);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	Cube* texture2;
	texture2 = loadCube("/2");
	glGenTextures(1, &tbufferS);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tbufferS);

	for (int i = 0; i<6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, texture2->width, texture2->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture2->data[i]);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	tex1 = glGetUniformLocation(programC, "tex1");
	tex2 = glGetUniformLocation(program, "tex2");
	glUniform1i(tex1, 0);
	glUniform1i(tex2, 1);





	int verts = 8;
	int nt = 2 * verts;
	int x, y, z;

	GLfloat* tex = new GLfloat[nt];
	for (int i = 0; i < verts; i++) {
		x = vertices[3 * i][0];
		y = vertices[3 * i][1];
		z = vertices[3 * i][2];

		theta = atan2(x, z);
		phi = atan2(y, sqrt(x*x + z*z));

		tex[2 * i] = fabs(theta) / M_PI;
		tex[2 * i + 1] = phi / M_PI;
	}


	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);

	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

	glUseProgram(programC);
	vPosition = glGetAttribLocation(programC, "vPosition");
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(programC, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*) sizeof(vertices));
	glEnableVertexAttribArray(vNormal);


	//////////////////////////////////////////////////////////////////

	sphere();



	
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).

	if (h == 0)
		h = 1;

	float ratio = 1.0 * w / h;

	glViewport(0, 0, w, h);

	projection = glm::perspective(45.0f, ratio, 1.0f, 100.0f);

}

void displayFunc(void) {

	glm::mat4 view;
	int viewLoc;
	int projLoc;
	int colourLoc, colourLocS;
	int eyeLoc;
	int lightLoc, lightLocS;
	int materialLoc, materialLocS;
	int thetaLoc, lLoc;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programC);

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));

	viewLoc = glGetUniformLocation(programC, "modelView");
	glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(view));
	projLoc = glGetUniformLocation(programC, "projection");
	glUniformMatrix4fv(projLoc, 1, 0, glm::value_ptr(projection));

	colourLoc = glGetUniformLocation(programC, "colour");
	glUniform4f(colourLoc, 1.0, 0.0, 0.0, 1.0);
	eyeLoc = glGetUniformLocation(programC, "Eye");
	glUniform3f(eyeLoc, eyex, eyey, eyez);
	lightLoc = glGetUniformLocation(programC, "light");
	glUniform3f(lightLoc, 1.0, 1.0, 1.0);
	materialLoc = glGetUniformLocation(programC, "material");
	glUniform4f(materialLoc, 0.3, 0.7, 0.7, 150.0);

	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 3 * triangles, GL_UNSIGNED_INT, NULL);


	glUseProgram(program);

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));

	viewLoc = glGetUniformLocation(program, "modelView");
	glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(view));
	projLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projLoc, 1, 0, glm::value_ptr(projection));

	colourLoc = glGetUniformLocation(program, "colour");
	glUniform4f(colourLoc, 1.0, 0.0, 0.0, 1.0);
	eyeLoc = glGetUniformLocation(program, "Eye");
	glUniform3f(eyeLoc, eyex, eyey, eyez);
	lightLoc = glGetUniformLocation(program, "light");
	glUniform3f(lightLoc, 1.0, 1.0, 1.0);
	materialLoc = glGetUniformLocation(program, "material");
	glUniform4f(materialLoc, 0.3, 0.7, 0.7, 150.0);


	//L = 0.5;
	//lLoc = glGetUniformLocation(program, "L");
	//glUniform1f(lLoc, L);

	//angle = 10;
	//thetaLoc = glGetUniformLocation(program, "theta");
	//glUniform1f(thetaLoc, angle);

	glBindVertexArray(objVAO);
	glDrawElements(GL_TRIANGLES, 3 * triangles, GL_UNSIGNED_INT, NULL);


	glutSwapBuffers();
}

void keyboardFunc(unsigned char key, int x, int y) {

	switch (key) {
	case 'a':
		phi -= 0.1;
		break;
	case 'd':
		phi += 0.1;
		break;
	case 'w':
		theta += 0.1;
		break;
	case 's':
		theta -= 0.1;
		break;
	}

	eyex = r*sin(theta)*cos(phi);
	eyey = r*sin(theta)*sin(phi);
	eyez = r*cos(theta);

	glutPostRedisplay();

}

int main(int argc, char **argv) {
	int fs, fsC;
	int vs, vsC;
	int user;

	char vertexName[256];
	char fragmentName[256];
	char *fragment;
	char *vertex;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("Viewer");
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		printf("Error starting GLEW: %s\n", glewGetErrorString(error));
		exit(0);
	}

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboardFunc);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	vs = buildShader(GL_VERTEX_SHADER, "lab2c.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "lab2c.fs");
	program = buildProgram(vs, fs, 0);

	vsC = buildShader(GL_VERTEX_SHADER, "cube.vs");
	fsC = buildShader(GL_FRAGMENT_SHADER, "cube.fs");
	programC = buildProgram(vsC, fsC, 0);

	init();

	eyex = 0.0;
	eyez = 0.0;
	eyey = 3.0;

	theta = 1.5;
	phi = 1.5;
	r = 3.0;

	glutMainLoop();

}
