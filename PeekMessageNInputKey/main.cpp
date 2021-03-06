// PeekMessageNInputKey.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "main.h"
#include <list>
#include <math.h>

using namespace std;

#define MAX_LOADSTRING 100

enum MOVE_DIR {
	MD_BACK = -1,
	MD_NONE,
	MD_FRONT
};

struct _tagArea {
	bool bStart;
	POINT ptStart;
	POINT ptEnd;
};

typedef struct _tagRectangle {
	float left, top, right, bottom;
} RECTANGLE, *PRECTANGLE;

typedef struct _tagBullet {
	RECTANGLE	tRC;
	float		fDist;
	float		fLimitDist;
} BULLET, *PBULLET;

typedef struct _tagMonster {
	RECTANGLE	tRC;
	float		fSpeed;
	float		fTime;
	float		fLimitTime;
	int			iDir;
} MONSTER, *PMONSTER;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
_tagArea	g_tArea;
HWND		g_hWnd;
HDC			g_hDC;
bool		g_bLoop;
RECTANGLE	g_tPlayerRC = { 100, 100, 200, 200 };
MONSTER		g_tMonster;

// 플레이어 총알
list<BULLET> g_PlayerBulletList;

// 몬스터 총알
list<BULLET> g_MonsterBulletList;

// 시간을 구하기 위한 변수들
LARGE_INTEGER g_tSecond;
LARGE_INTEGER g_tTime;
float g_fDeltaTime;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void Run();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PEEKMESSAGENINPUTKEY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	g_bLoop = true;
	// 화면용 DC 생성
	g_hDC = GetDC(g_hWnd);

	// 몬스터 초기화
	g_tMonster.tRC.left = 800.f - 100.f;
	g_tMonster.tRC.top = 0.f;
	g_tMonster.tRC.right = 800.f;
	g_tMonster.tRC.bottom = 100.f;
	g_tMonster.fSpeed = 300.f;
	g_tMonster.fTime = 0.f;
	g_tMonster.fLimitTime = 1.1f;
	g_tMonster.iDir = MD_FRONT;

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PEEKMESSAGENINPUTKEY));

	MSG msg;

	QueryPerformanceFrequency(&g_tSecond);
	QueryPerformanceCounter(&g_tTime);

	// 기본 메시지 루프입니다.
	while (g_bLoop)
	{
		// PeekMessage는 메시지가 메시지 큐에 없어도 바로 빠져 나옴
		// 메시지가 있을 경우 true
		// 메시지가 없을 경우 false
		// 메시지가 없는 시간이 윈도우의 데드타임, Idle 타임
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// 윈도우 데드타임일 경우
		else {
			Run();
		}
	}

	ReleaseDC(g_hWnd, g_hDC);

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PEEKMESSAGENINPUTKEY));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL; //  MAKEINTRESOURCEW(IDC_PEEKMESSAGENINPUTKEY);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hWnd = hWnd;

	// 실제 윈도우 타이틀바나 메뉴를 포함한 윈도우의 크기를 구해줌
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	// 위에서 구해준 크기로 윈도우 클라이언트 영역의 크기를
	// 원하는 크기로 맞춰줘야함
	SetWindowPos(hWnd, HWND_TOPMOST, 100, 100, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.

		TCHAR strMouse[64] = {};
		// wsprintf : 유니코드 문자열을 만들어주는 함수
		wsprintf(strMouse, TEXT("Start = (%d, %d)"), g_tArea.ptStart.x, g_tArea.ptStart.y);
		TextOut(hdc, 600, 30, strMouse, lstrlen(strMouse));
		wsprintf(strMouse, TEXT("End = (%d, %d)"), g_tArea.ptEnd.x, g_tArea.ptEnd.y);
		TextOut(hdc, 605, 45, strMouse, lstrlen(strMouse));

		if (g_tArea.bStart) {
			Rectangle(hdc, g_tArea.ptStart.x, g_tArea.ptStart.y, g_tArea.ptEnd.x, g_tArea.ptEnd.y);
		}
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_LBUTTONDOWN:
		// 마우스 위치는 lParam에 들어옴 (하위16비트, 상위16비트를 이용해서)
		if (!g_tArea.bStart) {
			g_tArea.bStart = true;
			g_tArea.ptStart.x = lParam & 0x0000ffff;
			g_tArea.ptStart.y = lParam >> 16;
			g_tArea.ptEnd = g_tArea.ptStart;

			// InvalidateRect 함수는 강제로 WM_PRINT 메시지를 호출하는 함수
			// 1번 인자: 윈도우 핸들
			// 2번 인자: 갱신할 영역 (NULL: 전체 화면)
			// 3번: TRUE일 때 화면을 지우고 갱신, FALSE일 때 화면을 안지우고 갱신
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_MOUSEMOVE:
		if (g_tArea.bStart) {
			g_tArea.ptEnd.x = lParam & 0x0000ffff;
			g_tArea.ptEnd.y = lParam >> 16;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_LBUTTONUP:
		if (g_tArea.bStart) {
			g_tArea.bStart = false;
			g_tArea.ptEnd.x = lParam & 0x0000ffff;
			g_tArea.ptEnd.y = lParam >> 16;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_DESTROY:
		g_bLoop = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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

void Run()
{
	// ㅅㄷㄴㅅ
	// DeltaTime을 구해준다.
	LARGE_INTEGER tTime;
	QueryPerformanceCounter(&tTime);

	g_fDeltaTime = (tTime.QuadPart - g_tTime.QuadPart) / (float)g_tSecond.QuadPart;

	g_tTime = tTime;

	static float fTimeScale = 1.f;

	if (GetAsyncKeyState(VK_F1) & 0x8000) {
		fTimeScale -= g_fDeltaTime;

		if (fTimeScale < 0.f) {
			fTimeScale = 0.f;
		}
	}
	if (GetAsyncKeyState(VK_F2) & 0x8000) {
		fTimeScale += g_fDeltaTime;

		if (fTimeScale > 1.f) {
			fTimeScale = 1.f;
		}
	}

	// 플레이어 초당 이동속도
	//static float fMoveUpTime = 3.f;	// 3초에 걸려서 0.9f 가 0이 되야함
	float fExtraSpeed = 400 * 0.9f;
	float fSpeed = (400 + fExtraSpeed) * g_fDeltaTime * fTimeScale;

	if (GetAsyncKeyState('W') & 0x8000) {
		g_tPlayerRC.top -= fSpeed;
		g_tPlayerRC.bottom -= fSpeed;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		g_tPlayerRC.top += fSpeed;
		g_tPlayerRC.bottom += fSpeed;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		g_tPlayerRC.left -= fSpeed;
		g_tPlayerRC.right -= fSpeed;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		g_tPlayerRC.left += fSpeed;
		g_tPlayerRC.right += fSpeed;
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		BULLET tBullet = {};

		tBullet.tRC.left = g_tPlayerRC.right;
		tBullet.tRC.right = tBullet.tRC.left + 50.f;
		tBullet.tRC.top = (g_tPlayerRC.top + g_tPlayerRC.bottom) / 2.f - 25.f;
		tBullet.tRC.bottom = tBullet.tRC.top + 50.f;
		tBullet.fDist = 0.f;
		tBullet.fLimitDist = 800.f;
		g_PlayerBulletList.push_back(tBullet);
	}

	RECT rcWindow;
	//GetWindowRect(g_hWnd, &rcWindow);
	GetClientRect(g_hWnd, &rcWindow);
	SetRect(&rcWindow, 0, 0, 800, 600);

	if (g_tPlayerRC.top <= 0) {
		g_tPlayerRC.top = 0;
		g_tPlayerRC.bottom = 100;
	}
	else if (g_tPlayerRC.bottom >= rcWindow.bottom) {
		g_tPlayerRC.top = rcWindow.bottom - 100;
		g_tPlayerRC.bottom = rcWindow.bottom;
	}
	if (g_tPlayerRC.left <= 0) {
		g_tPlayerRC.left = 0;
		g_tPlayerRC.right = 100;
	}
	else if (g_tPlayerRC.right >= rcWindow.right) {
		g_tPlayerRC.left = rcWindow.right - 100;
		g_tPlayerRC.right = rcWindow.right;
	}

	// 몬스터 이동
	g_tMonster.tRC.top += g_tMonster.fSpeed * g_fDeltaTime * fTimeScale * g_tMonster.iDir;
	g_tMonster.tRC.bottom += g_tMonster.fSpeed * g_fDeltaTime * fTimeScale * g_tMonster.iDir;

	if (g_tMonster.tRC.bottom >= 600) {
		g_tMonster.iDir = MD_BACK;
		g_tMonster.tRC.top = rcWindow.bottom - 100;
		g_tMonster.tRC.bottom = rcWindow.bottom;
	}
	else if (g_tMonster.tRC.top <= 0) {
		g_tMonster.iDir = MD_FRONT;
		g_tMonster.tRC.top = 0;
		g_tMonster.tRC.bottom = 100;
	}

	// 몬스터 총알 발사 로직
	g_tMonster.fTime += g_fDeltaTime;

	if (g_tMonster.fTime >= g_tMonster.fLimitTime) {
		g_tMonster.fTime -= g_tMonster.fLimitTime;
		BULLET tBullet = {};

		tBullet.tRC.left = g_tMonster.tRC.left - 50.f;
		tBullet.tRC.right = g_tMonster.tRC.left;
		tBullet.tRC.top = (g_tMonster.tRC.top + g_tMonster.tRC.bottom) / 2.f - 25.f;
		tBullet.tRC.bottom = tBullet.tRC.top + 50.f;
		tBullet.fDist = 0.f;
		tBullet.fLimitDist = 800.f;
		g_MonsterBulletList.push_back(tBullet);
	}

	// 플레이어 총알 이동
	list<BULLET>::iterator iter;
	list<BULLET>::iterator iterEnd = g_PlayerBulletList.end();

	fSpeed = 600.f * g_fDeltaTime * fTimeScale;

	for (iter = g_PlayerBulletList.begin(); iter != iterEnd;) {
		(*iter).tRC.left += fSpeed;
		(*iter).tRC.right += fSpeed;

		(*iter).fDist += fSpeed;

		if (((*iter).fDist >= (*iter).fLimitDist) ||
			((*iter).tRC.left >= 800.f)) {
			iter = g_PlayerBulletList.erase(iter);
			iterEnd = g_PlayerBulletList.end();
		}
		else if (g_tMonster.tRC.left <= (*iter).tRC.right && (*iter).tRC.left <= g_tMonster.tRC.right
			&& g_tMonster.tRC.top <= (*iter).tRC.bottom && (*iter).tRC.top <= g_tMonster.tRC.bottom) {
			iter = g_PlayerBulletList.erase(iter);
			iterEnd = g_PlayerBulletList.end();
		}
		else {
			++iter;
		}
	}

	// 몬스터  총알 이동
	iterEnd = g_MonsterBulletList.end();
	for (iter = g_MonsterBulletList.begin(); iter != iterEnd;) {
		(*iter).tRC.left -= fSpeed;
		(*iter).tRC.right -= fSpeed;

		(*iter).fDist += fSpeed;

		if (((*iter).fDist >= (*iter).fLimitDist) ||
			((*iter).tRC.right <= 0.f)) {
			iter = g_MonsterBulletList.erase(iter);
			iterEnd = g_MonsterBulletList.end();
		}
		else if (g_tPlayerRC.left <= (*iter).tRC.right && (*iter).tRC.left <= g_tPlayerRC.right
			&& g_tPlayerRC.top <= (*iter).tRC.bottom && (*iter).tRC.top <= g_tPlayerRC.bottom) {
			iter = g_MonsterBulletList.erase(iter);
			iterEnd = g_MonsterBulletList.end();
		}
		else {
			++iter;
		}
	}

	// 출력
	//Rectangle(g_hDC, 0, 0, 850, 600);
	Rectangle(g_hDC, g_tPlayerRC.left, g_tPlayerRC.top, g_tPlayerRC.right, g_tPlayerRC.bottom);
	Rectangle(g_hDC, g_tMonster.tRC.left, g_tMonster.tRC.top, g_tMonster.tRC.right, g_tMonster.tRC.bottom);

	iterEnd = g_PlayerBulletList.end();
	for (iter = g_PlayerBulletList.begin(); iter != iterEnd; ++iter) {
		Rectangle(g_hDC, (*iter).tRC.left, (*iter).tRC.top, (*iter).tRC.right, (*iter).tRC.bottom);
	}

	iterEnd = g_MonsterBulletList.end();
	for (iter = g_MonsterBulletList.begin(); iter != iterEnd; ++iter) {
		Rectangle(g_hDC, (*iter).tRC.left, (*iter).tRC.top, (*iter).tRC.right, (*iter).tRC.bottom);
	}
}