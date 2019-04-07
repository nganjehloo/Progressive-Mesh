#include "Mesh.h"
#include <GL/glew.h>
#include <ctime>

Mesh::Mesh()
{
}

void Mesh::loadOffFile(const char* objectpath) {
	std::cout << "----LOADING OFF FILE----"  << std::endl;
	std::string offData;
	std::ifstream offFile;
	unsigned int numvertices = 0;
	unsigned int numfaces = 0;
	// ensures ifstream objects can throw exceptions:
	offFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		offFile.open(objectpath);
		getline(offFile, offData);
		if(offData.compare("OFF") != 0)
			std::cout << "ERROR::OFF::FILE_TYPE_NOT_OFF" << std::endl;

		getline(offFile, offData);
		std::istringstream iss(offData);

		iss >> offData;
		std::cout << "NUM VERTICES: " << offData << std::endl;
		numvertices = stoul(offData, nullptr);

		iss >> offData;
		std::cout << "NUM FACES: " << offData << std::endl;
		numfaces = stoul(offData, nullptr);

		std::cout << "LOADING VERTICES" <<  std::endl;
		for (int i = 0; i < numvertices; ++i) {
			getline(offFile, offData);
			//std::cout << offData << std::endl;
			std::istringstream iss(offData);
			string num1, num2, num3;
			vec3 vertpos;
			Vertex * vertex = new Vertex();
			float v1, v2, v3;

			iss >> num1 >> num2 >> num3;;
			v1 = stof(num1, nullptr);
			v2 = stof(num2, nullptr);
			v3 = stof(num3, nullptr);
			vertpos = vec3(v1, v2, v3);

			vertex->vertex = vertpos;
			vertices.push_back(vertex);
		}

		std::cout << "LOADING FACES" << std::endl;
		for (int i = 0; i < numfaces; ++i) {
			getline(offFile, offData);
			//std::cout << offData << std::endl;
			std::istringstream iss(offData);
			string trash, num1, num2, num3;
			vec3 vertpos;
			Face * face = new Face();
			unsigned int i1, i2, i3;

			iss >> trash >> num1 >> num2 >> num3;
			i1 = stoul(num1, nullptr);
			i2 = stoul(num2, nullptr);
			i3 = stoul(num3, nullptr);
	
			face->vertexindices.push_back(vertices[i1]);
			face->vertexindices.push_back(vertices[i2]);
			face->vertexindices.push_back(vertices[i3]);

			faces.push_back(face);
		}
			
		// close file handlers
		offFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::OFF::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	std::cout << "SETTING INITIAL VERTEX TO FACE ADJACENCY" << std::endl;
	for (int i = 0; i < numfaces; ++i) {
		for (int j = 0; j < 3; j++)
		{
			Vertex* vertidx = faces[i]->vertexindices[j];
			//cout << vertidx << endl;
			//cout << vertices[vertidx].vertex.x << " " << vertices[vertidx].vertex.y << " " << vertices[vertidx].vertex.z << endl;
			vertidx->faceindices.push_back(faces[i]);
		}
	}

	numVerts = numvertices;
	numFaces = numfaces;

	calcFaceNormals();
	calcVertexNormals();
	calcQuadrics();
	calcPairs();
	std::cout << "----DONE LOADING OFF FILE----" << std::endl;
}

void Mesh::setupMesh() {
	cout << "----SETTING UP MESH----" << endl;
	vertexarr.clear();
	for (int i = 0; i < faces.size(); ++i) {
		for (int j = 0; j < 3; ++j) {
			Vertex* idx = faces[i]->vertexindices[j];
			Vertex tempv = *idx;
			if (flatshading) {
				tempv.normal = faces[i]->normal;
			}
				vertexarr.push_back(tempv);
		}
	}

	/*for (int i = 0; i < vertexarr.size(); ++i) {
		cout << vertexarr[i].x << " " << vertexarr[i].y << " " << vertexarr[i].z << endl;
	}*/

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexarr.size() * sizeof(Vertex), vertexarr.data(), GL_STATIC_DRAW);

	//Position at layout location 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0);
	cout << "----DONE SETTING UP MESH----" << endl;

}

