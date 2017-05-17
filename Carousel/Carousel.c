/******************************************************************
*
* Carousel.c
*
* by Davide De Sclavis, Manuel Buchauer und Lukas Dötlinger
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
#include "Setup.h"
#include "OBJParser.h"

/*----------------------------------------------------------------*/
GLuint VAO;

/* Flag for starting/stopping animation */
GLboolean anim = GL_TRUE;

/* Reference time for animation */
int oldTime = 0;

/* Define handle to a vertex buffer object */
GLuint VBO6;

/* Define handle to a color buffer object */
GLuint CBO6;

/* Define handle to an index buffer object */
GLuint IBO6;

buffer_object* carousel;
buffer_object* pig1;
buffer_object* pig2;
buffer_object* pig3;
buffer_object* pig4;

/* Indices to vertex attributes; in this case positon and color */ 
enum DataID {vPosition = 0, vColor = 1, vNormal = 2}; 

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;  /* Shader for carousel */
GLuint ShaderProgram2;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */ 

float ModelMatrix[16]; /* carousel matrix */ 
float Model2Matrix[16]; /* pig 1 matrix */
float Model3Matrix[16]; /* pig 2 matrix */
float Model4Matrix[16]; /* pig 3 matrix */
float Model5Matrix[16]; /* pig 4 matrix */
float Model6Matrix[16]; /* Room matrix */
float Model7Matrix[16]; //For lamp

/* Transformation matrices for initial position */
float TranslateOrigin[16];
float TranslateDown[16];
float RotateX[16];
float RotateZ[16];
float InitialTransform[16];

/* Indices for different rotation modes */
enum {clockwise=1, counterclockwise=2};
int rotationMode = counterclockwise; 
/* Indices for different camera modes */
enum {Default=0, Mode1=1, Mode2=2, Pause=3};
int cameraMode = Default; 
/* Indices for different rotation speeds */
enum {slow=4, standard=2, fast=1};
int rotationSpeed = standard;

/* Buffers for Carousel */
GLfloat* vertex_buffer_data;
GLfloat* vertex_normals;
GLfloat* color_buffer_data;
GLushort* index_buffer_data;
obj_scene_data data;

/* Mouse and keyboard motion */
float camera_z = 0;
float camera_y = 0;
float camera_x = 0;
float rotate_x = 0;
float rotate_y = 0;
int mouse_oldx;
int mouse_oldy;

/* light sources */
float LightPosition1[] = { 5.0, 4.0, 5.0 };
float LightColor1[] = { 1.0, 1.0, 1.0 };

float ambientFactor = 1;
float diffuseFactor = 1;
float specularFactor = 1;

int ambientToggle = 1;
int diffuseToggle = 1;
int specularToggle = 1;
int light1Toggle = 1;


/* Buffer for Room */
GLfloat vertex_buffer_data2[] = {
	// Floor 
	7.0, -1.0, 7.0,
	-7.0, -1.0, 7.0,
	7.0, -1.0, -7.0,
	-7.0, -1.0, -7.0,
	// Roof 
	7.0, 4.0, 7.0,
	-7.0, 4.0, 7.0,
	7.0, 4.0, -7.0,
	-7.0, 4.0, -7.0,
};

GLfloat color_buffer_data2[] = {
	// Floor 
	0.0,0.0,0.0,
	0.87,0.72,0.53,
	// Back wall
	0.87,0.72,0.53,
	0.87,0.72,0.53,
	// Right wall 
	0.87,0.72,0.53,
	0.87,0.72,0.53,
	// Left wall 
	0.87,0.72,0.53,
	0.87,0.72,0.53,
};

GLushort index_buffer_data2[] = {
	// Floor 
	0, 1, 3,
	0, 2, 3,
	// Back wall 
	3, 2, 6,
	3, 7, 6,
	// Right wall 
	0, 2, 6,
	0, 4, 6,
	// Left wall 
	1, 3, 7,
	1, 5, 7,
};

/* Buffer for the pigs */
GLfloat* vertex_buffer_data3;
GLfloat* color_buffer_data3;
GLushort* index_buffer_data3;
obj_scene_data data3;

