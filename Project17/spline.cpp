#include "spline.h"  
#include<math.h>  

void point::setPoint(float _x, float _y)
{
	x = _x;
	y = _y;
}

void spline::initLength()
{
	A = new float[n - 1];
	B = new float[n - 1];
	C = new float[n - 1];
	D = new float[n - 1];
	E = new float[n - 1];
	for (int i = 0; i<n - 1; i++) {
		A[i] = 9 * (a[0][i] * a[0][i] + a[1][i] * a[1][i]);
		B[i] = 12 * (a[0][i] * b[0][i] + a[1][i] * b[1][i]);
		C[i] = 6 * (a[0][i] * c[0][i] + a[1][i] * c[1][i]) + 4 * (b[0][i] * b[0][i] + b[1][i] * b[1][i]);
		D[i] = 4 * (b[0][i] * c[0][i] + b[1][i] * c[1][i]);
		E[i] = c[0][i] * c[0][i] + c[1][i] * c[1][i];
	}
}

float spline::f(int i, float x)
{
	return sqrt(((((A[i] * x + B[i])*x) + C[i])*x + D[i])*x + E[i]);
}

float spline::simpson(int j, float x, float y)
{

	const int n = 10;
	const float h = (y - x) / n;
	float ans = 0.0f;
	for (int i = 1; i <= n - 1; i++) {
		if (i % 2) {
			ans += 4 * f(j, x + 1.0f*i / n*(y - x));
		}
		else ans += 2 * f(j, x + 1.0f*i / n*(y - x));
	}
	ans += f(j, x) + f(j, y);
	ans *= h / 3;

	return ans;
}

//µÚi¶Î£¬u  
float spline::getLen(int i, float u)
{
	return simpson(i, 0, u);
}

float spline::getU(int i, float s, float u1, float u2)
{
	float ms = getLen(i, (u1 + u2) / 2);
	if (ms - s>-1.0f && ms - s<1.0f) {
		return (u1 + u2) / 2;
	}
	else if (ms > s)return getU(i, s, u1, (u1 + u2) / 2);
	else if (ms < s)return getU(i, s, (u1 + u2) / 2, u2);
}

spline::spline(point* p, int _n, int _grain, float _tension)
{
	n = _n;
	grain = _grain;
	tension = _tension;

	knots = new point[n + 2];
	for (int i = 1; i <= n; i++) {
		knots[i].x = p[i - 1].x;
		knots[i].y = p[i - 1].y;
	}
	knots[0].x = p[0].x;
	knots[0].y = p[0].y;
	knots[n + 1].x = p[n - 1].x;
	knots[n + 1].y = p[n - 1].y;
	Spline = new point[(n - 1)* grain + 1];

	a[0] = new float[n - 1];
	b[0] = new float[n - 1];
	c[0] = new float[n - 1];
	d[0] = new float[n - 1];

	a[1] = new float[n - 1];
	b[1] = new float[n - 1];
	c[1] = new float[n - 1];
	d[1] = new float[n - 1];

	count = 0;
	CubicSpline();
	initLength();
}

int spline::size()
{
	return (n - 1)*grain + 1;
}

float spline::getX(int i)
{
	return Spline[i].x;
}

float spline::getY(int i)
{
	return Spline[i].y;
}

void spline::CubicSpline()
{
	point *s, *k0, *kml, *k1, *k2;
	int i, j;
	float* u = new float[grain];
	GetCardinalMatrix();
	for (i = 0; i<grain; i++) {
		u[i] = ((float)i) / grain;//u [0,1]  
	}
	s = Spline;
	kml = knots;
	k0 = kml + 1;
	k1 = k0 + 1;
	k2 = k1 + 1;
	for (i = 0; i<n - 1; i++) {
		init(0, i, kml->x, k0->x, k1->x, k2->x);
		init(1, i, kml->y, k0->y, k1->y, k2->y);
		for (j = 0; j<grain; j++) {
			s->x = Matrix(0, i, u[j]);
			s->y = Matrix(1, i, u[j]);
			s++;
		}
		k0++, kml++, k1++, k2++;
	}
	s->x = knots[n].x;
	s->y = knots[n].y;
	delete u;
}


void spline::GetCardinalMatrix()
{
	float a1 = tension;
	m[0] = -a1, m[1] = 2 - a1, m[2] = a1 - 2, m[3] = a1;
	m[4] = 2 * a1, m[5] = a1 - 3, m[6] = 3 - 2 * a1, m[7] = -a1;
	m[8] = -a1, m[9] = 0, m[10] = a1, m[11] = 0;
	m[12] = 0, m[13] = 1, m[14] = 0, m[15] = 0;
}

void spline::init(int i, int j, float a0, float b0, float c0, float d0)
{
	a[i][j] = m[0] * a0 + m[1] * b0 + m[2] * c0 + m[3] * d0;
	b[i][j] = m[4] * a0 + m[5] * b0 + m[6] * c0 + m[7] * d0;
	c[i][j] = m[8] * a0 + m[9] * b0 + m[10] * c0 + m[11] * d0;
	d[i][j] = m[12] * a0 + m[13] * b0 + m[14] * c0 + m[15] * d0;
}

//iÎª0£ºX£¬iÎª1£ºY  
//u  
float spline::Matrix(int i, int j, float u)
{
	return(d[i][j] + u*(c[i][j] + u*(b[i][j] + u*a[i][j])));
}

float spline::getXFromU(int i, float u)
{
	return Matrix(0, i, u);
}

float spline::getYFromU(int i, float u)
{
	return Matrix(1, i, u);
}

spline::~spline()
{
	delete[] knots;
	delete[] Spline;
}