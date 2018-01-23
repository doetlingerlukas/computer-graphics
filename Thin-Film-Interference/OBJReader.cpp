#include "OBJReader.hpp"

vector<Triangle> loadOBJ(string filepath, Color c, Refl_t r) {
	
	ifstream obj_file(filepath);
	string line;
	
	vector<Vector> vertex;
	vector<Triangle> faces;
	
	while (getline(obj_file, line)) {
		
		if(line[0] == 'v' && line[1] != 't') {
			string v, x, y, z;
			
			istringstream iss( line );
			iss >> v >> x >> y >> z;
			
			vertex.push_back(Vector(atof(x.c_str()), atof(y.c_str()), atof(z.c_str())));
		} else if(line[0] == 'f') {
			string d, f1, f2, f3;
			
			istringstream iss( line );
			iss >> d;
			getline(iss, f1, '/');
			iss >> d;
			getline(iss, f2, '/');
			iss >> d;
			getline(iss, f3, '/');
			
			faces.push_back(Triangle(vertex[atof(f1.c_str()) - 1], 
				vertex[atof(f2.c_str()) - 1], vertex[atof(f3.c_str()) - 1], c, r));
		}
	}
	
	return faces;
}

vector<Triangle> translateOBJ(vector<Triangle> obj, Vector t_v) {
	
	vector<Triangle> to_return;
	
	for(Triangle t : obj) {
		to_return.push_back(Triangle(t.a + t_v, t.b + t_v, t.c + t_v, t.color, t.refl));
	}
	
	return to_return;
}

vector<Triangle> scaleOBJ(vector<Triangle> obj, double s) {
	
	vector<Triangle> to_return;
	
	for(Triangle t : obj) {
		to_return.push_back(Triangle(t.a * s, t.b * s, t.c * s, t.color, t.refl));
	}
	
	return to_return;
}


