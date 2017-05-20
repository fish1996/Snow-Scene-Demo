#ifndef SPLINE_H  
#define SPLINE_H  

class point
{
public:
	float x;
	float y;
	void setPoint(float _x, float _y);
};

class spline
{
private:
	float *a[2], *b[2], *c[2], *d[2];//每段Spline曲线的参数  
	float *A, *B, *C, *D, *E;
	float m[16];//矩阵M  
	point* knots;
	point* Spline;
	int grain;
	int n;
	int count;
	float tension;
	void CubicSpline();
	void initLength();
	void GetCardinalMatrix();
	float f(int j, float x);

	float Matrix(int i, int j, float u);
	void init(int i, int j, float a0, float b0, float c0, float d0);
	float simpson(int j, float x, float y);
public:
	spline(point* p, int _n, int _grain, float _tension);
	~spline();
	void print();
	float getX(int i);
	float getY(int i);
	float getXFromU(int i, float u);
	float getYFromU(int i, float u);
	float getLen(int i, float u);

	float getU(int i, float s, float u1, float u2);
	int size();
};

#endif // SPLINE_H  