/******************************************************************
*
* PathTracing.cpp
*
* Description: This program demonstrates global illumination rendering
* based on the path tracing method. The intergral in the rendering
* equation is approximated via Monte-Carlo integration. Explicit 
* direct lighting is included to improve quality. The rendered image 
* is saved in PPM format.
*
* The code is largely based on the software smallpt by Kevin Beason,
* released under the MIT License.
* 
* Code was extended by Manuel Buchauer, Davide De Sclavis and Lukas Dötlinger
* during the Advanced Computer Graphics Proseminar WS 2017.
* 
* Interactive Graphics and Simulation Group
* Department of Computer Science
* University of Innsbruck
*
*******************************************************************/

/* Standard includes */
#include <algorithm>
#include <cmath>   
#include <cstdlib> 
#include <iostream>
#include <fstream>

#include "Structs.hpp"
#include "OBJReader.hpp"

using namespace std;

/* Film diameter and refraction index */
#define film_diameter 0.05
#define film_refraction_index 1.8

/******************************************************************
* Hard-coded scene definition: The geometry is composed of spheres
* and triangles.
* Scene definitions may also be loaded with the included OBJ loader.
*******************************************************************/
vector<Sphere> spheres = {
	
    Sphere(22.0, Vector(50, 22, 90), Vector(), Vector(1,1,1)*0.999,  OFILM), /* Outer sphere */

    Sphere( 1.5, Vector(50, 81.6-16.5-20, 161.6), Vector(4,4,4)*100, Vector(), DIFF), /* Light */
};

vector<Triangle> tris = {
  /* Cornell Box walls */
  Triangle(Vector(  0.0,  0.0,   0.0), Vector( 100.0, 0.0,    0.0), Vector(0.0,  80.0,    0.0), Color(), Color(0.75, 0.75, 0.75), DIFF), // Back:   bottom-left
  Triangle(Vector(100.0, 80.0,   0.0), Vector(-100.0, 0.0,    0.0), Vector(0.0, -80.0,    0.0), Color(), Color(0.75, 0.75, 0.75), DIFF), // Back:   top-right
  Triangle(Vector(  0.0,  0.0, 170.0), Vector( 100.0, 0.0,    0.0), Vector(0.0,   0.0, -170.0), Color(), Color(0.75, 0.75, 0.75), DIFF), // Bottom: front-left
  Triangle(Vector(100.0,  0.0,   0.0), Vector(-100.0, 0.0,    0.0), Vector(0.0,   0.0,  170.0), Color(), Color(0.75, 0.75, 0.75), DIFF), // Bottom: back-right
  Triangle(Vector(  0.0, 80.0,   0.0), Vector( 100.0, 0.0,    0.0), Vector(0.0,   0.0,  170.0), Color(), Color(0.75, 0.75, 0.75), DIFF), // Top:    back-left
  Triangle(Vector(100.0, 80.0, 170.0), Vector(-100.0, 0.0,    0.0), Vector(0.0,   0.0, -170.0), Color(), Color(0.75, 0.75, 0.75), DIFF), // Top:    front-right
  Triangle(Vector(  0.0,  0.0, 170.0), Vector(   0.0, 0.0, -170.0), Vector(0.0,  80.0,    0.0), Color(), Color(0.75, 0.25, 0.25), DIFF), // Left:   front-bottom
  Triangle(Vector(  0.0, 80.0,   0.0), Vector(   0.0, 0.0,  170.0), Vector(0.0, -80.0,    0.0), Color(), Color(0.75, 0.25, 0.25), DIFF), // Left:   back-top
  Triangle(Vector(100.0,  0.0,   0.0), Vector(   0.0, 0.0,  170.0), Vector(0.0,  80.0,    0.0), Color(), Color(0.25, 0.25, 0.75), DIFF), // Right:  back-bottom
  Triangle(Vector(100.0, 80.0, 170.0), Vector(   0.0, 0.0, -170.0), Vector(0.0, -80.0,    0.0), Color(), Color(0.25, 0.25, 0.75), DIFF), // Right:  front-top
  Triangle(Vector(100.0,  0.0, 170.0), Vector(-100.0, 0.0,    0.0), Vector(0.0,  80.0,    0.0), Color(), Color(0.75, 0.75, 0.75), DIFF), // Front:  bottom-right
  Triangle(Vector(  0.0, 80.0, 170.0), Vector( 100.0, 0.0,    0.0), Vector(0.0, -80.0,    0.0), Color(), Color(0.75, 0.75, 0.75), DIFF), // Front:  top-left
};

