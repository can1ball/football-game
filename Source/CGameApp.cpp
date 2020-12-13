#include "CGameApp.h"
extern HINSTANCE g_hInst;

CGameApp::CGameApp() {
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pTime			= NULL;
	m_pTime2		= NULL;
	m_pBall		    = NULL;
	CurrentP1       = NULL;
	CurrentP2       = NULL;
	m_pPlayerMarker1= NULL;
	m_pPlayerMarker2= NULL;
	m_pPoarta1      = NULL;
	m_pPoarta2      = NULL;
	m_pFieldLines   = NULL;
	m_bInGameMenuActive = NULL;
	m_bMenuActive    = true;
	m_bGol           = false;
	m_bBlueHasBall = false;
    m_bWhiteHasBall = false;
	m_iSPowerType    = 0;
	m_iSPowerChance  = 0;
	m_iGolEchipa[0] = 0;
	m_iGolEchipa[1] = 0;
	m_iNrOfPlayers   = 5;
	m_iNrmenu		 = 0;
	m_iParallax		 = 0;
	m_LastFrameRate  = 0;
	m_iTeamWhoScored = 0;
	m_iSPowerRepTime = 0;
	m_iBallSpeed     = 800;
	m_iGateH         = 182;
	m_iSuperPChance  = 2200;
}

CGameApp::~CGameApp() {
	ShutDown();
}

bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow ) {
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) { 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

bool CGameApp::CreateDisplay() {
	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= GetSystemMetrics(SM_CYSCREEN);
	USHORT			Height			= GetSystemMetrics(SM_CXSCREEN);
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	ShowWindow(m_hWnd, SW_MAXIMIZE);

	return true;
}

int CGameApp::BeginGame() {
	MSG		msg;

	// Start main loop
	while(true) {
		// Did we recieve a message, or are we idling ?
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else {
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}
bool CGameApp::ShutDown() {
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam ) {
	static UINT fTimer;	

	switch (Message) {
		case WM_CREATE:
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED ) {
				m_bActive = false;
			} 
			else {
				m_bActive = true;
				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );		
			} 
			break;

		case WM_LBUTTONDOWN:
			// Capture the mouse
			SetCapture(m_hWnd);
			GetCursorPos(&m_OldCursorPos);
			break;

		case WM_MOUSEMOVE:
			GetCursorPos(&m_OldCursorPos);
			break;

		case WM_LBUTTONUP:
			ReleaseCapture();
			break;

		case WM_KEYDOWN:
			switch(wParam) {
				case VK_ESCAPE:
					if (m_bInGameMenuActive == false) {
						m_bInGameMenuActive = true;
					}
					else
						m_bInGameMenuActive = false;
					break;
				case 'O':
					CurrentP1->m_bSprint = true;
					break;
				case 'G':
					CurrentP2->m_bSprint = true;
					break;
				case 'P':
					Shoot(whiteTeam);
					break;
				case 'H':
					Shoot(blueTeam);
					break;
				case 'N':
					Save();
					break;
				case 'M':
					Load();
					break;
				default:
					break;
			}
			break;

		case WM_COMMAND:
			break;
		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);
	}
	
	return 0;
}

