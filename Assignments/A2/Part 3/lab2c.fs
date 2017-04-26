/*
 *  Simple fragment sharder for Lab 2
 */

#version 330 core

//in vec3 normal;
//in vec3 position;
//in vec2 tc;
//uniform sampler2D tex;
//uniform vec4 colour;
//uniform vec3 Eye;
//uniform vec3 light;
//uniform vec4 material;

in vec3 normal;
in vec3 position;
in vec2 tc;
uniform samplerCube tex2;
uniform vec4 colour;
uniform vec3 Eye;
uniform vec3 light;
uniform vec4 material;

//uniform float L;
float L = 0.5;
//uniform float theta;
float theta = 10;

int num = 1235;
int a = 141;
int c = 28411;
int m = 134456;

void main() {

	vec3 N;
	float specular;
	vec4 colour, colourRe, colourRa;
	N = normalize(normal);

	num = int((normal.x*normal.y + normal.x*normal.z) * 123456);  
	num = num % m;

	colour = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 tc, te;
	vec3 pos2;

	vec3 U = normalize(cross(vec3(N.y, N.x, N.z), N));
	vec3 W = normalize(cross(U, N));

	vec3 newVec;
	
	for (int i = 0; i < 360/theta; i++){
		newVec = L * cos(theta * i) * U + L * sin(theta * i) * W;

		pos2 = position;
		te = reflect(normalize(newVec - Eye), N);             
		colourRe = texture(tex2, te) * 0.7;
		colour += colourRe;
	}
	
	pos2 = position;
	te = reflect(normalize(pos2 - Eye), N);             
	colourRe = texture(tex2, te) * 0.7;
	colour += colourRe;

	colour = colour / ((360/theta) + 1);

	gl_FragColor = min(material.x*colour + material.y*colour + material.z*colour, vec4(1.0));
	gl_FragColor.a = colour.a;
}