vector<Triangle> goat = 
	translateOBJ(scaleOBJ(loadOBJ("goat.obj", Color(0.75, 0.75, 0.75)*0.999, DIFF), 0.08), 
	Vector(40.0, 0.0, 80.0));

/******************************************************************
* Check for closest intersection of a ray with the scene.
* Returns true if intersection is found, as well as ray parameter
* of intersection and id of intersected object.
*******************************************************************/
bool intersectScene(const Ray &ray, double &t, size_t &id, Type &type) {
    t = 1e20;
	
	/* Check for intersection with spheres in scene. */
    for (size_t i = 0; i < spheres.size(); i ++) {
        double d = spheres[i].Intersect(ray);
        if (d > 0.0  && d < t) {
            t = d;
            id = i;
            type = SPH;
        }
    }
    /* Check for intersection with triangles in scene. */
    for (size_t i = 0; i < tris.size(); i ++) {
        double d = tris[i].intersect(ray);
        if (d > 0.0 && d < t) {
            t  = d;
            id = i;
            type = TRI;
        }
    }
    return t < 1e20;
}

/******************************************************************
* Function to sample a vector around a given vector based on
* an angle. Used for computation of glossy and translucent 
* materials in Radiance function.
*******************************************************************/
Vector sampleVector(Vector vec, double max_angle) {
	Vector sw = vec;
	Vector su = fabs(sw.x) > 0.1 ? Vector(0.0, 1.0, 0.0) : Vector(1.0, 0.0, 0.0);
	su = (su.Cross(sw)).Normalized();
	Vector sv = sw.Cross(su);
	
	double cos_a_max = max_angle;
	double eps1 = drand48();
	double eps2 = drand48();
	double cos_a = 1.0 - eps1 + eps1 * cos_a_max;
	double sin_a = sqrt(1.0 - cos_a * cos_a);
	double phi = 2.0*M_PI * eps2;
	Vector l = su * cos(phi) * sin_a + 
			   sv * sin(phi) * sin_a + 
			   sw * cos_a;
	return l.Normalized();
}

