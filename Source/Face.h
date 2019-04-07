#pragma once
#include <vector>
#include <unordered_map>
#include "Vertex.h"
class Vertex;
using namespace std;
class Face
{
public:
	vector< Vertex * > vertexindices;
	glm::vec3 normal;
	Face();
	~Face();
};

