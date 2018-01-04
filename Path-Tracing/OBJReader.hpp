#ifndef _OBJREADER_H_
#define _OBJREADER_H_

#include "Structs.hpp"

#include <sstream>

using namespace std;

vector<Triangle> loadOBJ(string filepath, Color c, Refl_t r);

#endif // _OBJREADER_H_