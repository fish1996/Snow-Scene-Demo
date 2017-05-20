#include"collision.h"
#include<math.h>
const float PI = 3.1415926536f;

float min(float x, float y)
{
	return x < y ? x : y;
}

float max(float x, float y)
{
	return x > y ? x : y;
}

struct dot {
	float x;
	float y;
	dot(float _x, float _y) :x(_x), y(_y) { }
};

double Direction(dot pi, dot pj, dot pk) {
	return (pk.x - pi.x)*(pj.y - pi.y) - (pj.x - pi.x)*(pk.y - pi.y);
}

bool OnSegment(dot pi, dot pj, dot pk) {
	if ((min(pi.x, pj.x) <= pk.x) && (pk.x <= max(pi.x, pj.x))
		&& (min(pi.y, pj.y) <= pk.y) && (pk.y <= max(pi.y, pj.y)))
		return true;
	else return false;
}

bool SegmentIntersect(dot p1, dot p2, dot p3, dot p4)
{
	int d1, d2, d3, d4;
	d1 = Direction(p3, p4, p1);
	d2 = Direction(p3, p4, p2);
	d3 = Direction(p1, p2, p3);
	d4 = Direction(p1, p2, p4);
	if (((d1>0 && d2<0) || (d1<0 && d2>0)) && ((d3>0 && d4<0) || (d3<0 && d4>0)))
		return true;
	else if (d1 == 0 && OnSegment(p3, p4, p1))
		return true;
	else if (d2 == 0 && OnSegment(p3, p4, p2))
		return true;
	else if (d3 == 0 && OnSegment(p1, p2, p3))
		return true;
	else if (d4 == 0 && OnSegment(p1, p2, p4))
		return true;
	else return false;
}
/*
float abs(float x)
{
	return x >= 0 ? x : -x;
}*/

void move(float* eye, float* center, direction dir)
{
	const float d = 1.0f;
	float x1, x2, y1, y2, x, y;
	x1 = eye[0], y1 = eye[2], x2 = center[0], y2 = center[2];
	float len = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
	if (x2 - x1<0.1f&&x2 - x1>-0.1f)x2 = x1;
	if (y2 - y1 < 0.1f&&y2 - y1>-0.1f)y2 = y1;
	switch (dir) {
	case front: {
		eye[0] = d*(x2 - x1) / len + x1;
		eye[2] = d*(y2 - y1) / len + y1;
		center[0] = eye[0] + x2 - x1;
		center[2] = eye[2] + y2 - y1;
		break;
	}
	case back: {
		eye[0] = d*(x1 - x2) / len + x1;
		eye[2] = d*(y1 - y2) / len + y1;
		center[0] = eye[0] + x2 - x1;
		center[2] = eye[2] + y2 - y1;
		break;
	}
	case left: {
		eye[0] = d*(y2 - y1) / len + x1;
		eye[2] = d*(x1 - x2) / len + y1;
		center[0] = eye[0] + x2 - x1;
		center[2] = eye[2] + y2 - y1;
		break;
	}
	case right: {
		eye[0] = d*(y1 - y2) / len + x1;
		eye[2] = d*(x2 - x1) / len + y1;
		center[0] = eye[0] + x2 - x1;
		center[2] = eye[2] + y2 - y1;

		break;
	}
	default:break;
	}
	return;
}

void rotate(float* eye, float* center, direction dir)
{
	const float alpha = 1.0f / (10 * PI);
	float x1, x2, y1, y2;
	x1 = eye[0], y1 = eye[2], x2 = center[0], y2 = center[2];
	switch (dir) {
	case left: {
		center[0] = x1 + (x2 - x1)*cos(alpha) + (y2 - y1)*sin(alpha);
		center[2] = y1 + (y2 - y1)*cos(alpha) - (x2 - x1)*sin(alpha);
		break;
	}
	case right: {
		center[0] = x1 + (x2 - x1)*cos(alpha) - (y2 - y1)*sin(alpha);
		center[2] = y1 + (y2 - y1)*cos(alpha) + (x2 - x1)*sin(alpha);
		break;
	}
	default:break;
	}
}


void outCollisionTest(float* eye, float* center, float x1, float x2, float z1, float z2)
{

	if (x1 < 0)x1 += 2;
	else x1 -= 2;

	if (x2 < 0)x2 += 2;
	else x2 -= 2;

	if (z1 < 0)z1 += 2;
	else z1 -= 2;

	if (z2 < 0)z2 += 2;
	else z2 -= 2;

	if (center[0] < x1) {
		center[0] = x1;
	}

	if (center[0] > x2) {
		center[0] = x2;
	}
	if (center[2] < z1) {
		center[2] = z1;
	}
	if (center[2] > z2) {
		center[2] = z2;
	}

	float distance = sqrt((eye[0] - center[0])*(eye[0] - center[0]) +
		(eye[2] - center[2])*(eye[2] - center[2]));

	if (distance <= 2.0f) {
		eye[0] = 2.0f*(eye[0] - center[0]) / distance + center[0];
		eye[2] = 2.0f*(eye[2] - center[2]) / distance + center[2];
	}
	bool flag = false;
	if (eye[0] < x1) {
		flag = true;
		eye[0] = x1;
	}
	if (eye[0] > x2) {
		flag = true;
		eye[0] = x2;
	}
	if (eye[2] < z1) {
		flag = true;
		eye[2] = z1;
	}
	if (eye[2] > z2) {
		flag = true;
		eye[2] = z2;
	}
	if (flag) {
		distance = sqrt((eye[0] - center[0])*(eye[0] - center[0]) +
			(eye[2] - center[2])*(eye[2] - center[2]));

		if (distance <= 2.0f) {
			center[0] = 2.0f*(center[0] - eye[0]) / distance + eye[0];
			center[2] = 2.0f*(center[2] - eye[2]) / distance + eye[2];
		}
	}
	return;
}

