#pragma once
#include"spline.h"
#include<vector>
#define GLUT_DISABLE_ATEXIT_HACK  

#include "glut.h"
using namespace std;
class butterfly {
private:
	int   tex;		
	float x;		
	float y;		
	float z;		
	float dx;
	float dy;
	float yi;		
	float spinz;	
	float spinzi;	
	float flap;		
	float fi;		
	int index;
	int id;
	static spline* line;
	static int texture[3];
	friend class butterflyManager;
public:
	void set(int _index);
	void update();
	void show();
	butterfly(int i);
};

class butterflyManager
{
private:
	int num;
	int texture[3];
	int cnt;
	bool isGen;
	spline* line;
	point *p;
	vector<butterfly>obj;
	void genPath();
public:
	void show();
	bool isDraw;
	butterflyManager(int* tex);
}; 