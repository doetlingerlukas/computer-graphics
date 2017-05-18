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

typedef struct buffer_object{
	GLuint VBO;
	GLuint CBO;
	GLuint IBO;
	GLuint VN;
} buffer_object;

typedef struct buffer_data{
	GLfloat* vertex_buffer_data;
	GLfloat* vertex_normals;
	GLfloat* color_buffer_data;
	GLushort* index_buffer_data;
} buffer_data;

void setupAndDraw(GLuint vbo, GLuint cbo, GLuint ibo, GLuint sp, float* mm);
void etupAndDraw(buffer_object* bo, GLuint sp, float* mm);
void setupVertexBuffer(GLuint vbo, GLfloat* vbo_data);
void setupIndexBuffer(GLuint ibo, GLushort* ibo_data);
void setupColorBuffer(GLuint cbo, GLfloat* cbo_data);

#endif // __SETUP_H__
