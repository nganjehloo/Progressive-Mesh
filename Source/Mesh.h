#pragma once
#include "Vertex.h"
#include "Face.h"
#include <iostream>>
#include <vector>
#include <set>
#include <functional>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "Pair.h"
#include "Split.h"
using namespace std;

struct ComparePair {
	bool operator()(Pair const & p1, Pair const & p2) const{
		return p1.cost < p2.cost;
	}
};
class Mesh
{
public:
	Mesh();
	bool wireframe = false;
	bool flatshading = false;

	vector<Vertex*> vertices;
	vector<Vertex> vertexarr;
	vector<Face *> faces;
	set<Pair, ComparePair> pairs;
	vector<Split> splits;

	void loadOffFile(const char* modelpath);
	void setupMesh();
	void drawMesh();
	void calcFaceNormals();
	void calcFaceNormal(Face * face);
	void calcVertexNormals();
	void calcVertexNormal(Vertex * vert);
	void edgeCollapse(Vertex * to, Vertex * from);
	void vertexSplit(Split  vert);
	void calcQuadrics();
	void calcQuadric(Vertex * vert);
	void calcPair(Vertex * vert);
	void calcPairs();
	~Mesh();
private:
	GLuint NBO, VBO, VAO;
	unsigned int numVerts;
	unsigned int numFaces;
	GLfloat verticest[12] = {
		0.5f,  0.5f, 0.0f,  // Top Right
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  // Bottom Left
		-0.5f,  0.5f, 0.0f   // Top Left 
	};
};

