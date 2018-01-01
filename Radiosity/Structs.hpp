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
	
    double x, y, z;			/* Position XYZ or color RGB */

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

typedef Vector Color;

struct Ray {
	Vector org, dir;		/* Origin and direction */
	Ray(const Vector org_, const Vector &dir_);
};

struct Image {
    int width, height;
    Color *pixels;

    Image(int _w, int _h);
	Color getColor(int x, int y);
    void setColor(int x, int y, const Color &c);
    void addColor(int x, int y, const Color &c);
    int toInteger(double x);
    void Save(const string &filename);
};


#endif // _STRUCTS_H_
