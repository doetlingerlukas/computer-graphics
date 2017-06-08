/******************************************************************
*
* Setup.h
* 
* Computer Graphics Proseminar SS 2017
* 
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/


#ifndef __SETUP_H__
#define __SETUP_H__

#include "OBJParser.h"

typedef struct buffer_object{
	GLuint VBO;
	GLuint CBO;
	GLuint IBO;
	GLuint VN;
	GLuint VT;
} buffer_object;

typedef struct buffer_data{
	GLfloat* vertex_buffer_data;
	GLfloat* vertex_textures;
	GLfloat* vertex_normals;
	GLfloat* color_buffer_data;
	GLushort* index_buffer_data;
} buffer_data;

typedef struct rgb{
	float r;
	float g;
	float b;
} rgb;

typedef struct vertex{
	GLfloat x, y, z;
} vertex;

void setupAndDraw(buffer_object* bo, GLuint sp, float* mm);
void etupAndDraw(buffer_object* bo, GLuint sp, float* mm);

GLfloat* calcRightVertices(obj_scene_data d, buffer_data* bd);
GLushort* calcRightFaces(obj_scene_data d, buffer_data* bd);

vertex substractVertex(vertex v1, vertex v2);
vertex addVertex(vertex v1, vertex v2);
vertex crossProduct(vertex u, vertex v);
vertex normalize(vertex vert);
GLfloat* calcFaceNormals(obj_scene_data d, buffer_data* bd);
GLfloat* calcVertexNormals(obj_scene_data d, buffer_data* bd);

#endif // __SETUP_H__
