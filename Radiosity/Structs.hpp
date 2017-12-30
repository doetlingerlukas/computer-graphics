#ifndef _STRUCTS_H_
#define _STRUCTS_H_

/* Standard includes */
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

using namespace std;

struct Vector {
	
    double x, y, z;           /* Position XYZ or color RGB */

    Vector(const Vector &b);
    Vector(double x_=0, double y_=0, double z_=0);
    
    Vector operator+(const Vector &b) const;
    Vector operator-(const Vector &b) const;
    Vector operator/(double c) const;
    Vector operator*(double c) const;
    friend Vector operator*(double c, const Vector &b) { 
        return b * c; 
    }
    Vector MultComponents(const Vector &b) const;
    double LengthSquared() const;
    double Length() const;
	Vector Normalized() const;
	double Dot(const Vector &b) const;
	Vector Cross(const Vector &b) const;
	Vector Invert() const;
	bool Equals(const Vector &b) const;
};


#endif // _STRUCTS_H_
