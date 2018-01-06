#include "Structs.hpp"

/*------------------------------------------------------------------
| Struct for standard vector operations in 3D 
| (used for points, vectors, and colors).
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

double Vector::Max() {
	return fmax(x, fmax(x, y));
}

Vector& Vector::clamp() {
	x = x<0 ? 0.0 : x>1.0 ? 1.0 : x;
	y = y<0 ? 0.0 : y>1.0 ? 1.0 : y;
	z = z<0 ? 0.0 : z>1.0 ? 1.0 : z;
	return *this;   
}

Vector Vector::Interpolate(double alpha, const Vector &b) const {
	return Vector((x*alpha + b.x*(1-alpha)), (y*alpha + b.y*(1-alpha)), 
			(z*alpha + b.z*(1-alpha))) / 2;
}

/*------------------------------------------------------------------
| Structure for rays (e.g. viewing ray, ray tracing).
------------------------------------------------------------------*/

Ray::Ray(const Vector org_, const Vector &dir_) : org(org_), dir(dir_) {}

/*------------------------------------------------------------------
| Struct holds pixels/colors of rendered image.
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
| Scene objects can be triangles.
| Triangles are subdivided into smaller patches for
| computation (subdivision equal for all triangle).
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
					const Color &emission_, const Color &color_, Refl_t refl_) :
	a(p0_), edge_a(a_), edge_b(b_), emission(emission_), color(color_), refl(refl_) {
		
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

Triangle::Triangle( Vector a_, Vector b_, Vector c_, Color color_, Refl_t refl_):
	a(a_), b(b_), c(c_), color(color_), refl(refl_) {
	
	edge_a = b - a;
	edge_b = c - a;
	emission = Color();
	normal = calc_normal(a, b, c).Normalized();
}

void Triangle::calc_patches() {
	vector<vector<Vector>> ps;
	vector<Triangle> ts;
		
	patches.push_back({a, b, c});
	tri_patches.push_back(Triangle(a, b - a, c - a, emission, color, refl));
		
	for(int d = 0; d < div_num; d++) {
		ps = patches;
		ts = tri_patches;
		patches.clear();
		tri_patches.clear();
		unsigned int size  = ts.size();
			
		for(unsigned int e = 0; e < size; e++){
			/* divide triangle in 4 subtriangles */
			Triangle t1 = Triangle(ts[e].a, (ts[e].a + (ts[e].edge_a / 2)) - ts[e].a,
				(ts[e].a + (ts[e].edge_b / 2)) - ts[e].a, ts[e].emission, ts[e].color, ts[e].refl);
			Triangle t2 = Triangle(t1.b, ts[e].b - t1.b, 
				(t1.b + (ts[e].edge_b / 2)) - t1.b, ts[e].emission, ts[e].color, ts[e].refl);
			Triangle t3 = Triangle(t2.c, t1.c - t2.c, t2.a - t2.c, ts[e].emission,
				ts[e].color, ts[e].refl);
			Triangle t4 = Triangle(t1.c, t2.c - t1.c, ts[e].c - t1.c, ts[e].emission,
				ts[e].color, ts[e].refl);
			
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
| Scene objects can be spheres. Material either perfectly diffuse, 
| specular (mirror reflection) or transparent (refraction/reflection)
| (DIFFuse, SPECular, REFRactive).
------------------------------------------------------------------*/

Sphere::Sphere(double radius_, Vector position_, Vector emission_, 
				Vector color_, Refl_t refl_):
	radius(radius_), position(position_), emission(emission_), 
	color(color_), refl(refl_) {}

double Sphere::Intersect(const Ray &ray) const { 
	/* Check for ray-sphere intersection by solving for t:
		t^2*d.d + 2*t*(o-p).d + (o-p).(o-p) - R^2 = 0 */
	Vector op = position - ray.org; 
	double eps = 1e-4;
	double b = op.Dot(ray.dir);
	double radicant = b*b - op.Dot(op) + radius*radius;
	if (radicant < 0.0) 
		return 0.0;		/* No intersection */
	else   
		radicant = sqrt(radicant);
        
	double t;
	t = b - radicant;	/* Check smaller root first */
	if(t > eps)
		return t;
        
	t = b + radicant;
	if(t > eps)			/* Check second root */
		return t;
        
	return 0.0;			/* No intersection in ray direction */  
}	
	