/******************************************************************
* Recursive path tracing for computing radiance via Monte-Carlo
* integration, considering diffuse or thin film materials.
* After 5 bounces Russian Roulette is used to possibly terminate rays. 
* Emitted light from light source only included on first direct hit.
* On diffuse surfaces light sources are explicitely sampled.
* For transparent and translucent objects, Schlickï¿½s approximation
* is employed.
* A more detailed explaination is to be found in the README.
*******************************************************************/
Color Radiance(const Ray &ray, int depth, int E, bool notInFilm, Wave wave) {
    depth++;

    double t;                               
    size_t id = 0; 
    Type description_type; 
    
    if (!intersectScene(ray, t, id, description_type)) {
        return Color(0.0, 0.0, 0.0); 
	}
	
	bool isSphere = description_type == SPH ? true : false;
	
	Sphere obj_s = isSphere ? spheres[id] : spheres[0];
	Triangle obj_t = isSphere ? tris[0] : tris[id];
	
	Color col = isSphere ? obj_s.color : obj_t.color;

	/* Intersection point */
    Vector hitpoint = ray.org + ray.dir * t;
    
    /* Calculate normals. */
    Vector normal = isSphere ? (hitpoint - obj_s.position).Normalized() : obj_t.normal;
    Vector nl = normal;
    if (normal.Dot(ray.dir) >= 0) 
        nl = nl.Invert();

    /* Maximum RGB reflectivity for Russian Roulette */
    double p = col.Max();
    if (depth > 5 || !p) {  /* After 5 bounces or if max reflectivity is zero */
	
        if (drand48() < p)            /* Russian Roulette */
            col = col * (1/p);        /* Scale estimator to remain unbiased */
        else 
			/* No further bounces, only return potential emission */
            return (isSphere ? obj_s.emission : obj_t.emission) * E;  
     }
     
     if ((isSphere ? obj_s.refl : obj_t.refl) == DIFF) {
			                  
        /* Compute random reflection vector on hemisphere */
        double r1 = 2.0 * M_PI * drand48(); 
        double r2 = drand48(); 
        double r2s = sqrt(r2); 
        
        /* Set up local orthogonal coordinate system u,v,w on surface */
        Vector w = nl; 
        Vector u = fabs(w.x) > 0.1 ? Vector(0.0, 1.0, 0.0) : Vector(1.0, 0.0, 0.0); 
        u = (u.Cross(w)).Normalized();
        Vector v = w.Cross(u);  

        /* Random reflection vector d */
        Vector d = (u * cos(r1) * r2s + 
                    v * sin(r1) * r2s + 
                    w * sqrt(1 - r2)).Normalized();  

        /** Explicit computation of direct lighting **/
        Vector e;
        for (size_t i = 0; i < spheres.size(); i ++) {
			
            Sphere sphere = spheres[i];
            if (sphere.emission.x <= 0 && sphere.emission.y <= 0 && sphere.emission.z <= 0
				&& !isSphere) 
                continue; /* Skip objects that are not light sources */
      
            /* Randomly sample spherical light source from surface intersection */
            /* Create random sample direction l towards spherical light source */
            double cos_a_max = sqrt(1.0 - sphere.radius * sphere.radius / 
                               (hitpoint - sphere.position).Dot(hitpoint-sphere.position));
            cos_a_max = cos_a_max != cos_a_max ? 1 : cos_a_max;
            
            Vector l = sampleVector(sphere.position - hitpoint,	cos_a_max);

            /* Shoot shadow ray, check if intersection is with light source */
            size_t index = id;
            double t_;
            Type temp_type;
            if (intersectScene(Ray(hitpoint,l), t_, index, temp_type) && index == i) {
					  
                double omega = 2*M_PI * (1 - cos_a_max);

                /* Add diffusely reflected light from light source; note constant BRDF 1/PI */
                e = e + col.MultComponents(sphere.emission * l.Dot(nl) * omega) / M_PI; 
            }
        }
        /* Return potential light emission, direct lighting, and indirect lighting (via
           recursive call for Monte-Carlo integration */      
        return (isSphere ? obj_s.emission : obj_t.emission)
			* E + e + col.MultComponents(Radiance(Ray(hitpoint,d), depth, 0, false, wave));
			
    }
    
    Ray reflRay (hitpoint, ray.dir - normal * 2 * normal.Dot(ray.dir)); 
    bool into = normal.Dot(nl) > 0;
    double nc = 1; 
    double nt = film_refraction_index;
	
	if (wave == R) {
		double vr = drand48();
		nt = nt + 0.01 + (vr/10) - 0.02;
	} else if (wave == G) {
		double vg = drand48();
		nt = nt + 0.07 + (vg/10) - 0.02;
	} else if (wave == B) {
		double vb = drand48();
		nt = nt + 0.13 + (vb/10) - 0.02;
	}
	
    double nnt = into ? nc/nt : nt/nc;
    double ddn = ray.dir.Dot(nl);
    double cos2t = 1 - nnt * nnt * (1 - ddn*ddn);

    /* Determine transmitted ray direction */    
    Vector tdir = into ?
        (ray.dir * nnt - normal * (ddn * nnt + sqrt(cos2t))) :
		(ray.dir * nnt + normal * (ddn * nnt + sqrt(cos2t)));
		
	
	/* Thin film calculations */
	double theta1 = acos((normal.Dot(ray.org - hitpoint))/
		(normal.Length() * (ray.org - hitpoint).Length()));
	double theta2 = asin(nnt * sin(theta1));
	Vector inner_hitpoint = hitpoint + tdir.Normalized() * (film_diameter/cos(theta2));
	Vector inner_normal = isSphere ? 
		(inner_hitpoint - obj_s.position).Normalized() : obj_t.normal;
	Vector inner_rdir = tdir - inner_normal * 2 * inner_normal.Dot(tdir);
    
    double t1 = acos((normal.Dot(inner_rdir))/(normal.Length() * (inner_rdir).Length()));
	double t2 = asin(nnt * sin(t1));
    Vector inner_hitpoint2 = hitpoint + reflRay.dir.Normalized() * (film_diameter/cos(t2));
    Vector inner_rdir2 = reflRay.dir - inner_normal * 2 * inner_normal.Dot(reflRay.dir);
 
    /* Thin film reflective */
    if ((isSphere ? obj_s.refl : obj_t.refl) == OFILM) {
		if (cos2t < 0) {
			return (isSphere ? obj_s.emission : obj_t.emission)
				+ col.MultComponents(Radiance(Ray(hitpoint, tdir), depth, 1, false, wave) * 0.5
				+ Radiance(Ray(inner_hitpoint2, inner_rdir2), depth, 1, false, wave) * 0.5);
		}
		if (depth < 2) {
			return (isSphere ? obj_s.emission : obj_t.emission)
				+ col.MultComponents(Radiance(reflRay, depth, 1, false, wave) * 0.5 
				+ Radiance(Ray(inner_hitpoint, inner_rdir), depth, 1, false, wave) * 0.5);
		} else {
			if (drand48() < 0.5)
				return (isSphere ? obj_s.emission : obj_t.emission)
					+ col.MultComponents(Radiance(reflRay, depth, 1, false, wave));
			else
				return (isSphere ? obj_s.emission : obj_t.emission)
					+ col.MultComponents(Radiance(Ray(inner_hitpoint, inner_rdir),
					depth, 1, false, wave));
		}
	}
    
    /* Reflect only at certain angles */
    bool reflect = true;
    if (theta1 < 4.5 || theta1 > 100) {
		reflect = false;
	}
    
    if (cos2t < 0) {
		if (notInFilm) {
			return (isSphere ? obj_s.emission : obj_t.emission)
				+ col.MultComponents(Radiance(reflRay, depth, 1, false, wave));
		}
		return (isSphere ? obj_s.emission : obj_t.emission)
			+ col.MultComponents((Radiance(Ray(hitpoint, tdir), depth, 1, false, wave)
			+ Radiance(Ray(inner_hitpoint2, inner_rdir2), depth, 1, false, wave)) * 0.5);
 
	} 
	if (reflect) {  
		return (isSphere ? obj_s.emission : obj_t.emission)
			+ col.MultComponents(Radiance(reflRay, depth, 1, false, wave) * 0.5 
			+ Radiance(Ray(inner_hitpoint, inner_rdir), depth, 1, false, wave) * 0.5);
	} else {
		return (isSphere ? obj_s.emission : obj_t.emission)
		+ col.MultComponents(Radiance(Ray(hitpoint, tdir), depth, 1, true, wave));
	}
}


