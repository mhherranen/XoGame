// main.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szChild[MAX_LOADSTRING];					// the child window class name

// global handle for frame and client windows
HWND g_hWndFrame;
HWND g_hWndMDIClient;

// other global variables
INT_PTR g_nResult;
int g_iGameResult;

// container for open games (Game class)
std::vector<Game> Games;
Game*	pGame;

// container for thread handles and IDs 
std::vector<HANDLE> hThreadVec;
std::vector<DWORD> IDThreadVec;

//create an instance of the parameter class.
CParams   g_Params;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM				RegisterChildWinClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    MPMDIChildWndProc(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	EndGameDialog(HWND, UINT, WPARAM, LPARAM);
void				NewGame(GameMode Mode);
int					MessageBoxHiddenTwoAIGame(double);
void				NewTraining();
DWORD WINAPI		MyThreadFunction(LPVOID lpParam);
void				ErrorHandler(LPTSTR);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_XOGAME, szWindowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_XOGAME_CHILD, szChild, MAX_LOADSTRING);

    MyRegisterClass(hInstance);
	RegisterChildWinClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	
	//static bool Done = CreateLookupTable();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_XOGAME));
	
	MSG msg;
	BOOL bRet;

	// Main message loop:
	while ((bRet = GetMessage(&msg, (HWND)NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			if (!TranslateMDISysAccel(g_hWndMDIClient, &msg)
				&& !TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XOGAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_XOGAME);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM RegisterChildWinClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XOGAME));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_XOGAME);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// Register the MDI child window class. 

	wcex.lpfnWndProc = (WNDPROC)MPMDIChildWndProc;
//	wcex.hIcon = LoadIcon(hInst, IDNOTE);
	wcex.lpszMenuName = (LPCTSTR)NULL;
//	wcex.cbWndExtra = CBWNDEXTRA;
	wcex.lpszClassName = szChild;
	
	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   g_hWndFrame = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   
   if (!g_hWndFrame)
   {
      return FALSE;
   }

   ShowWindow(g_hWndFrame, nCmdShow);
   UpdateWindow(g_hWndFrame);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_CREATE	- create rendering resorces
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_SIZE		- resize the main window
//  WM_DESTROY  - release rendering resources, post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
		CLIENTCREATESTRUCT ccs;

		// Retrieve the handle to the window menu and assign the 
		// first child window identifier. 

		ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), WINDOWMENU);
		ccs.idFirstChild = IDM_WINDOWCHILD;

		// Create the MDI client window. 

		g_hWndMDIClient = CreateWindowEx(WS_EX_CLIENTEDGE, L"MDIClient", (LPCTSTR)NULL,
			WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hWnd, (HMENU)IDC_XOGAME, hInst, (LPVOID)&ccs);

		if (!g_hWndMDIClient)
		{
			return FALSE;
		}

		ShowWindow(g_hWndMDIClient, SW_SHOW);
		}
		break;

	case WM_COMMAND:
		{
		int wmId = LOWORD(wParam);
        // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_NEWGAME_TWOHUMAN:
				NewGame(TwoHumans);
				break;
			case ID_NEWGAME_HUMANAI:
				NewGame(HumanAI);
				break;
			case ID_NEWGAME_AIHUMAN:
				NewGame(AIHuman);
				break;
			case ID_NEWGAME_TWOAI:
				NewGame(TwoAIs);
				break;
			case ID_NEWGAME_TWOAIHIDDEN:
				{
				// compute the duration of the game as well
				clock_t TimeBefore;
				clock_t TimeAfter;

				TimeBefore = clock();
				// create and store Game class instance
				Games.push_back(Game(hWnd, TwoAIsFast));
				TimeAfter = clock();
				const double dTimeDurationClicks = (double)(TimeAfter - TimeBefore);
				// result of the game
				g_iGameResult = Games.back().GetWinner();
				// destroy the game
//				delete &Games.back();
				Games.pop_back();

				MessageBoxHiddenTwoAIGame(dTimeDurationClicks);		
				}
				break;
			case ID_NEWTRAINING:
				NewTraining();
				break;
			// Handle MDI Window commands
			default:
				{
				if (LOWORD(wParam) >= IDM_WINDOWCHILD)
				{
					DefFrameProc(hWnd, g_hWndMDIClient, message, wParam, lParam);
				}
				else
				{
					HWND hChild = (HWND)SendMessage(g_hWndMDIClient, WM_MDIGETACTIVE, 0, 0);
					if (hChild)
					{
						SendMessage(hChild, WM_COMMAND, wParam, lParam);
					}
				}
				}
            }
        }
        break;
	case WM_DESTROY:	
		PostQuitMessage(0);
        break;
    default:
        return DefFrameProc(hWnd, g_hWndMDIClient, message, wParam, lParam);
    }
    return 0;
}