/*Buffer for the lamp */												//======================================================
GLfloat* vertex_buffer_data4;
GLfloat* color_buffer_data4;
GLushort* index_buffer_data4;
obj_scene_data data4;

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

void Display(){
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   
	
    /* Associate carousel Model with shader matrices */
    glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "ProjectionMatrix"), 1, GL_TRUE, ProjectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "ViewMatrix"), 1, GL_TRUE, ViewMatrix);    
       
    
    /** Carousel **/
    etupAndDraw(carousel, ShaderProgram, ModelMatrix);
    
    
    /** Room **/
    setupAndDraw(VBO6, CBO6, IBO6, ShaderProgram2, Model6Matrix);
    
        
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);
    
    /** Pigs **/
	etupAndDraw(pig1, ShaderProgram, Model2Matrix);
	etupAndDraw(pig2, ShaderProgram, Model3Matrix);
	etupAndDraw(pig3, ShaderProgram, Model4Matrix);
	etupAndDraw(pig4, ShaderProgram, Model5Matrix);
	
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	
	 /**LAMP **/
    setupAndDraw(VBO7, CBO7, IBO7, ShaderProgram, Model7Matrix);
	
	/** Light sources **/
	GLint LightPos1Uniform = glGetUniformLocation(ShaderProgram, "LightPosition1");
	glUniform3f(LightPos1Uniform, LightPosition1[0], LightPosition1[1], LightPosition1[2]);
	
	GLint LightCol1Uniform = glGetUniformLocation(ShaderProgram, "LightColor1");
	glUniform3f(LightCol1Uniform, LightColor1[0]*light1Toggle, LightColor1[1]*light1Toggle, LightColor1[2]*light1Toggle);
	
	GLint AmbientFactorUniform = glGetUniformLocation(ShaderProgram, "AmbientFactor");
	glUniform1f(AmbientFactorUniform, ambientFactor * ambientToggle);
	
	GLint DiffuseFactorUniform = glGetUniformLocation(ShaderProgram, "DiffuseFactor");
	glUniform1f(DiffuseFactorUniform, diffuseFactor * diffuseToggle);
	
	GLint SpecularFactorUniform = glGetUniformLocation(ShaderProgram, "SpecularFactor");
	glUniform1f(SpecularFactorUniform, specularFactor * specularToggle);	
	
	/* Only draw lines. At this point necessary for drawing the obj file */
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
    /* Swap between front and back buffer */ 
    glutSwapBuffers();
}

/******************************************************************
*
* Mouse
*
* Function is called on mouse button press; has been seta
* with glutMouseFunc(), x and y specify mouse coordinates
* 
* mouseMotion
* 
* Function to control the view in cameraMode 2; has been 
* set with glutMotionFunc(), x and y specify mouse coordinates
*
*******************************************************************/

void mouseMotion(int x, int y){
	float SPEED = 0.01 / (2 * M_PI);
	
	if(cameraMode == Mode2){
		rotate_x += -(mouse_oldy - y) * SPEED; 
		rotate_y += -(mouse_oldx - x) * SPEED; 
		//printf("rx:%f, ry:%f \n", rotate_x, rotate_y);
		
		mouse_oldx = x;
		mouse_oldy = y;
		//printf("x:%d, y:%d\n", mouse_oldx, mouse_oldy);
	}
	
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
   
    if(state == GLUT_DOWN) {
        
		switch(button){
			
		case GLUT_LEFT_BUTTON:    
			break;
			
		case GLUT_MIDDLE_BUTTON:  
			break;
		
		case GLUT_RIGHT_BUTTON: 
			break;
		}
    }
}

/******************************************************************
*
* Keyboard
*
* Function to be called on key press in window; set by
* glutKeyboardFunc(); x and y specify mouse position on keypress;
* not used in this example 
*
*******************************************************************/

