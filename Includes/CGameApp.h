#ifndef _CGAMEAPP_H_
#define _CGAMEAPP_H_

#include "Main.h"
#include "CTimer.h"
#include "GenericPlayer.h"
#include "Elements.h"
#include "BackBuffer.h"
#include "ImageFile.h"



class CGameApp
{
public:
			   CGameApp();
	virtual    ~CGameApp();

	LRESULT	   DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	bool	   InitInstance( LPCTSTR lpCmdLine, int iCmdShow);
	int		   BeginGame();
	bool	   ShutDown();
	
private:
	bool		BuildObjects();
	void		ReleaseObjects();
	void		FrameAdvance();
	bool		CreateDisplay();
	void		ChangeDevice();
	void		SetupGameState();
	void		AnimateObjects();
	void		DrawObjects();
	void		ProcessInput();
	void		Shoot(std::list<GenericPlayer*> lista);
	void		CheckClosestPlayer(std::list<GenericPlayer*> lista);
	void		Score(int param);
	void		Time();
	void		AI();
	void		Goal();
	void        Save();
	void		Clasament(int aux);
	void		Load();
	void		Powers();
	void        DrawPlayers();
	void        DrawPictures();
	void		Move(ULONG ulDirection, std::list<GenericPlayer*> lista);
	void        BallColision(std::list<GenericPlayer*> lista);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	CTimer				   m_Timer;		
	ULONG				   m_LastFrameRate;	
	POINT				   m_OldCursorPos;
	HINSTANCE			   m_hInstance;
	HWND				   m_hWnd;			
	HICON				   m_hIcon;			
	HMENU				   m_hMenu;		

	int					   selectionMainMenu = 1;
	int					   selectionInGameMenu = 1;
	
	bool				   m_bActive;	
	bool				   m_bMenuActive;
	bool				   m_bInGameMenuActive;
	bool                   m_bGol;
	//bool				   m_bClasament;
	bool                   m_bActiveSP;
	bool				   m_bBlueHasBall;
	bool				   m_bWhiteHasBall;
	float				   m_iParallax;
	float				   m_a;

	int					   m_iNrOfPlayers;
	int					   m_iNrmenu;
	int					   m_iScore[1];
	int					   m_iSPowerType;
	int					   m_iSPowerChance;
	int				       m_iSPowerRepTime;
	int					   m_iTeamWhoScored;
	int					   m_iBallSpeed;
	int				       m_iGateH;
	int					   m_iSuperPChance;
	int					   m_iGolEchipa[2];
	ULONG				   m_nViewX;		
	ULONG				   m_nViewY;		   
	ULONG				   m_nViewWidth;	  
	ULONG				   m_nViewHeight;	 

	CImageFile			   m_imgBackground;
	CImageFile			   m_imgParallax;
	CImageFile			   m_imgParallax1;
	CImageFile			   m_imgBackgroundMainMenu;
	CImageFile			   m_imgBackgroundInGame;
	CImageFile			   m_imgScore;

	BackBuffer*			   m_pBBuffer;
	GenericPlayer*		   CurrentP1;
	GenericPlayer*		   CurrentP2;
	Mask*                  Maska;
	Ball*			   	   m_pBall;
	Timp*				   m_pTime;
	Timp*		           m_pTime2;
	Scor*		           m_pScor1;
	Scor*		           m_pScor2;
	Elements*			   m_pPoarta1;
	Elements*			   m_pPoarta2;
	Elements*			   m_pPlayerMarker1;
	Elements*			   m_pPlayerMarker2;
	PUp*				   m_pPowerUp[2];
	string				   framerate;

	Sprite*   m_pFieldLines;
	Sprite*   m_pCircleLine;
	Sprite*   m_pGateLine1;
	Sprite*   m_pGateLine2;
	Sprite*   m_pSuperPGreen;
	Sprite*   m_pSuperPRed;
	Sprite*   m_pSuperPOrange;
	std::list<GenericPlayer*> whiteTeam, blueTeam;
	std::list<GenericPlayer*>::iterator it,it2;
};

#endif