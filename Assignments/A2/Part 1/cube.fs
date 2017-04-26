/*
 *  Simple fragment sharder for Lab 2
 */

#version 330 core

in vec3 normal;
in vec3 position;
in vec2 tc;
uniform samplerCube tex;
uniform vec4 colour;
uniform vec3 Eye;
uniform vec3 light;
uniform vec4 material;

void main() {
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
	float diffuse;
	vec3 L = normalize(light - position);
	vec3 N;
	vec3 R = normalize(reflect(-L,normal));
	float specular;
	vec4 colour;
	N = normalize(normal);
	diffuse = dot(N,L);


	//vec3 Rt = normalize(R + (1 - R) * pow((1 - dot(N, Eye)), 5));



	vec3 tc;
	tc = reflect(-position, normal);// + Rt;
	colour = texture(tex, tc) * 0.7;
	gl_FragColor = min(material.x*colour + material.y*colour + material.z*colour, vec4(1.0));
	gl_FragColor.a = colour.a;
}


