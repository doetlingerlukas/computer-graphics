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

void setupAndDraw(GLuint vbo, GLuint cbo, GLuint ibo, GLuint sp, float* mm);
void setupVertexBuffer(GLuint vbo, GLfloat* vbo_data);
void setupIndexBuffer(GLuint ibo, GLushort* ibo_data);
void setupColorBuffer(GLuint cbo, GLfloat* cbo_data);

#endif // __SETUP_H__
