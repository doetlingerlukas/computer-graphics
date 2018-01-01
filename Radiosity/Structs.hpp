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

double area_of_triangle(double a, double b, double c);
Vector calc_normal(Vector a, Vector b, Vector c);

struct Triangle {
	Vector a, b, c;
	Vector edge_a, edge_b;
	Color emission, color;
	Vector normal;
	double area;
	vector<Color> patch;
	int div_num;
	double a_len, b_len;
	vector<vector<Vector>> patches;
	vector<Triangle> tri_patches;
	
	Triangle( const Vector p0_, const Vector &a_, const Vector &b_, 
              const Color &emission_, const Color &color_);
	
	void calc_patches();
    void init_patchs(const int num_);
    double intersect(const Ray &ray);
};

struct Rectangle {
    Vector p0;                     
    Vector edge_a, edge_b;
    Color emission, color;
    Vector normal;
    vector<Color> patch; 
    int a_num, b_num; 
    double a_len, b_len;    

    Rectangle(const Vector p0_, const Vector &a_, const Vector &b_, 
              const Color &emission_, const Color &color_);

    Color sample_patch(int ia, int ib) const;
    void init_patchs(const int a_num_, const int b_num_);
    const double intersect(const Ray &ray);
};

#endif // _STRUCTS_H_
