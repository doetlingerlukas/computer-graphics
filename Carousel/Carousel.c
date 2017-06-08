/******************************************************************
*
* Carousel.c
*
* by Davide De Sclavis, Manuel Buchauer und Lukas Dötlinger
*
* The Carousel.c displays a Carousel with some objects
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
#include "LoadTexture.h"  
#include "Matrix.h"
#include "Setup.h"
#include "OBJParser.h"

/*----------------------------------------------------------------*/
GLuint VAO;

/* Flag for starting/stopping animation */
GLboolean anim = GL_TRUE;

/* Reference time for animation */
int oldTime = 0;

/* Buffer object structs for all models */
buffer_object* carousel;
buffer_object* room;
buffer_object* pig1;
buffer_object* pig2;
buffer_object* pig3;
buffer_object* pig4;
buffer_object* lamp1;
buffer_object* lamp2;

/* Indices to vertex attributes */ 
enum DataID {vPosition = 0, vColor = 1, vNormal = 2, vUV = 3}; 

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

/* Shader1: Phong shader */
GLuint ShaderProgram; 

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */ 

float ModelMatrix[16]; /* carousel matrix */ 
float Model2Matrix[16]; /* pig 1 matrix */
float Model3Matrix[16]; /* pig 2 matrix */
float Model4Matrix[16]; /* pig 3 matrix */
float Model5Matrix[16]; /* pig 4 matrix */
float Model6Matrix[16]; /* Room matrix */
float Model7Matrix[16]; //For lamp
float Model8Matrix[16]; /*for 2nd lamp*/

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
/* Indices for different lightning modes */
enum {lmode1=1, lmode2=2};
int lightMode = lmode1;

/* Buffers for Carousel */
buffer_data* carousel_data;
obj_scene_data data_c;

/* Buffer for the pigs */
buffer_data* pig_data;
obj_scene_data data_p;

/*buffer for the lamps*/
buffer_data* lamp_data;
obj_scene_data data_l;

/*buffer for the room*/
buffer_data* room_data;
obj_scene_data data_r;


/* Mouse and keyboard motion */
float camera_z = 0;
float camera_y = 0;
float camera_x = 0;
float rotate_x = 0;
float rotate_y = 0;
int mouse_oldx;
int mouse_oldy;

/*colour values*/
int hsv_h=0;
float hsv_v=0.02;
float hsv_s=1.0;

float rgb_r=1.0;
float rgb_g=0.1;
float rgb_b=0.1;

/* light sources */
float LightPosition1[] = { -6.0, 3.0, 3.0 };
float LightColor1[] = { 1.0, 0.1, 0.1 };
float LightPosition2[] = { 6.0, 3.0, 3.0};
float LightColor2[] ={ 0.0, 0.5, 0.5};

float ambientFactor = 0.2f;
float diffuseFactor = 1.0f;
float specularFactor = 1.5F;

int ambientToggle = 1;
int diffuseToggle = 1;
int specularToggle = 1;
int light1Toggle = 1;
int light2Toggle= 1;

