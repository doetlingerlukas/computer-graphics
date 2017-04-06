/******************************************************************
*
* RotatingCube.c
*
* Description: This example demonstrates a colored, rotating
* cube in shader-based OpenGL. The use of transformation
* matrices, perspective projection, and indexed triangle sets 
* are shown.
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
GLuint VBO;
GLuint VBO2; 

/* Define handle to a color buffer object */
GLuint CBO;
GLuint CBO2; 

/* Define handle to an index buffer object */
GLuint IBO;
GLuint IBO2;

GLuint VAO;


/* Indices to vertex attributes; in this case positon and color */ 
enum DataID {vPosition = 0, vColor = 1}; 

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;
GLuint ShaderProgram2; /* Shader for second Model */

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */ 
float ModelMatrix[16]; /* Model matrix */ 

float ProjectionMatrix2[16]; /* Perspective projection matrix */
float ViewMatrix2[16]; /* Camera view matrix */ 
float Model2Matrix[16]; /* Model2 matrix */

/* Transformation matrices for initial position */
float TranslateOrigin[16];
float TranslateDown[16];
float RotateX[16];
float RotateZ[16];
float InitialTransform[16];

/* Buffers for first Model */

GLfloat vertex_buffer_data[] = { /* base plate */
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

     /**/
   

    
      
    

/*Topplate*/

    -2.0,-2.0,2,
     2.0, -2.0,2,
     2.0, 2.0, 2,
     -2.0,2.0,2,

    -2.0,-2.0,2.2,
     2.0, -2.0,2.2,
     2.0, 2.0, 2.2,
     -2.0,2.0,2.2,

     0.0, -3.0, 2,
     3.0,  0.0, 2,
     0.0,  3.0, 2,
    -3.0,  0.0, 2,
    
     0.0, -3.0, 2.2,
     3.0,  0.0, 2.2,
     0.0,  3.0, 2.2,
    -3.0,  0.0, 2.2,

     
     0.0,0.0,4, //32

     0.2,0.2,-1.5,//33
    -0.2,0.2,-1.5,
    -0.2,-0.2,-1.5,
     0.2,-0.2,-1.5,
    
     0.2,0.2,2,//37
    -0.2,0.2,2,
    -0.2,-0.2,2,
     0.2,-0.2,2,
 

     /*1.stange*/
    -1.9, -1.9, -1.5,//41
    -1.9, -1.8, -1.5,
    -1.8, -1.8, -1.5,
    -1.8, -1.9, -1.5,

    -1.9, -1.9, 2,//45
    -1.9, -1.8, 2,
    -1.8, -1.8, 2,
    -1.8, -1.9, 2,
    

   /*2.stange*/
    1.9, 1.9, -1.5,//49
    1.9, 1.8, -1.5,
    1.8, 1.8, -1.5,
    1.8, 1.9, -1.5,
    
    1.9, 1.9, 2,//53
    1.9, 1.8, 2,
    1.8, 1.8, 2,
    1.8, 1.9, 2,

  /*3.stange*/
    1.9, -1.9, -1.5,//57
    1.9, -1.8, -1.5,
    1.8, -1.8, -1.5,
    1.8, -1.9, -1.5,
    
    1.9, -1.9, 2,//61
    1.9, -1.8, 2,
    1.8, -1.8, 2,
    1.8, -1.9, 2,


   /*4.stange*/
    -1.9, 1.9, -1.5,//65
    -1.9, 1.8, -1.5,
    -1.8, 1.8, -1.5,
    -1.8, 1.9, -1.5,
    
    -1.9, 1.9, 2,//69
    -1.9, 1.8, 2,
    -1.8, 1.8, 2,
    -1.8, 1.9, 2,

    
    /*unterste*/

    -2.2, -2.2, -1.85,//73
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
     

    
};   


