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

/*------------------------------------------------------------------
| Basic geometric element of scene description;
| Triangles are subdivided into smaller patches for radiosity
| computation (subdivision equal for all triangle)
------------------------------------------------------------------*/

/* help function for triangle */
double area_of_triangle(double a, double b, double c) {
	double s = (a+b+c)/2;
	return sqrt(s*(s-a)*(s-b)*(s-c));
}

/* function to calc normal for triangle */
Vector calc_normal(Vector a, Vector b, Vector c){
	Vector u = b - a;
	Vector v = c - a;
	
	return u.Cross(v);
}

Triangle::Triangle( const Vector p0_, const Vector &a_, const Vector &b_, 
					const Color &emission_, const Color &color_) :
	a(p0_), edge_a(a_), edge_b(b_), emission(emission_), color(color_){
		
	b = a + edge_a;
	c = a + edge_b;
	normal = calc_normal(a, b, c).Normalized();
	a_len = edge_a.Length();
	b_len = edge_b.Length();
	double a_to_b = (a - b).Length();
	double a_to_c = (a - c).Length();
	double b_to_c = (b - c).Length();	
	area = area_of_triangle(a_to_b, a_to_c, b_to_c);
}

void Triangle::calc_patches() {
	vector<vector<Vector>> ps;
	vector<Triangle> ts;
		
	patches.push_back({a, b, c});
	tri_patches.push_back(Triangle(a, b - a, c - a, emission, color));
		
	for(int d = 0; d < div_num; d++) {
		ps = patches;
		ts = tri_patches;
		patches.clear();
		tri_patches.clear();
		unsigned int size  = ts.size();
			
		for(unsigned int e = 0; e < size; e++){
			/* divide triangle in 4 subtriangles */
			Triangle t1 = Triangle(ts[e].a, (ts[e].a + (ts[e].edge_a / 2)) - ts[e].a,
				(ts[e].a + (ts[e].edge_b / 2)) - ts[e].a, ts[e].emission, ts[e].color);
			Triangle t2 = Triangle(t1.b, ts[e].b - t1.b, 
				(t1.b + (ts[e].edge_b / 2)) - t1.b, ts[e].emission, ts[e].color);
			Triangle t3 = Triangle(t2.c, t1.c - t2.c, t2.a - t2.c, ts[e].emission,
				ts[e].color);
			Triangle t4 = Triangle(t1.c, t2.c - t1.c, ts[e].c - t1.c, ts[e].emission,
				ts[e].color);
			
			tri_patches.push_back(t1);
			patches.push_back({t1.a, t1.b, t1.c});
			tri_patches.push_back(t2);
			patches.push_back({t2.a, t2.b, t2.c});
			tri_patches.push_back(t3);
			patches.push_back({t3.a, t3.b, t3.c});
			tri_patches.push_back(t4);
			patches.push_back({t4.a, t4.b, t4.c});
				
		}
	}
}

void Triangle::init_patchs(const int num_) {
	div_num = num_;
	patch.clear();
	patch.resize(pow(4, num_));
	calc_patches();
}

/* Triangle-ray intersection test. */
/* Implementation of the Möller-Trumbore intersection algorithm */
/* based on wikipedia.org */
double Triangle::intersect(const Ray &ray) {
		
	static const double EPSILON = 0.0000001;
	Vector b_to_a = b - a;
	Vector c_to_a = c - a;

	Vector h = ray.dir.Cross(c_to_a);
	double ax = b_to_a.Dot(h);

	if (ax > -EPSILON && ax < EPSILON)
		return 0.0;

	double f = 1.0 / ax;
	Vector s = ray.org - a;
	double u = f * s.Dot(h);

	if (u < 0.0 || u > 1.0)
		return 0.0;

	Vector q = s.Cross(b_to_a);
	double v = f * ray.dir.Dot(q);

	if (v < 0.0 || u + v > 1.0)
		return 0.0;

	double t = f * c_to_a.Dot(q);

	if (t <= EPSILON)
		return 0.0;

	return t;
}

/*------------------------------------------------------------------
| Basic geometric element of scene description
------------------------------------------------------------------*/

Rectangle::Rectangle(const Vector p0_, const Vector &a_, const Vector &b_, 
					const Color &emission_, const Color &color_) :
					p0(p0_), edge_a(a_), edge_b(b_), emission(emission_), color(color_) {
	normal = edge_a.Cross(edge_b);
	normal = normal.Normalized();        
	a_len = edge_a.Length();
	b_len = edge_b.Length();
}

Color Rectangle::sample_patch(int ia, int ib) const {
	if (ia < 0) ia = 0;
	if (ia >= a_num) ia = a_num - 1;
	if (ib < 0) ib = 0;
	if (ib >= b_num) ib = b_num - 1;
	return patch[ia * b_num + ib];
}

void Rectangle::init_patchs(const int a_num_, const int b_num_) {
	a_num = a_num_;
	b_num = b_num_;
	patch.clear();
	patch.resize(a_num * b_num);
}

const double Rectangle::intersect(const Ray &ray) {
	/* Check for plane-ray intersection first */
	const double t = (p0 - ray.org).Dot(normal) / ray.dir.Dot(normal);
	if (t <= 0.00001)
		return 0.0;

	/* Determine if intersection is within rectangle */
	Vector p = ray.org + ray.dir * t;
	Vector d = p - p0;
	const double ddota = d.Dot(edge_a);
	if (ddota < 0.0 || ddota > edge_a.LengthSquared())
		return 0.0;
        
	const double ddotb = d.Dot(edge_b);
	if (ddotb < 0.0 || ddotb > edge_b.LengthSquared())
		return 0.0;
        
	return t;
} 



