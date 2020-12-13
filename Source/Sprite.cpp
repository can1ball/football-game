#include "Sprite.h"
extern HINSTANCE g_hInst;

Sprite::Sprite(int imageID, int maskID)
{
	mhImage = LoadBitmap(g_hInst, MAKEINTRESOURCE(imageID));
	mhMask = LoadBitmap(g_hInst, MAKEINTRESOURCE(maskID));

	GetObject(mhImage, sizeof(BITMAP), &mImageBM);
	GetObject(mhMask, sizeof(BITMAP), &mMaskBM);


	assert(mImageBM.bmWidth == mMaskBM.bmWidth);
	assert(mImageBM.bmHeight == mMaskBM.bmHeight);	

	mcTransparentColor = 0;
	mhSpriteDC = 0;
}
Sprite::Sprite(const char *szImageFile, const char *szMaskFile)
{
	mhImage = (HBITMAP)LoadImage(g_hInst, szImageFile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	mhMask = (HBITMAP)LoadImage(g_hInst, szMaskFile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	GetObject(mhImage, sizeof(BITMAP), &mImageBM);
	GetObject(mhMask, sizeof(BITMAP), &mMaskBM);

	assert(mImageBM.bmWidth == mMaskBM.bmWidth);
	assert(mImageBM.bmHeight == mMaskBM.bmHeight);

	mcTransparentColor = 0;
	mhSpriteDC = 0;
}
Sprite::Sprite(const char *szImageFile, COLORREF crTransparentColor)
{
	mhImage = (HBITMAP)LoadImage(g_hInst, szImageFile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	mhMask = 0;
	mhSpriteDC = 0;
	mcTransparentColor = crTransparentColor;

	GetObject(mhImage, sizeof(BITMAP), &mImageBM);
}
Sprite::~Sprite()
{
	// Free the resources we created in the constructor.
	DeleteObject(mhImage);
	DeleteObject(mhMask);

	DeleteDC(mhSpriteDC);
}

void Sprite::update(float dt)
{
	mPosition += mVelocity * dt;
}
void Sprite::setBackBuffer(const BackBuffer *pBackBuffer)
{
	mpBackBuffer = pBackBuffer;
	if(mpBackBuffer)
	{
		DeleteDC(mhSpriteDC);
		mhSpriteDC = CreateCompatibleDC(mpBackBuffer->getDC());
	}
}
void Sprite::draw()
{
	if( mhMask != 0 )
		drawMask();
	else
		drawTransparent();
}
void Sprite::drawMask()
{
	if( mpBackBuffer == NULL )
		return;

	HDC hBackBufferDC = mpBackBuffer->getDC();

	int w = width();
	int h = height();

	int x = (int)mPosition.x - (w / 2);
	int y = (int)mPosition.y - (h / 2);

	HGDIOBJ oldObj = SelectObject(mhSpriteDC, mhMask);
	BitBlt(hBackBufferDC, x, y, w, h, mhSpriteDC, 0, 0, SRCAND);
	SelectObject(mhSpriteDC, mhImage);
	BitBlt(hBackBufferDC, x, y, w, h, mhSpriteDC, 0, 0, SRCPAINT);
	SelectObject(mhSpriteDC, oldObj);
}
void Sprite::drawTransparent()
{
	if( mpBackBuffer == NULL )
		return;

	HDC hBackBuffer = mpBackBuffer->getDC();

	int w = width();
	int h = height();

	int x = (int)mPosition.x - (w / 2);
	int y = (int)mPosition.y - (h / 2);

	COLORREF crOldBack = SetBkColor(hBackBuffer, RGB(255, 255, 255));
	COLORREF crOldText = SetTextColor(hBackBuffer, RGB(0, 0, 0));

	HDC dcImage, dcTrans;
	dcImage=CreateCompatibleDC(hBackBuffer);
	dcTrans=CreateCompatibleDC(hBackBuffer);

	SelectObject(dcImage, mhImage);
	BITMAP bitmap;
	GetObject(mhImage, sizeof(BITMAP), &bitmap);
	HBITMAP bitmapTrans = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);

	SelectObject(dcTrans, bitmapTrans);
	SetBkColor(dcImage, mcTransparentColor);

	BitBlt(dcTrans, 0, 0, bitmap.bmWidth, bitmap.bmHeight, dcImage, 0, 0, SRCCOPY);
	BitBlt(hBackBuffer, x, y, bitmap.bmWidth, bitmap.bmHeight, dcImage, 0, 0, SRCINVERT);
	BitBlt(hBackBuffer, x, y, bitmap.bmWidth, bitmap.bmHeight, dcTrans, 0, 0, SRCAND);
	BitBlt(hBackBuffer, x, y, bitmap.bmWidth, bitmap.bmHeight, dcImage, 0, 0, SRCINVERT);

	DeleteDC(dcImage);
	DeleteDC(dcTrans);
	DeleteObject(bitmapTrans);

	SetBkColor(hBackBuffer, crOldBack);
	SetTextColor(hBackBuffer, crOldText);
}
RECT Sprite::RectPosition()
{
    RECT r;
    r.left   = mPosition.x - width() / 2;
    r.right  = mPosition.x + width() / 2;
    r.top    = mPosition.y - height() / 2;
    r.bottom = mPosition.y + height() / 2;
    return r;    
}

AnimatedSprite::AnimatedSprite(const char *szImageFile, const char *szMaskFile, const RECT& rcFirstFrame, int iFrameCount) 
			: Sprite (szImageFile, szMaskFile)
{
	mptFrameCrop.x = rcFirstFrame.left;
	mptFrameCrop.y = rcFirstFrame.top;
	mptFrameStartCrop = mptFrameCrop;
	miFrameWidth = rcFirstFrame.right - rcFirstFrame.left;
	miFrameHeight = rcFirstFrame.bottom - rcFirstFrame.top;
	miFrameCount = iFrameCount;
}
void AnimatedSprite::SetFrame(int iIndex, int index2)
{
	assert(iIndex >= 0 && iIndex < miFrameCount && "AnimatedSprite frame Index must be in range!");

	mptFrameCrop.x = mptFrameStartCrop.x + iIndex*miFrameWidth;
	mptFrameCrop.y = mptFrameStartCrop.y + index2*miFrameHeight;
}
void AnimatedSprite::draw()
{
	if( mpBackBuffer == NULL )
		return;

	int w = miFrameWidth;
	int h = miFrameHeight;

	HDC hBackBufferDC = mpBackBuffer->getDC();

	int x = (int)mPosition.x - (w / 2);
	int y = (int)mPosition.y - (h / 2);

	HGDIOBJ oldObj = SelectObject(mhSpriteDC, mhMask);
	BitBlt(hBackBufferDC, x, y, w, h, mhSpriteDC, mptFrameCrop.x, mptFrameCrop.y, SRCAND);
	SelectObject(mhSpriteDC, mhImage);
	BitBlt(hBackBufferDC, x, y, w, h, mhSpriteDC, mptFrameCrop.x, mptFrameCrop.y, SRCPAINT);
	SelectObject(mhSpriteDC, oldObj);
}
RECT AnimatedSprite::RectAnimatedSpritePosition()
{
    RECT r;
    r.left   = mPosition.x - width() / 2;
    r.right  = mPosition.x + width() / 2;
	r.top	 = mPosition.y; // -height() / 2;
    r.bottom = mPosition.y + height() / 2;
    return r;    
}