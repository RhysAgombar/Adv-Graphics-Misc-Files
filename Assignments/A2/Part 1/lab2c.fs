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
uniform samplerCube tex;
uniform vec4 colour;
uniform vec3 Eye;
uniform vec3 light;
uniform vec4 material;

void main() {
	vec3 N;
	float specular;
	vec4 colour, colourRe, colourRa;
	N = normalize(normal);

	float n = 1.52;

	float R0 = pow((n - 1)/(n + 1), 2);

	float Rt = (R0 + (1 - R0) * pow((1 - dot(N, Eye)), 5));   

	vec3 tc, te, ta;
	float ref = 1 / n;

	te = reflect(normalize(position - Eye), N);             
	ta = refract(normalize(position - Eye), N, n);                                                                                      


	colourRe = texture(tex, te);
	colourRa = texture(tex, ta);

	colour = normalize(colourRa * Rt + colourRe); 

	gl_FragColor = min(material.x*colour + material.y*colour + material.z*colour, vec4(1.0));
	gl_FragColor.a = colour.a;
}