void Keyboard(unsigned char key, int x, int y)   {
    
    switch( key ) {
	/* Switch camera modes */
	case '1': 
		cameraMode = Mode1;
		break;
	case '2':
		cameraMode = Mode2;	
		SetIdentityMatrix(ViewMatrix);
		SetTranslation(0.0, 0.0, -15.0, ViewMatrix);
		camera_x = 0;
		camera_y = 0;
		camera_z = 0;
		rotate_x = 0;
		rotate_y = 0;
		break;
	case '0':
		cameraMode = Default;
		SetTranslation(0.0, 0.0, -10.0, ViewMatrix);
		break;

	/* Toggle animation */
	case 'p':
		if (cameraMode == Pause)
			cameraMode = Mode1;
		else 
			cameraMode = Pause;
		break;
	case 'b':
		if (anim)
			anim = GL_FALSE;		
		else
			anim = GL_TRUE;
		break;
	case 'r':
		if (rotationMode == clockwise)
			rotationMode = counterclockwise;
		else 
			rotationMode = clockwise;
		break;
	case '+':
	    if (rotationSpeed == 4)
			rotationSpeed = 2;
		else if (rotationSpeed == 2)
			rotationSpeed = 1;
		else 
			rotationSpeed = 4;
	    break;
	    
	/* Move camera in camera mode 2 */
	case 'w':
		camera_z += 0.1;
		break;
	case 's':
		camera_z -= 0.1;
		break;
	case 'a':
		camera_x += 0.1;
		break;
	case 'd':
		camera_x -= 0.1;
		break; 
	case 't':
		camera_y -= 0.1;
		break;
	case 'g':
		camera_y += 0.1;
		break;
	  
	/* Close the scene */
	case 'q': case 'Q':  
	    exit(0);    
		break;
    }

    glutPostRedisplay();
}

/******************************************************************
*
* OnIdle
*
* 
*
*******************************************************************/

