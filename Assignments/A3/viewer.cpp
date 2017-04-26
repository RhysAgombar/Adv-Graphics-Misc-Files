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
#include <math.h>
#include <stdio.h>
#include "Shaders.h"
#include "tiny_obj_loader.h"
#include <iostream>
#include <stack>
#include <time.h> 
#include <math.h>

using namespace std;

GLuint program;			// shader programs
int window;

glm::mat4 projection;

glm::vec3 cameraPos = glm::vec3(0.0f, 30.0f, 1.0f); // 0, 0, 3
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -30.0f, -1.0f); 
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

GLfloat lastX = 160; // center of window
GLfloat lastY = 160;
GLfloat pitch = 0; // center of window
GLfloat yaw = 0;

float eyex, eyey, eyez;	// eye position

struct Master {
	GLuint vao;
	int indices;
	GLuint vbuffer;
};

struct Monkey {
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
};

struct Attractor {
	glm::vec3 position = glm::vec3(0, 0, 0);
//	glm::vec3 acceleration = glm::vec3(0, 0, 0);
};

Master *monkey;
Master *ground;

Attractor redAttractor, blueAttractor;

const int redTribeSize = 5;
const int blueTribeSize = 5;
Monkey redTribe[redTribeSize];
Monkey blueTribe[blueTribeSize];

const glm::vec3 redStartPosition = glm::vec3(13, 0.0, 13);
const glm::vec3 redEndPosition = glm::vec3(-15.0, 0.0, -15.0);

const glm::vec3 blueStartPosition = glm::vec3(13.0, 0.0, -13.0);
const glm::vec3 blueEndPosition = glm::vec3(-15.0, 0.0, 15.0);

//int timeStep = 0;
vector<glm::vec3> obstacles;

Master *make_shape_from_obj(std::vector<tinyobj::shape_t> shapes) {
	Master *result;
	GLuint vao;

	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;
	GLfloat *vertices;
	GLfloat *normals;
	GLuint *indices;
	int nv;
	int nn;
	int ni;
	int i;


	result = new Master;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	result->vao = vao;
	

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
	result->indices = ni;

	//triangles = ni / 3;
	indices = new GLuint[ni];
	for (i = 0; i<ni; i++) {
		indices[i] = shapes[0].mesh.indices[i];
	}

	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv + nn) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv * sizeof(GLfloat), nn * sizeof(GLfloat), normals);
	result->vbuffer = vbuffer;

	glGenBuffers(1, &ibuffer);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni * sizeof(GLuint), indices, GL_STATIC_DRAW);

	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nv * sizeof(GLfloat)));
	glEnableVertexAttribArray(vNormal);

	return(result);
}

Master *make_shape_from_obj_ground(std::vector<tinyobj::shape_t> shapes) {
	Master *result;
	GLuint vao;

	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;
	GLfloat *vertices;
	GLfloat *normals;
	GLuint *indices;
	int nv;
	int nn;
	int ni;
	int i;

	result = new Master;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	result->vao = vao;


	/*  Retrieve the vertex coordinate data */

	nv = shapes[0].mesh.positions.size();
	vertices = new GLfloat[nv];
	for (i = 0; i<nv; i++) {
		vertices[i] = shapes[0].mesh.positions[i];

		if ((i % 3) == 1) {
			if (vertices[i] > 0.01) {
				float x = shapes[0].mesh.positions[i - 1];
				float z = shapes[0].mesh.positions[i + 1];

				obstacles.push_back(glm::vec3(x, 0.0, z));
			}
		}

	}

	/*  Retrieve the vertex normals */

	nn = shapes[0].mesh.normals.size();
	normals = new GLfloat[nn];
	for (i = 0; i<nn; i++) {
		normals[i] = shapes[0].mesh.normals[i];
	}

	/*  Retrieve the triangle indices */

	ni = shapes[0].mesh.indices.size();
	result->indices = ni;

	//triangles = ni / 3;
	indices = new GLuint[ni];
	for (i = 0; i<ni; i++) {
		indices[i] = shapes[0].mesh.indices[i];
	}

	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv + nn) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv * sizeof(GLfloat), nn * sizeof(GLfloat), normals);
	result->vbuffer = vbuffer;

	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni * sizeof(GLuint), indices, GL_STATIC_DRAW);

	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nv * sizeof(GLfloat)));
	glEnableVertexAttribArray(vNormal);

	return(result);
}

