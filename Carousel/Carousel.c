/******************************************************************
*
* Carousel.c
*
* by Davide De Sclavis und Lukas Dötlinger
*
* The Carousel.c displays a Carousel with some basic objects
* that are moving up ad down.
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
#include "LoadShader.h"   /* Provides loading function for shader code */
#include "Matrix.h"

/*----------------------------------------------------------------*/

/* Define handle to a vertex buffer object */
GLuint VBO, VBO2, VBO3, VBO4, VBO5;

/* Define handle to a color buffer object */
GLuint CBO, CBO2, CBO3, CBO4, CBO5;

/* Define handle to an index buffer object */
GLuint IBO, IBO2, IBO3, IBO4, IBO5;

GLuint VAO;

/* Indices to vertex attributes; in this case positon and color */ 
enum DataID {vPosition = 0, vColor = 1}; 

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;
GLuint ShaderProgram2; /* Shader for second Model */
GLuint ShaderProgram3; /* Shader for third Model */
GLuint ShaderProgram4; /* Shader for fourth Model */
GLuint ShaderProgram5; /* Shader for fifth Model */

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */ 
float ModelMatrix[16]; /* Model matrix */ 

float Model2Matrix[16]; /* Model2 matrix */

float Model3Matrix[16]; /* Model3 matrix */

float Model4Matrix[16]; /* Model4 matrix */

float Model5Matrix[16]; /* Model5 matrix */

/* Transformation matrices for initial position */
float TranslateOrigin[16];
float TranslateDown[16];
float RotateX[16];
float RotateZ[16];
float InitialTransform[16];

/* Buffers for first Model */

GLfloat vertex_buffer_data[] = { 
    
	 /*Baseplate*/
    -2.0, -2.0, -1.5,
     2.0, -2.0, -1.5,
     2.0,  2.0, -1.5,
    -2.0,  2.0, -1.5,

    -2.0, -2.0, -2.0,
     2.0, -2.0, -2.0,
     2.0,  2.0, -2.0,
    -2.0,  2.0, -2.0,

     0.0, -3.0, -1.5,
     3.0,  0.0, -1.5,
     0.0,  3.0, -1.5,
    -3.0,  0.0, -1.5,
    
     0.0, -3.0, -2.0,
     3.0,  0.0, -2.0,
     0.0,  3.0, -2.0,
    -3.0,  0.0, -2.0,
  

	 /*Topplate*/
    -2.0, -2.0, 2.0,
     2.0, -2.0, 2.0,
     2.0,  2.0, 2.0,
    -2.0,  2.0, 2.0,

    -2.0, -2.0, 2.2,
     2.0, -2.0, 2.2,
     2.0,  2.0, 2.2,
    -2.0,  2.0, 2.2,

     0.0, -3.0, 2.0,
     3.0,  0.0, 2.0,
     0.0,  3.0, 2.0,
    -3.0,  0.0, 2.0,
    
     0.0, -3.0, 2.2,
     3.0,  0.0, 2.2,
     0.0,  3.0, 2.2,
    -3.0,  0.0, 2.2,

     
     0.0,  0.0, 4.0, //index 32


	 /*Center pole*/
     0.2,  0.2,-1.5,//index 33
    -0.2,  0.2,-1.5,
    -0.2, -0.2,-1.5,
     0.2, -0.2,-1.5,
    
     0.2,  0.2, 2.0,//index 37
    -0.2,  0.2, 2.0,
    -0.2, -0.2, 2.0,
     0.2, -0.2, 2.0,
 

	 /*1st pole*/
    -1.9, -1.9, -1.5,//index 41
    -1.9, -1.8, -1.5,
    -1.8, -1.8, -1.5,
    -1.8, -1.9, -1.5,

    -1.9, -1.9, 2.0,//index 45
    -1.9, -1.8, 2.0,
    -1.8, -1.8, 2.0,
    -1.8, -1.9, 2.0,
    

	/*2nd pole*/
    1.9, 1.9, -1.5,//index 49
    1.9, 1.8, -1.5,
    1.8, 1.8, -1.5,
    1.8, 1.9, -1.5,
    
    1.9, 1.9,  2.0,//index 53
    1.9, 1.8,  2.0,
    1.8, 1.8,  2.0,
    1.8, 1.9,  2.0,
	/*3rd pole*/
    1.9, -1.9, -1.5,//index 57
    1.9, -1.8, -1.5,
    1.8, -1.8, -1.5,
    1.8, -1.9, -1.5,
    
    1.9, -1.9, 2.0,//index 61
    1.9, -1.8, 2.0,
    1.8, -1.8, 2.0,
    1.8, -1.9, 2.0,


	/*4th pole*/
    -1.9, 1.9, -1.5,//index 65
    -1.9, 1.8, -1.5,
    -1.8, 1.8, -1.5,
    -1.8, 1.9, -1.5,
    
    -1.9, 1.9, 2.0,//index 69
    -1.9, 1.8, 2.0,
    -1.8, 1.8, 2.0,
    -1.8, 1.9, 2.0,

    
	/*Groundplate*/
    -2.2, -2.2, -1.85,//index 73
     2.2, -2.2, -1.85,
     2.2,  2.2, -1.85,
    -2.2,  2.2, -1.85,

    -2.2, -2.2, -2.0,
     2.2, -2.2, -2.0,
     2.2,  2.2, -2.0,
    -2.2,  2.2, -2.0,

     0.0, -3.2, -1.85,
     3.2,  0.0, -1.85,
     0.0,  3.2, -1.85,
    -3.2,  0.0, -1.85,
    
     0.0, -3.2, -2.0,
     3.2,  0.0, -2.0,
     0.0,  3.2, -2.0,
    -3.2,  0.0, -2.0,
     
     

	 /*5th plole*/
     0.0, -2.9, 2.2, //index 89
     0.0, -2.8, 2.2,
     0.1, -2.8, 2.2,
     0.1, -2.9, 2.2,

     0.0, -2.9, -1.5,
     0.0, -2.8, -1.5,
     0.1, -2.8, -1.5,
     0.1, -2.9, -1.5,

	 /*6th pole*/
     0.0, 2.9, 2.2, //index 97
     0.0, 2.8, 2.2,
    -0.1, 2.8, 2.2,
    -0.1, 2.9, 2.2,

     0.0, 2.9, -1.5,
     0.0, 2.8, -1.5,
    -0.1, 2.8, -1.5,
    -0.1, 2.9, -1.5,


	 /*7th pole*/
     2.9, 0.0, 2.2,//index 105
     2.9, 0.1, 2.2,
     2.8, 0.1, 2.2,
     2.8, 0.0, 2.2,
     
     2.9, 0.0, -1.5,
     2.9, 0.1, -1.5,
     2.8, 0.1, -1.5,
     2.8, 0.0, -1.2,
    
	 /*8th pole*/
     -2.9,  0.0, 2.2,//index 113
     -2.9, -0.1, 2.2,
     -2.8, -0.1, 2.2,
     -2.8,  0.0, 2.2,
     
     -2.9,  0.0, -1.5,
     -2.9, -0.1, -1.5,
     -2.8, -0.1, -1.5,
     -2.8,  0.0, -1.5, //index 120  

    
       
};   