/******************************************************************
* Main routine: Computation of path tracing image (2x2 subpixels).
* Key parameters:
* - Image dimensions: width, height 
* - Number of samples per subpixel (non-uniform filtering): samples 
* Rendered result saved as PPM image file.
*******************************************************************/

int main(int argc, char *argv[]) {
	
	/* Add loaded obj to triangle-rendering-vector */
	//for(Triangle t : goat) {tris.push_back(t);}
	
    int width = 1024;
    int height = 768;
    int samples = 1;
    bool thinLense = false;

    if(argc == 2)
        samples = atoi(argv[1]);  
	else if(argc == 3) {
		samples = atoi(argv[1]); 
		thinLense = argv[2][0] == 't' ? true : false;
	}
    
    /* Set camera origin and viewing direction (negative z direction) */
    Ray camera(Vector(50.0, 52.0, 295.6), Vector(0.0, -0.042612, -1.0).Normalized());
	
    /* Image edge vectors for pixel sampling */
    Vector cx = Vector(width * 0.5135 / height);
    Vector cy = (cx.Cross(camera.dir)).Normalized() * 0.5135;

    /* Final rendering */
    Image img(width, height);

    /* Loop over image rows */
    for (int y = 0; y < height; y ++) {
		 
        cout << "\rRendering (" << samples * 4 << " spp) " << (100.0 * y / (height - 1)) << "%     ";
        srand(y * y * y);
 
        /* Loop over row pixels */
        #pragma omp parallel for
        for (int x = 0; x < width; x ++)  
        {
            img.setColor(x, y, Color());
 
            /* 2x2 subsampling per pixel */
            for (int sy = 0; sy < 2; sy ++) 
            {
                for (int sx = 0; sx < 2; sx ++) 
                {
                    Color accumulated_radiance = Color();

                    /* Compute radiance at subpixel using multiple samples */
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
                        Vector start = camera.org + Vector(0,-10.0,-50.0) + dir * 130.0;

                        dir = dir.Normalized();

                        /* Accumulate radiance */
                        accumulated_radiance = accumulated_radiance + 
                            Color(Radiance(Ray(start, dir), 0, 1, thinLense, R).x,
								  Radiance(Ray(start, dir), 0, 1, thinLense, G).y,
								  Radiance(Ray(start, dir), 0, 1, thinLense, B).z) / samples;
                    } 
                    
                    accumulated_radiance = accumulated_radiance.clamp() * 0.25;

                    img.addColor(x, y, accumulated_radiance);
                }
            }
        }
    }
    cout << endl;

    img.Save(string("image.ppm"));
}
