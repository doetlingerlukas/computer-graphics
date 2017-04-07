/******************************************************************
*
* Setup.c
* 
* 
*
* Description: Helper for Buffer and Shader setup.
* 	
* Computer Graphics Proseminar SS 2017
* 
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* OpenGL includes */
#include <GL/glew.h>
#include <GL/freeglut.h>

/******************************************************************
*
* SetupBuffers
*
*******************************************************************/

void setupVertexBuffer(GLuint vbo, GLfloat* vbo_data)
{
	glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);
}

void setupIndexBuffer(GLuint ibo, GLushort* ibo_data)
{
	glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ibo_data), ibo_data, GL_STATIC_DRAW);
}

void setupColorBuffer(GLuint cbo, GLfloat* cbo_data)
{
	glGenBuffers(1, &cbo);
    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cbo_data), cbo_data, GL_STATIC_DRAW);
}





