#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "ropascce.h"

const TCHAR _szAppName[] = TEXT("RopascCE");
HINSTANCE _hInst;
enum Language _eCurrentLanguage = EN;
int _nVictories = 0;
int _nLosses = 0;
int _nDraws = 0;
short _sLastPlayerMove = 0;
short _sLastCpuMove = 0;

const struct decodeUINT MainMessages[] =
{
	WM_CREATE, DoCreateMain,
	WM_PAINT, DoPaintMain,
	WM_COMMAND, DoCommandMain,
	WM_HIBERNATE, DoHibernateMain,
	WM_ACTIVATE, DoActivateMain,
	WM_DESTROY, DoDestroyMain,
};

const struct decodeCMD MainCommandItems[] =
{
	ID_GAME_NEW, DoMainCommandNew,
	ID_GAME_ROCK, DoMainCommandRock,
	ID_GAME_PAPER, DoMainCommandPaper,
	ID_GAME_SCISSORS, DoMainCommandScissors,
	ID_GAME_EXIT, DoMainCommandExit,
	ID_LANGUAGE_ENGLISH, DoMainCommandEnglish,
	ID_LANGUAGE_GERMAN, DoMainCommandGerman,
	ID_HELP_HOWTOPLAY, DoMainCommandHowToPlay,
	ID_HELP_ABOUT, DoMainCommandAbout,
};