/*
*  The init procedure creates the OpenGL data structures
*  that contain the triangle geometry, compiles our
*  shader program and links the shader programs to
*  the data.
*/

void init() {

	srand(time(NULL));


	int vs;
	int fs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::vector<tinyobj::shape_t> shapesG;
	std::vector<tinyobj::material_t> materialsG;


	redAttractor.position = redEndPosition;
	//redAttractor.acceleration = redEndPosition;
	// redAttractor.acceleration.operator*= (1 / 6000.0f);

	blueAttractor.position = blueEndPosition;
	//blueAttractor.acceleration = blueEndPosition;
	// blueAttractor.acceleration.operator*= (1 / 6000.0f);

	for (int i = 0; i < redTribeSize; i++) {
		redTribe[i].position = glm::vec3(redStartPosition.x + ((rand() % 100) / 15.0f) - 5.0, redStartPosition.y, redStartPosition.z + ((rand() % 100) / 15.0f) - 5.0);
	}

	for (int i = 0; i < blueTribeSize; i++) {
		blueTribe[i].position = glm::vec3(blueStartPosition.x + ((rand() % 100) / 15.0f) - 5.0, blueStartPosition.y, blueStartPosition.z + ((rand() % 100) / 15.0f) - 5.0);
	}


	/*
	*  compile and build the shader program
	*/
	vs = buildShader(GL_VERTEX_SHADER, "lab2.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "lab2.fs");
	program = buildProgram(vs, fs, 0);
	dumpProgram(program, "Assignment 3");

	glUseProgram(program);

	std::string err = tinyobj::LoadObj(shapes, materials, "monkey.obj", 0);

	if (!err.empty()) {
		std::cerr << err << std::endl;
		return;
	}

	err = tinyobj::LoadObj(shapesG, materialsG, "ground.obj", 0);

	if (!err.empty()) {
		std::cerr << err << std::endl;
		return;
	}

	monkey = make_shape_from_obj(shapes);

	ground = make_shape_from_obj_ground(shapesG);

}

/*
*  Executed each time the window is resized,
*  usually once at the start of the program.
*/
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).

	if (h == 0)
		h = 1;

	float ratio = 1.0 * w / h;

	glViewport(0, 0, w, h);

	projection = glm::perspective(45.0f, ratio, 1.0f, 100.0f);

}

float findLength(glm::vec3 inVec) {
	float out = pow(inVec.x, 2) + pow(inVec.y, 2) + pow(inVec.z, 2);

	out = sqrt(out);

	return out;
}

float getDistance(glm::vec3 point1, glm::vec3 point2) {
	float x = pow(point2.x - point1.x, 2);
	float y = pow(point2.y - point1.y, 2);
	float z = pow(point2.z - point1.z, 2);

	float out = sqrt(x + y + z);

	return out;
}

glm::vec3 getVecN(glm::vec3 point1, glm::vec3 point2) {
	glm::vec3 out = point2;
	out.operator-= (point1);

	return glm::normalize(out);
}

glm::vec3 getVec(glm::vec3 point1, glm::vec3 point2) {
	glm::vec3 out = point2;
	out.operator-= (point1);

	return out;
}

