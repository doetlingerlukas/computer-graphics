#include "Structs.hpp"

/*------------------------------------------------------------------
| Struct for standard vector operations in 3D 
| (used for points, vectors, and colors)
------------------------------------------------------------------*/

Vector::Vector(const Vector &b) : x(b.x), y(b.y), z(b.z) {}
Vector::Vector(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

Vector Vector::operator+(const Vector &b) const {
	return Vector(x + b.x, y + b.y, z + b.z);
}

Vector Vector::operator-(const Vector &b) const {
    return Vector(x - b.x, y - b.y, z - b.z);
}

Vector Vector::operator/(double c) const {
	return Vector(x / c, y / c, z / c);
}

Vector Vector::operator*(double c) const {
    return Vector(x * c, y * c, z * c);
}

Vector Vector::MultComponents(const Vector &b) const {
	return Vector(x * b.x, y * b.y, z * b.z);
}

double Vector::LengthSquared() const { 
	return x*x + y*y + z*z; 
}

double Vector::Length() const { 
	return sqrt(LengthSquared()); 
} 

Vector Vector::Normalized() const {
	return Vector(x, y, z) / sqrt(x*x + y*y + z*z);
}

double Vector::Dot(const Vector &b) const {
    return x * b.x + y * b.y + z * b.z;
}

Vector Vector::Cross(const Vector &b) const {
	return Vector((y * b.z) - (z * b.y), 
				  (z * b.x) - (x * b.z), 
                  (x * b.y) - (y * b.x));
}
    
Vector Vector::Invert() const {
	return Vector(-x, -y, -z);
}

bool Vector::Equals(const Vector &b) const {
	if(x == b.x && y == b.y && z == b.z){
		return true;
	} else {
		return false;
	}
}

/*------------------------------------------------------------------
| Structure for rays (e.g. viewing ray, ray tracing)
------------------------------------------------------------------*/

Ray::Ray(const Vector org_, const Vector &dir_) : org(org_), dir(dir_) {}

/*------------------------------------------------------------------
| Struct holds pixels/colors of rendered image
------------------------------------------------------------------*/

Image::Image(int _w, int _h) : width(_w), height(_h) {
	pixels = new Color[width * height];        
}

Color Image::getColor(int x, int y) {
	int image_index = (height-y-1) * width + x;	
	return pixels[image_index];
}

void Image::setColor(int x, int y, const Color &c) {
	int image_index = (height-y-1) * width + x;	
	pixels[image_index] = c;
}

void Image::addColor(int x, int y, const Color &c) {
	int image_index = (height-y-1) * width + x;	
	pixels[image_index] = pixels[image_index] + c;
}

int Image::toInteger(double x) { 
	/* Clamp to [0,1] */
	if (x<0.0) x = 0.0;		
        
	if (x>1.0) x = 1.0;             

	/* Apply gamma correction and convert to integer */
	return int(pow(x,1/2.2)*255+.5); 
}

void Image::Save(const string &filename) {
	/* Save image in PPM format */
	FILE *f = fopen(filename.c_str(), "wb");
	fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
	for (int i = 0; i < width * height; i++)
	fprintf(f,"%d %d %d ", toInteger(pixels[i].x), 
							toInteger(pixels[i].y), 
							toInteger(pixels[i].z));
	fclose(f);
}