GLfloat color_buffer_data[] = { 
    
	/*Baseplate*/
    0.0, 0.0, 1.0,
    1.0, 0.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,   
    1.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 0.0, 0.0, 
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.3, 0.0, 
    1.0, 0.3, 0.0,
    1.0, 0.3, 0.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 0.0,
    
 
	/*Topplate*/
    0.0, 1.0, 1.0,
    0.0, 1.0, 1.0,
    1.0, 0.0, 0.0,
    1.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 1.0, 0.0,    
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,


	/*Peek*/
    0.0, 1.0, 0.0,


	/*mittelstange*/
    1.0,1.0,0.0,
    1.0,1.0,0.0,
    1.0,1.0,0.0,
    1.0,1.0,0.0,
    1.0,1.0,0.0,
    1.0,1.0,0.0,
    1.0,1.0,0.0,
    1.0,1.0,0.0,

	/*1st pole*/
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0, 
    1.0,0.0,0.0,
    1.0,0.0,0.0,
	
	/*2nd pole*/
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0, 
    1.0,0.0,0.0,
    1.0,0.0,0.0,
   
	/*3rd pole*/
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0, 
    1.0,0.0,0.0,
    1.0,0.0,0.0,
   
	/*4th pole*/
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    1.0,0.0,0.0, 
    1.0,0.0,0.0,
    1.0,0.0,0.0,
    	
   /*Groundplate*/
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,
   0.0,1.0,0.0,

	/*5th pole*/
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0, 
    0.0,0.5,1.0,
    0.0,0.5,1.0,

	/*6th pole*/
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0, 
    0.0,0.5,1.0,
    0.0,0.5,1.0,

	/*7th pole*/
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0, 
    0.0,0.5,1.0,
    0.0,0.5,1.0,

	/*8th pole*/
   0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0,
    0.0,0.5,1.0, 
    0.0,0.5,1.0,
    0.0,0.5,1.0,


  
}; 