/* Variables for texture handling */
GLuint TextureID;
GLuint TextureUniform;
TextureDataPtr Texture;

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
       
    
	/** Texturing **/
	/* Activate first (and only) texture unit */
    glActiveTexture(GL_TEXTURE0);

    /* Bind current texture  */
    glBindTexture(GL_TEXTURE_2D, TextureID);
    
    /* Get texture uniform handle from fragment shader */ 
    TextureUniform  = glGetUniformLocation(ShaderProgram, "myTextureSampler");

    /* Set location of uniform sampler variable */ 
    glUniform1i(TextureUniform, 0);
    
    /** Carousel **/
    setupAndDraw(carousel, ShaderProgram, ModelMatrix);
    
    /** Room **/
    setupAndDraw(room, ShaderProgram, Model6Matrix);
    
    /** Pigs **/
	setupAndDraw(pig1, ShaderProgram, Model2Matrix);
	setupAndDraw(pig2, ShaderProgram, Model3Matrix);
	setupAndDraw(pig3, ShaderProgram, Model4Matrix);
	setupAndDraw(pig4, ShaderProgram, Model5Matrix);
	
	 /**LAMP **/
    setupAndDraw(lamp1, ShaderProgram, Model7Matrix);
    setupAndDraw(lamp2, ShaderProgram, Model8Matrix);
	
	/** Light sources **/
	GLint LightPos1Uniform = glGetUniformLocation(ShaderProgram, "LightPosition1");
	glUniform3f(LightPos1Uniform, LightPosition1[0], LightPosition1[1], LightPosition1[2]);
	
	GLint LightCol1Uniform = glGetUniformLocation(ShaderProgram, "LightColor1");
	glUniform3f(LightCol1Uniform, LightColor1[0]*light1Toggle, LightColor1[1]*light1Toggle, LightColor1[2]*light1Toggle);
	
	GLint LightPos2Uniform = glGetUniformLocation(ShaderProgram, "LightPosition2");
	glUniform3f(LightPos2Uniform, LightPosition2[0], LightPosition2[1], LightPosition2[2]);
	
	GLint LightCol2Uniform = glGetUniformLocation(ShaderProgram, "LightColor2");
	glUniform3f(LightCol2Uniform, LightColor2[0]*light2Toggle, LightColor2[1]*light2Toggle, LightColor2[2]*light2Toggle);
	
	GLint AmbientFactorUniform = glGetUniformLocation(ShaderProgram, "AmbientFactor");
	glUniform1f(AmbientFactorUniform, ambientFactor * ambientToggle);
	
	GLint DiffuseFactorUniform = glGetUniformLocation(ShaderProgram, "DiffuseFactor");
	glUniform1f(DiffuseFactorUniform, diffuseFactor * diffuseToggle);
	
	GLint SpecularFactorUniform = glGetUniformLocation(ShaderProgram, "SpecularFactor");
	glUniform1f(SpecularFactorUniform, specularFactor * specularToggle);
	
	GLint viewPosLoc = glGetUniformLocation(ShaderProgram, "viewPos");
	glUniform3f(viewPosLoc, camera_x, camera_y, camera_z);
    
	
	/* Only draw lines */
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
* hsv2RGB
* Function called to change hsv values into rgb values
*
*******************************************************************/