bool CGameApp::BuildObjects() {
	PlaySound("data/menu/main_sound.wav", NULL, SND_FILENAME | SND_ASYNC);

	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	RECT p1;
	p1.left = 0;
	p1.top = 0;
	p1.right = 960;
	p1.bottom = 540;
	//build elements
	m_pBall = new Ball(m_pBBuffer,"data/ball/ball.bmp","data/ball/ball_mask.bmp");
	m_pPlayerMarker1 = new Elements(m_pBBuffer, "data/player/marker.bmp", 2);
	m_pPlayerMarker2 = new Elements(m_pBBuffer, "data/player/marker1.bmp", 2);
	m_pPoarta1 = new Elements(m_pBBuffer, "data/field/1.bmp", 1);
	m_pPoarta2 = new Elements(m_pBBuffer, "data/field/1.bmp", 1);
	m_pFieldLines = new Sprite("data/field/CenterLine.bmp", RGB(0xff, 0x00, 0xff));
	m_pCircleLine = new Sprite("data/field/CircleLine.bmp", RGB(0xff, 0x00, 0xff));
	m_pGateLine1 = new Sprite("data/field/GateLine.bmp", RGB(0xff, 0x00, 0xff));
	m_pGateLine2 = new Sprite("data/field/GateLine.bmp", RGB(0xff, 0x00, 0xff));
	m_pSuperPGreen = new Sprite("data/player/SuperPGreen.bmp", RGB(0xff, 0x00, 0xff));
	m_pSuperPRed = new Sprite("data/player/SuperPRed.bmp", RGB(0xff, 0x00, 0xff));
	m_pSuperPOrange = new Sprite("data/player/SuperPOrange.bmp", RGB(0xff, 0x00, 0xff));
	m_pPowerUp[0] = new PUp(m_pBBuffer, "data/player/powerups.bmp", "data/player/powerups_mask.bmp", 0);
	m_pPowerUp[1] = new PUp(m_pBBuffer, "data/player/powerups.bmp", "data/player/powerups_mask.bmp", 1);
	m_pFieldLines->setBackBuffer(m_pBBuffer);
	m_pCircleLine->setBackBuffer(m_pBBuffer);
	m_pGateLine1->setBackBuffer(m_pBBuffer);
	m_pGateLine2->setBackBuffer(m_pBBuffer);
	m_pSuperPGreen->setBackBuffer(m_pBBuffer);
	m_pSuperPRed->setBackBuffer(m_pBBuffer);
	m_pSuperPOrange->setBackBuffer(m_pBBuffer);

	//build time
	m_pTime = new Timp(m_pBBuffer, "data/menu/nr.bmp", "data/menu/nr_mask.bmp");
	m_pTime2 = new Timp(m_pBBuffer, "data/menu/nr.bmp", "data/menu/nr_mask.bmp");
	m_pTime->m_iTimeFrame  = 9;
	m_pTime2->m_iTimeFrame = 9;

	//build score
	m_pScor1 = new Scor(m_pBBuffer, "data/menu/nr.bmp", "data/menu/nr_mask.bmp");
	m_pScor2 = new Scor(m_pBBuffer, "data/menu/nr.bmp", "data/menu/nr_mask.bmp");
	m_pScor1->m_iScoreFrame = 1;
	m_pScor2->m_iScoreFrame = 1;

	//build players
	for (int i = 1; i <= m_iNrOfPlayers; i++)
		whiteTeam.push_back(new Player(m_pBBuffer,"data/player/p1.bmp","data/player/p1_mask.bmp"));
	for (int i = 1; i <= m_iNrOfPlayers; i++)
		blueTeam.push_back(new Player(m_pBBuffer, "data/player/p2.bmp", "data/player/p2_mask.bmp"));
	Maska = new Mask(m_pBBuffer, "data/player/mask.bmp", "data/player/mask_mask.bmp");

	//build field
	if (!m_imgParallax.LoadBitmapFromFile("data/field/terain.bmp", GetDC(m_hWnd), m_nViewWidth, m_nViewHeight))
		return false;
	if (!m_imgParallax1.LoadBitmapFromFile("data/field/terain.bmp", GetDC(m_hWnd), m_nViewWidth, m_nViewHeight))
		return false;
	
	//meniu
	if (!m_imgBackgroundMainMenu.LoadBitmapFromFile("data/menu/mainmenu1.bmp", GetDC(m_hWnd), m_nViewWidth, m_nViewHeight))
		return false;

	//meniu in game
	if (!m_imgBackgroundInGame.LoadBitmapFromFile("data/menu/menu1.bmp", GetDC(m_hWnd), m_nViewWidth, m_nViewHeight))
		return false;

	if (!m_imgScore.LoadBitmapFromFile("data/menu/teams.bmp", GetDC(m_hWnd), 300, 37))
		return false;
	return true;
}

void CGameApp::SetupGameState() {
	//player position
	it = whiteTeam.begin();
	(*it)->m_iPozx = m_nViewWidth - 300;
	(*it)->m_iPozy = m_nViewHeight - 200;
	(*it++)->Position() = Vec2(double(m_nViewWidth - 300), double(m_nViewHeight - 200));
	(*it)->m_iPozx = m_nViewWidth - 300;
	(*it)->m_iPozy = 200;
	(*it++)->Position() = Vec2(double(m_nViewWidth - 300), double(200));
	(*it)->m_iPozx = m_nViewWidth - 300;
	(*it)->m_iPozy = m_nViewHeight / 2;
	(*it++)->Position() = Vec2(double(m_nViewWidth - 300), double(m_nViewHeight / 2));
	(*it)->m_iPozx = m_nViewWidth - 600;
	(*it)->m_iPozy = m_nViewHeight - 300;
	(*it++)->Position() = Vec2(double(m_nViewWidth - 600), double(m_nViewHeight - 300));
	(*it)->m_iPozx = m_nViewWidth - 600;
	(*it)->m_iPozy = 300;
	(*it++)->Position() = Vec2(double(m_nViewWidth - 600), double(300));

	it = blueTeam.begin();
	(*it)->m_iPozx = 300;
	(*it)->m_iPozy = m_nViewHeight - 200;
	(*it++)->Position() = Vec2(double(300), double(m_nViewHeight - 200));
	(*it)->m_iPozx = 300;
	(*it)->m_iPozy = 200;
	(*it++)->Position() = Vec2(double(300), double(200));
	(*it)->m_iPozx = 300;
	(*it)->m_iPozy = m_nViewHeight / 2;
	(*it++)->Position() = Vec2(double(300), double(m_nViewHeight / 2));
	(*it)->m_iPozx = 600;
	(*it)->m_iPozy = m_nViewHeight - 300;
	(*it++)->Position() = Vec2(double(600), double(m_nViewHeight - 300));
	(*it)->m_iPozx = 600;
	(*it)->m_iPozy = 300;
	(*it++)->Position() = Vec2(double(600), double(300));

	//ball position
	m_pBall->Position() = Vec2(double(m_nViewWidth / 2), double(m_nViewHeight / 2));
	m_pBall->Velocity() = Vec2(0, 0);

	//elements reset
	m_pTime2->m_iTimeFrame = 9;
	m_pTime->m_iTimeFrame = 9;
	m_pScor1->m_iScoreFrame = 0;
	m_pScor2->m_iScoreFrame = 0;
	m_pPowerUp[0]->m_iPFrame = 0;
	m_pPowerUp[1]->m_iPFrame = 0;

	// Terrain elements
	m_pGateLine1->mPosition = Vec2(double(0), double(m_nViewHeight / 2));
	m_pGateLine2->mPosition = Vec2(double(m_nViewWidth), double(m_nViewHeight / 2));
	m_pCircleLine->mPosition = Vec2(double(m_nViewWidth / 2), double(m_nViewHeight / 2));
	m_pFieldLines->mPosition = Vec2(double(m_nViewWidth / 2), double(m_nViewHeight / 2));
	m_pPoarta1->Position() = Vec2(double(0), double(m_nViewHeight / 2));
	m_pPoarta2->Position() = Vec2(double(m_nViewWidth), double(m_nViewHeight / 2));

	m_pSuperPGreen->mPosition = Vec2(30, 30);
	m_pSuperPRed->mPosition = Vec2(30, 30);
	m_pSuperPOrange->mPosition = Vec2(30, 30);

	m_iGolEchipa[0] = 0;
	m_iGolEchipa[1] = 0;
}

