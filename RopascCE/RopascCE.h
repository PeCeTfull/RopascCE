#define dim(x) (sizeof(x) / sizeof(x[0]))

struct decodeUINT
{
	UINT Code;
	LRESULT (*Fxn)(HWND, UINT, WPARAM, LPARAM);
};
struct decodeCMD
{
	UINT Code;
	LRESULT (*Fxn)(HWND, WORD, HWND, WORD);
};

#define IDC_CMDBAR                      1

#define STD_BMPS                        (STD_PRINT + 1)

#define ID_ABSTRACT_ROCK                1
#define ID_ABSTRACT_PAPER               2
#define ID_ABSTRACT_SCISSORS            3

#define SMALL_LOC_STRING_BUFFER_SIZE    32
#define MEDIUM_LOC_STRING_BUFFER_SIZE   128
#define BIG_LOC_STRING_BUFFER_SIZE      512

const enum Language
{
	EN = 0,
	DE = 1,
};

int InitApp(HINSTANCE);
HWND InitInstance(HINSTANCE, LPWSTR, int);
int TermInstance(HINSTANCE, int);

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT DoCreateMain(HWND, UINT, WPARAM, LPARAM);
LRESULT DoPaintMain(HWND, UINT, WPARAM, LPARAM);
LRESULT DoCommandMain(HWND, UINT, WPARAM, LPARAM);
LRESULT DoHibernateMain(HWND, UINT, WPARAM, LPARAM);
LRESULT DoActivateMain(HWND, UINT, WPARAM, LPARAM);
LRESULT DoDestroyMain(HWND, UINT, WPARAM, LPARAM);

LPARAM DoMainCommandNew(HWND, WORD, HWND, WORD);
LPARAM DoMainCommandExit(HWND, WORD, HWND, WORD);
LPARAM DoMainCommandRock(HWND, WORD, HWND, WORD);
LPARAM DoMainCommandPaper(HWND, WORD, HWND, WORD);
LPARAM DoMainCommandScissors(HWND, WORD, HWND, WORD);
LPARAM DoMainCommandEnglish(HWND, WORD, HWND, WORD);
LPARAM DoMainCommandGerman(HWND, WORD, HWND, WORD);
LPARAM DoMainCommandHowToPlay(HWND, WORD, HWND, WORD);
LPARAM DoMainCommandAbout(HWND, WORD, HWND, WORD);

long GetMessageTime();
short GetMoveResourceId(short);
HWND InitCommandBar(HWND);
void Play(HWND, short);
void RefreshResources(HWND);
