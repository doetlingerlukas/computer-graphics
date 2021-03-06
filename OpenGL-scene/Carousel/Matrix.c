/******************************************************************
*
* Matrix.c
*
* Description: Helper routine for matrix computations.
* 	
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

/******************************************************************
*
* print function for classic printf debugging
* Not working on ZID PCs
*
*******************************************************************/
/*
void print_matrix(float* matrix) 
{
	printf("[\n");
	for (int i = 0; i < 4; i++) {
		printf("  [%8f,\t%8f,\t%8f,\t%8f],\n", matrix[i*4], matrix[i*4 + 1], matrix[i*4 + 2], matrix[i*4 + 3]);
	}
	printf("]\n");
}
*/

/******************************************************************
*
* SetBillboardMatrix
*
*******************************************************************/

void SetBillboardMatrix(float* m, float* v, float* result)  
{
    float matrix[16] = {
		v[1], v[5], v[9], 0.0f,
		v[2], v[6], v[10], 0.0f,
		v[3], v[7], v[11], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
    };
	
	float inversed[16];
	SetInverse(matrix, inversed);

    memcpy(result, inversed, 16*sizeof(float));
}

/******************************************************************
*
* SetIdentityMatrix
*
*******************************************************************/

void SetIdentityMatrix(float* result)  
{
    float identity[16] = {
    1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
    };

    memcpy(result, identity, 16*sizeof(float));
}


/******************************************************************
*
* SetScalingS
*
*******************************************************************/

void setScalingS(float factor, float* result)
{
	float temp[16] = 
    {  
		factor,        0.0,          0.0, 	0.0,
	0.0,  	    factor,			 0.0, 		0.0,
	0.0,  		   0.0,       factor,		0.0,
	0.0,           0.0,          0.0, 		1.0  
    };

    memcpy(result, temp, 16*sizeof(float));
}

/******************************************************************
*
* SetRotationX
*
*******************************************************************/

void SetRotationX(float anglex, float* result)
{
    anglex = M_PI/180 * anglex;   /* Conversion angle from degree to radians */

    float temp[16] = 
    {  
        1.0,           0.0,          0.0, 0.0,
	0.0,  cosf(anglex),-sinf(anglex), 0.0,
	0.0,  sinf(anglex), cosf(anglex), 0.0,
	0.0,           0.0,          0.0, 1.0  
    };

    memcpy(result, temp, 16*sizeof(float));
}


/******************************************************************
*
* SetRotationY
*
*******************************************************************/

void SetRotationY(float angley, float* result)
{
    angley = M_PI/180 * angley;   /* Conversion angle from degree to radians */

    float temp[16] = 
    {
        cosf(angley),  0.0,   sinf(angley),  0.0,
	         0.0,  1.0,            0.0,  0.0,
       -sinf(angley),  0.0,   cosf(angley),  0.0,
	         0.0,  0.0,            0.0,  1.0   
    };
  
    memcpy(result, temp, 16*sizeof(float));
}


/******************************************************************
*
* SetRotationZ
*
*******************************************************************/

void SetRotationZ(float anglez, float* result)
{
    anglez = M_PI/180 * anglez;   /* Conversion angle from degree to radian */

    float temp[16] = 
    {	
        cosf(anglez), -sinf(anglez),  0.0,  0.0,
        sinf(anglez),  cosf(anglez),  0.0,  0.0,          
                 0.0,           0.0,  1.0,  0.0,
                 0.0,           0.0,  0.0,  1.0   
    };

    memcpy(result, temp, 16*sizeof(float));
}


/******************************************************************
*
* SetTranslation
*
*******************************************************************/

void SetTranslation(float x, float y, float z, float* result)
{
  float temp[16] = 
  {
      1.0,  0.0,  0.0,    x,
      0.0,  1.0,  0.0,    y,
      0.0,  0.0,  1.0,    z,
      0.0,  0.0,  0.0,  1.0
  };

  memcpy(result, temp, 16*sizeof(float));
}


/******************************************************************
*
* MultiplyMatrix
*
*******************************************************************/