void CGameApp::ReleaseObjects() {
	if (m_pBBuffer != NULL)
		delete m_pBBuffer, m_pBBuffer = NULL;
	for (it = whiteTeam.begin(); it != whiteTeam.end(); ++it)
		if ((*it) != NULL)
			delete (*it), (*it) = NULL;
	for (it = blueTeam.begin(); it != blueTeam.end(); ++it)
		if ((*it) != NULL)
			delete (*it), (*it) = NULL;
	if (m_pBall != NULL)
		delete m_pBall, m_pBall = NULL;
	if (m_pTime != NULL)
		delete m_pTime, m_pTime = NULL;
	if (m_pTime2 != NULL)
		delete m_pTime2, m_pTime2 = NULL;
	if (m_pPlayerMarker1 != NULL)
		delete m_pPlayerMarker1, m_pPlayerMarker1 = NULL;
	if (m_pPlayerMarker2 != NULL)
		delete m_pPlayerMarker2, m_pPlayerMarker2 = NULL;
	if (m_pPoarta1 != NULL)
		delete m_pPoarta1, m_pPoarta1 = NULL;
	if (m_pPoarta2 != nullptr)
		delete m_pPoarta2, m_pPoarta2 = NULL;
	for (int i = 0; i <= 1; i++)
		if (m_pPowerUp[i] != NULL)
			delete m_pPowerUp[i], m_pPowerUp[i] = NULL;
	
}

void CGameApp::FrameAdvance() {
	static TCHAR FrameRate[ 50 ];
	static TCHAR TitleBuffer[ 255 ];
	framerate = FrameRate;

	m_Timer.Tick( );

	if ( !m_bActive ) return;
	
	if ( m_LastFrameRate != m_Timer.GetFrameRate() ) {
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s"), FrameRate );
				
		SetWindowText( m_hWnd, TitleBuffer );
	}

	ProcessInput();
	AnimateObjects();
	DrawObjects();	
}

