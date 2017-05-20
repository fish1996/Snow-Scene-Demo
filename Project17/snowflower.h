#pragma once
#include "vec.h"
#include<list>
#include<vector>
#define GLUT_DISABLE_ATEXIT_HACK  
#include"glut.h"

using namespace std;

class snowflower
{
private:
	float x;
	float z;
	vec size;
	vec angle;
	void draw(GLuint texture);

public:
	snowflower(float _x, float _z,const vec& v,const vec& a);
	snowflower();
	void show(int texture);
};

class snow
{
private:
	int texture;
	int count;
	//GLint genList(snowflower* s,int begin,int end);
	list<snowflower> s;
public:
	snow(int t);
	void remove();
	void show();
	void add(float x,float z,const vec& v, const vec& a);
};