/******************************************************************
*
* Radiosity.cpp
*
* Description: This file demonstrates global illumination rendering
* based on the radiosity method. The geometry is divided into patches
* for which the form factors are determined employing Monte Carlo 
* integration. Radiosity values for the patches are computed with
* an iterative solver. The final image (i.e. radiance) is obtained 
* via tracing rays into the scene. Two output files are saved - 
* one with constant shading of patches and one with bicubic color
* interpolation.
*
* The code is extended from software by user Hole and Kevin Beason,
* released under the MIT License. 
*
* http://kagamin.net/hole/license.txt
* http://kagamin.net/hole/smallpt-license.txt
*
* Advanced Computer Graphics Proseminar WS 2015
* 
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/


/* Standard includes */
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

#include <unistd.h>

using namespace std;

const double Over_M_PI = 1.0/M_PI;

static double *form_factor;
static int patch_num = 0;

/*------------------------------------------------------------------
| Struct for standard vector operations in 3D 
| (used for points, vectors, and colors)
------------------------------------------------------------------*/
struct Vector 
{
    double x, y, z;           /* Position XYZ or color RGB */

    Vector(const Vector &b) : x(b.x), y(b.y), z(b.z) {}
    Vector(double x_=0, double y_=0, double z_=0) : x(x_), y(y_), z(z_) {}
    
    Vector operator+(const Vector &b) const 
    {
        return Vector(x + b.x, y + b.y, z + b.z);
    }

    Vector operator-(const Vector &b) const
    {
        return Vector(x - b.x, y - b.y, z - b.z);
    }

    Vector operator/(double c) const 
    {
        return Vector(x / c, y / c, z / c);
    }

    Vector operator*(double c) const 
    {
        return Vector(x * c, y * c, z * c);
    }

    friend Vector operator*(double c, const Vector &b) 
    { 
        return b * c; 
    }

    Vector MultComponents(const Vector &b) const
    {
        return Vector(x * b.x, y * b.y, z * b.z);
    }

    const double LengthSquared() const 
    { 
        return x*x + y*y + z*z; 
    }

    const double Length() const 
    { 
        return sqrt(LengthSquared()); 
    } 

    const Vector Normalized() const
    {
        return Vector(x, y, z) / sqrt(x*x + y*y + z*z);
    }

    const double Dot(const Vector &b) const 
    {
        return x * b.x + y * b.y + z * b.z;
    }

    const Vector Cross(const Vector &b) const
    {
        return Vector((y * b.z) - (z * b.y), 
                      (z * b.x) - (x * b.z), 
                      (x * b.y) - (y * b.x));
    }
    
    const Vector Invert() const 
    {
		return Vector(-x, -y, -z);
	}
	
	const bool Equals(const Vector &b) const
	{
		if(x == b.x && y == b.y && z == b.z){
			return true;
		} else {
			return false;
		}
	}
};

typedef Vector Color;
const Color BackgroundColor(0.0, 0.0, 0.0);

/*------------------------------------------------------------------
| Structure for rays (e.g. viewing ray, ray tracing)
------------------------------------------------------------------*/
struct Ray 
{
	Vector org, dir;    /* Origin and direction */
	Ray(const Vector org_, const Vector &dir_) : org(org_), dir(dir_) {}
};


/*------------------------------------------------------------------
| Struct holds pixels/colors of rendered image
------------------------------------------------------------------*/
struct Image 
{
    int width, height;
    Color *pixels;

    Image(int _w, int _h) : width(_w), height(_h) 
    {
        pixels = new Color[width * height];        
    }

    Color getColor(int x, int y) 
    {
        int image_index = (height-y-1) * width + x;	
        return pixels[image_index];
    }

    void setColor(int x, int y, const Color &c) 
    {
        int image_index = (height-y-1) * width + x;	
        pixels[image_index] = c;
    }

    void addColor(int x, int y, const Color &c) 
    {
        int image_index = (height-y-1) * width + x;	
        pixels[image_index] = pixels[image_index] + c;
    }

    int toInteger(double x)
    { 
        /* Clamp to [0,1] */
        if (x<0.0)
            x = 0.0;		
        
        if (x>1.0)
            x = 1.0;             

        /* Apply gamma correction and convert to integer */
        return int(pow(x,1/2.2)*255+.5); 
    }