void processMovement() {

	//timeStep++;

	Attractor rtAttractor, btAttractor;

	rtAttractor.position = glm::vec3(0.0, 0.0, 0.0);

	for (int j = 0; j < redTribeSize; j++) {
		rtAttractor.position.operator+=(redTribe[j].position);
	}

	rtAttractor.position.operator/=(redTribeSize);

	for (int j = 0; j < blueTribeSize; j++) {
		btAttractor.position.operator+=(blueTribe[j].position);
	}

	btAttractor.position.operator/=(blueTribeSize);


	for (int i = 0; i < redTribeSize; i++) {
		redTribe[i].velocity += getVec(redTribe[i].position, redAttractor.position);
		glm::vec3 rc = getVec(redTribe[i].position, rtAttractor.position);
		float rcDist = getDistance(rtAttractor.position, redTribe[i].position);
		if (rcDist > 2) {
			rc.operator*=(5);
			redTribe[i].velocity += rc;
		}

		for (int j = 0; j < obstacles.size(); j++) {
			glm::vec3 obstaclePush = getVecN(obstacles.at(j), redTribe[i].position);

			float sc = glm::dot(redTribe[i].velocity, obstaclePush)/2;

			//obstaclePush.operator*=(abs(sc));

			redTribe[i].velocity += obstaclePush / (pow(getDistance(obstacles.at(j), redTribe[i].position), 2)/abs(sc));
		}

		for (int j = 0; j < blueTribeSize; j++) {
			glm::vec3 bluePush = getVecN(blueTribe[j].position, redTribe[i].position);

			bluePush.operator*=(10);

			float dist = getDistance(blueTribe[j].position, redTribe[i].position);

			if (dist < 10) {
				redTribe[i].velocity += bluePush / (dist / 3);
			}
						
		}

		for (int j = 0; j < redTribeSize; j++) {
			if (j != i) {
				glm::vec3 redPush = getVecN(redTribe[j].position, redTribe[i].position);

				redPush.operator*=(10);

				redTribe[i].velocity += redPush / pow(getDistance(redTribe[j].position, redTribe[i].position), 2);
			}
		}


		if (findLength(redTribe[i].velocity) > 0.03) {
			redTribe[i].velocity = glm::normalize(redTribe[i].velocity);
			redTribe[i].velocity.operator*= (0.03);
		}

		redTribe[i].position.operator+= (redTribe[i].velocity);

	}


	for (int i = 0; i < blueTribeSize; i++) {
		blueTribe[i].velocity += getVec(blueTribe[i].position, blueAttractor.position);
		glm::vec3 bc = getVec(blueTribe[i].position, btAttractor.position);
		float bcDist = getDistance(btAttractor.position, blueTribe[i].position);
		if (bcDist > 2) {
			bc.operator*=(5);
			blueTribe[i].velocity += bc;
		}

		for (int j = 0; j < obstacles.size(); j++) {
			glm::vec3 obstaclePush = getVecN(obstacles.at(j), blueTribe[i].position);

			float sc = glm::dot(blueTribe[i].velocity, obstaclePush) / 2;

			//obstaclePush.operator*=(3);

			blueTribe[i].velocity += obstaclePush / (pow(getDistance(obstacles.at(j), blueTribe[i].position), 2)/abs(sc));
		}

		for (int j = 0; j < redTribeSize; j++) {
			glm::vec3 redPush = getVecN(redTribe[j].position, blueTribe[i].position);

			redPush.operator*=(10);

			float dist = getDistance(redTribe[j].position, blueTribe[i].position);

			if (dist < 10) {
				blueTribe[i].velocity += redPush / (dist / 3);
			}

		}

		for (int j = 0; j < blueTribeSize; j++) {
			if (j != i) {
				glm::vec3 bluePush = getVecN(blueTribe[j].position, blueTribe[i].position);

				bluePush.operator*=(10);

				blueTribe[i].velocity += bluePush / pow(getDistance(blueTribe[j].position, blueTribe[i].position), 2);
			}
		}

		if (findLength(blueTribe[i].velocity) > 0.03) {
			blueTribe[i].velocity = glm::normalize(blueTribe[i].velocity);
			blueTribe[i].velocity.operator*= (0.03);
		}

		blueTribe[i].position.operator+= (blueTribe[i].velocity);

	}

	Sleep(5);

	float redDist = getDistance(rtAttractor.position, redAttractor.position);
	float blueDist = getDistance(btAttractor.position, blueAttractor.position);
	if (redDist < 1.5 && blueDist < 1.5) {
		exit(0);
	}

	glutPostRedisplay();
}