void inCollisionTest(float* eye, float* center, float x1, float z1,
	float x2, float z2)
{

	//printf("%f,%f,%f,%f\n", center[0], center[2], eye[0], eye[2]);  

	const float d = 2.0f;
	float _x1 = center[0], _x2 = eye[0], _z1 = center[2], _z2 = eye[2];
	float len = sqrt((_x2 - _x1)*(_x2 - _x1) + (_z2 - _z1)*(_z2 - _z1));

	dot d1(eye[0], eye[2]), d2(center[0], center[2]);
	dot d3(x1, z1), d4(x1, z2), d5(x2, z1), d6(x2, z2);

	if (SegmentIntersect(d1, d2, d4, d6)) {
		if (center[2] < eye[2]) {
			center[0] = _x1 + (_x2 - _x1)*(z2 - _z1) / (_z2 - _z1);
			center[2] = z2;
			eye[0] = center[0] + d*(_x2 - _x1) / len;
			eye[2] = center[2] + d*(_z2 - _z1) / len;
		}
		else if (center[2] > eye[2]) {

			eye[0] = _x2 + (_x1 - _x2)*(z2 - _z2) / (_z1 - _z2);
			eye[2] = z2;
			center[0] = eye[0] + d*(_x1 - _x2) / len;
			center[2] = eye[2] + d*(_z1 - _z2) / len;
		}
	}

	else if (SegmentIntersect(d1, d2, d5, d6)) {
		if (center[0]<eye[0]) {
			center[0] = x2;
			center[2] = _z1 + (_z2 - _z1)*(x2 - _x1) / (_x2 - _x1);
			eye[0] = center[0] + d*(_x2 - _x1) / len;
			eye[2] = center[2] + d*(_z2 - _z1) / len;
		}
		else if (center[0]>eye[0]) {
			eye[0] = x2;
			eye[2] = _z2 + (_z1 - _z2)*(x2 - _x2) / (_x1 - _x2);
			center[0] = eye[0] + d*(_x1 - _x2) / len;
			center[2] = eye[2] + d*(_z1 - _z2) / len;
		}
	}


	else if (SegmentIntersect(d1, d2, d3, d5)) {
		if (center[2] > eye[2]) {
			center[0] = _x1 + (_x2 - _x1)*(z1 - _z1) / (_z2 - _z1);
			center[2] = z1;
			eye[0] = center[0] + d*(_x2 - _x1) / len;
			eye[2] = center[2] + d*(_z2 - _z1) / len;
		}
		else if (center[2] < eye[2]) {
			eye[0] = _x2 + (_x1 - _x2)*(z1 - _z2) / (_z1 - _z2);
			eye[2] = z1;
			center[0] = eye[0] + d*(_x1 - _x2) / len;
			center[2] = eye[2] + d*(_z1 - _z2) / len;
		}
	}

	else if (SegmentIntersect(d1, d2, d3, d4)) {
		if (center[0] > eye[0]) {
			center[0] = x1;
			center[2] = _z1 + (_z2 - _z1)*(x1 - _x1) / (_x2 - _x1);
			eye[0] = center[0] + d*(_x2 - _x1) / len;
			eye[2] = center[2] + d*(_z2 - _z1) / len;
		}
		else if (center[0] < eye[0]) {
			eye[0] = x1;
			eye[2] = _z2 + (_z1 - _z2)*(x1 - _x2) / (_x1 - _x2);
			center[0] = eye[0] + d*(_x1 - _x2) / len;
			center[2] = eye[2] + d*(_z1 - _z2) / len;
		}
	}
	else if (_x1 - _x2 < 0.1f&&_x2 - _x1 >= -0.1f
		&& ((_z1>z1&&_z1<z2) || (_z2>z1&&_z2<z2))) {
		if (_x1 > x1&&_x1 < (x1 + x2) / 2) {

			center[0] = x1;
			eye[0] = x1;
		}
		else if (_x1 >(x1 + x2) / 2 && _x1 < x2) {

			center[0] = x2;
			eye[0] = x2;
		}
	}
	else if (_z1 - _z2 < 0.1f&&_z2 - _z1 >= -0.1f
		&& ((_x1>x1&&_x1<x2) || (_x2>x1&&_x2<x2))) {
		if (_z1 > z1&&_z1 < (z1 + z2) / 2) {

			center[2] = z1;
			eye[2] = z1;
		}
		else if (_z1 >(z1 + z2) / 2 && _z1 < z2) {

			center[2] = z2;
			eye[2] = z2;
		}
	}
}