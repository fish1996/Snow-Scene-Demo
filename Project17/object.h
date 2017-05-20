#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include<map>
#include<set>

using namespace std;

typedef struct
{
	float x;
	float y;
	float z;
} VERTEX;

class Object {

public:
	typedef std::vector<VERTEX>::iterator viter;
	typedef std::vector< std::pair<float, float> >::iterator titer;
	typedef std::vector<VERTEX>::iterator niter;
	typedef std::vector<int>::iterator fiter;
	Object(viter vbeg, viter vend, titer tbeg, titer tend, niter nbeg, niter nend, fiter fbeg, fiter fend, int r, int c, std::string mat)
		:vertexs(vbeg, vend), texcoords(tbeg, tend), normals(nbeg, nend), faces(fbeg, fend), row(r), col(c), material(mat) {}
	Object() {}

	std::vector<VERTEX> vertexs;
	std::vector< std::pair<float, float> > texcoords;
	std::vector<VERTEX> normals;
	std::vector<int> faces;
	std::string material;
	int row, col;
};

class Material {

public:
	Material(float *a, float *d, float *s, float *e, unsigned m)
	{
		ambient[0] = *a;
		ambient[1] = *(a + 1);
		ambient[2] = *(a + 2);
		ambient[3] = 0;
		diffuse[0] = *d;
		diffuse[1] = *(d + 1);
		diffuse[2] = *(d + 2);
		diffuse[3] = 0;
		specular[0] = *s;
		specular[1] = *(s + 1);
		specular[2] = *(s + 2);
		specular[3] = 0;
		emission[0] = *e;
		emission[1] = *(e + 1);
		emission[2] = *(e + 2);
		emission[3] = 0;
		map = m;
	}
	Material()
	{
		ambient[0] = 0;
		ambient[1] = 0;
		ambient[2] = 0;
		ambient[3] = 0;
		diffuse[0] = 1.0;
		diffuse[1] = 1.0;
		diffuse[2] = 1.0;
		diffuse[3] = 0;
		specular[0] = 1.0;
		specular[1] = 1.0;
		specular[2] = 1.0;
		specular[3] = 0;
		emission[0] = 0;
		emission[1] = 0;
		emission[2] = 0;
		emission[3] = 0;
		map = 0;
	}
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emission[4];
	unsigned map;
};



inline bool startswith(string &a, string &b)
{
	if (a.size()<b.size())
	{
		return false;
	}
	return a.substr(0, b.size()) == b;
}

void readObj(string &cd, string file, map<string, Object> &m, set<string> &n, map<string, Material> &matname);
void loadObj(set<string>& objname, map<string, Object> &objmap, map<string, Material> &matname);

#endif