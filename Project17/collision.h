#pragma once
typedef enum { left, right, front, back }direction;
void move(float* eye, float* center, direction dir);
void rotate(float* eye, float* center, direction dir);
void inCollisionTest(float* eye, float* center, float x1, float z1,
	float x2, float z2);
void outCollisionTest(float* eye, float* center, float x1, float z1,
	float x2, float z2);