GLushort index_buffer_data[] = { 
    
	/*Baseplate*/
    0, 1, 3,
    1, 2, 3,
    0, 8, 1,
    1, 9, 2,
    2,10, 3,
    3,11, 0,

    7, 5, 4,
    7, 5, 6,
    4,12, 5,
    5,13, 6,
    6,14, 7,
    7,15, 4,
 
    4, 8,12,
    4, 8, 0,
   12, 1, 5,
   12, 1, 8,
    5, 9,13,
    5, 9, 1,
   13, 2, 6,
   13, 2, 9,
    6,10,14,
    6,10, 2,
   14, 3, 7,
   14, 3,10,
    7,11,15,
    7,11, 3,
   15, 0, 4,
   15, 0,11,
  

	/*Topplate*/
    16,17,19,
    17,18,19,
    16,24,17,
    17,25,18,
    18,26,19,
    19,27,16,

    23,21,20,
    23,21,22,
    20,28,21,
    21,29,22,
    22,30,23,
    23,31,21,

    20,24,28,
    20,24,16,
    28,17,21,
    28,17,24,
    21,25,29,
    21,25,17,
    29,18,22,
    29,18,25,
    22,26,30,
    22,26,18,
    30,19,23,
    30,19,26,
    23,27,31,
    23,27,19,
    31,16,20,
    31,16,27,
    
    /*Roof*/
   19,26,32,
   26,18,32,
   18,25,32,
   25,17,32,
   17,24,32,
   24,16,32,
   16,27,32,
   27,19,32,
   

   /*center pole*/
   33,34,37,
   38,37,34,
   34,35,38,
   38,35,39,
   35,36,39,
   39,40,36,
   36,33,40,
   37,40,33,

   /*1st pole*/
   41,44,45,
   45,48,44,
   41,42,45,
   45,46,42,
   42,43,46,
   46,47,43,
   43,44,48,
   48,47,43,

   /*2nd pole*/
   49,52,53,
   53,56,52,
   49,50,53,
   53,54,50,
   50,51,54,
   54,55,51,
   51,52,56,
   56,55,51,

   
   /*3rd pole*/
   57,60,61,
   61,64,60,
   57,58,61,
   61,62,58,
   58,59,62,
   62,63,59,
   59,60,64,
   64,63,59,

   /*4th pole*/
   65,68,69,
   69,72,68,
   65,66,69,
   69,70,66,
   66,67,70,
   70,71,67,
   67,68,72,
   72,71,67,
   
   /*5th pole*/
   89,92,93,
   93,96,92,
   89,90,93,
   93,94,90,
   90,91,94,
   94,95,91,
   91,92,96,
   96,95,91,

    /*6th pole*/
	97,100,101,
	101,104,100,
	97,98,101,
	101,102,98,
	98,99,102,
	102,103,99,
	99,100,104,
	104,103,99,

	/*7th pole*/
	105,108,109,
	109,112,108,
	105,106,109,
	109,110,106,
	106,107,110,
	110,111,107,
	107,108,112,
	112,111,107,

	/*8th pole*/
	113,116,117,
	117,120,116,
	113,114,117,
	117,118,114,
	114,115,118,
	118,119,115,
	115,116,120,
	120,119,115,
	
	/*Groundplate*/	   
	73,74,76,
	74,75,76,
	73,81,74,
	74,82,75,
	75,83,76,
	76,84,73,
	80,78,77,
	80,78,79,
	77,85,78,
	78,86,79,
	79,87,80,
	80,88,77,
	77,81,85,
	77,81,73,
	85,74,78,
	85,74,81,
	78,82,86,
	78,82,74,
	86,75,79,
	86,75,82,
	79,83,87,
	79,83,75,
	87,76,80,
	87,76,83,
	80,84,88,
	80,84,76,
	88,73,77,
	88,73,84,


	    
};
/*	Buffers for second model
	Cubes on poles			*/
GLfloat vertex_buffer_data2[] = {  
    -0.3, -0.3,  0.3, 
     0.3, -0.3,  0.3, 
     0.3,  0.3,  0.3, 
    -0.3,  0.3,  0.3, 
 
    -0.3, -0.3, -0.3, 
     0.3, -0.3, -0.3, 
     0.3,  0.3, -0.3, 
    -0.3,  0.3, -0.3, 
};    
 
