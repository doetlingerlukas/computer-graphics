/******************************************************************
*
* Setup.c
* 
*
* Description: Helper for setup.
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

/* Local includes */
#include "Setup.h"

/******************************************************************
*
* Setup for display function
*
*******************************************************************/

void setupAndDraw(GLuint vbo, GLuint cbo, GLuint ibo, GLuint sp, float* mm)
{
	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    if(cbo != 0){
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, cbo);
		glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, 0, 0); 
	}
	
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    GLint size; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    
    if(0 != 0){
		glEnableVertexAttribArray(2);
		//glBindBuffer(GL_ARRAY_BUFFER, vn);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			0                          // array buffer offset
		);
	}
    
    /* Associate first Model with shader matrices */
    glUniformMatrix4fv(glGetUniformLocation(sp, "ModelMatrix"), 1, GL_TRUE, mm);

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}

void etupAndDraw(buffer_object* bo, GLuint sp, float* mm){
	
	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, bo->VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, bo->CBO);
	glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, 0, 0); 
	
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo->IBO);
    GLint size; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    
    glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, bo->VN);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	
	/* Associate first Model with shader matrices */
    glUniformMatrix4fv(glGetUniformLocation(sp, "ModelMatrix"), 1, GL_TRUE, mm);

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    
    /* Disable attributes */
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

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

/******************************************************************
*
* Vertex normals
*
*******************************************************************/
/*
void calcTriangleNormals(obj_scene_data d, buffer_data* bd){
	int vert = d.vertex_count;
    int indx = d.face_count;
    
    
}*/







