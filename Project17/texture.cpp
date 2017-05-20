#define _CRT_SECURE_NO_WARNINGS  
#include <stdio.h>              // 标准输入输出头文件  
#include <olectl.h>              // OLE控制库头文件  
#include <math.h>              // 数学函数头文件  

#include "texture.h"  

bool BuildTexture(char *szPathName, GLuint &texid)      // 载入图片并转换为纹理  
{
	HDC   hdcTemp;            // DC用来保存位图  
	HBITMAP  hbmpTemp;            // 保存临时位图  
	IPicture *pPicture;            // 定义IPicture Interface  
	OLECHAR  wszPath[MAX_PATH + 1];         // 图片的完全路径  
	char  szPath[MAX_PATH + 1];          // 图片的完全路径  
	long  lWidth;             // 图像宽度  
	long  lHeight;            // 图像高度  
	long  lWidthPixels;           // 图像的宽带(以像素为单位)  
	long  lHeightPixels;           // 图像的高带(以像素为单位)  
	GLint  glMaxTexDim;           // 保存纹理的最大尺寸  

	if (strstr(szPathName, "http://"))         // 如果路径包含 http:// 则...  
	{
		strcpy(szPath, szPathName);          // 把路径拷贝到 szPath  
	}
	else                // 否则从文件导入图片  
	{
		GetCurrentDirectory(MAX_PATH, szPath);       // 取得当前路径  
		strcat(szPath, "\\");           // 添加字符"\"  
		strcat(szPath, szPathName);          // 添加图片的相对路径  
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);  // 把ASCII码转化为Unicode标准码  
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if (FAILED(hr))              // 如果导入失败  
	{
		// 图片载入失败出错信息  
		MessageBox(HWND_DESKTOP, "图片导入失败!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // 返回 FALSE  
	}

	hdcTemp = CreateCompatibleDC(GetDC(0));        // 建立窗口设备描述表  
	if (!hdcTemp)              // 建立失败?  
	{
		pPicture->Release();           // 释放IPicture  
									   // 图片载入失败出错信息  
		MessageBox(HWND_DESKTOP, "图片导入失败!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // 返回 FALSE  
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);     // 取得支持的纹理最大尺寸  

	pPicture->get_Width(&lWidth);          // 取得IPicture 宽度 (转换为Pixels格式)  
	lWidthPixels = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);          // 取得IPicture 高度 (转换为Pixels格式)  
	lHeightPixels = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// 调整图片到最好的效果  
	if (lWidthPixels <= glMaxTexDim)         // 图片宽度是否超过显卡最大支持尺寸  
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels) / log(2.0f)) + 0.5f);
	else                // 否则,将图片宽度设为显卡最大支持尺寸  
		lWidthPixels = glMaxTexDim;

	if (lHeightPixels <= glMaxTexDim)         // 图片高度是否超过显卡最大支持尺寸  
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels) / log(2.0f)) + 0.5f);
	else                // 否则,将图片高度设为显卡最大支持尺寸  
		lHeightPixels = glMaxTexDim;

	// 建立一个临时位图  
	BITMAPINFO bi = { 0 };            // 位图的类型  
	DWORD  *pBits = 0;            // 指向位图Bits的指针  

	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    // 设置结构大小  
	bi.bmiHeader.biBitCount = 32;         // 32 位  
	bi.bmiHeader.biWidth = lWidthPixels;       // 宽度像素值  
	bi.bmiHeader.biHeight = lHeightPixels;      // 高度像素值  
	bi.bmiHeader.biCompression = BI_RGB;        // RGB 格式  
	bi.bmiHeader.biPlanes = 1;         // 一个位平面  

									   // 建立一个位图这样我们可以指定颜色和深度 并访问每位的值  
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);

	if (!hbmpTemp)              // 建立失败?  
	{
		DeleteDC(hdcTemp);            // 删除设备描述表  
		pPicture->Release();           // 释放IPicture  
									   // 图片载入失败出错信息  
		MessageBox(HWND_DESKTOP, "图片导入失败!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // 返回 FALSE  
	}

	SelectObject(hdcTemp, hbmpTemp);         // 选择临时DC句柄和临时位图对象  

											 // 在位图上绘制IPicture  
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// 将BGR转换为RGB　将ALPHA值设为255  
	for (long i = 0; i < lWidthPixels * lHeightPixels; i++)    // 循环遍历所有的像素  
	{
		BYTE* pPixel = (BYTE*)(&pBits[i]);       // 获取当前像素  
		BYTE  temp = pPixel[0];         // 临时存储第一个颜色像素(蓝色)  
		pPixel[0] = pPixel[2];         // 将红色值存到第一位  
		pPixel[2] = temp;           // 将蓝色值存到第三位  
		pPixel[3] = 255;           // ALPHA值设为255  
	}

	glGenTextures(1, &texid);           // 创建纹理  

										// 使用来自位图数据生成 的典型纹理  
	glBindTexture(GL_TEXTURE_2D, texid);        // 绑定纹理  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 线形滤波  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // 线形滤波  

																		// 生成纹理  
	glTexImage2D(GL_TEXTURE_2D, 0, 3, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);

	DeleteObject(hbmpTemp);            // 删除对象  
	DeleteDC(hdcTemp);             // 删除设备描述表  

	pPicture->Release();            // 释放 IPicture  

	return true;              // 返回 TRUE  
}