    void Save(const string &filename) 
    {
        /* Save image in PPM format */
        FILE *f = fopen(filename.c_str(), "wb");
        fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
        for (int i = 0; i < width * height; i++)
            fprintf(f,"%d %d %d ", toInteger(pixels[i].x), 
                                   toInteger(pixels[i].y), 
                                   toInteger(pixels[i].z));
        fclose(f);
    }
};

/*------------------------------------------------------------------
| Basic geometric element of scene description;
| Rectangles are subdivided into smaller patches for radiosity
| computation (subdivision equal for all rectangles)
------------------------------------------------------------------*/

/* help function for triangle */
double area_of_triangle( double a, double b, double c ) {
	double s = (a+b+c)/2;
	return sqrt(s*(s-a)*(s-b)*(s-c));
}

/* function to calc normal for triangle */
Vector calc_normal(Vector a, Vector b, Vector c){
	Vector u = b - a;
	Vector v = c - a;
	
	return u.Cross(v);
}

/* interpolate colors for a vertex */
Color interpolate_vertex(vector<Color> data) {
	Color to_return = Color(0.0, 0.0, 0.0);
	
	double i = data.size();
	for(unsigned int i = 0; i < data.size(); i++) {
		to_return = to_return + data[i];
	}
	
	Color r = Color(
		(to_return.x > 0 ? (to_return.x / i) : 0), 
		(to_return.y > 0 ? (to_return.y / i) : 0), 
		(to_return.z > 0 ? (to_return.z / i) : 0));
	
	return r;
}

struct Triangle {
	Vector a, b, c;
	Vector edge_a, edge_b;
	Color emission, color;
	Vector normal;
	double area;
	
	vector<Color> patch;
	vector<Color> vertex_colors;
	int a_num, b_num;
	double a_len, b_len;
	
	vector<vector<Vector>> patches;
	/* for easy intersection test with subpatches */
	vector<Triangle> tri_patches;
	