void OnIdle(){	
	/* Determine delta time between two frames to ensure constant animation */
	int newTime = glutGet(GLUT_ELAPSED_TIME);
	int delta = 0;
	if (!anim){
		delta += newTime - oldTime;
	}
    oldTime = newTime - delta;
	
    float angle = (oldTime / 1000.0) * (180.0/M_PI);
    float slide_angle = sinf(angle/100);
    float RotationMatrixAnim[16];
    
    float TranslationMatrixMove[16];
    float TranslationMatrixMove2[16];
    float TranslationMatrixMove3[16];
    float TranslationMatrixMove4[16];
    float TranslationMatrixMove5[16];
    float TranslationMatrixSlide[16];
    float TranslationMatrixMove7[16];
    float RotationMatrixX[16];
    float RotationMatrixX2[16];
    float RotationMatrixY2[16];
    float RotationMatrixY3[16];
    float RotationMatrixY4[16];
    float RotationMatrixZ[16];
    float ScalingMatrixCarousel[16];
    float ScalingMatrix[16];
	
	if(rotationMode == clockwise){
		angle = -angle;
		slide_angle = -slide_angle;
	}
    /* Time dependent rotation */
    SetRotationY(angle, RotationMatrixAnim);
    /* Sliding animation for pigs */
    SetTranslation(0.0, slide_angle, 0.0, TranslationMatrixSlide);
    
    /* Move the pigs */
    SetTranslation(-0.7, 0.6, 3.3, TranslationMatrixMove2);
    SetTranslation(0.7, 0.6, -3.3, TranslationMatrixMove3);
    SetTranslation(3.3, 0.6, 0.7, TranslationMatrixMove4);
    SetTranslation(-3.3, 0.6, -0.7, TranslationMatrixMove5);
    
    /* Rotate and scale for the pigs */
    SetRotationX(90, RotationMatrixX);
    SetRotationY(90, RotationMatrixY2);
    SetRotationY(-90, RotationMatrixY3);
    SetRotationY(180, RotationMatrixY4);
    setScalingS(0.7, ScalingMatrix);
    
    /* Translate and scale for carousel*/
    setScalingS(2, ScalingMatrixCarousel);
    SetTranslation(0.0, -3.0, 0.0, TranslationMatrixMove);

	/* View changes */
	float viewRotationAngle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (180.0/M_PI);
	float RotationMatrixAnimView[16];
	float TranslationMatrixView[16];
	float RotationMatrixViewX[16];
	
	if (rotationMode == clockwise){
		viewRotationAngle = -viewRotationAngle;
	}
	if (cameraMode == Mode1){
		SetRotationY(viewRotationAngle/rotationSpeed, RotationMatrixAnimView);
		SetTranslation(0.0, -10.0, -12.0, TranslationMatrixView);
		SetRotationX(45, RotationMatrixViewX);
		SetTranslation(0.0, 0.0, 0.0, ViewMatrix);
		MultiplyMatrix(RotationMatrixAnimView, ViewMatrix, ViewMatrix);
		MultiplyMatrix(TranslationMatrixView, ViewMatrix, ViewMatrix);
		MultiplyMatrix(RotationMatrixViewX, ViewMatrix, ViewMatrix);
	}
    
    /* Full view control  */
    if (cameraMode == Mode2){
		SetRotationY(rotate_y * (180.0/M_PI), RotationMatrixAnimView);
		SetRotationX(rotate_x * (180.0/M_PI), RotationMatrixViewX);
		SetTranslation(camera_x, camera_y, -15.0 + camera_z, TranslationMatrixView);
		SetTranslation(0.0, 0.0, 0.0, ViewMatrix);
		MultiplyMatrix(TranslationMatrixView, ViewMatrix, ViewMatrix);
		MultiplyMatrix(RotationMatrixAnimView, ViewMatrix, ViewMatrix);
		MultiplyMatrix(RotationMatrixViewX, ViewMatrix, ViewMatrix);
	}

    /* Apply carousel rotation and move carousel down */
    MultiplyMatrix(RotationMatrixAnim ,InitialTransform, ModelMatrix);
    MultiplyMatrix(ScalingMatrixCarousel, ModelMatrix, ModelMatrix);
    MultiplyMatrix(TranslationMatrixMove, ModelMatrix, ModelMatrix);
    MultiplyMatrix(TranslateDown, ModelMatrix, ModelMatrix);
    
    
    /* Room */   
    MultiplyMatrix(RotationMatrixX, InitialTransform, Model6Matrix);
	MultiplyMatrix(TranslateDown, Model6Matrix, Model6Matrix);
    
       
    /* Applay Transformation on the pigs */
    /* Rotate pigs on X */
    MultiplyMatrix(RotationMatrixX, InitialTransform, Model2Matrix);
    MultiplyMatrix(RotationMatrixX, InitialTransform, Model3Matrix);
    MultiplyMatrix(RotationMatrixX, InitialTransform, Model4Matrix);
    MultiplyMatrix(RotationMatrixX, InitialTransform, Model5Matrix);
    /* Rotate pigs on Y */
    MultiplyMatrix(RotationMatrixY2, Model2Matrix, Model2Matrix);
    MultiplyMatrix(RotationMatrixY3, Model3Matrix, Model3Matrix);
    MultiplyMatrix(RotationMatrixY4, Model4Matrix, Model4Matrix);
    /* Scale pigs */
    MultiplyMatrix(ScalingMatrix, Model2Matrix, Model2Matrix);
    MultiplyMatrix(ScalingMatrix, Model3Matrix, Model3Matrix);
    MultiplyMatrix(ScalingMatrix, Model4Matrix, Model4Matrix);
    MultiplyMatrix(ScalingMatrix, Model5Matrix, Model5Matrix);
    /* Move pigs */
    MultiplyMatrix(TranslationMatrixMove2, Model2Matrix, Model2Matrix);
    MultiplyMatrix(TranslationMatrixMove3, Model3Matrix, Model3Matrix);
    MultiplyMatrix(TranslationMatrixMove4, Model4Matrix, Model4Matrix);
    MultiplyMatrix(TranslationMatrixMove5, Model5Matrix, Model5Matrix);
    /* Apply rotation and sliding animation on pigs */
    MultiplyMatrix(RotationMatrixAnim, Model2Matrix, Model2Matrix);
    MultiplyMatrix(RotationMatrixAnim, Model3Matrix, Model3Matrix);
    MultiplyMatrix(RotationMatrixAnim, Model4Matrix, Model4Matrix);
    MultiplyMatrix(RotationMatrixAnim, Model5Matrix, Model5Matrix);
    
    MultiplyMatrix(TranslationMatrixSlide, Model2Matrix, Model2Matrix);
    MultiplyMatrix(TranslationMatrixSlide, Model3Matrix, Model3Matrix);
    MultiplyMatrix(TranslationMatrixSlide, Model4Matrix, Model4Matrix);
    MultiplyMatrix(TranslationMatrixSlide, Model5Matrix, Model5Matrix);
    /* Translate pigs to final position */
    MultiplyMatrix(TranslateDown, Model2Matrix, Model2Matrix);
    MultiplyMatrix(TranslateDown, Model3Matrix, Model3Matrix);
    MultiplyMatrix(TranslateDown, Model4Matrix, Model4Matrix);
    MultiplyMatrix(TranslateDown, Model5Matrix, Model5Matrix);    
    
    setScalingS(0.08, ScalingMatrix);    
    SetRotationX(90, RotationMatrixX2);
    SetTranslation(5.0,0.0,-2.5, TranslationMatrixMove7);
      
    MultiplyMatrix(RotationMatrixX2, InitialTransform, Model7Matrix); 
    MultiplyMatrix(ScalingMatrix, Model7Matrix, Model7Matrix); 
    MultiplyMatrix(TranslateDown, Model7Matrix, Model7Matrix);
    MultiplyMatrix(TranslateDown, Model7Matrix, Model7Matrix);
    MultiplyMatrix(TranslationMatrixMove7, Model7Matrix, Model7Matrix); 
    
	
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
void setupDaterBufferObject(buffer_object* bo){
	glGenBuffers(1, &bo->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, bo->VBO);
    glBufferData(GL_ARRAY_BUFFER, data.vertex_count*3*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW);  
    
    glGenBuffers(1, &bo->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.face_count*3*sizeof(GLushort), index_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &bo->CBO);
    glBindBuffer(GL_ARRAY_BUFFER, bo->CBO);
    glBufferData(GL_ARRAY_BUFFER, data.face_count*3*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &bo->VN);
	glBindBuffer(GL_ARRAY_BUFFER, bo->VN);
	glBufferData(GL_ARRAY_BUFFER, data.vertex_count*3*sizeof(GLfloat), vertex_normals, GL_STATIC_DRAW);

}

void SetupDataBuffers(){
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
	
	/* Carousel */
    setupDaterBufferObject(carousel);

	/* Room */
	glGenBuffers(1, &VBO6);
    glBindBuffer(GL_ARRAY_BUFFER, VBO6);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data2), vertex_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO6);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO6);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data2), index_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &CBO6);
    glBindBuffer(GL_ARRAY_BUFFER, CBO6);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data2), color_buffer_data2, GL_STATIC_DRAW);
    
    /* Lamp */	//==============================================================================================================
    glGenBuffers(1, &VBO7);
    glBindBuffer(GL_ARRAY_BUFFER, VBO7);
    glBufferData(GL_ARRAY_BUFFER, data4.vertex_count*3*sizeof(GLfloat), vertex_buffer_data4, GL_STATIC_DRAW);    

    glGenBuffers(1, &IBO7);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO7);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data4.face_count*3*sizeof(GLushort), index_buffer_data4, GL_STATIC_DRAW);   

    glGenBuffers(1, &CBO7);
    glBindBuffer(GL_ARRAY_BUFFER, CBO7);    
    glBufferData(GL_ARRAY_BUFFER, data4.face_count*3*sizeof(GLfloat), color_buffer_data4, GL_STATIC_DRAW);

	//============================================================================================================================

	/* All four pigs */
	glGenBuffers(1, &pig1->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, pig1->VBO);
    glBufferData(GL_ARRAY_BUFFER, data3.vertex_count*3*sizeof(GLfloat), vertex_buffer_data3, GL_STATIC_DRAW);  
    
    glGenBuffers(1, &pig1->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pig1->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data3.face_count*3*sizeof(GLushort), index_buffer_data3, GL_STATIC_DRAW);
	
	glGenBuffers(1, &pig1->CBO);
    glBindBuffer(GL_ARRAY_BUFFER, pig1->CBO);
    glBufferData(GL_ARRAY_BUFFER, data3.face_count*3*sizeof(GLfloat), color_buffer_data3, GL_STATIC_DRAW);
	
	glGenBuffers(1, &pig1->VN);
	glBindBuffer(GL_ARRAY_BUFFER, pig1->VN);
	glBufferData(GL_ARRAY_BUFFER, data.vertex_count*3*sizeof(GLfloat), vertex_normals, GL_STATIC_DRAW);
	
	
	glGenBuffers(1, &pig2->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, pig2->VBO);
    glBufferData(GL_ARRAY_BUFFER, data3.vertex_count*3*sizeof(GLfloat), vertex_buffer_data3, GL_STATIC_DRAW);  
    
    glGenBuffers(1, &pig2->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pig2->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data3.face_count*3*sizeof(GLushort), index_buffer_data3, GL_STATIC_DRAW);

	glGenBuffers(1, &pig3->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, pig3->VBO);
    glBufferData(GL_ARRAY_BUFFER, data3.vertex_count*3*sizeof(GLfloat), vertex_buffer_data3, GL_STATIC_DRAW);  
    
    glGenBuffers(1, &pig3->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pig3->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data3.face_count*3*sizeof(GLushort), index_buffer_data3, GL_STATIC_DRAW);

	glGenBuffers(1, &pig4->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, pig4->VBO);
    glBufferData(GL_ARRAY_BUFFER, data3.vertex_count*3*sizeof(GLfloat), vertex_buffer_data3, GL_STATIC_DRAW);  
    
    glGenBuffers(1, &pig4->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pig4->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data3.face_count*3*sizeof(GLushort), index_buffer_data3, GL_STATIC_DRAW);

}