GLfloat color_buffer_data2[] = {  
    1.0, 0.0, 0.0, 
    1.0, 1.0, 0.0, 
    1.0, 1.0, 0.0, 
    1.0, 1.0, 0.0, 
    1.0, 0.0, 0.0, 
    1.0, 0.0, 0.0, 
    1.0, 1.0, 0.0, 
};  
 
GLushort index_buffer_data2[] = { 

  0,1,4,
  1,5,4,  
  1,2,5,
  2,5,6,
  2,3,6,
  3,6,7,
  0,3,7,
  0,4,7,
  0,1,3,
  1,2,3,
  4,5,7,
  5,6,7,

};

/*----------------------------------------------------------------*/


/******************************************************************
*
* Display
*
* This function is called when the content of the window needs to be
* drawn/redrawn. It has been specified through 'glutDisplayFunc()';
* Enable vertex attributes, create binding between C program and 
* attribute name in shader
*
*******************************************************************/

void Display()
{
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    GLint size; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	
	
    /* Associate carousel Model with shader matrices */
    GLint projectionUniform = glGetUniformLocation(ShaderProgram, "ProjectionMatrix");
    if (projectionUniform == -1) 
    {
        fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
	exit(-1);
    }
    glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, ProjectionMatrix);
    
    GLint ViewUniform = glGetUniformLocation(ShaderProgram, "ViewMatrix");
    if (ViewUniform == -1) 
    {
        fprintf(stderr, "Could not bind uniform ViewMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, ViewMatrix);
   
    GLint RotationUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (RotationUniform == -1) 
    {
        fprintf(stderr, "Could not bind uniform ModelMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix); 
    
    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    
    /* Disable attributes */
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor); 
    
    
    
    /** Second Model **/
	glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO2);
    glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, 0, 0);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
    GLint size2; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size2);
    
    /* Associate first Model with shader matrices */
    GLint RotationUniform2 = glGetUniformLocation(ShaderProgram2, "ModelMatrix");
    glUniformMatrix4fv(RotationUniform2, 1, GL_TRUE, Model2Matrix);

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size2 / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    /* Disable attributes */
    glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	
	/** Third Model **/
	glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO3);
    glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, 0, 0);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO3);
    GLint size3; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size3);
    
    /* Associate first Model with shader matrices */
    GLint RotationUniform3 = glGetUniformLocation(ShaderProgram3, "ModelMatrix");
    glUniformMatrix4fv(RotationUniform3, 1, GL_TRUE, Model3Matrix);

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size3 / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    /* Disable attributes */
    glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	
	/** Fourth Model **/
	glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO4);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO4);
    glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, 0, 0);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
    GLint size4; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size4);
    
    /* Associate first Model with shader matrices */
    GLint RotationUniform4 = glGetUniformLocation(ShaderProgram4, "ModelMatrix");
    glUniformMatrix4fv(RotationUniform4, 1, GL_TRUE, Model4Matrix);

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size4 / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    /* Disable attributes */
    glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	
	/** Fifth Model **/
	glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO5);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO5);
    glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, 0, 0);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
    GLint size5; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size5);
    
    /* Associate first Model with shader matrices */
    GLint RotationUniform5 = glGetUniformLocation(ShaderProgram5, "ModelMatrix");
    glUniformMatrix4fv(RotationUniform5, 1, GL_TRUE, Model5Matrix);

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size5 / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    /* Disable attributes */
    glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);  

    /* Swap between front and back buffer */ 
    glutSwapBuffers();
}


/******************************************************************
*
* OnIdle
*
* 
*
*******************************************************************/

