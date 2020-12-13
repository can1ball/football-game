#include "Main.h"
#include "CGameApp.h"
CGameApp	g_App;	 
HINSTANCE	g_hInst;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int iCmdShow )
{
	srand(time(NULL));
	int retCode;

	// Enable Memory Leak Checking
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// initialize global instance
	g_hInst = hInstance;

	// Initialise the engine.
	if (!g_App.InitInstance( lpCmdLine, iCmdShow )) return 1;
	
	// Begin the gameplay process. Will return when app due to exit.
	retCode = g_App.BeginGame();

	// Shut down the engine, just to be polite, before exiting.
	if ( !g_App.ShutDown() )  MessageBox( 0, _T("Failed to shut system down correctly, please check file named 'debug.txt'.\r\n\r\nIf the problem persists, please contact technical support."), _T("Non-Fatal Error"), MB_OK | MB_ICONEXCLAMATION );

	// Return the correct exit code.
	return retCode;
}