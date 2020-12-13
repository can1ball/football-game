#ifndef SPRITE_H
#define SPRITE_H

#include "main.h"
#include "Vec2.h"
#include "BackBuffer.h"

class Sprite
{
public:
	Sprite(int imageID, int maskID);
	Sprite(const char *szImageFile, const char *szMaskFile);
	Sprite(const char *szImageFile, COLORREF crTransparentColor);

	virtual ~Sprite();

	virtual int width(){ return mImageBM.bmWidth; }
	virtual int height(){ return mImageBM.bmHeight; }

	void update(float dt);

	void setBackBuffer(const BackBuffer *pBackBuffer);
	virtual void draw();

	Vec2 mPosition;
	Vec2 mVelocity;
	RECT RectPosition();
private:
	Sprite(const Sprite& rhs);
	Sprite& operator=(const Sprite& rhs);

protected:
	HBITMAP mhImage;
	HBITMAP mhMask;
	BITMAP mImageBM;
	BITMAP mMaskBM;

	HDC mhSpriteDC;
	const BackBuffer *mpBackBuffer;

	COLORREF mcTransparentColor;
	void drawTransparent();
	void drawMask();
};

class AnimatedSprite : public Sprite
{
public:
	AnimatedSprite(const char *szImageFile, const char *szMaskFile, const RECT& rcFirstFrame, int iFrameCount);
	virtual ~AnimatedSprite() { }

	virtual int width() const { return miFrameWidth; }
	virtual int height() const { return miFrameHeight; }

	void SetFrame(int iIndex, int index2);
	int GetFrameCount() { return miFrameCount; }
	virtual void draw();

	RECT RectAnimatedSpritePosition();
	
protected:
	POINT mptFrameStartCrop;
	POINT mptFrameCrop;		
	int miFrameWidth;		
	int miFrameHeight;		
	int miFrameCount;	
};
#endif