void OnIdle()
{
	/* print function for classic printf debugging */
	void print_matrix(float* matrix) {
		printf("[\n");
		for (int i = 0; i < 4; i++) {
			printf("  [%8f,\t%8f,\t%8f,\t%8f],\n", matrix[i*4], matrix[i*4 + 1], matrix[i*4 + 2], matrix[i*4 + 3]);
		}
		printf("]\n");
	}
	
    float angle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (180.0/M_PI); 
    float RotationMatrixAnim[16];
    
    float TranslationMatrixMove2[16];
    float TranslationMatrixMove3[16];
    float TranslationMatrixMove4[16];
    float TranslationMatrixMove5[16];
    float TranslationMatrixSlide[16];

    /* Time dependent rotation */
    SetRotationY(angle, RotationMatrixAnim);
    
    /* Move the cubes */
    SetTranslation(0.0, 0.0, 2.8, TranslationMatrixMove2);
    SetTranslation(0.0, 0.0, -2.8, TranslationMatrixMove3);
    SetTranslation(2.8, 0.0, 0.0, TranslationMatrixMove4);
    SetTranslation(-2.8, 0.0, 0.0, TranslationMatrixMove5);
    
    /* Sliding animation */
    SetTranslation(0.0, sinf(angle/100), 0.0, TranslationMatrixSlide);

    /* Apply carousel rotation; finally move carousel down */
    MultiplyMatrix(RotationMatrixAnim, InitialTransform, ModelMatrix);
    MultiplyMatrix(TranslateDown, ModelMatrix, ModelMatrix);
    
    /* Applay Transformation on the cubes */
	MultiplyMatrix(TranslationMatrixMove2, InitialTransform, Model2Matrix);
    MultiplyMatrix(TranslationMatrixMove3, InitialTransform, Model3Matrix);
    MultiplyMatrix(TranslationMatrixMove4, InitialTransform, Model4Matrix);
    MultiplyMatrix(TranslationMatrixMove5, InitialTransform, Model5Matrix);
    
    MultiplyMatrix(RotationMatrixAnim, Model2Matrix, Model2Matrix);
    MultiplyMatrix(RotationMatrixAnim, Model3Matrix, Model3Matrix);
    MultiplyMatrix(RotationMatrixAnim, Model4Matrix, Model4Matrix);
    MultiplyMatrix(RotationMatrixAnim, Model5Matrix, Model5Matrix);
    
    MultiplyMatrix(TranslationMatrixSlide, Model2Matrix, Model2Matrix);
    MultiplyMatrix(TranslationMatrixSlide, Model3Matrix, Model3Matrix);
    MultiplyMatrix(TranslationMatrixSlide, Model4Matrix, Model4Matrix);
    MultiplyMatrix(TranslationMatrixSlide, Model5Matrix, Model5Matrix);
    
    
    MultiplyMatrix(TranslateDown, Model2Matrix, Model2Matrix);
    MultiplyMatrix(TranslateDown, Model3Matrix, Model3Matrix);
    MultiplyMatrix(TranslateDown, Model4Matrix, Model4Matrix);
    MultiplyMatrix(TranslateDown, Model5Matrix, Model5Matrix);
    

    /* Request redrawing forof window content */  
    glutPostRedisplay();
}


/******************************************************************
*
* SetupDataBuffers
*
* Create buffer objects and load data into buffers
*
*******************************************************************/

void SetupDataBuffers()
{

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
	
	/* First Model */
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &CBO);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

	/* Second Model */
	glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data2), vertex_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data2), index_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &CBO2);
    glBindBuffer(GL_ARRAY_BUFFER, CBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data2), color_buffer_data2, GL_STATIC_DRAW);
	
	/* Third Model */
	glGenBuffers(1, &VBO3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data2), vertex_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data2), index_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &CBO3);
    glBindBuffer(GL_ARRAY_BUFFER, CBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data2), color_buffer_data2, GL_STATIC_DRAW);

	/* Fourth Model */
	glGenBuffers(1, &VBO4);
    glBindBuffer(GL_ARRAY_BUFFER, VBO4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data2), vertex_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO4);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO4);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data2), index_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &CBO4);
    glBindBuffer(GL_ARRAY_BUFFER, CBO4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data2), color_buffer_data2, GL_STATIC_DRAW);

	/* Fifth Model */
	glGenBuffers(1, &VBO5);
    glBindBuffer(GL_ARRAY_BUFFER, VBO5);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data2), vertex_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO5);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO5);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data2), index_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &CBO5);
    glBindBuffer(GL_ARRAY_BUFFER, CBO5);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data2), color_buffer_data2, GL_STATIC_DRAW);

}


/******************************************************************
*
* AddShader
*
* This function creates and adds individual shaders
*
*******************************************************************/

void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType)
{
    /* Create shader object */
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) 
    {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    /* Associate shader source code string with shader object */
    glShaderSource(ShaderObj, 1, &ShaderCode, NULL);

    GLint success = 0;
    GLchar InfoLog[1024];

    /* Compile shader source code */
    glCompileShader(ShaderObj);
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) 
    {
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    /* Associate shader with shader program */
    glAttachShader(ShaderProgram, ShaderObj);
}


/******************************************************************
*
* CreateShaderProgram
*
* This function creates the shader program; vertex and fragment
* shaders are loaded and linked into program; final shader program
* is put into the rendering pipeline 
*
*******************************************************************/