bool BuildTexture(char *filename, TextureTga *texture)     // 载入一个.TGA 文件到内存  
{
	GLubyte  TGAheader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };    // 没有压缩的TGA Header  
	GLubyte  TGAcompare[12];           // 用来比较 TGA Header  
	GLubyte  header[6];            // Header里，头六个有用字节  
	GLuint  bytesPerPixel;           // 保存TGA文件里每个像素用到的字节数  
	GLuint  imageSize;            // 用来保存随机产生的图像的大小  
	GLuint  temp;             // 临时变量  
	GLuint  type = GL_RGBA;           // 将默认的GL模式设置为RBGA (32 BPP)  

	FILE *file = fopen(filename, "rb");         // 打开 TGA 文件  

	if (file == NULL)             // 文件是否已存在?  
	{
		// 图片载入失败出错信息  
		MessageBox(HWND_DESKTOP, "图片导入失败!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // 返回FALSE  
	}

	if (fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare)// 是否有十二个字节可读?  
		|| memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0   // header和我们想要的是否相符?  
		|| fread(header, 1, sizeof(header), file) != sizeof(header))  // 如果是读下六个字节  
	{
		fclose(file);             // 如果失败,关闭文件  
								  // 图片载入失败出错信息  
		MessageBox(HWND_DESKTOP, "图片导入失败!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // 返回FALSE  
	}

	texture->width = header[1] * 256 + header[0];      // 确定的TGA 宽度 (高字节*256+低字节)  
	texture->height = header[3] * 256 + header[2];      // 确定的TGA 高度 (高字节*256+低字节)  

	if (texture->width <= 0           // 宽度是否小于等于0  
		|| texture->height <= 0          // 高度是否小于等于0  
		|| (header[4] != 24 && header[4] != 32))       // TGA 是24位或32位?  
	{
		fclose(file);             // 任何一个不成立, 则关闭文件  
								  // 图片载入失败出错信息  
		MessageBox(HWND_DESKTOP, "图片导入失败!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // 返回FALSE  
	}

	texture->bpp = header[4];          // 获取TGA每个像素的位(24 or 32)  
	bytesPerPixel = texture->bpp / 8;         // 除以8以取得 每个像素的字节  
	imageSize = texture->width*texture->height*bytesPerPixel;  // 计算TAG数据所需要的内存  

	texture->imageData = (GLubyte *)malloc(imageSize);     // 开辟一个内存空间用来存储TGA数据  

	if (texture->imageData == NULL           // 用来存储的内存是否存在?  
		|| fread(texture->imageData, 1, imageSize, file) != imageSize) // 图像大小是否和内存空间大小相符？  
	{
		if (texture->imageData != NULL)         // 图像数据是否载入  
		{
			free(texture->imageData);         // 如果是 释放图像数据  
		}
		fclose(file);             // 关闭文件  
		MessageBox(HWND_DESKTOP, "图片导入失败!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // 返回FALSE  
	}

	for (GLuint i = 0; i<int(imageSize); i += bytesPerPixel)    // 循环遍历图像数据  
	{                 // 交换第一和第三字节(红 和 蓝)  
		temp = texture->imageData[i];          // 将图像数据‘i’的值存在临时变量中  
		texture->imageData[i] = texture->imageData[i + 2];    // 将第三个字节的值存到第一个字节里  
		texture->imageData[i + 2] = temp;        // 将临时变量的值存入第三字节(第一字节的值)  
	}

	fclose(file);              // 关闭文件  

							   //创建一种纹理  
	glGenTextures(1, &texture->texID);         // 产生OpenGL纹理ID  

	glBindTexture(GL_TEXTURE_2D, texture->texID);      // 绑定纹理  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 线性滤波  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 线性滤波  

	if (texture->bpp == 24)            // TGA图片是不是 24 位的  
	{
		type = GL_RGB;             // 如果是将'type'设置为 GL_RGB  
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture->width, texture->height, 0, type, GL_UNSIGNED_BYTE, texture->imageData);

	return true;              // 纹理创建成功，返回正确  
}