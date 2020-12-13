#include "GenericPlayer.h"

GenericPlayer::GenericPlayer(const BackBuffer* pBackBuffer, const char* fisier, const char* fisierMasca)
{
	RECT p1;
	p1.left = 0;
	p1.top = 0;
	p1.right = 22.71;
	p1.bottom = 24;
	m_pSprite = new AnimatedSprite(fisier, fisierMasca, p1, 12);
	m_pSprite->setBackBuffer(pBackBuffer);

	RECT p2;
	p2.left = 0;
	p2.top = 0;
	p2.right = 89;
	p2.bottom = 91;
	m_pAnimationSprite = new AnimatedSprite("data/player/mask.bmp", "data/player/mask_mask.bmp", p2, 9);
	m_pAnimationSprite->setBackBuffer(pBackBuffer);

	m_fTimer = 0;
	m_iPlayerFrame = 0;
	m_iLastMoveRightLeft = 0;
	m_iMoveDelay = 200;
	m_iLastMoveTopDown = 0;
	m_bDoAnimation = false;
	m_bMovingTop = false;
	m_bMovingDown = false;
	m_bMovingRight = false;
	m_bMovingLeft = false;
	m_bPlayerHasBall = false;
	m_bClosestPlayer = false;
	m_bSprint = false;
	m_bPlayerIsShooting = false;
	m_bControlledByPlayer = false;
}

GenericPlayer::~GenericPlayer()
{
	delete m_pSprite;
}

void GenericPlayer::Update(float dt)
{
	m_pSprite->update(dt);
}

void GenericPlayer::Draw()
{
	m_pSprite->draw();
}

void GenericPlayer::PlayerMove(ULONG ulDirection)
{
	m_bMovingRight = false;
	m_bMovingLeft = false;
	m_bMovingTop = false;
	m_bMovingDown = false;

	if (ulDirection & GenericPlayer::DIR_LEFT)
	{
		m_bMovingLeft = true;
		m_iLastMoveRightLeft = 2;
		m_pSprite->mVelocity.x = -100;
	}
	else if (ulDirection & GenericPlayer::DIR_RIGHT)
	{
		m_bMovingRight = true;
		m_iLastMoveRightLeft = 1;
		m_pSprite->mVelocity.x = 100;
	}
	else
		m_pSprite->mVelocity.x = 0;

	if (ulDirection & GenericPlayer::DIR_TOP)
	{
		m_bMovingTop = true;
		m_iLastMoveTopDown = 1;
		m_pSprite->mVelocity.y = -100;
	}
	else if (ulDirection & GenericPlayer::DIR_DOWN)
	{
		m_bMovingDown = true;
		m_iLastMoveTopDown = 2;
		m_pSprite->mVelocity.y = 100;
	}
	else
		m_pSprite->mVelocity.y = 0;

	MoveFrame();
	Bounce();
}

void GenericPlayer::PlayerMoveSprint(ULONG ulDirection)
{
	if (ulDirection & GenericPlayer::DIR_LEFT)
	{
		m_bMovingLeft = true;
		m_bMovingRight = false;
		m_iLastMoveRightLeft = 2;
		m_pSprite->mVelocity.x = -250;
	}
	else if (ulDirection & GenericPlayer::DIR_RIGHT)
	{
		m_bMovingRight = true;
		m_bMovingLeft = false;
		m_iLastMoveRightLeft = 1;
		m_pSprite->mVelocity.x = 250;
	}
	else
	{
		m_bMovingRight = false;
		m_bMovingLeft = false;
		m_pSprite->mVelocity.x = 0;
	}

	if (ulDirection & GenericPlayer::DIR_TOP)
	{
		m_bMovingTop = true;
		m_bMovingDown = false;
		m_iLastMoveTopDown = 1;
		m_pSprite->mVelocity.y = -250;
	}
	else if (ulDirection & GenericPlayer::DIR_DOWN)
	{
		m_bMovingDown = true;
		m_bMovingTop = false;
		m_iLastMoveTopDown = 2;
		m_pSprite->mVelocity.y = 250;
	}
	else
	{
		m_bMovingDown = false;
		m_bMovingTop = false;
		m_pSprite->mVelocity.y = 0;
	}

	m_bSprint = false;
	MoveFrame();
	Bounce();
}