//
//  FUNCTION: MPMDIChildWndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the child windows.
//
LRESULT CALLBACK    MPMDIChildWndProc(HWND hWndChild, UINT message, WPARAM wParam, LPARAM lParam)
{
	// pointer to Games container
	if (Games.size() == 0)
		return DefMDIChildProc(hWndChild, message, wParam, lParam);
	
	int i = 0;
	for (; i < Games.size() && Games[i].GetHandle() != hWndChild; i++);
	
	if (i == Games.size())
		return DefMDIChildProc(hWndChild, message, wParam, lParam);

	Game*	pGame = &Games[i];

	INT_PTR res = NULL;

	switch (message)
	{
//	case WM_CREATE:
//		{
//		ShowWindow(hWndChild, SW_SHOW);
//		UpdateWindow(hWndChild);
//
//		return pGame->GetpRenderGame()->Create();
//		}
	case WM_ENDGAME:
		switch (pGame->GetWinner())
		{
		case WHITE:
			res = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_WHITE_WINS), hWndChild, EndGameDialog);
			break;
		case BLACK:
			res = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_BLACK_WINS), hWndChild, EndGameDialog);
			break;
		case DRAW:
			res = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DRAW), hWndChild, EndGameDialog);
			break;
		}
		if (res == IDC_CLOSE)
			PostMessage(g_hWndMDIClient, WM_MDIDESTROY, (WPARAM)hWndChild, 0);
		else if (res == IDC_NEWGAME)
			pGame->ResetGame();
		break;
	case WM_AIMOVE:
		pGame->OnAIMove();
		break;
	case WM_LBUTTONDOWN:
		D2D1_POINT_2F ptMouse = pGame->GetpRenderGame()->m_pDPIScale->PixelsToDips(
												GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		pGame->OnLButtonDown(ptMouse);
		break;
	case WM_PAINT:
		pGame->GetpRenderGame()->OnPaint(pGame);
		break;
	case WM_DESTROY:
		pGame->Destroy();
		Games.erase(Games.begin() + i);
		break;
	case WM_SIZE:
		pGame->GetpRenderGame()->Resize();
		return  DefMDIChildProc(hWndChild, message, wParam, lParam);
	default:
		return  DefMDIChildProc(hWndChild, message, wParam, lParam);
	}
	return 0;
}


// Message handler for 'About' dialog box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


// Message handler for 'New Game' dialog box.
INT_PTR CALLBACK EndGameDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	// get owner window handle
	HWND hWndOwner = GetWindow(hDlg, GW_OWNER);

	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_NEWGAME:
			g_nResult = IDC_NEWGAME;
			EndDialog(hDlg, g_nResult);
			return (INT_PTR)TRUE;
		case IDC_CLOSE:
			g_nResult = IDC_CLOSE;
			EndDialog(hDlg, g_nResult);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


// method to create a new game with new child window
void NewGame(GameMode Mode)
{
	RECT rcClient;

	GetClientRect(g_hWndMDIClient, &rcClient);

	const int width = int(min(rcClient.bottom, rcClient.right)*GAMEWINDOW_SIZE);

	HWND hWndChild = CreateMDIWindow(szChild, szTitle,
		MDIS_ALLCHILDSTYLES,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		width,
		g_hWndMDIClient,
		hInst,
		0
		);
	// create and store Game class instance
	Games.push_back(Game(hWndChild, Mode));

	ShowWindow(hWndChild, SW_SHOW);
	UpdateWindow(hWndChild);
}


// method to display hidden AI game result and duration in Message Box
int MessageBoxHiddenTwoAIGame(double dTimeDurationClicks)
{
	const double duration = dTimeDurationClicks / CLOCKS_PER_SEC;

	wchar_t wchBuf[512];
	swprintf_s(wchBuf, _countof(wchBuf), L"The result of the game is %ld. The calculation took %ld clicks (%f seconds).\n\n Do you want to continue with new hidden AI vs. AI game?",
		g_iGameResult, (int)dTimeDurationClicks, duration);

	int msgboxID = MessageBox(
		NULL,
		(LPCWSTR)wchBuf,
		(LPCWSTR)L"Hidden AI vs. AI game ended",
		MB_YESNO | MB_DEFBUTTON1
		);

	switch (msgboxID)
	{
	case IDYES:
		PostMessage(g_hWndFrame, WM_COMMAND, MAKEWPARAM(ID_NEWGAME_TWOAIHIDDEN,0), 0);
		break;
//	case IDNO:
//		PostMessage(m_hwnd, WM_DESTROY, 0, 0);
//		break;
	}
	return msgboxID;
}


// create new training instance
void NewTraining()
{
	// Create new thread to begin execution on its own.
	HANDLE hThread;
	DWORD IDThread;

	hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		MyThreadFunction,       // thread function name
		NULL,					// argument to thread function 
		0,                      // use default creation flags 
		&IDThread);   // returns the thread identifier 

	// Check the return value for success.
	// If CreateThread fails, terminate execution. 
	// This will automatically clean up threads and memory. 

	if (hThread == NULL)
	{
		ErrorHandler(TEXT("CreateThread"));
		ExitProcess(3);
	}

	hThreadVec.push_back(hThread);
	IDThreadVec.push_back(IDThread);
}


// thread process function
DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	return 0;
}


// method to display last error
void ErrorHandler(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code.

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message.

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	// Free error-handling buffer allocations.

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}