void Mesh::calcFaceNormals() {
	cout << "CALCULATING FACE NORMALS" << endl;
	for (int i = 0; i < faces.size(); ++i) {
		Vertex* idx1 = faces[i]->vertexindices[0];
		Vertex* idx2 = faces[i]->vertexindices[1];
		Vertex* idx3 = faces[i]->vertexindices[2];

		vec3 v1 = idx1->vertex;
		vec3 v2 = idx2->vertex;
		vec3 v3 = idx3->vertex;

		vec3 v21 = v2 - v1;
		vec3 v31 = v3 - v1;

		faces[i]->normal = normalize(cross(v21, v31));
	}
}



void Mesh::calcFaceNormal(Face * face) {
	Vertex* idx1 = face->vertexindices[0];
	Vertex* idx2 = face->vertexindices[1];
	Vertex* idx3 = face->vertexindices[2];

	vec3 v1 = idx1->vertex;
	vec3 v2 = idx2->vertex;
	vec3 v3 = idx3->vertex;

	vec3 v21 = v2 - v1;
	vec3 v31 = v3 - v1;

	vec3 temp = cross(v21, v31);
	if (temp.x != 0.0 && temp.y != 0.0 && temp.z != 0.0) {
		face->normal = normalize(temp);
	}else{
		face->normal = face->normal;
	}
}

void Mesh::calcVertexNormals() {
	cout << "CALCULATING VERTEX NORMALS" << endl;
	for (int i = 0; i < vertices.size(); ++i) {
		vec3 normsum = vec3(0.0f);
		unsigned int facesize = vertices[i]->faceindices.size();
		for (int j = 0; j < facesize; ++j) {
			normsum += vertices[i]->faceindices[j]->normal;
		}
		float scale = 1 / (float)facesize;
		normsum = normsum * scale;
		vertices[i]->normal = normsum;
	}
}

void Mesh::calcVertexNormal(Vertex * vert) {
	//cout << "another vert" << endl;
	vec3 normsum = vec3(0.0f);
	unsigned int facesize = vert->faceindices.size();
	for (int j = 0; j < facesize; ++j) {
		vec3 temp = vert->faceindices[j]->normal;
		//cout << "face norm" <<temp.x << " " << temp.y << " " << temp.z << endl;
		normsum += vert->faceindices[j]->normal;
		//cout << "normsum" << normsum.x << " " << normsum.y << " " << normsum.z << endl;
	}
	float scale = 1 / (float)facesize;
	normsum = normsum * scale;
	vert->normal = normsum;
}