Vec2& GenericPlayer::Position()
{
	return m_pSprite->mPosition;
}

Vec2& GenericPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}

void GenericPlayer::FrameMove(ULONG ulDirection, int moveType)
{
	// Sprite frame counter
	// This allows us to have an animated player movement
	if (ulDirection)
		m_iPlayerFrame++;
	// Keeps the frame counter in range
	if (m_iPlayerFrame >= m_pSprite->GetFrameCount())
		m_iPlayerFrame = 0;
	if (m_iPlayerFrame < 0)
		m_iPlayerFrame = m_pSprite->GetFrameCount() - 1;

	m_pSprite->SetFrame(m_iPlayerFrame, moveType);
}



void GenericPlayer::MoveFrame()
{
	m_iMoveDelay++;

	if (m_iMoveDelay > 3)
	{
		m_iMoveDelay = 0;

		if (m_bMovingTop == true)
		{
			//top right
			if (m_bMovingRight == true)
				FrameMove(1, 6);
			//top left
			else if (m_bMovingLeft == true)
				FrameMove(1, 5);
			//top
			else
				FrameMove(1, 4);
		}
		else if (m_bMovingDown == true)
		{
			//down right
			if (m_bMovingRight == true)
				FrameMove(1, 2);
			//down left
			else if (m_bMovingLeft == true)
				FrameMove(1, 1);
			//down
			else
				FrameMove(1, 0);
		}
		else if (m_bMovingLeft == true)
			//left
			FrameMove(1, 3);
		else if (m_bMovingRight == true)
			//right
			FrameMove(1, 7);
	}
}

void GenericPlayer::MoveFrame(ULONG ulDirection, ULONG ulDirection2)
{
	m_iMoveDelay++;

	if (m_iMoveDelay > 3)
	{
		m_iMoveDelay = 0;

		if (ulDirection == 1)
		{
			//top right
			if (ulDirection2 == 8)
				FrameMove(1, 6);
			//top left
			else if (ulDirection2 == 4)
				FrameMove(1, 5);
			//top
			else
				FrameMove(1, 4);
		}
		else if (ulDirection == 2)
		{
			//down right
			if (ulDirection2 == 8)
				FrameMove(1, 2);
			//down left
			else if (ulDirection2 == 4)
				FrameMove(1, 1);
			//down
			else
				FrameMove(1, 0);
		}
		else if (ulDirection == 4)
			//left
			FrameMove(1, 3);
		else if (ulDirection == 8)
			//right
			FrameMove(1, 7);
	}

}

void GenericPlayer::Bounce()
{
	//Modifies the position of the ball when the player switches the position
	if (m_pSprite->mPosition.y - m_pSprite->height() / 2 < -1)
		m_pSprite->mPosition.y = m_pSprite->height() / 2;

	if (m_pSprite->mPosition.y + m_pSprite->height() / 2 > GetSystemMetrics(SM_CYSCREEN) - 1)
		m_pSprite->mPosition.y = GetSystemMetrics(SM_CYSCREEN) - m_pSprite->height() / 2;

	if (m_pSprite->mPosition.x - m_pSprite->width() / 2 < -1)
		m_pSprite->mPosition.x = m_pSprite->width() / 2;

	if (m_pSprite->mPosition.x + m_pSprite->width() / 2 > GetSystemMetrics(SM_CXSCREEN) - 1)
		m_pSprite->mPosition.x = GetSystemMetrics(SM_CXSCREEN) - m_pSprite->width() / 2;
}