GLfloat color_buffer_data[] = { /* baseplate */
    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,

    1.0, 1.0, 1.0,
    1.0, 0.0, 0.0,
    
    1.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    1.0, 1.0, 1.0,

    1.0,0.0,0.0, 
    1.0,0.0,0.0,
    1.0,0.0,0.0,

   1.0,0.3,0.0, 
   1.0,0.3,0.0,
   1.0,0.3,0.0,


    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,

    1.0, 1.0, 1.0,
    1.0, 0.0, 0.0,
    
    1.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    1.0, 1.0, 1.0,

   1.0,0.0,1.0,
   1.0,0.0,1.0,
   1.0,0.0,1.0,

     0.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,

    1.0, 1.0, 1.0,
    1.0, 0.0, 0.0,
    
    1.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    1.0, 1.0, 1.0,

    1.0,1.0,0.0,
    1.0,1.0,0.0,
    1.0,1.0,0.0,
    1.0,1.0,0.0,
    1.0,1.0,0.0,
    1.0,1.0,0.0,

   1.0,0.0,1.0,
   1.0,0.0,1.0,
   1.0,0.0,1.0,
   1.0,0.0,1.0,
    
   1.0,0.0,0.0,
   1.0,0.0,0.0,
   1.0,0.0,0.0,
   1.0,0.0,0.0, 
   1.0,0.0,0.0,
    1.0,0.0,0.0,
   1.0,0.0,0.0,
   1.0,0.0,0.0,
   1.0,0.0,0.0, 
   1.0,0.0,0.0,
    1.0,0.0,0.0,
   1.0,0.0,0.0,
   1.0,0.0,0.0,
   1.0,0.0,0.0, 
   1.0,0.0,0.0,

   0.2,1.0,1.0,
   0.2,1.0,1.0,
   0.2,1.0,1.0,
   0.2,1.0,1.0,
   0.2,1.0,1.0,
   0.2,1.0,1.0,
   0.2,1.0,1.0,

    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,

    1.0, 1.0, 1.0,
   	 1.0, 0.0, 0.0,
    
    	1.0, 1.0, 0.0,
   	0.0, 1.0, 0.0,
    	1.0, 1.0, 1.0,


        0.0,1.0,1.0,
	0.0,1.0,1.0,
	0.0,1.0,1.0,
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
   

  
}; 

GLushort index_buffer_data[] = { /* baseplate */
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
    5, 9, 13,
    5, 9, 1,
    13, 2, 6,
    13, 2 ,9,
    6,10,14,
    6,10, 2,
    14, 3, 7,
    14, 3 ,10,
    7,11,15,
    7,11, 3,
    15, 0, 4,
    15, 0, 11,
  
    /*28*/

/*topplate*/
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
    
   /*dach*/ 
   19,26,32,
   26,18,32,
   18,25,32,
   25,17,32,
   17,24,32,
   24,16,32,
   16,27,32,
   27,19,32,
   

/*mittelstange*/
   33,34,37,
   38,37,34,
   34,35,38,
   38,35,39,
   35,36,39,
   39,40,36,
   36,33,40,
   37,40,33,

  /*stangen*/
   41,44,45,
   45,48,44,
   41,42,45,
   45,46,42,
   42,43,46,
   46,47,43,
   43,44,48,
   48,47,43,


   49,52,53,
   53,56,52,
   49,50,53,
   53,54,50,
   50,51,54,
   54,55,51,
   51,52,56,
   56,55,51,

   

   57,60,61,
   61,64,60,
   57,58,61,
   61,62,58,
   58,59,62,
   62,63,59,
   59,60,64,
   64,63,59,

   
   65,68,69,
   69,72,68,
   65,66,69,
   69,70,66,
   66,67,70,
   70,71,67,
   67,68,72,
   72,71,67,
  

       /*unterste*/
	   
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

/* Buffers for second Model */

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
    
    /*
    setShaderMatrix(ShaderProgram, "ProjectionMatrix", ProjectionMatrix);
	setShaderMatrix(ShaderProgram, "ViewMatrix", ViewMatrix);
	setShaderMatrix(ShaderProgram, "ModelMatrix", ModelMatrix);*/
	
	
	
    /* Associate first Model with shader matrices */
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
    
    /*
	// link projection-matrice to shader (to be able to use it in the shader)
	setShaderMatrix(ShaderProgram2, "ProjectionMatrix2", ProjectionMatrix2);
	// link view-matrice to shader
	setShaderMatrix(ShaderProgram2, "ViewMatrix2", ViewMatrix2);
	// link model-matrice to shader
	setShaderMatrix(ShaderProgram2, "Model2Matrix", Model2Matrix);*/
	

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size2 / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

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
    float angle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (180.0/M_PI); 
    float RotationMatrixAnim[16];

    /* Time dependent rotation */
    SetRotationY(angle, RotationMatrixAnim);

    /* Apply model rotation; finally move cube down */
    MultiplyMatrix(RotationMatrixAnim, InitialTransform, ModelMatrix);
    MultiplyMatrix(RotationMatrixAnim, InitialTransform, Model2Matrix);
    MultiplyMatrix(TranslateDown, ModelMatrix, ModelMatrix);
    MultiplyMatrix(TranslateDown, Model2Matrix, Model2Matrix);

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

    GLint Success = 0;
    GLchar ErrorLog[1024];

    /* Link shader code into executable shader program */
    glLinkProgram(ShaderProgram);
    glLinkProgram(ShaderProgram2);

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
    SetIdentityMatrix(ProjectionMatrix2);
    SetIdentityMatrix(ViewMatrix2);
    SetIdentityMatrix(Model2Matrix);

    /* Set projection transform */
    float fovy = 45.0;
    float aspect = 1.0; 
    float nearPlane = 1.0; 
    float farPlane = 50.0;
    SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);
	SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix2);
	
    /* Set viewing transform */
    float camera_disp = -10.0;
    SetTranslation(0.0, 0.0, camera_disp, ViewMatrix);
    SetTranslation(0.0, 0.0, camera_disp, ViewMatrix2);

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

