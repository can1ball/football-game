#include "Elements.h"

Ball::Ball(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca)
{
	RECT BallRectangle;
	BallRectangle.left = 0;
	BallRectangle.top = 0;
	BallRectangle.right = 18.28;
	BallRectangle.bottom = 20;
	m_pSprite = new AnimatedSprite(fisier, fisierMasca, BallRectangle, 6);
	m_pSprite->setBackBuffer(pBackBuffer);

	m_fTimer = 0;
	m_iBallFrame = 0;

}
Ball::~Ball()
{
	delete m_pSprite;
}
void Ball::BallMove()
{

		if (m_pSprite->mVelocity.y != 0 && m_pSprite->mVelocity.y > 0)
			m_pSprite->mVelocity.y -= 10;
		if (m_pSprite->mVelocity.y != 0 && m_pSprite->mVelocity.y < 0)
			m_pSprite->mVelocity.y += 10;
		if (m_pSprite->mVelocity.x != 0 && m_pSprite->mVelocity.x > 0)
			m_pSprite->mVelocity.x -=10;
		if (m_pSprite->mVelocity.x != 0 && m_pSprite->mVelocity.x < 0)
			m_pSprite->mVelocity.x += 10;

	Bounce();
}
void Ball::BallFrameAdvance()
{
	m_iBallFrame++;

	if (m_iBallFrame >= m_pSprite->GetFrameCount())
		m_iBallFrame = 0;
	if (m_iBallFrame < 0)
		m_iBallFrame = m_pSprite->GetFrameCount() - 1;

	m_pSprite->SetFrame(m_iBallFrame, 0);
}


void Ball::Update(float dt)
{
	m_pSprite->update(dt);
}


void Ball::Draw()
{
	m_pSprite->draw();
}


Vec2& Ball::Position()
{
	return m_pSprite->mPosition;
}


Vec2& Ball::Velocity()
{
	return m_pSprite->mVelocity;
}


void Ball::Bounce()
{
	if (m_pSprite->mPosition.y - m_pSprite->height() / 2 < 0)
		m_pSprite->mVelocity.y = -m_pSprite->mVelocity.y;

	if (m_pSprite->mPosition.y + m_pSprite->height() / 2 > GetSystemMetrics(SM_CYSCREEN))
		m_pSprite->mVelocity.y = -m_pSprite->mVelocity.y;

	if (m_pSprite->mPosition.x - m_pSprite->width() / 2 < 0)
		m_pSprite->mVelocity.x = -m_pSprite->mVelocity.x;

	if (m_pSprite->mPosition.x + m_pSprite->width() / 2 > GetSystemMetrics(SM_CXSCREEN))
		m_pSprite->mVelocity.x = -m_pSprite->mVelocity.x;
}


void Elements::Draw()
{
	if (element == 1)
	{
		m_pPoarta->draw();
	}
	if (element == 2)
	{
		m_pMarker->draw();
	}
}


Vec2& Elements::Position()
{
	if (element == 1)
	{
		return m_pPoarta->mPosition;
	}
	if (element == 2)
	{
		return m_pMarker->mPosition;
	}
}


Timp::Timp(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca)
{
	RECT p1;
	p1.left = 0;
	p1.top = 0;
	p1.right = 30;
	p1.bottom = 30;
	m_pSprite = new AnimatedSprite(fisier, fisierMasca, p1, 10);
	m_pSprite->setBackBuffer(pBackBuffer);
}


Timp::~Timp()
{
	delete m_pSprite;
}


Scor::Scor(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca)
{
	RECT p1;
	p1.left = 0;
	p1.top = 0;
	p1.right = 30;
	p1.bottom = 30;
	m_pSprite = new AnimatedSprite(fisier, fisierMasca, p1, 10);
	m_pSprite->setBackBuffer(pBackBuffer);
}



Scor::~Scor()
{
	delete m_pSprite;
}


Player::Player(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca)
{
	RECT PlayerRectangle;
	PlayerRectangle.left = 0;
	PlayerRectangle.top = 0;
	PlayerRectangle.right = 73;
	PlayerRectangle.bottom = 100;
	m_pSprite = new AnimatedSprite(fisier, fisierMasca, PlayerRectangle, 12);
	m_pSprite->setBackBuffer(pBackBuffer);

	m_fTimer = 0;
	m_bDoAnimation = false;
}


Mask::Mask(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca)
{
	RECT PlayerRectangle;
	PlayerRectangle.left = 0;
	PlayerRectangle.top = 0;
	PlayerRectangle.right = 89;
	PlayerRectangle.bottom = 91;
	m_pSprite = new AnimatedSprite(fisier, fisierMasca, PlayerRectangle, 9);
	m_pSprite->setBackBuffer(pBackBuffer);

	m_fTimer = 0;
	m_bDoAnimation = false;
}


void Mask::FrameMove()
{
	m_iMoveDelay++;

	if (m_iMoveDelay > 4)
	{
		m_iMoveDelay = 0;
		m_iPlayerFrame++;

		if (m_iPlayerFrame >= m_pSprite->GetFrameCount())
			m_iPlayerFrame = 6;
		if (m_iPlayerFrame < 0)
			m_iPlayerFrame = m_pSprite->GetFrameCount() - 1;
	}

	m_pSprite->SetFrame(m_iPlayerFrame, 0);
}

Elements::Elements(const BackBuffer *pBackBuffer, const char* fisier, int elem)
{
	element = elem;
	if (element == 1)
	{
		m_pPoarta = new Sprite(fisier, RGB(0xff, 0x00, 0xff));
		m_pPoarta->setBackBuffer(pBackBuffer);
	}
	if (element == 2)
	{
		m_pMarker = new Sprite(fisier, RGB(0xff, 0x00, 0xff));
		m_pMarker->setBackBuffer(pBackBuffer);
	}
}


Elements::~Elements()
{
	//delete m_pPoarta;
	//delete m_pMarker;
}

PUp::PUp(const BackBuffer *pBackBuffer, const char* fisier, const char* fisierMasca, int select)
{
	RECT p1;
	p1.left = 0;
	p1.top = 0;
	p1.right = 81;
	p1.bottom = 81;

	RECT p2;
	p2.left = 81;
	p2.top = 81;
	p2.right = 162;
	p2.bottom = 162;

	if (select == 0)
		m_pSprite = new AnimatedSprite(fisier, fisierMasca, p1, 8);
	else
		m_pSprite = new AnimatedSprite(fisier, fisierMasca, p2, 8);

	m_pSprite->setBackBuffer(pBackBuffer);
	m_bActPower = false;
}


PUp::~PUp()
{
	delete m_pSprite;
}