	Triangle( const Vector p0_, const Vector &a_, const Vector &b_, 
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
	
	void calc_vertex_colors(vector<Triangle> patches) {
		vector<Color> a_colors;
		vector<Color> b_colors;
		vector<Color> c_colors;
		
		for(Triangle p : patches) {
			if (a.Equals(p.a)) {
				a_colors.push_back(p.color);
			}
			if (b.Equals(p.b)) {
				b_colors.push_back(p.color);
			}
			if (c.Equals(p.c)) {
				c_colors.push_back(p.color);
			}
		}
		
		vertex_colors.push_back(interpolate_vertex(a_colors));
		vertex_colors.push_back(interpolate_vertex(b_colors));
		vertex_colors.push_back(interpolate_vertex(c_colors));
	}
	
	void calc_patches() {
		Vector e_a = edge_a / a_num;
		Vector e_b = edge_b / b_num;
		
		int row = 0;
		int row_size = b_num;
		while (row_size > 0){
			for(int i = 0; i < row_size; i++){
				int y = row/2;
				int orientation = row % 2;
				
				Vector v1 = a + (i + orientation) * e_a + y * e_b;
				Vector v2 = a + (i + 1) * e_a + (y + orientation) * e_b;
				Vector v3 = a + i * e_a + (y + 1) * e_b;
				
				patches.push_back({v1, v2, v3});
				
				tri_patches.push_back(Triangle(v1, v2 - v1, v3 - v1, emission, 
					color));				
			}
		
			row++;
			
			if (row % 2 == 1){
				row_size--;
			}
		}
	}
	
	Color sample_patch(int ia, int ib) const 
    {
        if (ia < 0) ia = 0;
        if (ia >= a_num) ia = a_num - 1;
        if (ib < 0) ib = 0;
        if (ib >= b_num) ib = b_num - 1;
        return patch[ia * b_num + ib];
    }

    void init_patchs(const int a_num_, const int b_num_) 
    {
        a_num = a_num_;
        b_num = b_num_;
        patch.clear();
        patch.resize(a_num * b_num);
        calc_patches();
    }
	
	/* Triangle-ray intersection test. */
	/* Implementation of the Möller-Trumbore intersection algorithm */
	/* based on wikipedia.org */
    double intersect(const Ray &ray) {
		
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
};

struct Rectangle 
{
    Vector p0;                     
    Vector edge_a, edge_b;
    Color emission, color;
    Vector normal;
	
    vector<Color> patch; 
    int a_num, b_num;       /* Number of patches/subdivision of edges */
    double a_len, b_len;    /* Edge lengths */

    Rectangle(const Vector p0_, const Vector &a_, const Vector &b_, 
              const Color &emission_, const Color &color_) :
              p0(p0_), edge_a(a_), edge_b(b_), emission(emission_), color(color_) 
    {
        normal = edge_a.Cross(edge_b);
        normal = normal.Normalized();        
        a_len = edge_a.Length();
        b_len = edge_b.Length();
    }

    Color sample_patch(int ia, int ib) const 
    {
        if (ia < 0) ia = 0;
        if (ia >= a_num) ia = a_num - 1;
        if (ib < 0) ib = 0;
        if (ib >= b_num) ib = b_num - 1;
        return patch[ia * b_num + ib];
    }

    void init_patchs(const int a_num_, const int b_num_) 
    {
        a_num = a_num_;
        b_num = b_num_;
        patch.clear();
        patch.resize(a_num * b_num);
    }

    /* Rectangle-ray intersection */
    const double intersect(const Ray &ray) 
    {
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
};


/******************************************************************
* Hard-coded scene definition: the geometry is composed of rectangles. 
* These are defined by:
* - vector to corner(origin), edge a, edge b 
* - emitted light energy (light sources), surface reflectivity (~color)
*******************************************************************/
Rectangle recs[] = 
{	
    /* Cornell Box walls */
    Rectangle(Vector(0.0, 0.0, 0.0), Vector(100.0, 0.0, 0.0), Vector(0.0, 80.0, 0.0),   
              Vector(), Color(0.75, 0.75, 0.75)), /* Back */
    Rectangle(Vector(0.0, 0.0, 170.0), Vector(100.0, 0.0, 0.0), Vector(0.0, 0.0, -170.0), 
              Vector(), Color(0.75, 0.75, 0.75)), /* Bottom */
    Rectangle(Vector(0.0, 80.0, 0.0), Vector(100.0, 0.0, 0.0), Vector(0.0, 0.0, 170.0),  
              Vector(), Color(0.75, 0.75, 0.75)), /* Top */
    Rectangle(Vector(0.0, 0.0, 170.0), Vector(0.0, 0.0, -170.0), Vector(0.0, 80.0, 0.0),   
              Vector(), Color(0.75, 0.25, 0.25)), /* Left */
    Rectangle(Vector(100.0, 0.0, 0.0), Vector(0.0, 0.0, 170.0), Vector(0.0, 80.0, 0.0),   
              Vector(), Color(0.25, 0.25, 0.75)), /* Right */
    Rectangle(Vector(100.0, 0.0, 170.0), Vector(-100.0, 0.0, 0.0), Vector(0.0, -80.0, 0.0),  
              Vector(), Color(0,1,0)), /* Front (not visible) */
	
    /* Area light source on top */
    Rectangle(Vector(40.0, 79.99, 65.0), Vector(20.0, 0.0, 0.0), Vector(0.0, 0.0, 20.0), 
              Vector(12,12,12), Color(0.75, 0.75, 0.75)), 

    /* Cuboid in room */
    Rectangle(Vector(30.0, 0.0, 100.0), Vector(0.0, 0.0, -20.0), Vector(0.0, 40.0, 0.0),   
              Vector(), Color(0.75, 0.75, 0.75)), /* Right */
    Rectangle(Vector(10.0, 0.0, 80.0), Vector(0.0, 0.0, 20.0), Vector(0.0, 40.0, 0.0),   
              Vector(), Color(0.75, 0.75, 0.75)), /* Left */
    Rectangle(Vector(10.0, 0.0, 100.0), Vector(20.0, 0.0, 0.0), Vector(0.0, 40.0, 0.0),   
              Vector(), Color(0.75, 0.75, 0.75)), /* Front */
    Rectangle(Vector(30.0, 0.0, 80.0), Vector(-20.0, 0.0, 0.0), Vector(0.0, -40.0, 0.0),  
              Vector(), Color(0.75, 0.75, 0.75)), /* Back */
    Rectangle(Vector(10.0, 40.0, 100.0), Vector(20.0, 0.0, 0.0), Vector(0.0, 0.0, -20.0), 
              Vector(), Color(0.75, 0.75, 0.75)), /* Top */
};


vector<Triangle> Split_Rectangle(Rectangle rec) {
	vector<Triangle> tris;
	
	tris.push_back(Triangle(rec.p0, rec.edge_a, rec.edge_b, rec.emission, 
			rec.color));
	tris.push_back(Triangle(rec.p0.operator +(rec.edge_a.operator +(rec.edge_b)),
			(rec.edge_a.Invert()) , (rec.edge_b.Invert()) ,
			rec.emission, rec.color));
			
	return tris;
}

vector<Triangle> Rectangles_To_Triangles() {
	unsigned int recs_size = sizeof(recs)/sizeof(recs[0]);
	vector<Triangle> tris;
	
	for(unsigned int i = 0; i < recs_size; i++){
		vector<Triangle> temp_tris = Split_Rectangle(recs[i]);
		tris.push_back(temp_tris[0]);
		tris.push_back(temp_tris[1]);
	}
	
	return tris;
}

/* Triangle version of recs*/
vector<Triangle> tris = Rectangles_To_Triangles();

/******************************************************************
* Check for closest intersection of a ray with the scene;
* Returns true if intersection is found, as well as ray parameter
* of intersection and id of intersected object
*******************************************************************/
bool Intersect_Scene(const Ray &ray, double *t, int *id, Vector *normal) {
	
    const int n = tris.size();
    *t  = 1e20;
    *id = -1;
	
    for (int i = 0; i < n; i ++)
    {
        double d = tris[i].intersect(ray);
        if (d > 0.0 && d < *t) 
        {
            *t  = d;
            *id = i;
            *normal = tris[i].normal;
        }
    }
    return *t < 1e20;
}

/******************************************************************
* Determine all form factors for all pairs of patches (of all
* rectangles);
* Evaluation of integrals in form factor equation is done via
* Monte Carlo integration; samples are uniformly distributed and
* equally weighted;
* Computation accelerated by exploiting symmetries of form factor
* estimation;
*******************************************************************/
Vector get_sample_point(Vector v1, Vector v2, Vector v3){
	srand(time(nullptr));
	
	double epsilon1 = drand48();
	double epsilon2 = drand48();
	
	double lambda0 = 1.0 - sqrt(epsilon1);
	double lambda1 = epsilon2 * sqrt(epsilon1);
	double lambda2 = 1.0 - lambda0 - lambda1;
	
	Vector q = lambda0 * v1 + lambda1 * v2 + lambda2 * v3;
	
	return q;
}

void Calculate_Form_Factors(const int a_div_num, const int b_div_num, 
                            const int mc_sample) 
{
    /* Total number of patches in scene */
    const int n = tris.size();
    for (int i = 0; i < n; i ++) 
    {
        tris[i].init_patchs(a_div_num, b_div_num); 
        patch_num += tris[i].a_num * tris[i].b_num;
    }
    
    std::cout << "Number of triangles: " << n << endl;
    cout << "Number of patches: " << patch_num << endl;
    int form_factor_num = patch_num * patch_num;
    cout << "Number of form factors: " << form_factor_num << endl;
    
    /* 1D-array to hold form factor pairs */
    form_factor = new double[form_factor_num];
    memset(form_factor, 0.0, sizeof(double) * form_factor_num);

    /* 1D-array with patch areas */
    double *patch_area = new double[patch_num];
    memset(patch_area, 0.0, sizeof(double) * patch_num);

    /* Precompute patch areas, assuming same size for each triangle */
    for (int i = 0; i < n; i ++) 
    {
        int patch_i = 0;
        for (int k = 0; k < i; k ++)
            patch_i += tris[k].a_num * tris[k].b_num;

        for (int ia = 0; ia < tris[i].a_num; ia ++) 
        {
            for (int ib = 0; ib < tris[i].b_num; ib ++) 
            {
				int index = patch_i + ia* tris[i].b_num + ib;
                patch_area[index] = tris[i].area / tris[i].patches.size(); 
            }
        }
    }
    

    /* Offsets for indexing of patches in 1D-array */
    int *offset = new int[n];
    
    for (int i = 0; i < n; i ++) {
        offset[i] = 0;
        for (int k = 0; k < i; k ++)
            offset[i] += tris[k].patches.size();
    }

    /* Loop over all triangles in scene */
    for (int i = 0; i < n; i ++) {
		
        int patch_i = offset[i];	
        cout << i << " "; 
		
        /* Loop over all patches in triangles i */
        for (unsigned long ip = 0; ip < tris[i].patches.size(); ip ++) {
			
            cout << "*" << flush;
				
            const Vector normal_i = tris[i].normal;
 
            int patch_j = 0;

            /* Loop over all triangles in scene for triangle i */
            for (int j = 0; j < n; j ++) {
				
				const Vector normal_j = tris[j].normal;

				/* Loop over all patches in triangle j */
                for (unsigned long jp = 0; jp < tris[j].patches.size(); jp ++) {
                      				
					/* Do not compute form factors for patches on same rectangle;
						also exploit symmetry to reduce computation;
                        intemediate values; will be divided by patch area below */
					if (i < j) {
						double F = 0;
                                
                                
						vector<Vector> patches_i = tris[i].patches[patch_i - offset[i]];
						vector<Vector> patches_j = tris[j].patches[patch_j - offset[j]];
								
						/* Uniform PDF for Monte Carlo (1/Ai)x(1/Aj) */
						const double pdf = 
							(1.0 / patch_area[offset[i] + ip]) *
							(1.0 / patch_area[offset[j] + jp]);

                        /* Determine rays of NixNi uniform samples of patch 
							on i to NjxNj uniform samples of patch on j */
                        for (int is = 0; is < mc_sample; is ++) {
							for (int js = 0; js < mc_sample; js ++) {
                                    
								/* Determine sample points xi, xj on both patches */
                                const Vector xi = get_sample_point(patches_i[0], patches_i[1], 
									patches_i[2]);
                                const Vector xj = get_sample_point(patches_j[0], patches_j[1],
									patches_j[2]);

                                /* Check for visibility between sample points */
                                const Vector ij = (xj - xi).Normalized();

                                double t; 
                                int id;
                                Vector normal; 
                                if (Intersect_Scene(Ray(xi, ij), &t, &id, &normal) && id != j) {
									continue; /* If intersection with other rectangle */
                                }

                                /* Cosines of angles beteen normals and ray inbetween */
                                const double d0 = normal_i.Dot(ij);
                                const double d1 = normal_j.Dot(-1.0 * ij);

                                /* Continue if patches facing each other */
                                if (d0 > 0.0 && d1 > 0.0) {
									/* Sample form factor */
                                    const double K = d0 * d1 / (M_PI * (xj - xi).LengthSquared());

                                    /* Add weighted sample to estimate */
									F += K / pdf;
                                }
                            }
                        } 

                        /* Divide by number of samples */
                        F /= (mc_sample) * (mc_sample); 
 
                        form_factor[patch_i * patch_num + patch_j] = F;
					}
                    patch_j ++;
                        
                }
            }
            patch_i ++;
        }

        cout << endl;
    }

    /* Copy upper to lower triangular values */
    for (int i = 0; i < patch_num-1; i ++) 
    {
        for (int j = i+1; j < patch_num; j ++) 
        {
             form_factor[j * patch_num + i] = form_factor[i * patch_num + j];
        }
    }

    /* Divide by area to get final form factors */
    for (int i = 0; i < patch_num; i ++) 
    {
        for (int j = 0; j < patch_num; j ++) 
        {
             form_factor[i * patch_num + j] /= patch_area[i];

             /* Clamp to [0,1] */
             if(form_factor[i * patch_num + j] > 1.0) 
                 form_factor[i * patch_num + j] = 1.0;
		}
    }
}


/******************************************************************
* Iterative computation of radiosity via Gathering; i.e. solution
* using Gauss-Seidel iteration - reuse already computed values;
* run-time O(n^2) 
*******************************************************************/

void Calculate_Radiosity(const int iteration) 
{
    const int n = tris.size();
    int patch_i = 0;
	
    for (int i = 0; i < n; i ++) {
		
        for (unsigned long ip = 0; ip < tris[i].patches.size(); ip ++)  {
			
            Color B;
            int patch_j = 0;
            
			for (int j = 0; j < n; j ++) {
				
                for (unsigned long jp = 0; jp < tris[j].patches.size(); jp ++) {
						
					const double Fij = form_factor[patch_i * patch_num + patch_j];
					 
					/* Add form factor multiplied with radiosity of previous step */		
					if (Fij > 0.0) {
						B = B + Fij * tris[j].patch[jp];
					}
					patch_j ++;
				}
            }
            /* Multiply sum with color of patch and add emission */
            B = tris[i].color.MultComponents(B) + tris[i].emission;
  
            /* Store overall patch radiosity of current iteration */
			tris[i].patch[ip] = B;
			
			patch_i ++;
        }
    }
}


/******************************************************************
* Helper functions for smooth bicubic (Catmull-Rom) interpolation 
* using 4x4 color patches;
* First interpolate in y, followed by interpolation of results in x
*******************************************************************/

Color cubicInterpolate (Color p[4], double x) 
{
    return p[1] + 0.5 * x * (p[2] - p[0] + x * (2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + 
                                                 x * (3.0*(p[1] - p[2]) + p[3] - p[0])));
}

Color bicubicInterpolate (Color p[4][4], double x, double y) 
{
	Color arr[4];

	arr[0] = cubicInterpolate(p[0], y);
	arr[1] = cubicInterpolate(p[1], y);
	arr[2] = cubicInterpolate(p[2], y);
	arr[3] = cubicInterpolate(p[3], y);
	
    return cubicInterpolate(arr, x);
}

/* interpolate all vertices for a patch */
vector<vector<Color>> patch_vertices_interpolation(
	vector<vector<vector<Color>>> to_i) {
	
	vector<vector<Color>> colors1;
	
	for(vector<vector<Color>> patch_data : to_i){
		vector<Color> patch_color;
		for(vector<Color> vertex : patch_data){
			patch_color.push_back(interpolate_vertex(vertex));
		}
		colors1.push_back(patch_color);
	}
	
	return colors1;
}

/* calculate all patch_vertex colors for triangle by interpolation */
vector<vector<Color>> color_for_patch_vertices(Triangle tri) {
	
	vector<vector<Color>> colors;
	vector<vector<vector<Color>>> to_interpolate;
	
	for(unsigned int pi = 0; pi < tri.tri_patches.size(); pi++) {
		
		vector<Color> vertex1;
		vector<Color> vertex2;
		vector<Color> vertex3;
		
		for(unsigned int t = 0; t < tris.size(); t++) {
			if(tri.normal.Equals(tris[t].normal)) {
				
				for(unsigned int pj = 0; pj < tris[t].tri_patches.size(); pj++) {
					
					if(tri.tri_patches[pi].a.Equals(tris[t].tri_patches[pj].a) || 
						tri.tri_patches[pi].a.Equals(tris[t].tri_patches[pj].b) ||
						tri.tri_patches[pi].a.Equals(tris[t].tri_patches[pj].c)){
						vertex1.push_back(tris[t].patch[pj]);
					}
					if(tri.tri_patches[pi].b.Equals(tris[t].tri_patches[pj].a) ||
						tri.tri_patches[pi].b.Equals(tris[t].tri_patches[pj].b) ||
						tri.tri_patches[pi].b.Equals(tris[t].tri_patches[pj].c)){
						vertex2.push_back(tris[t].patch[pj]);
					}
					if(tri.tri_patches[pi].c.Equals(tris[t].tri_patches[pj].a) ||
						tri.tri_patches[pi].c.Equals(tris[t].tri_patches[pj].b) ||
						tri.tri_patches[pi].c.Equals(tris[t].tri_patches[pj].c)){
						vertex3.push_back(tris[t].patch[pj]);
					}
					
				}
			}
		}
		
		to_interpolate.push_back({vertex1, vertex2, vertex3});
	}
	
	colors = patch_vertices_interpolation(to_interpolate);
	
	return colors;
}

/* calculate all vertex colors */
vector<vector<vector<Color>>> all_vertex_colors () {
	
	vector<vector<vector<Color>>> vertex_colors;
	
	for(Triangle tri : tris) {
		vertex_colors.push_back(color_for_patch_vertices(tri));
	}
	
	return vertex_colors;
}

/* vertex color values */
/* sorted by triangle -> patches -> (A Color, B Color, C Color) */
vector<vector<vector<Color>>> triangle_vertex_colors;

/******************************************************************
* Compute radiance from radiosity by shooting rays into the scene;
* Radiance directly proportional to radiosity for assumed diffuse
* emitters/surfaces (multiply by PI);
* At intersections either constant patch color is returned or a
* smoothly interpolated color of 4x4 neighboring patches
*******************************************************************/

Color Radiance(const Ray &ray, const int depth, bool interpolation = true) 
{
    double t; 
    int id;  
    Vector normal; 

    /* Find intersected rectangle */
    if (!Intersect_Scene(ray, &t, &id, &normal)) {
        return BackgroundColor;    
    }
    

    /* Determine intersection point on rectangle */
    Triangle &obj = tris[id];
    const Vector hitpoint = ray.org + t * ray.dir; 

    /* Determine intersected patch */    
    int index = 0;
    for(unsigned long ip = 0; ip < obj.tri_patches.size(); ip++){
		if(obj.tri_patches[ip].intersect(ray) > 0.0){
			index = ip;
		}
	}
            
    /* Barycentric interpolation for smooth image */
    if (interpolation) {
		
		vector<Color> cs = triangle_vertex_colors[id][index];
		
		Triangle patch = obj.tri_patches[index];
		
		double a_to_h = (hitpoint - patch.a).Length();
		double b_to_h = (hitpoint - patch.b).Length();
		double c_to_h = (hitpoint - patch.c).Length();
		
		double a1 = area_of_triangle(c_to_h, b_to_h, 
			(patch.c - patch.b).Length());
		double a2 = area_of_triangle(a_to_h, c_to_h, 
			(patch.c - patch.a).Length());
		double a3 = area_of_triangle(a_to_h, b_to_h, 
			(patch.b - patch.a).Length());
			
		double lambda1 = a1 / patch.area;
		double lambda2 = a2 / patch.area;
		double lambda3 = a3 / patch.area;
		
		Color interp = ((cs[0] * lambda1) + (cs[1] * lambda2)) + (cs[2] * lambda3);
		
		/*
		cout << cs[0].x << " "; 
        cout << cs[1].x << " "; 
        cout << cs[2].x << endl; 
		*/
		return interp;
		
    } else {         
        return obj.patch[index];
    }
}



/******************************************************************
* Main routine: Computation of radiosity image
* Key parameters
* - Image dimensions: width, height 
* - Number of samples for antialiasing (non-uniform filter): samples 
* - Number of patches along edges a,b: patches_a, patches_b
* - Number of uniform samples per patch edge: MC_samples
* - Number of iterations for iterative solver: iterations
* Rendered result saved as PPM image file
*******************************************************************/

int main(int argc, char **argv) {
	
	/* tests for the intersection test */
	auto triangle = Triangle(Vector(2.0, 2.0, 0.0), Vector(2.0, 0.0, 0.0), 
		Vector(0.0, 2.0, 0.0), Color(), Color());
    auto ray = Ray(Vector(2.25, 2.25, -1.0), Vector(0.0, 0.0, 1.0).Normalized());
    auto intersection = triangle.intersect(ray);
    if (intersection > 0.0) {}
    else {cout << "fail" << endl;}
    
    intersection = 0.0;

	triangle = Triangle(Vector(0.0, 0.0, 0.0), Vector(2.0, 0.0, 0.0), 
		Vector(0.0, 2.0, 0.0), Color(), Color());
    ray = Ray(Vector(0.1, 0.1, -1.0), Vector(0.0, 0.0, 1.0).Normalized());
    intersection = triangle.intersect(ray);
    if (intersection > 0.0) {}
    else {cout << "fail" << endl;}
  
	intersection = 0.0;

    ray = Ray(Vector(0.1, 0.1, 1.0), Vector(0.0, 0.0, -1.0).Normalized());
    intersection = triangle.intersect(ray);
    if (intersection > 0.0) {}
    else {cout << "fail" << endl;}
    
    intersection = 1.0;

    ray = Ray(Vector(0.1, 0.1, -1.0), Vector(0.0, 0.0, -1.0).Normalized());
    intersection = triangle.intersect(ray);
    if (intersection == 0.0) {}
    else {cout << "fail" << endl;}
		
	intersection = 1.0;
	
    ray = Ray(Vector(2.0, 2.0, 1.0), Vector(0.0, 0.0, -1.0).Normalized());
    intersection = triangle.intersect(ray);
    if (intersection == 0.0) {}
    else {cout << "fail" << endl;}
 
 
	/* main part */
    int width = 640;
    int height = 480;
    int samples = 4;

    /* Set camera origin and viewing direction (negative z direction) */
    Ray camera(Vector(50.0, 52.0, 295.6), Vector(0.0, -0.042612, -1.0).Normalized());

    /* Image edge vectors for pixel sampling */
    Vector cx = Vector(width * 0.5135 / height);
    Vector cy = (cx.Cross(camera.dir)).Normalized() * 0.5135;

    /* Two final renderings; one with constant, one with interpolated patch colors */
    Image img(width, height);
    Image img_interpolated(width, height);

    cout << "Calculating form factors" << endl;
    int patches_a = 8;
    int patches_b = 8;
    int MC_samples = 3;

    Calculate_Form_Factors(patches_a, patches_b, MC_samples);

    /* Iterative solution of radiosity linear system */
    cout << "Calculating radiosity" << endl;
    int iterations = 40; 
    for (int i = 0; i < iterations; i ++) 
    {
        cout << i << " ";
        Calculate_Radiosity(i);
    }
    cout << endl;
 
	triangle_vertex_colors = all_vertex_colors();
 
    /* Loop over image rows */
    for (int y = 0; y < height; y ++) 
    {
        cout << "\rRendering (" << samples * 4 << " spp) " << (100.0 * y / (height - 1)) << "%     ";
        srand(y * y * y);

        /* Loop over row pixels */
        for (int x = 0; x < width; x ++) 
        {
            img.setColor(x, y, Color());
            img_interpolated.setColor(x, y, Color());

            /* 2x2 subsampling per pixel */
            for (int sy = 0; sy < 2; sy ++) 
            {
                for (int sx = 0; sx < 2; sx ++) 
                {
                    Color accumulated_radiance = Color();
                    Color accumulated_radiance2 = Color();

                    /* Computes radiance at subpixel using multiple samples */
                    for (int s = 0; s < samples; s ++) 
                    {
                        const double r1 = 2.0 * drand48();
                        const double r2 = 2.0 * drand48();

                        /* Transform uniform into non-uniform filter samples */
                        double dx;
                        if (r1 < 1.0)
                            dx = sqrt(r1) - 1.0;
                        else
                            dx = 1.0 - sqrt(2.0 - r1);

                        double dy;
                        if (r2 < 1.0)
                            dy = sqrt(r2) - 1.0;
                        else
                            dy = 1.0 - sqrt(2.0 - r2);

                        /* Ray direction into scene from camera through sample */
                        Vector dir = cx * ((x + (sx + 0.5 + dx) / 2.0) / width - 0.5) +
                                     cy * ((y + (sy + 0.5 + dy) / 2.0) / height - 0.5) + 
                                     camera.dir;
                        
                        /* Extend camera ray to start inside box */
                        Vector start = camera.org + dir * 130.0;

                        /* Determine constant radiance */
                        accumulated_radiance = accumulated_radiance + 
                            Radiance(Ray(start, dir.Normalized()), 0, false) / samples;

                        /* Determine interpolated radiance */
                        accumulated_radiance2 = accumulated_radiance2 + 
                            Radiance(Ray(start, dir.Normalized()), 0, true) / samples;
                    }

                    img.addColor(x, y, accumulated_radiance);
                    img_interpolated.addColor(x, y, accumulated_radiance2); 
                 }
            }
        }
    }

    cout << endl;
	
    img.Save(string("image_patches.ppm"));
    img_interpolated.Save(string("image_smooth.ppm"));
}