void CGameApp::ProcessInput() {
	static UCHAR pKeyBuffer[ 256 ];
	ULONG		Direction1 = 0;
	ULONG		Direction2 = 0;
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;

	if ( !GetKeyboardState( pKeyBuffer ) ) return;

	if (m_bMenuActive == true) {

		if (pKeyBuffer[VK_UP] & 0xF0) {
			m_imgBackgroundMainMenu.LoadBitmapFromFile("data/menu/mainmenu1.bmp", GetDC(m_hWnd), m_nViewWidth, m_nViewHeight);
			this->selectionMainMenu = 1;
		}
		if (pKeyBuffer[VK_DOWN] & 0xF0) {
			m_imgBackgroundMainMenu.LoadBitmapFromFile("data/menu/mainmenu.bmp", GetDC(m_hWnd), m_nViewWidth, m_nViewHeight);
			this->selectionMainMenu = 2;
		}
		if ((pKeyBuffer[VK_RETURN] & 0xF0) && selectionMainMenu == 1) {
			PlaySound(NULL, 0, 0);
			PlaySound("data/menu/ingame_sound.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NOSTOP);
			m_bMenuActive = false;
		}
		if ((pKeyBuffer[VK_RETURN] & 0xF0) && selectionMainMenu == 2) {
			exit(1);
			//ShutDown();
		}
	}
	else if (m_bInGameMenuActive == true) {
		if (pKeyBuffer[VK_UP] & 0xF0) {
			m_imgBackgroundInGame.LoadBitmapFromFile("data/menu/menu1.bmp", GetDC(m_hWnd), m_nViewWidth, m_nViewHeight);
			this->selectionInGameMenu = 1;
		}
		if (pKeyBuffer[VK_DOWN] & 0xF0) {
			m_imgBackgroundInGame.LoadBitmapFromFile("data/menu/menu.bmp", GetDC(m_hWnd), m_nViewWidth, m_nViewHeight);
			this->selectionInGameMenu = 2;
		}
		if ((pKeyBuffer[VK_RETURN] & 0xF0) && selectionInGameMenu == 1) {
			m_bInGameMenuActive = false;
		}
		if ((pKeyBuffer[VK_RETURN] & 0xF0) && selectionInGameMenu == 2) {
			m_bMenuActive = true;
			m_imgBackgroundMainMenu.LoadBitmapFromFile("data/menu/mainmenu1.bmp", GetDC(m_hWnd), m_nViewWidth, m_nViewHeight);
			PlaySound(NULL, 0, 0);
			PlaySound("data/menu/main_sound.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NOSTOP);
			m_bInGameMenuActive = false;
			SetupGameState();
		}
	}
	else {
		if (pKeyBuffer[VK_UP] & 0xF0) Direction1 |= GenericPlayer::DIR_TOP;
		if (pKeyBuffer[VK_DOWN] & 0xF0) Direction1 |= GenericPlayer::DIR_DOWN;
		if (pKeyBuffer[VK_LEFT] & 0xF0) Direction1 |= GenericPlayer::DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) Direction1 |= GenericPlayer::DIR_RIGHT;

		if (pKeyBuffer['W'] & 0xF0) Direction2 |= GenericPlayer::DIR_TOP;
		if (pKeyBuffer['S'] & 0xF0) Direction2 |= GenericPlayer::DIR_DOWN;
		if (pKeyBuffer['A'] & 0xF0) Direction2 |= GenericPlayer::DIR_LEFT;
		if (pKeyBuffer['D'] & 0xF0) Direction2 |= GenericPlayer::DIR_RIGHT;

		if (m_bInGameMenuActive == false) {
			Move(Direction1, whiteTeam);
			Move(Direction2, blueTeam);
		}
	}
}

void CGameApp::AnimateObjects() {
	m_pBall->Update(m_Timer.GetTimeElapsed());

	for (it = whiteTeam.begin(); it != whiteTeam.end(); ++it)
		(*it)->Update(m_Timer.GetTimeElapsed());
	for (it = blueTeam.begin(); it != blueTeam.end(); ++it)
		(*it)->Update(m_Timer.GetTimeElapsed());
}

void CGameApp::DrawObjects() {
	m_pBBuffer->reset();

	if (m_bInGameMenuActive == true) {
		m_imgBackgroundInGame.Paint(m_pBBuffer->getDC(), 0, 0);
	}
	else if (m_bMenuActive == true) {
		m_imgBackgroundMainMenu.Paint(m_pBBuffer->getDC(), 0, 0);
	}
	else {
		//mecanica
		if (!m_bActiveSP) {
			CheckClosestPlayer(whiteTeam);
			CheckClosestPlayer(blueTeam);
		}
		if (!m_bWhiteHasBall)
			BallColision(blueTeam);
		if (!m_bBlueHasBall)
			BallColision(whiteTeam);

		m_pBall->BallMove();
		Goal();
		DrawPictures();
		AI();
	}
	TextOutA(m_pBBuffer->getDC(), 5, 5, framerate.c_str(), 5);

	m_pBBuffer->present();
}

void CGameApp::BallColision(std::list<GenericPlayer*> lista) {
	RECT x;
	
	for (it = lista.begin(); it != lista.end(); ++it) {
		(*it)->m_bPlayerHasBall = false;
		// The intersection between player and ball
		if (IntersectRect(&x, &(*it)->RectSpriteAnimePosition(), &m_pBall->RectSpriteAnimePosition())) {
			if ((*it)->m_bPlayerIsShooting == true) {
				(*it)->m_bPlayerHasBall = false;
			}
			else {
				(*it)->m_bPlayerHasBall = true;
			}
		}
		else
			(*it)->m_bPlayerIsShooting = false;

		if ((*it)->m_bPlayerHasBall) {
			// Right
			if ((*it)->m_bMovingRight) {
				// Ball's sprite advance
				m_pBall->BallFrameAdvance();
				m_pBall->m_pSprite->mPosition.x = (*it)->m_pSprite->mPosition.x + 20;
				m_pBall->m_pSprite->mPosition.y = (*it)->m_pSprite->mPosition.y + 35;
			}
			// Top-Right
			if ((*it)->m_bMovingRight && (*it)->m_bMovingTop) {
				// Ball's sprite advance
				m_pBall->BallFrameAdvance();
				m_pBall->m_pSprite->mPosition.x = (*it)->m_pSprite->mPosition.x - 10;
				m_pBall->m_pSprite->mPosition.y = (*it)->m_pSprite->mPosition.y + 35;
			}
			// Left
			if ((*it)->m_bMovingLeft) {
				// Ball's sprite advance
				m_pBall->BallFrameAdvance();
				m_pBall->m_pSprite->mPosition.x = (*it)->m_pSprite->mPosition.x - 20;
				m_pBall->m_pSprite->mPosition.y = (*it)->m_pSprite->mPosition.y + 35;
			}
			// Top-Left
			if ((*it)->m_bMovingLeft && (*it)->m_bMovingTop) {
				// Ball's sprite advance
				m_pBall->BallFrameAdvance();
				m_pBall->m_pSprite->mPosition.x = (*it)->m_pSprite->mPosition.x - 10;
				m_pBall->m_pSprite->mPosition.y = (*it)->m_pSprite->mPosition.y + 35;
			}
			// Top
			if ((*it)->m_bMovingTop) {
				// Ball's sprite advance
				m_pBall->BallFrameAdvance();
				m_pBall->m_pSprite->mPosition.x = (*it)->m_pSprite->mPosition.x;
				m_pBall->m_pSprite->mPosition.y = (*it)->m_pSprite->mPosition.y + 30;
			}
			// Down
			if ((*it)->m_bMovingDown) {
				// Ball's sprite advance
				m_pBall->BallFrameAdvance();
				m_pBall->m_pSprite->mPosition.x = (*it)->m_pSprite->mPosition.x;
				m_pBall->m_pSprite->mPosition.y = (*it)->m_pSprite->mPosition.y + 40;
			}
			// Right-Down
			if ((*it)->m_bMovingDown && (*it)->m_bMovingRight) {
				// Ball's sprite advance
				m_pBall->BallFrameAdvance();
				m_pBall->m_pSprite->mPosition.x = (*it)->m_pSprite->mPosition.x + 15;
				m_pBall->m_pSprite->mPosition.y = (*it)->m_pSprite->mPosition.y + 40;
			}
			// Left-Down
			if ((*it)->m_bMovingDown && (*it)->m_bMovingLeft) {
				// Ball's sprite advance
				m_pBall->BallFrameAdvance();
				m_pBall->m_pSprite->mPosition.x = (*it)->m_pSprite->mPosition.x - 15;
				m_pBall->m_pSprite->mPosition.y = (*it)->m_pSprite->mPosition.y + 40;
			}
		}
	}
}

void CGameApp::Shoot(std::list<GenericPlayer*> lista) {
	for (it = lista.begin(); it != lista.end(); ++it) {
		if ((*it)->m_bPlayerHasBall) {
			(*it)->m_bPlayerIsShooting = true;
			(*it)->m_bPlayerHasBall = false;
			// While the player moves
			if ((*it)->m_pSprite->mVelocity.x != 0 || (*it)->m_pSprite->mVelocity.y != 0) {
				// Ball direction correlates with the player's direction
				if ((*it)->m_bMovingTop)
					m_pBall->m_pSprite->mVelocity.y = -m_iBallSpeed;
				if ((*it)->m_bMovingDown)
					m_pBall->m_pSprite->mVelocity.y = m_iBallSpeed;
				if ((*it)->m_bMovingRight)
					m_pBall->m_pSprite->mVelocity.x = m_iBallSpeed;
				if ((*it)->m_bMovingLeft)
					m_pBall->m_pSprite->mVelocity.x = -m_iBallSpeed;
			}
			else {
				// If the player doesn't move we check its last movement direction
				if ((*it)->m_iLastMoveRightLeft == 1)
					m_pBall->m_pSprite->mVelocity.x = m_iBallSpeed;
				else
					m_pBall->m_pSprite->mVelocity.x = -m_iBallSpeed;
			}
		}
	}
}

void CGameApp::Move(ULONG ulDirection, std::list<GenericPlayer*> lista) {
	for (it = lista.begin(); it != lista.end(); ++it) {
		// Ball position near player
		(*it)->Bounce();
		// Player marker(arrow) position
		if ((*it)->m_bControlledByPlayer == true) {
			if (lista == whiteTeam)
				m_pPlayerMarker1->Position() = Vec2((*it)->m_pSprite->mPosition.x, (*it)->m_pSprite->mPosition.y - 65);
			else
				m_pPlayerMarker2->Position() = Vec2((*it)->m_pSprite->mPosition.x, (*it)->m_pSprite->mPosition.y - 65);

			// Sprint
			if ((*it)->m_bSprint == true)
				(*it)->PlayerMoveSprint(ulDirection);
			else
				(*it)->PlayerMove(ulDirection);
		}
		else
			(*it)->m_pSprite->mVelocity.x = (*it)->m_pSprite->mVelocity.y = 0;
	}
}

void CGameApp::CheckClosestPlayer(std::list<GenericPlayer*> lista) {
	static double ipot = 0;
	static double min = 999999;
	static double counter = 0;
	static int i = 0;

	static double Px, Py, Bx, By;
	i = 0;
	counter = 0;
	min = 9999999;

	// Check each player
	for (it = lista.begin(); it != lista.end(); ++it) {
		// Player coordinates
		Px = (*it)->m_pSprite->mPosition.x / 100;
		Py = (*it)->m_pSprite->mPosition.y / 100;
		// Ball coordinates
		Bx = m_pBall->m_pSprite->mPosition.x / 100;
		By = m_pBall->m_pSprite->mPosition.y / 100;

		// Player-Ball distance
		ipot = sqrt(pow(Bx - Px, 2) + pow(By - Py, 2));

		// The pick of the closest player
		if (ipot < min) {
			min = ipot;
			for (auto it2 = lista.begin(); it2 != lista.end(); ++it2) {
				(*it2)->m_bControlledByPlayer = false;
			}
			(*it)->m_bControlledByPlayer = true;

			if (lista == whiteTeam)
				CurrentP1 = (*it);
			else
				CurrentP2 = (*it);
		}
	}
}

void CGameApp::Time() {
	static float seconds = 0;

	seconds += m_Timer.GetTimeElapsed();

	// Frame delay
	if (seconds > 1) {
		m_pTime->m_iTimeFrame--;
		seconds = 0;
	}

	// While both digits of the clock are bigger than 0
	if (!(m_pTime2->m_iTimeFrame == 0 && m_pTime->m_iTimeFrame == -1)) {
		// Second digit decrease
		if (m_pTime->m_iTimeFrame < 0) {
			m_pTime2->m_iTimeFrame--;
			// Reset of the second digit to 9
			if (m_pTime2->m_iTimeFrame < 0)
				m_pTime2->m_iTimeFrame = m_pTime2->m_pSprite->GetFrameCount() - 1;

			m_pTime->m_iTimeFrame = m_pTime->m_pSprite->GetFrameCount() - 1;
		}
	}
	//first digit
	m_pTime2->m_pSprite->SetFrame(m_pTime2->m_iTimeFrame, 0);
	m_pTime2->m_pSprite->draw();
	m_pTime2->m_pSprite->mPosition.x = m_nViewWidth / 2 - 200;
	m_pTime2->m_pSprite->mPosition.y = 30;

	//last digit
	m_pTime->m_pSprite->SetFrame(m_pTime->m_iTimeFrame, 0);
	m_pTime->m_pSprite->draw();
	m_pTime->m_pSprite->mPosition.x = m_nViewWidth / 2 - 170;
	m_pTime->m_pSprite->mPosition.y = 30;
}

void CGameApp::Score(int param) {
	//incrementare scor
	if (param == 1)
		m_pScor1->m_iScoreFrame++;
	if (param == 2)
		m_pScor2->m_iScoreFrame++;

	//afisare scor
	// Blue team
	m_pScor1->m_pSprite->SetFrame(m_pScor1->m_iScoreFrame, 0);
	m_pScor1->m_pSprite->draw();
	m_pScor1->m_pSprite->mPosition.x = m_nViewWidth / 2 - 30;
	m_pScor1->m_pSprite->mPosition.y = 30;
	// White team
	m_pScor2->m_pSprite->SetFrame(m_pScor2->m_iScoreFrame, 0);
	m_pScor2->m_pSprite->draw();
	m_pScor2->m_pSprite->mPosition.x = m_nViewWidth / 2 + 20;
	m_pScor2->m_pSprite->mPosition.y = 30;
}


void CGameApp::AI() {
	for (it = whiteTeam.begin(); it != whiteTeam.end(); ++it) {
		if (CurrentP1->m_bPlayerHasBall) {
			if ((*it)->m_pSprite->mPosition.x > CurrentP1->m_pSprite->mPosition.x) {
				//atac
				if ((*it) != CurrentP1) {
					if ((*it)->m_pSprite->mPosition.x > (*it)->m_iPozx - 400) {
						(*it)->m_pSprite->mPosition.x -= 2;
						// Movement of the other players of the team
						(*it)->MoveFrame(4, 0);
					}

					//axa y
					if ((*it)->m_pSprite->mPosition.y > (*it)->m_iPozy) {
						(*it)->m_pSprite->mPosition.y -= 2;
					}
					else
						(*it)->m_pSprite->mPosition.y += 2;
				}
			}
			else if ((*it)->m_pSprite->mPosition.x < (*it)->m_iPozx + 100) {
				//defensiva
				if ((*it) != CurrentP1) {

					(*it)->m_pSprite->mPosition.x += 2;
					(*it)->MoveFrame(8, 0);
					//axa y
					if ((*it)->m_pSprite->mPosition.y >(*it)->m_iPozy)
						(*it)->m_pSprite->mPosition.y -= 2;
					else
						(*it)->m_pSprite->mPosition.y += 2;
				}
			}
		}//inamicul ataca
		else if (CurrentP2->m_bPlayerHasBall)
			if ((*it)->m_pSprite->mPosition.x < (*it)->m_iPozx)
				if ((*it) != CurrentP1) {
					(*it)->m_pSprite->mPosition.x += 2;
					(*it)->MoveFrame(8, 0);

					if ((*it)->m_pSprite->mPosition.y > (*it)->m_iPozy)
						(*it)->m_pSprite->mPosition.y -= 2;
					else
						(*it)->m_pSprite->mPosition.y += 2;
				}
	}
	for (it2 = blueTeam.begin(); it2 != blueTeam.end(); ++it2) {
		if (CurrentP2->m_bPlayerHasBall) {
			if ((*it2)->m_pSprite->mPosition.x < CurrentP2->m_pSprite->mPosition.x) {
				if ((*it2) != CurrentP2) {
					if ((*it2)->m_pSprite->mPosition.x < (*it2)->m_iPozx + 400) {
						(*it2)->m_pSprite->mPosition.x += 2;
						(*it2)->MoveFrame(8, 0);
					}

					if ((*it2)->m_pSprite->mPosition.y > (*it2)->m_iPozy)
						(*it2)->m_pSprite->mPosition.y -= 2;
					else
						(*it2)->m_pSprite->mPosition.y += 2;
				}
			}
			else if ((*it2)->m_pSprite->mPosition.x > (*it2)->m_iPozx - 100)
				if ((*it2) != CurrentP2) {
					(*it2)->m_pSprite->mPosition.x -= 2;
					(*it2)->MoveFrame(4, 0);

					if ((*it2)->m_pSprite->mPosition.y > (*it2)->m_iPozy)
						(*it2)->m_pSprite->mPosition.y -= 2;
					else
						(*it2)->m_pSprite->mPosition.y += 2;
				}
		}
		else if (CurrentP1->m_bPlayerHasBall)
			if ((*it2)->m_pSprite->mPosition.x > (*it2)->m_iPozx)
				if ((*it2) != CurrentP2) {
					(*it2)->m_pSprite->mPosition.x -= 2;

					if ((*it2)->m_pSprite->mPosition.y > (*it2)->m_iPozy)
						(*it2)->m_pSprite->mPosition.y -= 2;
					else
						(*it2)->m_pSprite->mPosition.y += 2;
				}
	}
}
void CGameApp::Powers()
{
	RECT x = {};
	static float seconds = 0;

	seconds += m_Timer.GetTimeElapsed();

	if (!m_bBlueHasBall)
	if (IntersectRect(&x, &CurrentP1->RectSpriteAnimePosition(), &m_pPowerUp[m_iSPowerType]->RectSpriteAnimePosition()) && CurrentP1->m_bPlayerHasBall)
	{
		m_bWhiteHasBall = true;
		m_bActiveSP = true;
	}
	if (!m_bWhiteHasBall)
	if (IntersectRect(&x, &CurrentP2->RectSpriteAnimePosition(), &m_pPowerUp[m_iSPowerType]->RectSpriteAnimePosition()) && CurrentP2->m_bPlayerHasBall)
	{
		m_bBlueHasBall = true;
		m_bActiveSP = true;
	}
	// The chance that the powerup activates
	m_iSPowerChance = rand() % m_iSuperPChance;
	
	if (m_iSPowerChance == 1 && m_pPowerUp[m_iSPowerType]->m_bActPower == false )
	{
		// Frame setup for the powerup
		m_pPowerUp[m_iSPowerType]->m_bActPower = true;
		m_pPowerUp[m_iSPowerType]->m_pSprite->SetFrame(0, 0);
		m_pPowerUp[m_iSPowerType]->m_pSprite->mPosition.x = rand() % (m_nViewWidth - 200) + 100;
		m_pPowerUp[m_iSPowerType]->m_pSprite->mPosition.y = rand() % (m_nViewHeight - 200) + 100;
	}
	//daca este deja activata
	if (m_pPowerUp[m_iSPowerType]->m_bActPower == true)
	{
		//se face delay intre frameuri
		if (seconds > 0.1)
		{
			m_pPowerUp[m_iSPowerType]->m_iPFrame++;
			seconds = 0;
		}
		//daca se ajunge la finalul sprite-ului
		if (m_pPowerUp[m_iSPowerType]->m_iPFrame == m_pPowerUp[m_iSPowerType]->m_pSprite->GetFrameCount())
		{
			m_iSPowerRepTime++;
			//daca animatia se executa de 2 ori se opreste
			if (m_iSPowerRepTime > 2)
			{
				m_pPowerUp[m_iSPowerType]->m_bActPower = false;
				m_pPowerUp[m_iSPowerType]->m_pSprite->mPosition.x = -9999;
				m_pPowerUp[m_iSPowerType]->m_pSprite->mPosition.y = -9999;
				m_iSPowerRepTime = 0;
				//se calculeaza urmatorul tip de superputere
				m_iSPowerType = rand() % 2;
			}
			//altfel se pozitioneaza pe primul frame
			m_pPowerUp[m_iSPowerType]->m_iPFrame = 0;
		}
		//se deseneaza animatia
		m_pPowerUp[m_iSPowerType]->m_pSprite->SetFrame(m_pPowerUp[m_iSPowerType]->m_iPFrame, 0);
		m_pPowerUp[m_iSPowerType]->m_pSprite->draw();
	}
}
void CGameApp::Goal()
{
	static float seconds = 0;

	//verificare coliziune minge-poarta
	//poarta din stanga
	if (m_pBall->m_pSprite->mPosition.x < 20)
	{
		if (m_pBall->m_pSprite->mPosition.y < m_nViewHeight/2 + m_iGateH/2 && m_pBall->m_pSprite->mPosition.y > m_nViewHeight/2 - m_iGateH/2)
		{
			m_bGol = true;
			m_pBall->Velocity() = Vec2(0, 0);
			m_iTeamWhoScored = 2;
			m_iGolEchipa[0]++;
			if (seconds == 0)
			{
				PlaySound(NULL, 0, 0);
				PlaySound("data/menu/whistle.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			seconds += m_Timer.GetTimeElapsed();
		}
	}
	//poarta din dreapta
	if (m_pBall->m_pSprite->mPosition.x > m_nViewWidth - 20)
	{
		if (m_pBall->m_pSprite->mPosition.y < m_nViewHeight/2 + m_iGateH/2 && m_pBall->m_pSprite->mPosition.y > m_nViewHeight/2 - m_iGateH/2)
		{

			m_bGol = true;
			m_pBall->Velocity() = Vec2(0, 0);
			m_iTeamWhoScored = 1;
			m_iGolEchipa[1]++;
			if (seconds == 0)
			{
				PlaySound(NULL, 0, 0);
				PlaySound("data/menu/whistle.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			seconds += m_Timer.GetTimeElapsed();
		}
	}

	//daca este gol se reinitializeaza pozitiile
	if (m_bGol == true && seconds > 1.5)
	{
		PlaySound(NULL, 0, 0);
		PlaySound("data/menu/ingame_sound.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NOSTOP);

		m_bGol = false;
		seconds = 0;
		Score(m_iTeamWhoScored);

		//player position
		it = whiteTeam.begin();
		(*it++)->Position() = Vec2(double(m_nViewWidth - 300), double(m_nViewHeight - 200));
		(*it++)->Position() = Vec2(double(m_nViewWidth - 300), double(200));
		(*it++)->Position() = Vec2(double(m_nViewWidth - 300), double(m_nViewHeight / 2));
		(*it++)->Position() = Vec2(double(m_nViewWidth - 600), double(m_nViewHeight - 300));
		(*it++)->Position() = Vec2(double(m_nViewWidth - 600), double(300));
		it = blueTeam.begin();
		(*it++)->Position() = Vec2(double(300), double(m_nViewHeight - 200));
		(*it++)->Position() = Vec2(double(300), double(200));
		(*it++)->Position() = Vec2(double(300), double(m_nViewHeight / 2));
		(*it++)->Position() = Vec2(double(600), double(m_nViewHeight - 300));
		(*it++)->Position() = Vec2(double(600), double(300));

		for (it = whiteTeam.begin(); it != whiteTeam.end(); it++) {
			(*it)->m_pSprite->SetFrame(0, 0);
		}
		for (it = blueTeam.begin(); it != blueTeam.end(); it++) {
			(*it)->m_pSprite->SetFrame(0, 0);
		}

		//ball position
		m_pBall->Position() = Vec2(double(m_nViewWidth / 2), double(m_nViewHeight / 2));
		m_pBall->Velocity() = Vec2(0, 0);
	}
}
void CGameApp::DrawPlayers()
{
	static float seconds = 0;

	m_pBall->Draw();

	if (m_bActiveSP)
	{
		seconds += m_Timer.GetTimeElapsed();
		if (seconds < 6)
		{
			//desenez cele 3 stari ale superputerii in functie de timpul ramas
			if (seconds < 3)
				m_pSuperPGreen->draw();
			else if (seconds > 3 && seconds < 4.5)
				m_pSuperPOrange->draw();
			else
				m_pSuperPRed->draw();

			//inlocuieste player-ul cu tornada
			if (m_bWhiteHasBall)
			{
				Maska->m_pSprite->mPosition.x = CurrentP1->m_pSprite->mPosition.x;
				Maska->m_pSprite->mPosition.y = CurrentP1->m_pSprite->mPosition.y;
			}
			else
			{
				Maska->m_pSprite->mPosition.x = CurrentP2->m_pSprite->mPosition.x;
				Maska->m_pSprite->mPosition.y = CurrentP2->m_pSprite->mPosition.y;
			}
			Maska->Draw();
			Maska->FrameMove();

			//deseneaza echipa 1
			for (it = whiteTeam.begin(); it != whiteTeam.end(); ++it)
				if (m_bWhiteHasBall)
				{
					if ((*it) != CurrentP1)
						(*it)->Draw();
				}
				else
					(*it)->Draw();
			//deseneaza echipa 2
			for (it = blueTeam.begin(); it != blueTeam.end(); ++it)
				if (m_bBlueHasBall)
				{
					if ((*it) != CurrentP2)
						(*it)->Draw();
				}
				else
					(*it)->Draw();
		}
		else
		{
			m_bWhiteHasBall = false;
			m_bBlueHasBall = false;
			seconds = 0;
			m_bActiveSP = false;
		}
	}
	else
	{
		for (it = whiteTeam.begin(); it != whiteTeam.end(); ++it)
			(*it)->Draw();
		for (it = blueTeam.begin(); it != blueTeam.end(); ++it)
			(*it)->Draw();
	}
}
void CGameApp::DrawPictures()
{
	//parallax image background
	m_imgParallax.Paint(m_pBBuffer->getDC(), int(m_iParallax), 0);
	m_imgParallax1.Paint(m_pBBuffer->getDC(), int(m_iParallax) - m_nViewWidth, 0);
	

	if (m_iParallax >= m_nViewHeight)
		m_iParallax = 0;
	else
		m_iParallax += float(0.3);

	//desenare linii teren
	m_pFieldLines->draw();
	m_pCircleLine->draw();
	m_pGateLine1->draw();
	m_pGateLine2->draw();

	//desenare jucarori
	DrawPlayers();

	//desenare poarta
	m_pPoarta1->Draw();
	m_pPoarta2->Draw();

	//desenare player selectat
	m_pPlayerMarker1->Draw();
	m_pPlayerMarker2->Draw();
	//contorizare timp
	Time();

	//afisare tabela scor
	m_imgScore.Paint(m_pBBuffer->getDC(), m_nViewWidth / 2 - 150, 10);
	Powers();
	Score(0);
}
void CGameApp::Save()
{
	FILE *f = fopen("save.txt", "w");

	for (it = whiteTeam.begin(); it != whiteTeam.end(); ++it)
		fprintf(f, "%f %f\n", (*it)->m_pSprite->mPosition.x, (*it)->m_pSprite->mPosition.y);
	for (it = blueTeam.begin(); it != blueTeam.end(); ++it)
		fprintf(f, "%f %f\n", (*it)->m_pSprite->mPosition.x, (*it)->m_pSprite->mPosition.y);

	fprintf(f, "%f %f\n", m_pBall->m_pSprite->mPosition.x, m_pBall->m_pSprite->mPosition.y);
	fprintf(f, "%f %f\n", m_pBall->m_pSprite->mVelocity.x, m_pBall->m_pSprite->mVelocity.y);
	fprintf(f, "%d\n", m_pScor1->m_iScoreFrame, m_pScor1->m_iScoreFrame);
	fprintf(f, "%d\n", m_pScor2->m_iScoreFrame, m_pScor2->m_iScoreFrame);
	fprintf(f, "%d\n", m_pTime->m_iTimeFrame, m_pTime->m_iTimeFrame);

	fclose(f);
}
void CGameApp::Load()
{
	FILE * f = fopen("save.txt", "r");
	float a = 0, b = 0;
	int c = 0;
	
	for (it = whiteTeam.begin(); it != whiteTeam.end(); ++it)
	{
		fscanf(f, "%f %f", &a, &b);
		(*it)->m_pSprite->mPosition = Vec2(a, b);
	}
	for (it = blueTeam.begin(); it != blueTeam.end(); ++it)
	{
		fscanf(f, "%f %f", &a, &b);
		(*it)->m_pSprite->mPosition = Vec2(a, b);
	}

	fscanf(f, "%f %f", &a, &b);
	m_pBall->m_pSprite->mPosition = Vec2(a, b);
	fscanf(f, "%f %f", &a, &b);
	m_pBall->m_pSprite->mVelocity = Vec2(a, b);

	fscanf(f, "%d", &c);
	m_pScor1->m_iScoreFrame = c;
	fscanf(f, "%d", &c);
	m_pScor2->m_iScoreFrame = c;

	fscanf(f, "%d", &c);
	m_pTime->m_iTimeFrame = c;
	fscanf(f, "%d", &c);
	m_pTime2->m_iTimeFrame = c;

	fclose(f);
}