void Mesh::edgeCollapse( Vertex * to, Vertex * from) {

	//REMOVE UNCESSARY FACES
	cout << "---CALLAPSING EDGE----" << endl;
	cout << to << " " << from << endl;
	cout << "PAIRS: " << pairs.size() << endl;
	cout << "FACES: " << faces.size() << endl;
	cout << "VERTS: " << vertices.size() << endl;

	Split split = Split();
	split.to = to;
	split.from = from;
	//add faces per vertex
	for (int i = 0; i < to->faceindices.size(); ++i) {
		split.tofaces.push_back(to->faceindices[i]);
	}

	for (int i = 0; i < from->faceindices.size(); ++i) {
		split.fromfaces.push_back(from->faceindices[i]);
	}

	for (int i = 0; i < to->faceindices.size(); ++i) {
		auto it1 = find(to->faceindices[i]->vertexindices.begin(), to->faceindices[i]->vertexindices.end(), to);
		auto it2 = find(to->faceindices[i]->vertexindices.begin(), to->faceindices[i]->vertexindices.end(), from);
		if (it1 != to->faceindices[i]->vertexindices.end() && it2 != to->faceindices[i]->vertexindices.end()) {
			split.commonfaces.push_back(to->faceindices[i]);
		}
	}


	//create new vertex @ midpoint for now
	Vertex * vnew = new Vertex();
	vnew->vertex = (to->vertex + from->vertex) / 2.0f;

	split.split = vnew;
	splits.push_back(split);

	cout << "New Vertex Created " << vnew->vertex.x << vnew->vertex.y << vnew->vertex.z << endl;
	//copy faces from to vector into new
	cout << to->faceindices.size() << endl;
	cout << from->faceindices.size() << endl;
	for (int i = 0; i < to->faceindices.size(); ++i) {
		Face * idx1 = to->faceindices[i];
		vnew->faceindices.push_back(idx1);
	}

	cout << "Copied to vertex faces to new vertex" << endl;
	//copy faces from from vector into new if already not there
	for (int i = 0; i < from->faceindices.size(); ++i) {
		vector<Face *>::iterator it = find(vnew->faceindices.begin(), vnew->faceindices.end(), from->faceindices[i]);
		if (it == vnew->faceindices.end()) {
			vnew->faceindices.push_back(from->faceindices[i]);
		}
	}
	cout << "Copied from vertex faces to new vertex" << endl;

	cout << "Set copied faces to have new vertex instead of old" << vnew->faceindices.size() << endl;
	for (int i = 0; i < vnew->faceindices.size(); ++i) {
		for (int j = 0; j < 3; ++j) {
			if (vnew->faceindices[i]->vertexindices[j] == to) {
				cout << "replacing to, in faces of vnew" << endl;
				vnew->faceindices[i]->vertexindices[j] = vnew;
			}
		}
	}
	for (int i = 0; i < vnew->faceindices.size(); ++i) {
		for (int j = 0; j < 3; ++j) {
			if (vnew->faceindices[i]->vertexindices[j] == from) {
				cout << "replacing from, in faces of vnew" << endl;
				vnew->faceindices[i]->vertexindices[j] = vnew;
			}
		}
	}

	//remove degenerate faces including fins by calculating area of adjacent faces
	for (int i = 0; i < vnew->faceindices.size(); ++i) {
		Face * face = vnew->faceindices[i];
		Vertex* idx1 = face->vertexindices[0];
		Vertex* idx2 = face->vertexindices[1];
		Vertex* idx3 = face->vertexindices[2];

		vec3 v1 = idx1->vertex;
		vec3 v2 = idx2->vertex;
		vec3 v3 = idx3->vertex;

		vec3 v21 = v2 - v1;
		vec3 v31 = v3 - v1;

		vec3 temp = cross(v21, v31);
		if (temp.x == 0.0 && temp.y == 0.0 && temp.z == 0.0) {
			auto it = find(faces.begin(), faces.end(), vnew->faceindices[i]);
			if (it != faces.end()) {
				cout << "removing degenerate faces from global list" << endl;
				faces.erase(it);
			}

			it = find(vnew->faceindices.begin(), vnew->faceindices.end(), vnew->faceindices[i]);
			if (it != vnew->faceindices.end()) {
				cout << "removing degenerate face from vnew" << endl;
				cout << "	vnew num faces: " << vnew->faceindices.size() << endl;
				cout << "	removing vnew face : " << i << endl;
				vnew->faceindices.erase(it);
			}
			
			it = find(idx1->faceindices.begin(), idx1->faceindices.end(),face);
			if (it != idx1->faceindices.end()) {
				cout << "removing degenerate faces from local list" << endl;
				idx1->faceindices.erase(it);
			}

			it = find(idx2->faceindices.begin(), idx2->faceindices.end(), face);
			if (it != idx2->faceindices.end()) {
				cout << "removing degenerate faces from local list" << endl;
				idx2->faceindices.erase(it);
			}

			it = find(idx3->faceindices.begin(), idx3->faceindices.end(), face);
			if (it != idx3->faceindices.end()) {
				cout << "removing degenerate faces from local list" << endl;
				idx3->faceindices.erase(it);
			}

		}
	}
	/*
	for (int i = 0; i < vnew->faceindices.size(); ++i) {
		int count = 0;
		for (int j = 0; j < 3; ++j) {
			//cout << vnew->faceindices[i]->vertexindices[j] << " " << from << endl;
			if (vnew->faceindices[i]->vertexindices[j] == vnew) {
				++count;
			}
		}
		if (count > 1) {
			auto it = find(faces.begin(), faces.end(), vnew->faceindices[i]);
			if (it != faces.end()) {
				cout << "removing degenerate faces" << endl;
				faces.erase(it);
				vnew->faceindices.erase(vnew->faceindices.begin() + i);
			}
		}
	}*/

	cout << "recalculating new face normals" << endl;
	for (int i = 0; i < vnew->faceindices.size(); ++i) {
		calcFaceNormal(vnew->faceindices[i]);
	}


	cout << "updating vnew error" << endl;
	vnew->Q = to->Q + from->Q;
	vnew->error = dot(vec4(vnew->vertex, 1.0f), (vnew->Q)*vec4(vnew->vertex, 1.0f));

	cout << "recalculating affected vertex normals" << endl;
	for (int i = 0; i < vnew->faceindices.size(); ++i) {
		for (int j = 0; j < 3; ++j) {
			calcVertexNormal(vnew->faceindices[i]->vertexindices[j]);
		}
	}
	cout << "adding new vert to list" << endl;
	vertices.push_back(vnew);


	cout << "calculating new pairs" << endl;

	//find pairs involving to and from and delete them then make new pairs with v1
	for (int i = 0; i < to->faceindices.size(); ++i) {
		for (int j = 0; j < 3; ++j) {
			Face * face = to->faceindices[i];
			if (face->vertexindices[j] != vnew) {
				Pair pair1 = Pair();
				pair1.from = to;
				pair1.to = face->vertexindices[j];
				pair1.cost = to->error + face->vertexindices[j]->error;

				Pair pair2 = Pair();
				pair2.from = face->vertexindices[j];
				pair2.to = to;
				pair2.cost = face->vertexindices[j]->error + to->error;

				auto it1 = pairs.find(pair1);

				if (it1 != pairs.end()) {
					cout << "deleting old pair" << endl;
					pairs.erase(it1);
				}

				auto it2 = pairs.find(pair2);
				if (it2 != pairs.end()) {
					cout << "deleting old pair" << endl;
					pairs.erase(it1);
				}
			}
		}
	}
	for (int i = 0; i < from->faceindices.size(); ++i) {
		for (int j = 0; j < 3; ++j) {
			Face * face = from->faceindices[i];
			if (face->vertexindices[j] != vnew) {
				Pair pair1 = Pair();
				pair1.from = from;
				pair1.to = face->vertexindices[j];
				pair1.cost = from->error + face->vertexindices[j]->error;

				Pair pair2 = Pair();
				pair2.from = face->vertexindices[j];
				pair2.to = from;
				pair2.cost = face->vertexindices[j]->error + from->error;

				auto it1 = pairs.find(pair1);

				if (it1 != pairs.end()) {
					cout << "deleting old pair" << endl;
					pairs.erase(it1);
				}

				auto it2 = pairs.find(pair2);
				if (it2 != pairs.end()) {
					cout << "deleting old pair" << endl;
					pairs.erase(it1);
				}
			}
		}
	}
	calcPair(vnew);

	cout << "deleting old verts from list" << endl;
	auto it1 = find(vertices.begin(), vertices.end(), to);

	if (it1 != vertices.end()) {
		cout << "deleting \"to\" from global list" << endl;
		vertices.erase(it1);
		//delete to;
	}

	it1 = find(vertices.begin(), vertices.end(), from);

	//cout << *it2 << endl;
	if (it1 != vertices.end()) {
		cout << "deleting \"from\" from global list" << endl;
		vertices.erase(it1);
		//delete from;
	}
	cout << "PAIRS: " << pairs.size() << endl;
	cout << "FACES: " << faces.size() << endl;
	cout << "VERTS: " << vertices.size() << endl;
	cout << "----DONE COLLAPSING EDGE----" << endl;
	//delete pointers to not waste mem
}

