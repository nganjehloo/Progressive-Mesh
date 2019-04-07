#pragma once
#include "Vertex.h"
class Pair
{
public:
	Pair();
	Vertex * from;
	Vertex * to;
	float cost;
	~Pair();
};

