/******************************************************************
*
* Setup.c
* 
*
* Description: Helper for setup. Helper functions to compute the
* vertex normals of an object.
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

void setupAndDraw(buffer_object* bo, GLuint sp, float* mm){
	
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
	
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, bo->VT);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	/* Associate first Model with shader matrices */
    glUniformMatrix4fv(glGetUniformLocation(sp, "ModelMatrix"), 1, GL_TRUE, mm);

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    
    /* Disable attributes */
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
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
vertex substractVertex(vertex v1, vertex v2){
	vertex r = {
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z
	};
	return r;
}

vertex addVertex(vertex v1, vertex v2){
	vertex r = {
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z
	};
	return r;
}

vertex crossProduct(vertex u, vertex v){
	vertex r = {
		(u.y * v.z) - (u.z * v.y),
		(u.z * v.x) - (u.x * v.z),
		(u.x * v.y) - (u.y * v.x)
	};
	return r;
}

vertex normalize(vertex vert){
	float d = sqrtf(vert.x * vert.x + vert.y * vert.y + vert.z * vert.z);
	
	vertex r = {
		vert.x / d,
		vert.y / d,
		vert.z / d
	};
	return r;
}

GLfloat* calcFaceNormals(obj_scene_data d, buffer_data* bd){
	int vert = d.vertex_count;
    int indx = d.face_count;
    
    GLfloat* face_normals = (GLfloat*) calloc (indx*3, sizeof(GLfloat));
    
    int i;
    for(i=0; i<indx; i++){
		GLushort v1 = bd->index_buffer_data[i*3];
		GLushort v2 = bd->index_buffer_data[i*3+1];
		GLushort v3 = bd->index_buffer_data[i*3+2];
		
		vertex vert1 = {
			bd->vertex_buffer_data[v1*3],
			bd->vertex_buffer_data[v1*3+1],
			bd->vertex_buffer_data[v1*3+2]
		};
		vertex vert2 = {
			bd->vertex_buffer_data[v2*3],
			bd->vertex_buffer_data[v2*3+1],
			bd->vertex_buffer_data[v2*3+2]
		};
		vertex vert3 = {
			bd->vertex_buffer_data[v3*3],
			bd->vertex_buffer_data[v3*3+1],
			bd->vertex_buffer_data[v3*3+2]
		};
		
		vertex u = substractVertex(vert2, vert1);
		vertex v = substractVertex(vert3, vert1);
		
		vertex nf = crossProduct(u, v);
		
		face_normals[i*3] = nf.x;
		face_normals[i*3+1] = nf.y;
		face_normals[i*3+2] = nf.z;
    }
    
    return face_normals;
}

GLfloat* calcVertexNormals(obj_scene_data d, buffer_data* bd){
	int vert = d.vertex_count;
    int indx = d.face_count;
	
	GLfloat* face_normals = calcFaceNormals(d, bd);
	GLfloat* vertex_normals = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
	
	int i, j;
	for(i=0; i<vert; i++){
		
		vertex normal = {0.0, 0.0, 0.0};
		
		for(j=0; j<indx; j++){
			if(i == bd->index_buffer_data[j*3] ||
				i == bd->index_buffer_data[j*3+1] ||
				i == bd->index_buffer_data[j*3+2]){
				
				vertex fn = {
					face_normals[j*3],
					face_normals[j*3+1],
					face_normals[j*3+2]
				};
				normal = addVertex(normal, fn);
			}
		}
		
		normal = normalize(normal);
		
		vertex_normals[i*3] = normal.x;
		vertex_normals[i*3+1] = normal.y;
		vertex_normals[i*3+2] = normal.z;
	}
	
	return vertex_normals;
}