void displayFunc() {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 viewPerspective;
	int viewLoc;
	int modelLoc;
	stack<glm::mat4> matrixStack;
	GLint vPosition;
	int colourLoc;
	int black;

	int projLoc;
	int camLoc;
	int lightLoc;
	int materialLoc;

	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	viewPerspective = projection * view;
	model = glm::mat4(1.0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	viewLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(viewPerspective));
	modelLoc = glGetUniformLocation(program, "modelView");
	colourLoc = glGetUniformLocation(program, "colour");
	vPosition = glGetAttribLocation(program, "vPosition");

	camLoc = glGetUniformLocation(program, "Eye");
	glUniform3f(camLoc, cameraPos.x, 0.0f, cameraPos.z);
	lightLoc = glGetUniformLocation(program, "light");
	glUniform3f(lightLoc, 0.0, 1.0, 0.0);
	materialLoc = glGetUniformLocation(program, "material");
	glUniform4f(materialLoc, 0.3, 0.7, 0.7, 150.0);


	/* ground colour - green */
	glUniform4f(colourLoc, 0.10, 0.592, 0.100, 1.0);

	/* draw ground */
	matrixStack.push(model);
	model = glm::translate(model, glm::vec3(0.0, -0.9, 0.0)); // (left/right, up, forward/back)
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glBindVertexArray(ground->vao);
	glBindBuffer(GL_ARRAY_BUFFER, ground->vbuffer);
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, ground->indices, GL_UNSIGNED_INT, NULL);
	model = matrixStack.top();
	matrixStack.pop();

	/* monkey colour - red */
	glUniform4f(colourLoc, 1.0, 0.0, 0.0, 1.0);

	/* draw monkey */

	for (int i = 0; i < redTribeSize; i++) {

		//glm::vec3 disp = redTribe[i].velocity;

		//disp.operator*= ( timeStep );

		matrixStack.push(model);
		model = glm::translate(model, redTribe[i].position); // (left/right, up, forward/back)
		glBindVertexArray(monkey->vao);
		glBindBuffer(GL_ARRAY_BUFFER, monkey->vbuffer);
		glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, monkey->indices, GL_UNSIGNED_INT, NULL);
		model = matrixStack.top();
		matrixStack.pop();
	}

	/* monkey colour - blue */
	glUniform4f(colourLoc, 0.0, 0.0, 1.0, 1.0);
	/* draw monkey */
	for (int i = 0; i < blueTribeSize; i++) {

		//glm::vec3 disp = blueTribe[i].velocity;

		//disp.operator*= (timeStep);

		matrixStack.push(model);
		model = glm::translate(model, blueTribe[i].position); // (left/right, up, forward/back)
		glBindVertexArray(monkey->vao);
		glBindBuffer(GL_ARRAY_BUFFER, monkey->vbuffer);
		glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, monkey->indices, GL_UNSIGNED_INT, NULL);
		model = matrixStack.top();
		matrixStack.pop();
	}


	glutSwapBuffers();

}

void keyboardFunc(unsigned char key, int x, int y) {

	GLfloat cameraSpeed = 0.15f;

	switch (key) {
	case 'a':
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		break;
	case 'd':
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		break;
	case 'w':
		cameraPos += glm::normalize(cameraUp) * cameraSpeed;
		break;
	case 's':
		cameraPos -= glm::normalize(cameraUp) * cameraSpeed;
		break;
	case 'e':
		cameraPos += cameraSpeed * cameraFront;
		break;
	case 'q':
		cameraPos -= cameraSpeed * cameraFront;
		break;
	}

	glutPostRedisplay();

}

int main(int argc, char **argv) {

	/*
	*  initialize glut, set some parameters for
	*  the application and create the window
	*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	window = glutCreateWindow("Assignment Three");

	/*
	*  initialize glew
	*/
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		printf("Error starting GLEW: %s\n", glewGetErrorString(error));
		exit(0);
	}

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboardFunc);

	glutIdleFunc(processMovement);

	eyex = 0.0;
	eyey = 6.0;
	eyez = 1.0;

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	init();

	//glClearColor(1.0, 1.0, 1.0, 1.0);

	glutMainLoop();

}
