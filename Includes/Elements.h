#ifndef _PLAYERS_H_
#define _PLAYERS_H_

#include "GenericPlayer.h"

class Ball
{
public:
	Ball::Ball(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca);
	virtual ~Ball();
	void					Update(float dt);
	void					Draw();
	void					BallMove();
	void                    BallFrameAdvance();
	Vec2&					Position();
	Vec2&					Velocity();
	void					Bounce();
	RECT					RectSpriteAnimePosition()
	{
		return m_pSprite->RectAnimatedSpritePosition();
	}
	RECT					RectSpritePosition()
	{
		return m_pSprite->RectPosition();
	}
	AnimatedSprite*			m_pSprite;
	float					m_fTimer;
	int                     m_iBallFrame;
	bool					m_bMovingL;
	bool					m_bMovingR;
	bool					m_bMovingT;
	bool					m_bMovingD;
};

class Timp
{
public:
	Timp::Timp(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca);
	virtual ~Timp();

	AnimatedSprite*			m_pSprite;
	int					    m_iTimeFrame;
};

class Scor
{
public:
	Scor::Scor(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca);
	virtual ~Scor();

	AnimatedSprite*			m_pSprite;
	int					    m_iScoreFrame;
};

class Player: public GenericPlayer
{
public:
	Player::Player(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca);
};

class Mask : public GenericPlayer
{
public:
	Mask::Mask(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca);
	void FrameMove();
};

class Elements
{
public:
	Elements::Elements(const BackBuffer *pBackBuffer, const char* fisier, int elem);
	virtual ~Elements();
	void                   Draw();
	Vec2&                  Position();

	Sprite*				   m_pPoarta;
	Sprite*				   m_pMarker;
	int                    element;
};

class PUp
{
public:
	PUp::PUp(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca, int select);
	virtual ~PUp();
	RECT					RectSpriteAnimePosition()
	{
		return m_pSprite->RectAnimatedSpritePosition();
	}
	AnimatedSprite*			m_pSprite;
	int					    m_iPFrame;
	bool					m_bActPower;
};

#endif