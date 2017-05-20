#define _CRT_SECURE_NO_WARNINGS  
#include <stdio.h>              // ��׼�������ͷ�ļ�  
#include <olectl.h>              // OLE���ƿ�ͷ�ļ�  
#include <math.h>              // ��ѧ����ͷ�ļ�  

#include "texture.h"  

bool BuildTexture(char *szPathName, GLuint &texid)      // ����ͼƬ��ת��Ϊ����  
{
	HDC   hdcTemp;            // DC��������λͼ  
	HBITMAP  hbmpTemp;            // ������ʱλͼ  
	IPicture *pPicture;            // ����IPicture Interface  
	OLECHAR  wszPath[MAX_PATH + 1];         // ͼƬ����ȫ·��  
	char  szPath[MAX_PATH + 1];          // ͼƬ����ȫ·��  
	long  lWidth;             // ͼ����  
	long  lHeight;            // ͼ��߶�  
	long  lWidthPixels;           // ͼ��Ŀ��(������Ϊ��λ)  
	long  lHeightPixels;           // ͼ��ĸߴ�(������Ϊ��λ)  
	GLint  glMaxTexDim;           // ������������ߴ�  

	if (strstr(szPathName, "http://"))         // ���·������ http:// ��...  
	{
		strcpy(szPath, szPathName);          // ��·�������� szPath  
	}
	else                // ������ļ�����ͼƬ  
	{
		GetCurrentDirectory(MAX_PATH, szPath);       // ȡ�õ�ǰ·��  
		strcat(szPath, "\\");           // ����ַ�"\"  
		strcat(szPath, szPathName);          // ���ͼƬ�����·��  
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);  // ��ASCII��ת��ΪUnicode��׼��  
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if (FAILED(hr))              // �������ʧ��  
	{
		// ͼƬ����ʧ�ܳ�����Ϣ  
		MessageBox(HWND_DESKTOP, "ͼƬ����ʧ��!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // ���� FALSE  
	}

	hdcTemp = CreateCompatibleDC(GetDC(0));        // ���������豸������  
	if (!hdcTemp)              // ����ʧ��?  
	{
		pPicture->Release();           // �ͷ�IPicture  
									   // ͼƬ����ʧ�ܳ�����Ϣ  
		MessageBox(HWND_DESKTOP, "ͼƬ����ʧ��!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // ���� FALSE  
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);     // ȡ��֧�ֵ��������ߴ�  

	pPicture->get_Width(&lWidth);          // ȡ��IPicture ��� (ת��ΪPixels��ʽ)  
	lWidthPixels = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);          // ȡ��IPicture �߶� (ת��ΪPixels��ʽ)  
	lHeightPixels = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// ����ͼƬ����õ�Ч��  
	if (lWidthPixels <= glMaxTexDim)         // ͼƬ����Ƿ񳬹��Կ����֧�ֳߴ�  
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels) / log(2.0f)) + 0.5f);
	else                // ����,��ͼƬ�����Ϊ�Կ����֧�ֳߴ�  
		lWidthPixels = glMaxTexDim;

	if (lHeightPixels <= glMaxTexDim)         // ͼƬ�߶��Ƿ񳬹��Կ����֧�ֳߴ�  
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels) / log(2.0f)) + 0.5f);
	else                // ����,��ͼƬ�߶���Ϊ�Կ����֧�ֳߴ�  
		lHeightPixels = glMaxTexDim;

	// ����һ����ʱλͼ  
	BITMAPINFO bi = { 0 };            // λͼ������  
	DWORD  *pBits = 0;            // ָ��λͼBits��ָ��  

	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    // ���ýṹ��С  
	bi.bmiHeader.biBitCount = 32;         // 32 λ  
	bi.bmiHeader.biWidth = lWidthPixels;       // �������ֵ  
	bi.bmiHeader.biHeight = lHeightPixels;      // �߶�����ֵ  
	bi.bmiHeader.biCompression = BI_RGB;        // RGB ��ʽ  
	bi.bmiHeader.biPlanes = 1;         // һ��λƽ��  

									   // ����һ��λͼ�������ǿ���ָ����ɫ����� ������ÿλ��ֵ  
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);

	if (!hbmpTemp)              // ����ʧ��?  
	{
		DeleteDC(hdcTemp);            // ɾ���豸������  
		pPicture->Release();           // �ͷ�IPicture  
									   // ͼƬ����ʧ�ܳ�����Ϣ  
		MessageBox(HWND_DESKTOP, "ͼƬ����ʧ��!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // ���� FALSE  
	}

	SelectObject(hdcTemp, hbmpTemp);         // ѡ����ʱDC�������ʱλͼ����  

											 // ��λͼ�ϻ���IPicture  
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// ��BGRת��ΪRGB����ALPHAֵ��Ϊ255  
	for (long i = 0; i < lWidthPixels * lHeightPixels; i++)    // ѭ���������е�����  
	{
		BYTE* pPixel = (BYTE*)(&pBits[i]);       // ��ȡ��ǰ����  
		BYTE  temp = pPixel[0];         // ��ʱ�洢��һ����ɫ����(��ɫ)  
		pPixel[0] = pPixel[2];         // ����ɫֵ�浽��һλ  
		pPixel[2] = temp;           // ����ɫֵ�浽����λ  
		pPixel[3] = 255;           // ALPHAֵ��Ϊ255  
	}

	glGenTextures(1, &texid);           // ��������  

										// ʹ������λͼ�������� �ĵ�������  
	glBindTexture(GL_TEXTURE_2D, texid);        // ������  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // �����˲�  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // �����˲�  

																		// ��������  
	glTexImage2D(GL_TEXTURE_2D, 0, 3, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);

	DeleteObject(hbmpTemp);            // ɾ������  
	DeleteDC(hdcTemp);             // ɾ���豸������  

	pPicture->Release();            // �ͷ� IPicture  

	return true;              // ���� TRUE  
}

