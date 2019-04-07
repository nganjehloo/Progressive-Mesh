#pragma once
#include "Vertex.h"
#include <vector>
class Split
{
public:
	Split();
	Vertex * split;
	Vertex * to;
	Vertex * from;
	vector<Face *> tofaces;
	vector<Face *> fromfaces;
	vector<Face *> commonfaces;
	~Split();
};