/******************************************************************
*
* AddShader
*
* This function creates and adds individual shaders
*
*******************************************************************/

void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType){
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

void CreateShaderProgram(){
    /* Allocate shader object */
    ShaderProgram = glCreateProgram();
    ShaderProgram2 = glCreateProgram();
    
    if (ShaderProgram == 0) {
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

    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Check if shader program can be executed */ 
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);

    if (!Success) {
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
* This function is called to initialize rendering elements, load obj objects,
* setup vertex buffer objects, and to setup the vertex and fragment shader
*
*******************************************************************/

void Initialize(void){   
	carousel = calloc(1, sizeof(struct buffer_object));
	pig1 = calloc(1, sizeof(struct buffer_object));
	pig2 = calloc(1, sizeof(struct buffer_object));
	pig3 = calloc(1, sizeof(struct buffer_object));
	pig4 = calloc(1, sizeof(struct buffer_object));
	
	int i;
    int success;

	/* Load carousel OBJ model */
    char* filename1 = "models/carousel.obj"; 
    success = parse_obj_scene(&data, filename1);

    if(!success)
        printf("Could not load file carousel. Exiting.\n");
        
    /*  Copy mesh data from structs into appropriate arrays */ 
    int vert = data.vertex_count;
    int indx = data.face_count;
    vertex_buffer_data = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    color_buffer_data = (GLfloat*) calloc (indx*3, sizeof(GLfloat));
    index_buffer_data = (GLushort*) calloc (indx*3, sizeof(GLushort));
    /* Vertices */
    for(i=0; i<vert; i++){
        vertex_buffer_data[i*3] = (GLfloat)(*data.vertex_list[i]).e[0];        
		vertex_buffer_data[i*3+1] = (GLfloat)(*data.vertex_list[i]).e[1];
		vertex_buffer_data[i*3+2] = (GLfloat)(*data.vertex_list[i]).e[2];
    }
    /* Colors */
    for(i=0; i<indx; i++){
		color_buffer_data[i*3] = 1.0 /*(rand() % 100) / 100.0*/;
		color_buffer_data[i*3+1] = 0.0 /*(rand() % 100) / 100.0*/;
		color_buffer_data[i*3+2] = 0.0 /*(rand() % 100) / 100.0*/;
    }
    /* Indices */
    for(i=0; i<indx; i++){
		index_buffer_data[i*3] = (GLushort)(*data.face_list[i]).vertex_index[0];
		index_buffer_data[i*3+1] = (GLushort)(*data.face_list[i]).vertex_index[1];
		index_buffer_data[i*3+2] = (GLushort)(*data.face_list[i]).vertex_index[2];
    }
    
    vertex_normals = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    vertex_normals = vertex_buffer_data;
	
	/*Load lamp OBJ model */
	 char* filename3 = "models/3d-model.obj"; 												/*============================================================*/
	success = parse_obj_scene(&data4, filename3);
	
	if(!success)
        printf("Could not load file lamp. Exiting.\n");
        
    /*  Copy mesh data from structs into appropriate arrays */ 

    indx = data4.face_count;
    vert = data4.vertex_count;
    vertex_buffer_data4 = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    color_buffer_data4 = (GLfloat*) calloc (indx*3, sizeof(GLfloat));
    index_buffer_data4 = (GLushort*) calloc (indx*3, sizeof(GLushort));
    /* Vertices */
    for(i=0; i<vert; i++){
        vertex_buffer_data4[i*3] = (GLfloat)(*data4.vertex_list[i]).e[0];
		vertex_buffer_data4[i*3+1] = (GLfloat)(*data4.vertex_list[i]).e[1];
		vertex_buffer_data4[i*3+2] = (GLfloat)(*data4.vertex_list[i]).e[2];
		
    }
    /* Colors */
    for(i=0; i<indx; i++){
		color_buffer_data4[i*3] = 0.36;//(rand() % 100) / 100.0;
		color_buffer_data4[i*3+1] = 0.7;//(rand() % 100) / 100.0;
		color_buffer_data4[i*3+2] = 0.3;//(rand() % 100) / 100.0;
    }    
    
    /* Indices */
    for(i=0; i<indx; i++){
		index_buffer_data4[i*3] = (GLushort)(*data4.face_list[i]).vertex_index[0];
		index_buffer_data4[i*3+1] = (GLushort)(*data4.face_list[i]).vertex_index[1];
		index_buffer_data4[i*3+2] = (GLushort)(*data4.face_list[i]).vertex_index[2];
    }
    
    //===================================================================================================
	 
	
	/* Load pig OBJ model */
    char* filename2 = "models/pig.obj"; 
    success = parse_obj_scene(&data3, filename2);

    if(!success)
        printf("Could not load file pig. Exiting.\n");
        
    /*  Copy mesh data from structs into appropriate arrays */ 
    vert = data3.vertex_count;
    indx = data3.face_count;
    vertex_buffer_data3 = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    color_buffer_data3 = (GLfloat*) calloc (indx*3, sizeof(GLfloat));
    index_buffer_data3 = (GLushort*) calloc (indx*3, sizeof(GLushort));
    /* Vertices */
    for(i=0; i<vert; i++){
        vertex_buffer_data3[i*3] = (GLfloat)(*data3.vertex_list[i]).e[0];
		vertex_buffer_data3[i*3+1] = (GLfloat)(*data3.vertex_list[i]).e[1];
		vertex_buffer_data3[i*3+2] = (GLfloat)(*data3.vertex_list[i]).e[2];
    }
    /* Colors */
    for(i=0; i<indx; i++){
		color_buffer_data3[i*3] = 0.0 /*(rand() % 100) / 100.0*/;
		color_buffer_data3[i*3+1] = 1.0 /*(rand() % 100) / 100.0*/;
		color_buffer_data3[i*3+2] = 0.0 /*(rand() % 100) / 100.0*/;
    }
    /* Indices */
    for(i=0; i<indx; i++){
		index_buffer_data3[i*3] = (GLushort)(*data3.face_list[i]).vertex_index[0];
		index_buffer_data3[i*3+1] = (GLushort)(*data3.face_list[i]).vertex_index[1];
		index_buffer_data3[i*3+2] = (GLushort)(*data3.face_list[i]).vertex_index[2];
    }
    
    
	
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
    SetIdentityMatrix(Model6Matrix);
    //=============================================================================================
    SetIdentityMatrix(Model7Matrix);
    //===============================================================================================

    /* Set projection transform */
    float fovy = 45.0;
    float aspect = 1.0; 
    float nearPlane = 0.25; 
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
    
    /*
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	*/
	
}


/******************************************************************
*
* main
*
* Main function to setup GLUT, GLEW, and enter rendering loop
*
*******************************************************************/

int main(int argc, char** argv){
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
    if (res != GLEW_OK){
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    /* Setup scene and rendering parameters */
    Initialize();


    /* Specify callback functions;enter GLUT event processing loop, 
     * handing control over to GLUT */
    glutIdleFunc(OnIdle);
    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard); 
    glutMouseFunc(Mouse);
    glutMotionFunc(mouseMotion);
    glutMainLoop();

    /* ISO C requires main to return int */
    return 0;
}

