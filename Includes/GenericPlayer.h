#ifndef _GENERICPLAYER_H_
#define _GENERICPLAYER_H_

#include "Main.h"
#include "Sprite.h"

class GenericPlayer
{
public:
	enum DIRECTION 
	{ 
		DIR_TOP	    = 1, 
		DIR_DOWN	= 2, 
		DIR_LEFT    = 4, 
		DIR_RIGHT	= 8, 
	};

	GenericPlayer(){ m_iMoveDelay = 4; };

	GenericPlayer(const BackBuffer *pBackBuffer,const char* fisier,const char* fisierMasca);
	virtual ~GenericPlayer();

	void					Update( float dt );
	void					Draw();
	void					PlayerMove(ULONG ulDirection);
	void					PlayerMoveSprint(ULONG ulDirection);
	void					FrameMove(ULONG ulDirection, int moveType);
	void                    MoveFrame();
	void					MoveFrame(ULONG ulDirection, ULONG ulDirection2);
	Vec2&					Position();
	Vec2&					Velocity();
	void					Bounce();

	RECT					RectSpriteAnimePosition() 
	{ 
		return m_pSprite->RectAnimatedSpritePosition(); 
	}

	AnimatedSprite*			m_pSprite;
	AnimatedSprite*			m_pAnimationSprite;

	float					m_fTimer;	
	bool					m_bDoAnimation;
	int						m_iExplosionFrame;
	int						m_iPlayerFrame;
	int						m_iTypeFrame;
	bool					m_bMovingLeft;
	bool					m_bMovingRight;
	bool					m_bMovingTop;
	bool					m_bMovingDown;
	int						m_iLastMoveRightLeft;
	int						m_iLastMoveTopDown;
	int						m_iMoveDelay;

	bool					m_bPlayerHasBall;
	bool					m_bPlayerIsShooting;
	bool					m_bClosestPlayer;
	bool					m_bControlledByPlayer;
	bool					m_bSprint;
	int                     m_iPozx, m_iPozy;
};

#endif