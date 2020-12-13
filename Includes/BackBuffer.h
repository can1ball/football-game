#ifndef BACKBUFFER_H
#define BACKBUFFER_H
#include "main.h"

class BackBuffer
{
public:
	BackBuffer(HWND hWnd, int width, int height);
	~BackBuffer();

	void present();
	void reset();

	HDC getDC() const { return mhDC; } 
	HWND getHWND() const { return mhWnd; }

	int width() const { return mWidth; }
	int height() const { return mHeight; }

private:
	BackBuffer(const BackBuffer& rhs);
	BackBuffer& operator=(const BackBuffer& rhs);

private:
	HWND mhWnd;
	HDC mhDC;
	HBITMAP mhSurface;
	HBITMAP mhOldObject;
	int mWidth;
	int mHeight;
};

#endif