void Mesh::vertexSplit(Split vert) {
	cout << "----SPLITTING VERTEX----" << endl;
	cout << "SPLITS: " << splits.size() << endl;
	cout << "PAIRS: " << pairs.size() << endl;
	cout << "FACES: " << faces.size() << endl;
	cout << "VERTS: " << vertices.size() << endl;

	cout << "vnew: " << vert.split << endl;
	cout << "to: " << vert.to << endl;
	cout << "from: " << vert.from << endl;
	for (int i = 0; i < vert.tofaces.size(); ++i){
		auto it = find(faces.begin(), faces.end(), (vert.tofaces[i]));
		if(it == faces.end())
			faces.push_back(vert.tofaces[i]);
		int count = 0;
		for (int j = 0; j < 3; j++){
			cout << i << " " << vert.tofaces[i]->vertexindices[j] << endl;
			if (vert.tofaces[i]->vertexindices[j] == vert.split) {
				cout << "found vnew" << endl;
				if (count > 0) {
					vert.tofaces[i]->vertexindices[j] = vert.from;
				}else {
					count++;
					vert.tofaces[i]->vertexindices[j] = vert.to;
				}
			}
		}
	}
	cout << "vnew: " << vert.split << endl;
	cout << "to: " << vert.to << endl;
	cout << "from: " << vert.from << endl;
	for (int i = 0; i < vert.tofaces.size(); ++i) {
		auto it = find(faces.begin(), faces.end(), (vert.tofaces[i]));
		for (int j = 0; j < 3; j++) {
			cout << i << " " << vert.tofaces[i]->vertexindices[j] << endl;
		}
	}

	for (int i = 0; i < vert.fromfaces.size(); ++i) {
		auto it = find(faces.begin(), faces.end(), (vert.fromfaces[i]));
		if (it == faces.end())
			faces.push_back(vert.fromfaces[i]);
		int count = 0;
		for (int j = 0; j < 3; j++) {
			if (vert.fromfaces[i]->vertexindices[j] == vert.split) {
				if (count > 0) {
					vert.fromfaces[i]->vertexindices[j] = vert.to;
				}else {
					vert.fromfaces[i]->vertexindices[j] = vert.from;
					count++;
				}
			}
		}
	}
	vertices.push_back(vert.to);
	vertices.push_back(vert.from);
	//find common vert
	/*for (int i = 0; i < vert.to->faceindices.size(); ++i) {
		for (int j = 0; j < 3; ++j) {
			Vertex * v1 = vert.to->faceindices[i]->vertexindices[j];
			for (int ii = 0; ii < vert.to->faceindices.size(); ++ii) {
				Vertex
			}
		}
	}*/
	for (int i = 0; i < vert.commonfaces.size(); ++i) {
		faces.push_back(vert.commonfaces[i]);
	}
	calcFaceNormals();
	calcVertexNormals();
	calcPairs();
	
	cout << "----DONE SPLITTING VERTEX----" << endl;
}