void hsv2rgb(){ 
	float c= hsv_v * hsv_s;
	float x= c * (1-fabs((hsv_h /60) % 2 - 1));
	float m= hsv_v - c;

	float rtemp=0.0;
	float gtemp=0.0;
	float btemp=0.0;

	if((0<= hsv_h) && (hsv_h< 60)){
		rtemp=c;
		gtemp=x;
		btemp=0.0;
	} 
	else if((60<= hsv_h) && (hsv_h< 120)){
		rtemp=x;
		gtemp=c;
		btemp=0.0;
	}
	else if((120<= hsv_h) && (hsv_h< 180)){
		rtemp=0.0;
		gtemp=c;
		btemp=x;
	}
	else if((180<= hsv_h) && (hsv_h< 240)){
		rtemp=0.0;
		gtemp=x;
		btemp=c;
	}
	else if((240<= hsv_h) && (hsv_h< 300)){
		rtemp=x;
		gtemp=0.0;
		btemp=c;
	}
	else if((300<= hsv_h) && (hsv_h< 360)){
		rtemp=c;
		gtemp=0.0;
		btemp=x;
	}

	rgb_r= (rtemp+m) *255;
	rgb_g= (gtemp+m) *255;
	rgb_b= (btemp+m) *255;
	LightColor1[0]= rgb_r;
	LightColor1[1]= rgb_g;
	LightColor1[2]= rgb_b;

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

	/* Switch light mode */
	case 'l':
		if(lightMode == 1){
			lightMode = 2;
			light2Toggle = 0;
		}else{
			lightMode = 1;
			LightPosition1[0] = -6.0;
			LightPosition1[1] = 3.0;
			LightPosition1[2] = 3.0;
			light2Toggle = 1;
		}
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

	/*change hue and value of the light*/
 	case '6':
		
		if ((hsv_h -= 5) < 0){
			hsv_h=360;
		}
		printf("Current hsv_h value: %d \n" , hsv_h);
		hsv2rgb();
		break;
	case '7':
		if ((hsv_h += 5)>360){
			hsv_h=0;
		}
		printf("Current hsv_h value: %d\n" , hsv_h);
		hsv2rgb();
		break;
	case '8':
		hsv_v -= 0.001;
		if (hsv_v < 0.0){
			hsv_v=0.025;
		}
		printf("Current hsv_v value: %f\n" , hsv_v);
		hsv2rgb();
		break;
	case '9':
		hsv_v += 0.001;
		if (hsv_v > 0.025){
			hsv_v=0.001;
		}
		printf("Current hsv_v value: %f\n" , hsv_v);
		hsv2rgb();
		break;
		
	/* Toggle lightning */
	case 'u':
		if(ambientToggle ==1)
			ambientToggle = 0;
		else 
			ambientToggle = 1;
		break;
	case 'i':
		if(diffuseToggle ==1)
			diffuseToggle = 0;
		else 
			diffuseToggle = 1;
		break;
	case 'o':
		if(specularToggle ==1)
			specularToggle = 0;
		else 
			specularToggle = 1;
		break;
	case 'y':
		if(light1Toggle ==1)
			light1Toggle = 0;
		else 
			light1Toggle = 1;
		break;
	case 'x':
		if(light2Toggle ==1)
			light2Toggle = 0;
		else 
			light2Toggle = 1;
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
* Yes, we know that this function is very messy and ugly, but
* it will be improved, structured and shortened very shortly!
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
    float TranslationMatrixMove8[16];
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
    

    /* lamp */
    setScalingS(0.08, ScalingMatrix);    
    SetRotationX(90, RotationMatrixX2);
    SetTranslation(6.0,0.0,3.0, TranslationMatrixMove7);
    SetTranslation(-6.0,0.0,3.0, TranslationMatrixMove8);  
    /* rotate lamps*/
    MultiplyMatrix(RotationMatrixX2, InitialTransform, Model7Matrix); 
    MultiplyMatrix(RotationMatrixX2, InitialTransform, Model8Matrix); 
    /*scale lamps*/
    MultiplyMatrix(ScalingMatrix, Model7Matrix, Model7Matrix); 
    MultiplyMatrix(ScalingMatrix, Model8Matrix, Model8Matrix); 
    /*translate lamps*/
    MultiplyMatrix(TranslateDown, Model7Matrix, Model7Matrix);
    MultiplyMatrix(TranslateDown, Model7Matrix, Model7Matrix);
    MultiplyMatrix(TranslateDown, Model8Matrix, Model8Matrix);
    MultiplyMatrix(TranslateDown, Model8Matrix, Model8Matrix);
    MultiplyMatrix(TranslationMatrixMove7, Model7Matrix, Model7Matrix); 
    MultiplyMatrix(TranslationMatrixMove8, Model8Matrix, Model8Matrix); 
    
    /* rotating light source */
	if(lightMode == 2){
		float lp[] = { 1.0, 1.0, 1.0 };
		multiplyMatrixWithVector(ModelMatrix, lp);
		LightPosition1[0] = lp[0];
		LightPosition1[1] = lp[1];
		LightPosition1[2] = lp[2];
	} 
	
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
void setupDataBufferObject(buffer_object* bo, buffer_data* bd, obj_scene_data d){
	glGenBuffers(1, &bo->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, bo->VBO);
    glBufferData(GL_ARRAY_BUFFER, d.vertex_count*3*sizeof(GLfloat), bd->vertex_buffer_data, GL_STATIC_DRAW);  
    
    glGenBuffers(1, &bo->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, d.face_count*3*sizeof(GLushort), bd->index_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &bo->CBO);
    glBindBuffer(GL_ARRAY_BUFFER, bo->CBO);
    glBufferData(GL_ARRAY_BUFFER, d.face_count*3*sizeof(GLfloat), bd->color_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &bo->VN);
	glBindBuffer(GL_ARRAY_BUFFER, bo->VN);
	glBufferData(GL_ARRAY_BUFFER, d.vertex_count*3*sizeof(GLfloat), bd->vertex_normals, GL_STATIC_DRAW);
	
	glGenBuffers(1, &bo->VT);
	glBindBuffer(GL_ARRAY_BUFFER, bo->VT);
	glBufferData(GL_ARRAY_BUFFER, d.vertex_texture_count*2*sizeof(GLfloat), bd->vertex_textures, GL_STATIC_DRAW);

}

void SetupDataBuffers(){
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
	
	/* Carousel */
    setupDataBufferObject(carousel, carousel_data, data_c);

	/* Room */
	setupDataBufferObject(room, room_data, data_r);
    
    /* Lamp */	
    setupDataBufferObject(lamp1, lamp_data, data_l);
    setupDataBufferObject(lamp2, lamp_data, data_l);
    
	/* All four pigs */
	setupDataBufferObject(pig1, pig_data, data_p);
	setupDataBufferObject(pig2, pig_data, data_p);
	setupDataBufferObject(pig3, pig_data, data_p);
	setupDataBufferObject(pig4, pig_data, data_p);
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

    GLint Success = 0;
    GLchar ErrorLog[1024];

    /* Link shader code into executable shader program */
    glLinkProgram(ShaderProgram);

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
}

/******************************************************************
*
* SetupTexture
*
* This function is called to load the texture and initialize
* texturing parameters
*
*******************************************************************/

void SetupTexture(void)
{	
    /* Allocate texture container */
    Texture = malloc(sizeof(TextureDataPtr));

    int success = LoadTexture("textures/wall.bmp", Texture);
    if (!success)
    {
        printf("Error loading texture. Exiting.\n");
	exit(-1);
    }

    /* Create texture name and store in handle */
    glGenTextures(1, &TextureID);
	
    /* Bind texture */
    glBindTexture(GL_TEXTURE_2D, TextureID);

    /* Load texture image into memory */
    glTexImage2D(GL_TEXTURE_2D,     /* Target texture */
		 0,                 /* Base level */
		 GL_RGB,            /* Each element is RGB triple */ 
		 Texture->width,    /* Texture dimensions */ 
            Texture->height, 
		 0,                 /* Border should be zero */
		 GL_BGR,            /* Data storage format for BMP file */
		 GL_UNSIGNED_BYTE,  /* Type of pixel data, one byte per channel */
		 Texture->data);    /* Pointer to image data  */
 
    /* Next set up texturing parameters */

    /* Repeat texture on edges when tiling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* Linear interpolation for magnification */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Trilinear MIP mapping for minification */ 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glGenerateMipmap(GL_TEXTURE_2D); 

    /* Note: MIP mapping not visible due to fixed, i.e. static camera */
}



/******************************************************************
*
* Initialize
*
* This function is called to initialize rendering elements, load obj objects,
* setup vertex buffer objects, and to setup the vertex and fragment shader
*
*******************************************************************/

obj_scene_data setupObj(char* file, buffer_data* bd, rgb color){
	obj_scene_data d;
	int success = parse_obj_scene(&d, file);
    if(!success)
        printf("Could not load file. Exiting.\n");
    int i;
    
    /*  Copy mesh data from structs into appropriate arrays */ 
    int vert = d.vertex_count;
    int indx = d.face_count;
    int texc = d.vertex_texture_count;
    
    bd->vertex_buffer_data = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    bd->vertex_normals = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    bd->vertex_textures = (GLfloat*) calloc (texc*2, sizeof(GLfloat));
    bd->color_buffer_data = (GLfloat*) calloc (indx*3, sizeof(GLfloat));
    bd->index_buffer_data = (GLushort*) calloc (indx*3, sizeof(GLushort));
    
    /* Vertices */
    for(i=0; i<vert; i++){
        bd->vertex_buffer_data[i*3] = (GLfloat)(*d.vertex_list[i]).e[0];
		bd->vertex_buffer_data[i*3+1] = (GLfloat)(*d.vertex_list[i]).e[1];
		bd->vertex_buffer_data[i*3+2] = (GLfloat)(*d.vertex_list[i]).e[2];
    }
    /* Colors */
    for(i=0; i<indx; i++){
		bd->color_buffer_data[i*3] = color.r != -1.0 ? color.r : (rand() % 100) / 100.0;
		bd->color_buffer_data[i*3+1] = color.g != -1.0 ? color.g : (rand() % 100) / 100.0;
		bd->color_buffer_data[i*3+2] = color.b != -1.0 ? color.b : (rand() % 100) / 100.0;
    }
    /* Indices */
    for(i=0; i<indx; i++){
		bd->index_buffer_data[i*3] = (GLushort)(*d.face_list[i]).vertex_index[0];
		bd->index_buffer_data[i*3+1] = (GLushort)(*d.face_list[i]).vertex_index[1];
		bd->index_buffer_data[i*3+2] = (GLushort)(*d.face_list[i]).vertex_index[2];
    }
    /* Normals */
    bd->vertex_normals = calcVertexNormals(d, bd);
    
    /* Textures */
    for(i=0; i<texc; i++){
		bd->vertex_textures[i*2] = (GLfloat)(*d.vertex_texture_list[i]).e[0];
		bd->vertex_textures[i*2+1] = (GLfloat)(*d.vertex_texture_list[i]).e[1];
	}
    
    return d;
}

void Initialize(void){   
	carousel = calloc(1, sizeof(struct buffer_object));
	room = calloc(1, sizeof(struct buffer_object));
	pig1 = calloc(1, sizeof(struct buffer_object));
	pig2 = calloc(1, sizeof(struct buffer_object));
	pig3 = calloc(1, sizeof(struct buffer_object));
	pig4 = calloc(1, sizeof(struct buffer_object));
	lamp1 = calloc(1, sizeof(struct buffer_object));
	lamp2 = calloc(1, sizeof(struct buffer_object));
	
	carousel_data = calloc(1, sizeof(struct buffer_data));
	pig_data = calloc(1, sizeof(struct buffer_data));
	lamp_data = calloc(1, sizeof(struct buffer_data));
	room_data = calloc(1, sizeof(struct buffer_data));
	
	    
    /*  Copy mesh data from structs into appropriate arrays */ 

	/* Load carousel OBJ model */
    char* filename1 = "models/carousel.obj"; 
    rgb colors1 = {1.0, 1.0, 1.0};
    data_c = setupObj(filename1, carousel_data, colors1);

	/* Load pig OBJ model */
    char* filename2 = "models/pig.obj"; 
    rgb colors2 = {1.0, 1.0, 1.0};
    data_p = setupObj(filename2, pig_data, colors2);
    
    /*Load lamp OBJ model */
	char* filename3 = "models/3d-model.obj"; 
	rgb colors3 = {1.0, 1.0, 1.0};
	data_l = setupObj(filename3, lamp_data, colors3);
	
	/*Load lamp OBJ model */
	char* filename4 = "models/room.obj"; 
	rgb colors4 = {1.0, 1.0, 1.0};
	data_r = setupObj(filename4, room_data, colors4);

	
    /* Set background (clear) color to dark blue */ 
    glClearColor(0.0, 0.0, 0.8, 0.0);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);    

    /* Setup vertex, color, and index buffer objects */
    SetupDataBuffers();

    /* Setup shaders and shader program */
    CreateShaderProgram();  

	/* Setup textures */
	SetupTexture();

    /* Initialize matrices */
    SetIdentityMatrix(ProjectionMatrix);
    SetIdentityMatrix(ViewMatrix);
    SetIdentityMatrix(ModelMatrix);
    
    SetIdentityMatrix(Model2Matrix);
    SetIdentityMatrix(Model3Matrix);
    SetIdentityMatrix(Model4Matrix);
    SetIdentityMatrix(Model5Matrix);
    SetIdentityMatrix(Model6Matrix);
    SetIdentityMatrix(Model7Matrix);
    SetIdentityMatrix(Model8Matrix);
    
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
    
    
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	
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