void CreateShaderProgram()
{
    /* Allocate shader object */
    ShaderProgram = glCreateProgram();
    ShaderProgram2 = glCreateProgram();
    ShaderProgram3 = glCreateProgram();
    ShaderProgram4 = glCreateProgram();
    ShaderProgram5 = glCreateProgram();

    if (ShaderProgram == 0) 
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    /* Load shader code from file */
    VertexShaderString = LoadShader("vertexshader.vs");
    FragmentShaderString = LoadShader("fragmentshader.fs");

    /* Separately add vertex and fragment shader to program */
    AddShader(ShaderProgram, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, FragmentShaderString, GL_FRAGMENT_SHADER);
    AddShader(ShaderProgram2, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram2, FragmentShaderString, GL_FRAGMENT_SHADER);
    AddShader(ShaderProgram3, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram3, FragmentShaderString, GL_FRAGMENT_SHADER);
    AddShader(ShaderProgram4, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram4, FragmentShaderString, GL_FRAGMENT_SHADER);
    AddShader(ShaderProgram5, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram5, FragmentShaderString, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024];

    /* Link shader code into executable shader program */
    glLinkProgram(ShaderProgram);
    glLinkProgram(ShaderProgram2);
    glLinkProgram(ShaderProgram3);
    glLinkProgram(ShaderProgram4);
    glLinkProgram(ShaderProgram5);

    /* Check results of linking step */
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

    if (Success == 0) 
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Check if shader program can be executed */ 
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);

    if (!Success) 
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Put linked shader program into drawing pipeline */
    glUseProgram(ShaderProgram);
    glUseProgram(ShaderProgram2);
    glUseProgram(ShaderProgram3);
    glUseProgram(ShaderProgram4);
    glUseProgram(ShaderProgram5);
}


/******************************************************************
*
* Initialize
*
* This function is called to initialize rendering elements, setup
* vertex buffer objects, and to setup the vertex and fragment shader
*
*******************************************************************/

void Initialize(void)
{   
    /* Set background (clear) color to dark blue */ 
    glClearColor(0.0, 0.0, 0.8, 0.0);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);    

    /* Setup vertex, color, and index buffer objects */
    SetupDataBuffers();

    /* Setup shaders and shader program */
    CreateShaderProgram();  

    /* Initialize matrices */
    SetIdentityMatrix(ProjectionMatrix);
    SetIdentityMatrix(ViewMatrix);
    SetIdentityMatrix(ModelMatrix);
    
    SetIdentityMatrix(Model2Matrix);
    SetIdentityMatrix(Model3Matrix);
    SetIdentityMatrix(Model4Matrix);
    SetIdentityMatrix(Model5Matrix);

    /* Set projection transform */
    float fovy = 45.0;
    float aspect = 1.0; 
    float nearPlane = 1.0; 
    float farPlane = 50.0;
    SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);
	
    /* Set viewing transform */
    float camera_disp = -10.0;
    SetTranslation(0.0, 0.0, camera_disp, ViewMatrix);

    /* Translate and rotate carousel into right position */
    SetTranslation(0, 0, 1, TranslateOrigin);
    SetRotationX(-90, RotateX);
    SetRotationZ(0, RotateZ);	

    /* Translate down */	
    SetTranslation(0, -sqrtf(sqrtf(2.0) * 1.0), 0, TranslateDown);

    /* Initial transformation matrix */
    MultiplyMatrix(RotateX, TranslateOrigin, InitialTransform);
    MultiplyMatrix(RotateZ, InitialTransform, InitialTransform);
}


/******************************************************************
*
* main
*
* Main function to setup GLUT, GLEW, and enter rendering loop
*
*******************************************************************/

int main(int argc, char** argv)
{
    /* Initialize GLUT; set double buffered window and RGBA color model */
    glutInit(&argc, argv);

    glutInitContextVersion(3,3);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(600, 600);
    glutCreateWindow("CG Proseminar - Carousel by Manuel Buchauer, Davide De Sclavis and Lukas Dötlinger");

    /* Initialize GL extension wrangler */
    glewExperimental = GL_TRUE;
    GLenum res = glewInit();
    if (res != GLEW_OK) 
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    /* Setup scene and rendering parameters */
    Initialize();


    /* Specify callback functions;enter GLUT event processing loop, 
     * handing control over to GLUT */
    glutIdleFunc(OnIdle);
    glutDisplayFunc(Display);
    glutMainLoop();

    /* ISO C requires main to return int */
    return 0;
}