void MultiplyMatrix(float* m1, float* m2, float* result)
{
    int i;
    float temp[16];

    for (i = 0; i < 16; i++)
        temp[i] = 0.0;
    
    temp[0] = m1[0]*m2[0] + m1[1]*m2[4] + m1[2]*m2[8] + m1[3]*m2[12];
    temp[1] = m1[0]*m2[1] + m1[1]*m2[5] + m1[2]*m2[9] + m1[3]*m2[13];
    temp[2] = m1[0]*m2[2] + m1[1]*m2[6] + m1[2]*m2[10] + m1[3]*m2[14];
    temp[3] = m1[0]*m2[3] + m1[1]*m2[7] + m1[2]*m2[11] + m1[3]*m2[15];

    temp[4] = m1[4]*m2[0] + m1[5]*m2[4] + m1[6]*m2[8] + m1[7]*m2[12];
    temp[5] = m1[4]*m2[1] + m1[5]*m2[5] + m1[6]*m2[9] + m1[7]*m2[13];
    temp[6] = m1[4]*m2[2] + m1[5]*m2[6] + m1[6]*m2[10] + m1[7]*m2[14];
    temp[7] = m1[4]*m2[3] + m1[5]*m2[7] + m1[6]*m2[11] + m1[7]*m2[15];

    temp[8] = m1[8]*m2[0] + m1[9]*m2[4] + m1[10]*m2[8] + m1[11]*m2[12];
    temp[9] = m1[8]*m2[1] + m1[9]*m2[5] + m1[10]*m2[9] + m1[11]*m2[13];
    temp[10] = m1[8]*m2[2] + m1[9]*m2[6] + m1[10]*m2[10] + m1[11]*m2[14];
    temp[11] = m1[8]*m2[3] + m1[9]*m2[7] + m1[10]*m2[11] + m1[11]*m2[15];

    temp[12] = m1[12]*m2[0] + m1[13]*m2[4] + m1[14]*m2[8] + m1[15]*m2[12];
    temp[13] = m1[12]*m2[1] + m1[13]*m2[5] + m1[14]*m2[9] + m1[15]*m2[13];
    temp[14] = m1[12]*m2[2] + m1[13]*m2[6] + m1[14]*m2[10] + m1[15]*m2[14];
    temp[15] = m1[12]*m2[3] + m1[13]*m2[7] + m1[14]*m2[11] + m1[15]*m2[15];

    memcpy(result, temp, 16*sizeof(float));
}


/******************************************************************
*
* SetPerspectiveMatrix
*
*******************************************************************/

void SetPerspectiveMatrix(float fov, float aspect, float nearPlane, 
                          float farPlane, float* result) 
{   
    float f = 1.0 / tan(fov * 3.1415926/360.0);
    float c1 = -(farPlane + nearPlane) / (farPlane - nearPlane);
    float c2 = -(2.0 * farPlane * nearPlane) / (farPlane - nearPlane);

    float temp[16] = 
    {
        f/aspect,  0.0,   0.0,  0.0,
	     0.0,    f,   0.0,  0.0,
	     0.0,  0.0,    c1,   c2,
         0.0,  0.0,  -1.0,  0.0
    };

    memcpy(result, temp, 16*sizeof(float));
}


/******************************************************************
*
* multiply Matrix with Vector
*
*******************************************************************/

void multiplyMatrixWithVector(float* m, float* v){
	
	float temp[3];
	
	temp[0] = m[0]*v[0] + m[1]*v[0] + m[2]*v[0] + m[3]*v[0];
	temp[1] = m[4]*v[1] + m[5]*v[1] + m[6]*v[1] + m[7]*v[1];
	temp[2] = m[8]*v[2] + m[9]*v[2] + m[10]*v[2] + m[11]*v[2];
	
	memcpy(v, temp, 3*sizeof(float));
}

/******************************************************************
*
* 4x4 inverse
*
*******************************************************************/

void SetInverse(float* m, float* r) {

 float inv[16];

 inv[0]  =  m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
 inv[4]  = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
 inv[8]  =  m[4] * m[9]  * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
 inv[12] = -m[4] * m[9]  * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
 inv[1]  = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
 inv[5]  =  m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
 inv[9]  = -m[0] * m[9]  * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
 inv[13] =  m[0] * m[9]  * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
 inv[2]  =  m[1] * m[6]  * m[15] - m[1] * m[7]  * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7]  - m[13] * m[3] * m[6];
 inv[6]  = -m[0] * m[6]  * m[15] + m[0] * m[7]  * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7]  + m[12] * m[3] * m[6];
 inv[10] =  m[0] * m[5]  * m[15] - m[0] * m[7]  * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7]  - m[12] * m[3] * m[5];
 inv[14] = -m[0] * m[5]  * m[14] + m[0] * m[6]  * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6]  + m[12] * m[2] * m[5];
 inv[3]  = -m[1] * m[6]  * m[11] + m[1] * m[7]  * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9]  * m[2] * m[7]  + m[9]  * m[3] * m[6];
 inv[7]  =  m[0] * m[6]  * m[11] - m[0] * m[7]  * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8]  * m[2] * m[7]  - m[8]  * m[3] * m[6];
 inv[11] = -m[0] * m[5]  * m[11] + m[0] * m[7]  * m[9]  + m[4] * m[1] * m[11] - m[4] * m[3] * m[9]  - m[8]  * m[1] * m[7]  + m[8]  * m[3] * m[5];
 inv[15] =  m[0] * m[5]  * m[10] - m[0] * m[6]  * m[9]  - m[4] * m[1] * m[10] + m[4] * m[2] * m[9]  + m[8]  * m[1] * m[6]  - m[8]  * m[2] * m[5];

 float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
 if (det == 0.0f)
  return;

 det = 1.0f / det;

 int i;
 for (i = 0; i < 16; i++)
  r[i] = inv[i] * det;

}







