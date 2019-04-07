#pragma once
#include <glm\glm.hpp>
#include <vector>
#include <unordered_map>
#include "Face.h"
class Face;
using namespace glm;
using namespace std;
class Vertex
{
public:
	vec3 vertex;
	vec3 normal;
	vector<Face *> faceindices;
	mat4 Q;
	float error = 0;
	Vertex();
	~Vertex();

};

