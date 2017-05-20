#pragma once  
#include "glut.h"
#include<stdio.h>  
struct TextureTga            // 建立一个结构体  
{
	GLubyte *imageData;           // 图像数据 (最高32bit)  
	GLuint bpp;            // 每一象素的图像颜色深度  
	GLuint width;            // 图像宽度  
	GLuint height;            // 图像高度  
	GLuint texID;            // 纹理ID  
};

// 载入BMP,JPG,GIF等文件  
bool BuildTexture(char *szPathName, GLuint &texid);

// 载入TGA文件  
bool BuildTexture(char *filename, TextureTga *texture); 