void Mesh::calcQuadrics() {
	cout << "CALCULATING QUADRICS FOR ALL VERTS" << endl;
	for (int i = 0; i < vertices.size(); ++i) {
		calcQuadric(vertices[i]);
	}
}

void Mesh::calcQuadric(Vertex * vert) {
	mat4 Quad = mat4(0.0f);
	for (int i = 0; i < vert->faceindices.size(); ++i) {
		Face face = *(vert->faceindices[i]);
		Vertex v1 = *(face.vertexindices[0]);

		float d = dot(-v1.vertex, face.normal);
		vec4 P = vec4(face.normal, d);
		Quad += outerProduct(P, P);
	}
	vert->Q = Quad;
	vert->error = dot(vec4(vert->vertex, 1.0f), (vert->Q)*vec4(vert->vertex, 1.0f));
}

void Mesh::calcPairs() {
	cout << "CALCULATING PAIRS" << endl;
	pairs.clear();
	for (int i = 0; i < vertices.size(); ++i) {
		//cout << "making pair" << endl;
		calcPair(vertices[i]);
	}
}

void Mesh::calcPair(Vertex * vert) {
	for (int i = 0; i < vert->faceindices.size(); ++i){
		Face * face = vert->faceindices[i];
		for (int j = 0; j < 3; ++j) {
			if (face->vertexindices[j] != vert) {
				Pair pair1 = Pair();
				pair1.from = vert;
				pair1.to = face->vertexindices[j];
				pair1.cost = vert->error + face->vertexindices[j]->error;
				
				Pair pair2 = Pair();
				pair2.from = face->vertexindices[j];
				pair2.to = vert;
				pair2.cost = face->vertexindices[j]->error + vert->error;

				auto it1 = pairs.find(pair1);
				auto it2 = pairs.find(pair2);
					
				if (it1 == pairs.end() && it2 == pairs.end()) {
					//cout << "inserting new pair" << endl;
					pairs.insert(pair1);
				}
			}
		}
	}
}

void Mesh::drawMesh() {
	glBindVertexArray(VAO);
	// Pass the matrices to the shader
	if (wireframe){
		glDrawArrays(GL_POINTS, 0, vertexarr.size());
	}else{
		glDrawArrays(GL_TRIANGLES, 0, vertexarr.size());
	}
	glBindVertexArray(0);
}


Mesh::~Mesh()
{
}