bool BuildTexture(char *filename, TextureTga *texture)     // ����һ��.TGA �ļ����ڴ�  
{
	GLubyte  TGAheader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };    // û��ѹ����TGA Header  
	GLubyte  TGAcompare[12];           // �����Ƚ� TGA Header  
	GLubyte  header[6];            // Header�ͷ���������ֽ�  
	GLuint  bytesPerPixel;           // ����TGA�ļ���ÿ�������õ����ֽ���  
	GLuint  imageSize;            // �����������������ͼ��Ĵ�С  
	GLuint  temp;             // ��ʱ����  
	GLuint  type = GL_RGBA;           // ��Ĭ�ϵ�GLģʽ����ΪRBGA (32 BPP)  

	FILE *file = fopen(filename, "rb");         // �� TGA �ļ�  

	if (file == NULL)             // �ļ��Ƿ��Ѵ���?  
	{
		// ͼƬ����ʧ�ܳ�����Ϣ  
		MessageBox(HWND_DESKTOP, "ͼƬ����ʧ��!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // ����FALSE  
	}

	if (fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare)// �Ƿ���ʮ�����ֽڿɶ�?  
		|| memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0   // header��������Ҫ���Ƿ����?  
		|| fread(header, 1, sizeof(header), file) != sizeof(header))  // ����Ƕ��������ֽ�  
	{
		fclose(file);             // ���ʧ��,�ر��ļ�  
								  // ͼƬ����ʧ�ܳ�����Ϣ  
		MessageBox(HWND_DESKTOP, "ͼƬ����ʧ��!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // ����FALSE  
	}

	texture->width = header[1] * 256 + header[0];      // ȷ����TGA ��� (���ֽ�*256+���ֽ�)  
	texture->height = header[3] * 256 + header[2];      // ȷ����TGA �߶� (���ֽ�*256+���ֽ�)  

	if (texture->width <= 0           // ����Ƿ�С�ڵ���0  
		|| texture->height <= 0          // �߶��Ƿ�С�ڵ���0  
		|| (header[4] != 24 && header[4] != 32))       // TGA ��24λ��32λ?  
	{
		fclose(file);             // �κ�һ��������, ��ر��ļ�  
								  // ͼƬ����ʧ�ܳ�����Ϣ  
		MessageBox(HWND_DESKTOP, "ͼƬ����ʧ��!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // ����FALSE  
	}

	texture->bpp = header[4];          // ��ȡTGAÿ�����ص�λ(24 or 32)  
	bytesPerPixel = texture->bpp / 8;         // ����8��ȡ�� ÿ�����ص��ֽ�  
	imageSize = texture->width*texture->height*bytesPerPixel;  // ����TAG��������Ҫ���ڴ�  

	texture->imageData = (GLubyte *)malloc(imageSize);     // ����һ���ڴ�ռ������洢TGA����  

	if (texture->imageData == NULL           // �����洢���ڴ��Ƿ����?  
		|| fread(texture->imageData, 1, imageSize, file) != imageSize) // ͼ���С�Ƿ���ڴ�ռ��С�����  
	{
		if (texture->imageData != NULL)         // ͼ�������Ƿ�����  
		{
			free(texture->imageData);         // ����� �ͷ�ͼ������  
		}
		fclose(file);             // �ر��ļ�  
		MessageBox(HWND_DESKTOP, "ͼƬ����ʧ��!\n(TextureLoad Failed!)", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;             // ����FALSE  
	}

	for (GLuint i = 0; i<int(imageSize); i += bytesPerPixel)    // ѭ������ͼ������  
	{                 // ������һ�͵����ֽ�(�� �� ��)  
		temp = texture->imageData[i];          // ��ͼ�����ݡ�i����ֵ������ʱ������  
		texture->imageData[i] = texture->imageData[i + 2];    // ���������ֽڵ�ֵ�浽��һ���ֽ���  
		texture->imageData[i + 2] = temp;        // ����ʱ������ֵ��������ֽ�(��һ�ֽڵ�ֵ)  
	}

	fclose(file);              // �ر��ļ�  

							   //����һ������  
	glGenTextures(1, &texture->texID);         // ����OpenGL����ID  

	glBindTexture(GL_TEXTURE_2D, texture->texID);      // ������  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // �����˲�  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // �����˲�  

	if (texture->bpp == 24)            // TGAͼƬ�ǲ��� 24 λ��  
	{
		type = GL_RGB;             // ����ǽ�'type'����Ϊ GL_RGB  
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture->width, texture->height, 0, type, GL_UNSIGNED_BYTE, texture->imageData);

	return true;              // �������ɹ���������ȷ  
}