const TBBUTTON tbMainMenuButtons[] =
{
	{0, 0, 0, TBSTYLE_SEP, 0, 0},
	{STD_BMPS, ID_GAME_ROCK, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
	{STD_BMPS + 1, ID_GAME_PAPER, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
	{STD_CUT, ID_GAME_SCISSORS, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	int rc = 0;
	HWND hwndMain;

	rc = InitApp(hInstance);
	if (rc) return rc;

	hwndMain = InitInstance(hInstance, lpCmdLine, nCmdShow);
	if (hwndMain == 0) return 0x10;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return TermInstance(hInstance, msg.wParam);
}

int InitApp(HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style = 0;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _szAppName;

	if (RegisterClass (&wc) == 0) return 1;

	return 0;
}

HWND InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;

	_hInst = hInstance;

	hWnd = CreateWindow(_szAppName,
		TEXT("Ropasc CE"),
		WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!IsWindow(hWnd)) return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int TermInstance(HINSTANCE hInstance, int nDefRC)
{
	return nDefRC;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	INT i;
	for (i = 0; i < dim(MainMessages); i++)
	{
		if (wMsg == MainMessages[i].Code)
			return (*MainMessages[i].Fxn)(hWnd, wMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, wMsg, wParam, lParam);
}

LRESULT DoCreateMain(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndCB;
	HICON hIcon;

	hwndCB = InitCommandBar(hWnd);

	hIcon = (HICON)SendMessage(hWnd, WM_GETICON, 0, 0);
	if (hIcon == 0)
	{
		hIcon = LoadImage(_hInst, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0);
		SendMessage(hWnd, WM_SETICON, FALSE, (LPARAM)hIcon);
	}

	return 0;
}

LRESULT DoPaintMain(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rect;
	HBRUSH hbr;
	HDC hdc;
	INT nRectTop;
	TCHAR szLocStringBuffer[SMALL_LOC_STRING_BUFFER_SIZE];
	TCHAR szMoveLocStringBuffer[SMALL_LOC_STRING_BUFFER_SIZE];
	TCHAR szOut[SMALL_LOC_STRING_BUFFER_SIZE];

	GetClientRect(hWnd, &rect);
	rect.top += CommandBar_Height(GetDlgItem(hWnd, IDC_CMDBAR));
	nRectTop = rect.top;

	hdc = BeginPaint(hWnd, &ps);

	hbr = SelectObject(hdc, GetStockObject(DKGRAY_BRUSH));
	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	SelectObject(hdc, hbr);

	hbr = SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	RoundRect(hdc, (rect.left + rect.right) / 2 - 82, (rect.top + rect.bottom) / 2 + 80, (rect.left + rect.right) / 2 + 82, (rect.top + rect.bottom) / 2 - 80, 25, 25);
	SelectObject(hdc, hbr);

	rect.top -= 108; // - 24 - 42 - 42

	LoadString(_hInst, IDS_VICTORIES + _eCurrentLanguage, szLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
	wsprintf(szOut, szLocStringBuffer, _nVictories);
	DrawText(hdc, TEXT("                                "), -1, &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DrawText(hdc, szOut, -1, &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect.top += 42;

	LoadString(_hInst, IDS_LOSSES + _eCurrentLanguage, szLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
	wsprintf(szOut, szLocStringBuffer, _nLosses);
	DrawText(hdc, TEXT("                                "), -1, &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DrawText(hdc, szOut, -1, &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect.top += 42;

	LoadString(_hInst, IDS_DRAWS + _eCurrentLanguage, szLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
	wsprintf(szOut, szLocStringBuffer, _nDraws);
	DrawText(hdc, TEXT("                                "), -1, &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DrawText(hdc, szOut, -1, &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect.top += 84; // 42 + 42

	LoadString(_hInst, IDS_PLAYERMOVE + _eCurrentLanguage, szLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
	LoadString(_hInst, GetMoveResourceId(_sLastPlayerMove), szMoveLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
	wsprintf(szOut, szLocStringBuffer, szMoveLocStringBuffer);
	DrawText(hdc, TEXT("                                "), -1, &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DrawText(hdc, szOut, -1, &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect.top += 42;

	LoadString(_hInst, IDS_CPUMOVE + _eCurrentLanguage, szLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
	LoadString(_hInst, GetMoveResourceId(_sLastCpuMove), szMoveLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
	wsprintf(szOut, szLocStringBuffer, szMoveLocStringBuffer);
	DrawText(hdc, TEXT("                                "), -1, &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DrawText(hdc, szOut, -1, &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect.top = nRectTop;

	EndPaint(hWnd, &ps);

	return 0;
}

LRESULT DoCommandMain(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	WORD idItem, wNotifyCode;
	HWND hwndCtl;
	INT i;

	idItem = (WORD)LOWORD(wParam);
	wNotifyCode = (WORD)HIWORD(wParam);
	hwndCtl = (HWND)lParam;

	for (i = 0; i < dim(MainCommandItems); i++)
	{
		if (idItem == MainCommandItems[i].Code)
			return (*MainCommandItems[i].Fxn)(hWnd, idItem, hwndCtl, wNotifyCode);
	}

	return 0;
}

LRESULT DoHibernateMain(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	if (GetActiveWindow() != hWnd)
	{
		CommandBar_Destroy(GetDlgItem(hWnd, IDC_CMDBAR));
	}

	return 0;
}

LRESULT DoActivateMain(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndCB;

	if ((LOWORD(wParam) != WA_INACTIVE) &&
		(GetDlgItem(hWnd, IDC_CMDBAR) == 0))
	{
		hwndCB = InitCommandBar(hWnd);
	}

	return 0;
}

LRESULT DoDestroyMain(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);

	return 0;
}

LPARAM DoMainCommandNew(HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode)
{
	_nVictories = 0;
	_nLosses = 0;
	_nDraws = 0;
	_sLastPlayerMove = 0;
	_sLastCpuMove = 0;

	InvalidateRect(hWnd, NULL, FALSE);

	return 0;
}

LPARAM DoMainCommandExit(HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode)
{
	SendMessage(hWnd, WM_CLOSE, 0, 0);

	return 0;
}

LPARAM DoMainCommandRock(HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode)
{
	Play(hWnd, ID_ABSTRACT_ROCK);

	return 0;
}

LPARAM DoMainCommandPaper(HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode)
{
	Play(hWnd, ID_ABSTRACT_PAPER);

	return 0;
}

LPARAM DoMainCommandScissors(HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode)
{
	Play(hWnd, ID_ABSTRACT_SCISSORS);

	return 0;
}

LPARAM DoMainCommandEnglish(HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode)
{
	_eCurrentLanguage = EN;
	RefreshResources(hWnd);

	return 0;
}

LPARAM DoMainCommandGerman(HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode)
{
	_eCurrentLanguage = DE;
	RefreshResources(hWnd);

	return 0;
}

LPARAM DoMainCommandHowToPlay(HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode)
{
	TCHAR szMessageTitleLocStringBuffer[SMALL_LOC_STRING_BUFFER_SIZE];
	TCHAR szMessageContentsLocStringBuffer[BIG_LOC_STRING_BUFFER_SIZE];

	LoadString(_hInst, IDS_HOW_TO_PLAY_TITLE + _eCurrentLanguage, szMessageTitleLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
	LoadString(_hInst, IDS_HOW_TO_PLAY + _eCurrentLanguage, szMessageContentsLocStringBuffer, BIG_LOC_STRING_BUFFER_SIZE);

	MessageBox(hWnd, szMessageContentsLocStringBuffer, szMessageTitleLocStringBuffer, MB_ICONASTERISK | MB_OK);

	return 0;
}

LPARAM DoMainCommandAbout(HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode)
{
	TCHAR szMessageTitleLocStringBuffer[SMALL_LOC_STRING_BUFFER_SIZE];
	TCHAR szMessageContentsLocStringBuffer[BIG_LOC_STRING_BUFFER_SIZE];
	TCHAR szFullMessageContentsLocStringBuffer[BIG_LOC_STRING_BUFFER_SIZE];

	LoadString(_hInst, IDS_ABOUT_PROGRAM_TITLE + _eCurrentLanguage, szMessageTitleLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
	LoadString(_hInst, IDS_ABOUT_PROGRAM + _eCurrentLanguage, szMessageContentsLocStringBuffer, BIG_LOC_STRING_BUFFER_SIZE);
	wsprintf(szFullMessageContentsLocStringBuffer, szMessageContentsLocStringBuffer, TEXT(__DATE__), TEXT(__TIME__));

	MessageBox(hWnd, szFullMessageContentsLocStringBuffer, szMessageTitleLocStringBuffer, MB_OK);

	return 0;
}

long GetMessageTime()
{
	SYSTEMTIME rSysTime;
	long lTotalMsecs = 0;

	GetSystemTime(&rSysTime);

	// Msecs = hours * 60min/hr * 60sec/min * 1000mSec/sec
	lTotalMsecs  = (long)rSysTime.wHour * 60L * 60L * 1000L;

	// Msecs = min              * 60sec/min * 1000mSec/sec
	lTotalMsecs += (long)rSysTime.wMinute * 60L * 1000L;

	// Msecs = sec                          * 1000mSec/sec
	lTotalMsecs += (long)rSysTime.wSecond * 1000L;

	// Msecs = mSecs
	lTotalMsecs += (long)rSysTime.wMilliseconds;

	return lTotalMsecs;
}

short GetMoveResourceId(short sMove)
{
	switch (sMove)
	{
		case ID_ABSTRACT_ROCK:
			return IDS_ROCK + _eCurrentLanguage;
		case ID_ABSTRACT_PAPER:
			return IDS_PAPER + _eCurrentLanguage;
		case ID_ABSTRACT_SCISSORS:
			return IDS_SCISSORS + _eCurrentLanguage;
		default:
			return IDS_NOTHING + _eCurrentLanguage;
	}
}

HWND InitCommandBar(HWND hWnd)
{
	HWND hwndCB = CommandBar_Create(_hInst, hWnd, IDC_CMDBAR);
	CommandBar_InsertMenubar(hwndCB, _hInst, IDR_MAINMENU + _eCurrentLanguage, 0);

	CommandBar_AddBitmap(hwndCB, HINST_COMMCTRL, IDB_STD_SMALL_COLOR, STD_BMPS, 0, 0);
	CommandBar_AddBitmap(hwndCB, NULL, (int)LoadBitmap(_hInst, TEXT("IDB_MOVES")), 2, 0, 0);

	CommandBar_AddButtons(hwndCB, dim(tbMainMenuButtons), tbMainMenuButtons);
	CommandBar_AddAdornments(hwndCB, 0, 0);

	return hwndCB;
}

void Play(HWND hWnd, short sPlayerMove)
{
	TCHAR szMessageTitleLocStringBuffer[SMALL_LOC_STRING_BUFFER_SIZE];
	TCHAR szMessageContentsLocStringBuffer[MEDIUM_LOC_STRING_BUFFER_SIZE];

	_sLastPlayerMove = sPlayerMove;

	srand(GetMessageTime());
	_sLastCpuMove = rand() % 3 + 1;

	if (_sLastPlayerMove == _sLastCpuMove)
	{
		_nDraws += 1;
		InvalidateRect(hWnd, NULL, FALSE);
	}
	else if (_sLastPlayerMove == ID_ABSTRACT_ROCK && _sLastCpuMove == ID_ABSTRACT_PAPER
		|| _sLastPlayerMove == ID_ABSTRACT_PAPER && _sLastCpuMove == ID_ABSTRACT_SCISSORS
		|| _sLastPlayerMove == ID_ABSTRACT_SCISSORS && _sLastCpuMove == ID_ABSTRACT_ROCK)
	{
		_nLosses += 1;
		InvalidateRect(hWnd, NULL, FALSE);

		if (_nLosses == 100 && _nLosses > _nVictories)
		{
			LoadString(_hInst, IDS_GAME_OVER + _eCurrentLanguage, szMessageTitleLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
			LoadString(_hInst, IDS_DEFEAT_100_TIMES + _eCurrentLanguage, szMessageContentsLocStringBuffer, MEDIUM_LOC_STRING_BUFFER_SIZE);
			MessageBox(hWnd, szMessageContentsLocStringBuffer, szMessageTitleLocStringBuffer, MB_ICONEXCLAMATION | MB_OK);
		}
	}
	else if (_sLastPlayerMove == ID_ABSTRACT_ROCK && _sLastCpuMove == ID_ABSTRACT_SCISSORS
		|| _sLastPlayerMove == ID_ABSTRACT_PAPER && _sLastCpuMove == ID_ABSTRACT_ROCK
		|| _sLastPlayerMove == ID_ABSTRACT_SCISSORS && _sLastCpuMove == ID_ABSTRACT_PAPER)
	{
		_nVictories += 1;
		InvalidateRect(hWnd, NULL, FALSE);

		if (_nVictories == 100 && _nVictories > _nLosses)
		{
			LoadString(_hInst, IDS_GAME_OVER + _eCurrentLanguage, szMessageTitleLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
			LoadString(_hInst, IDS_CONGRATULATIONS_100_TIMES + _eCurrentLanguage, szMessageContentsLocStringBuffer, MEDIUM_LOC_STRING_BUFFER_SIZE);
			MessageBox(hWnd, szMessageContentsLocStringBuffer, szMessageTitleLocStringBuffer, MB_ICONEXCLAMATION | MB_OK);
		}
	}
	else
	{
		InvalidateRect(hWnd, NULL, FALSE);
		LoadString(_hInst, IDS_CRITICAL_ERROR + _eCurrentLanguage, szMessageTitleLocStringBuffer, SMALL_LOC_STRING_BUFFER_SIZE);
		LoadString(_hInst, IDS_UNEXPECTED_MOVE + _eCurrentLanguage, szMessageContentsLocStringBuffer, MEDIUM_LOC_STRING_BUFFER_SIZE);
		MessageBox(hWnd, szMessageContentsLocStringBuffer, szMessageTitleLocStringBuffer, MB_ICONHAND | MB_OK);
	}
}

void RefreshResources(HWND hWnd)
{
    CommandBar_Destroy(GetDlgItem(hWnd, IDC_CMDBAR));
	InitCommandBar(hWnd);
	InvalidateRect(hWnd, NULL, FALSE);
}
