// PeekMessageNInputKey.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "main.h"
#include <list>
#include <math.h>

using namespace std;

#define PI 3.14159265358979323846f
#define MAX_LOADSTRING 100

enum MOVE_DIR {
	MD_BACK = -1,
	MD_NONE,
	MD_FRONT,
	MD_LEFT = -1,
	MD_RIGHT = 1,
};

struct _tagArea {
	bool bStart;
	POINT ptStart;
	POINT ptEnd;
};

typedef struct _tagRectangle {
	float left, top, right, bottom;
} RECTANGLE, *PRECTANGLE;

typedef struct _tagSphere {
	float x, y;
	float r;
} SPHERE, *PSPHERE;

typedef struct _tagBullet {
	SPHERE		tSphere;
	float		fDist;
	float		fLimitDist;
	float		fAngle;
} BULLET, *PBULLET;

typedef struct _tagMonster {
	SPHERE		tSphere;
	float		fSpeed;
	float		fTick;
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
SPHERE		g_tPlayer = { 50.f, 50.f, 50.f };
bool		g_bRotate = true;

POINT		g_tGunPos;
float		g_fGunLength = 70.f;
float		g_fPlayerAngle;

POINT		g_tGunEnemyPos;
float		g_fGunEnemyLength = 70.f;
float		g_fEnemyAngle;
MONSTER		g_tMonster;

// 플레이어 총알
list<BULLET> g_PlayerBulletList;

// 몬스터 총알
list<BULLET> g_MonsterBulletList;

// 블록 피하기 몬스터
list<MONSTER> g_MonsterList;

// 시간을 구하기 위한 변수들
LARGE_INTEGER g_tSecond;
LARGE_INTEGER g_tTime;
float g_fDeltaTime;

// 마우스 클릭 횟수
int iClickCount;
int iClickMax;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void Run();
void Run2();
void Run3();
void Run4();

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

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PEEKMESSAGENINPUTKEY));

	MSG msg;

	QueryPerformanceFrequency(&g_tSecond);
	QueryPerformanceCounter(&g_tTime);

	bool NormalRun = false;
	bool Game1Run = false;
	bool Game2Run = false;
	bool Game3Run = false;
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
			if (GetAsyncKeyState(VK_F5) & 0x8000) {
				// 몬스터 초기화
				g_tMonster.tSphere.x = 800.f - 50.f;
				g_tMonster.tSphere.y = 50.f;
				g_tMonster.tSphere.r = 50.f;
				g_tMonster.fSpeed = 300.f;
				g_tMonster.fTime = 0.f;
				g_tMonster.fLimitTime = 1.1f;
				g_tMonster.iDir = MD_FRONT;

				// 캐릭터 초기화
				if (g_bRotate == true) {
					g_tPlayerRC.left = 100;
					g_tPlayerRC.top = 100;
					g_tPlayerRC.right = 200;
					g_tPlayerRC.bottom = 200;
					g_bRotate = false;
				}
				else {
					g_tPlayer.x = 50.f;
					g_tPlayer.y = 50.f;
					g_tPlayer.r = 50.f;

					// 플레이어 총구의 위치를 구해준다.
					g_tGunPos.x = g_tPlayer.x + cosf(g_fPlayerAngle) * g_fGunLength;
					g_tGunPos.y = g_tPlayer.y + sinf(g_fPlayerAngle) * g_fGunLength;

					// 몬스터의 총구 위치를 구해준다.
					g_tGunPos.x = g_tMonster.tSphere.x + cosf(g_fEnemyAngle) * g_fGunEnemyLength;
					g_tGunPos.y = g_tMonster.tSphere.y + sinf(g_fEnemyAngle) * g_fGunEnemyLength;
					g_bRotate = true;
				}
				NormalRun = true;
				Game1Run = false;
				Game2Run = false;
				Game3Run = false;
				InvalidateRect(g_hWnd, NULL, TRUE);
			}
			else if (GetAsyncKeyState(VK_F6) & 0x8000) {
				// 캐릭터 초기화
				g_tPlayerRC.left = 375;
				g_tPlayerRC.top = 520;
				g_tPlayerRC.right = 425;
				g_tPlayerRC.bottom = 570;
				NormalRun = false;
				Game1Run = true;
				Game2Run = false;
				Game3Run = false;
				InvalidateRect(g_hWnd, NULL, TRUE);
			}
			else if (GetAsyncKeyState(VK_F7) & 0x8000) {
				// 플레이어 초기화
				iClickCount = 0;
				iClickMax = 3;
				// 몬스터 초기화
				g_tMonster.tSphere.x = 50.f - 25.f;
				g_tMonster.tSphere.y = 600.f;
				g_tMonster.tSphere.r = 25.f;
				g_tMonster.fSpeed = 300.f;
				g_tMonster.fTick = 0.f;
				g_tMonster.fTime = 0.f;
				g_tMonster.fLimitTime = 0.001f;
				g_tMonster.iDir = MD_FRONT;
				NormalRun = false;
				Game1Run = false;
				Game2Run = true;
				Game3Run = false;
				InvalidateRect(g_hWnd, NULL, TRUE);
			}
			else if (GetAsyncKeyState(VK_F8) & 0x8000) {
				NormalRun = false;
				Game1Run = false;
				Game2Run = false;
				Game3Run = true;
				InvalidateRect(g_hWnd, NULL, TRUE);
			}

			if (NormalRun) {
				Run();
			}
			else if (Game1Run) {
				Run2();
			}
			else if (Game2Run) {
				Run3();

			}
			else if (Game3Run) {

			}
			else {
				TCHAR strMouse[64] = {};
				// wsprintf : 유니코드 문자열을 만들어주는 함수
				wsprintf(strMouse, TEXT("F5: 일반       "));
				TextOut(g_hDC, 600, 30, strMouse, lstrlen(strMouse));
				wsprintf(strMouse, TEXT("F6: 블록 피하기 게임"));
				TextOut(g_hDC, 600, 50, strMouse, lstrlen(strMouse));
				wsprintf(strMouse, TEXT("F7: 사격 게임"));
				TextOut(g_hDC, 600, 70, strMouse, lstrlen(strMouse));
				wsprintf(strMouse, TEXT("F8: 갤러그 게임"));
				TextOut(g_hDC, 600, 90, strMouse, lstrlen(strMouse));

				LARGE_INTEGER tTime;
				QueryPerformanceCounter(&tTime);
				g_tTime = tTime;
			}
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

bool RectangleCircleCollision(RECTANGLE &tPlayer, BULLET &tBullet) {
	bool	bResult = false;
	float	r, fX, fY, fDist;

	r = tBullet.tSphere.r;

	if (tBullet.tSphere.x >= tPlayer.left - r && tBullet.tSphere.x <= tPlayer.right + r &&
		tBullet.tSphere.y >= tPlayer.top - r && tBullet.tSphere.y <= tPlayer.bottom + r) {
		// 큰 장방형 체크
		bResult = true;
		r = tBullet.tSphere.r;

		// 왼쪽 끝 체크
		if (tBullet.tSphere.x < tPlayer.left) {
			// 좌측 상단 모서리 체크
			if (tBullet.tSphere.y < tPlayer.top) {
				fX = tPlayer.left - tBullet.tSphere.x;
				fY = tPlayer.top - tBullet.tSphere.y;
				fDist = fX * fX + fY * fY;
				if (fDist >= r * r) {
					bResult = false;
				}
			}
			else {
				// 좌측 하단 모서리 체크
				if (tBullet.tSphere.y > tPlayer.bottom) {
					fX = tPlayer.left - tBullet.tSphere.x;
					fY = tPlayer.bottom - tBullet.tSphere.y;
					fDist = fX * fX + fY * fY;
					if (fDist >= r * r) {
						bResult = false;
					}
				}
			}
		}
		else {
			// 오른쪽 끝 체크
			if (tBullet.tSphere.x < tPlayer.right) {
				// 우측 상단 모서리 체크
				if (tBullet.tSphere.y < tPlayer.top) {
					fX = tPlayer.right - tBullet.tSphere.x;
					fY = tPlayer.top - tBullet.tSphere.y;
					fDist = fX * fX + fY * fY;
					if (fDist >= r * r) {
						bResult = false;
					}
				}
				else {
					// 우측 하단 모서리 체크
					if (tBullet.tSphere.y > tPlayer.bottom) {
						fX = tPlayer.right - tBullet.tSphere.x;
						fY = tPlayer.bottom - tBullet.tSphere.y;
						fDist = fX * fX + fY * fY;
						if (fDist >= r * r) {
							bResult = false;
						}
					}
				}
			}
		}
	}

	return bResult;
}

void Run4()
{

}

void Run3()
{
	// DeltaTime
	LARGE_INTEGER tTime;
	QueryPerformanceCounter(&tTime);

	g_fDeltaTime = (tTime.QuadPart - g_tTime.QuadPart) / (float)g_tSecond.QuadPart;

	g_tTime = tTime;

	static float fTimeScale = 1.f;
	static int iDead = 0;

	// 플레이어 초당 이동속도
	float fExtraSpeed = 0.f;
	float fSpeed = (400 + fExtraSpeed) * g_fDeltaTime * fTimeScale;

	static bool bClick = false;
	if (bClick == false && (GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
		bClick = true;
		if (iClickCount < iClickMax) {
			iClickCount++;
			// 마우스 위치를 얻어온다.
			POINT ptMouse;

			// 아래 함수는 마우스 위치를 얻어온다.
			// 마우스 위치는 스크린 좌표 기준으로 받아온다.
			GetCursorPos(&ptMouse);

			// 스크린 좌표를 클라이언트 좌표로 변환한다.
			ScreenToClient(g_hWnd, &ptMouse);

			float fX = g_tMonster.tSphere.x - ptMouse.x;
			float fY = g_tMonster.tSphere.y - ptMouse.y;
			float fDist = sqrtf(fX * fX + fY * fY);
			if (fDist <= g_tMonster.tSphere.r) {
				MessageBox(NULL, L"몬스터 클릭", L"마우스 클릭", MB_OK);
			}
		}
		else {
			TCHAR strLife[64] = {};
			wsprintf(strLife, TEXT("총알 없음..."));
			TextOut(g_hDC, 600, 100, strLife, lstrlen(strLife));
		}
	}
	else if (bClick == true && !(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
		bClick = false;
	}

	RECT rcWindow;
	//GetWindowRect(g_hWnd, &rcWindow);
	GetClientRect(g_hWnd, &rcWindow);
	SetRect(&rcWindow, 0, 0, 800, 600);

	//g_tMonster.tSphere.y += g_tMonster.fSpeed * g_fDeltaTime * fTimeScale * g_tMonster.iDir;

	//if ((g_tMonster.tSphere.y + g_tMonster.tSphere.r) >= 600) {
	//	g_tMonster.iDir = MD_BACK;
	//	g_tMonster.tSphere.y = 600 - g_tMonster.tSphere.r;
	//}
	//else if ((g_tMonster.tSphere.y - g_tMonster.tSphere.r) <= 0) {
	//	g_tMonster.iDir = MD_FRONT;
	//	g_tMonster.tSphere.y = g_tMonster.tSphere.r;
	//}
	static const float fAngle = 60.f;	// 60도
	static const float v0 = 40.5f;		// 던지는 힘
	static float gravity = 1000.8f + rand()%501;		// 중력

	gravity = 1000.8f + rand() % 501;

	g_tMonster.fTime += g_fDeltaTime;
	if (g_tMonster.fTime >= g_tMonster.fLimitTime) {
		g_tMonster.fTime -= g_tMonster.fLimitTime;

		float radian = fAngle * PI / 180;
		float sin = sinf(radian);
		float cos = cosf(radian);

		g_tMonster.fTick += g_fDeltaTime;

		// 1/2 = 0.5
		g_tMonster.tSphere.y -= ((v0 * sin * g_tMonster.fTick) - (0.5f * gravity * g_tMonster.fTick * g_tMonster.fTick));
		g_tMonster.tSphere.x += (v0 * cos * g_tMonster.fTick);
	}

	TCHAR delta[64] = {};
	_stprintf_s(delta, 64, TEXT("deltaTime = %f"), g_fDeltaTime);
	TextOut(g_hDC, 600, 60, delta, lstrlen(delta));
	_stprintf_s(delta, 64, TEXT("click = (%d / %d)"), iClickCount, iClickMax);
	TextOut(g_hDC, 600, 80, delta, lstrlen(delta));

	// 이동 표시
	//Rectangle(g_hDC, g_tPlayerRC.left, g_tPlayerRC.top, g_tPlayerRC.right, g_tPlayerRC.bottom);
	Ellipse(g_hDC, g_tMonster.tSphere.x - g_tMonster.tSphere.r, g_tMonster.tSphere.y - g_tMonster.tSphere.r,
		g_tMonster.tSphere.x + g_tMonster.tSphere.r, g_tMonster.tSphere.y + g_tMonster.tSphere.r);
}

void Run2()
{
	// DeltaTime
	LARGE_INTEGER tTime;
	QueryPerformanceCounter(&tTime);

	g_fDeltaTime = (tTime.QuadPart - g_tTime.QuadPart) / (float)g_tSecond.QuadPart;

	g_tTime = tTime;

	static float fTimeScale = 1.f;
	static int iDead = 0;

	// 플레이어 초당 이동속도
	float fExtraSpeed = 0.f;
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

	RECT rcWindow;
	GetClientRect(g_hWnd, &rcWindow);
	SetRect(&rcWindow, 0, 0, 800, 600);

	if (g_tPlayerRC.top <= 0) {
		g_tPlayerRC.top = 0;
		g_tPlayerRC.bottom = 50;
	}
	else if (g_tPlayerRC.bottom >= rcWindow.bottom) {
		g_tPlayerRC.top = rcWindow.bottom - 50;
		g_tPlayerRC.bottom = rcWindow.bottom;
	}
	if (g_tPlayerRC.left <= 0) {
		g_tPlayerRC.left = 0;
		g_tPlayerRC.right = 50;
	}
	else if (g_tPlayerRC.right >= rcWindow.right) {
		g_tPlayerRC.left = rcWindow.right - 50;
		g_tPlayerRC.right = rcWindow.right;
	}

	static float fTime = 0.f;
	static float fLimitTime = 0.4f;
	static int iMonsterCount = 30;

	list<MONSTER>::iterator iter;
	list<MONSTER>::iterator iterEnd;

	fTime += g_fDeltaTime;
	if (fTime >= fLimitTime) {
		fTime -= fLimitTime;

		bool bCollision = false;
		for (int i = 0; i < iMonsterCount - g_MonsterList.size(); i++) {
			MONSTER tMonster = {};

			// 몬스터 초기화
			tMonster.tSphere.x = 0.f;
			tMonster.tSphere.r = 25.f;

			switch (rand() % 8) {
			case 0:	tMonster.tSphere.y = 125.f;	break;
			case 1:	tMonster.tSphere.y = 175.f;	break;
			case 2:	tMonster.tSphere.y = 225.f;	break;
			case 3:	tMonster.tSphere.y = 275.f;	break;
			case 4:	tMonster.tSphere.y = 325.f;	break;
			case 5:	tMonster.tSphere.y = 375.f;	break;
			case 6:	tMonster.tSphere.y = 425.f;	break;
			case 7:	tMonster.tSphere.y = 475.f;	break;
			case 8:	tMonster.tSphere.y = 525.f;	break;
			}

			// 몬스터 겹침 체크
			iterEnd = g_MonsterList.end();
			for (iter = g_MonsterList.begin(); iter != iterEnd; iter++) {

				if ((*iter).tSphere.x - (*iter).tSphere.r <= tMonster.tSphere.x + tMonster.tSphere.r && 
					(*iter).tSphere.x + (*iter).tSphere.r >= tMonster.tSphere.x - tMonster.tSphere.r &&
					(*iter).tSphere.y - (*iter).tSphere.r <= tMonster.tSphere.y + tMonster.tSphere.r &&
					(*iter).tSphere.y + (*iter).tSphere.r >= tMonster.tSphere.y - tMonster.tSphere.r) {
					bCollision = true;
					break;
				}
			}

			if (bCollision == true) {
				continue;
			}

			tMonster.fSpeed = (rand() % 200) + 300.f;
			tMonster.iDir = MD_RIGHT;

			g_MonsterList.push_back(tMonster);
		}
	}

	iterEnd = g_MonsterList.end();
	for (iter = g_MonsterList.begin(); iter != iterEnd;) {
		(*iter).tSphere.x += (*iter).fSpeed * g_fDeltaTime * fTimeScale;

		if ((*iter).tSphere.x - (*iter).tSphere.r >= rcWindow.right) {
			iter = g_MonsterList.erase(iter);
			iterEnd = g_MonsterList.end();
		}
		else if ((*iter).tSphere.x - (*iter).tSphere.r <= g_tPlayerRC.right &&
			(*iter).tSphere.x + (*iter).tSphere.r >= g_tPlayerRC.left &&
			(*iter).tSphere.y - (*iter).tSphere.r <= g_tPlayerRC.bottom &&
			(*iter).tSphere.y + (*iter).tSphere.r >= g_tPlayerRC.top) {
			iDead++;
			g_tPlayerRC.left = 375;
			g_tPlayerRC.top = 520;
			g_tPlayerRC.right = 425;
			g_tPlayerRC.bottom = 570;
		}
		else {
			++iter;
		}
	}

	static int iLife = 10;
	TCHAR strLife[64] = {};
	wsprintf(strLife, TEXT("LIFE = %d / %d"), iDead, iLife);
	TextOut(g_hDC, 600, 60, strLife, lstrlen(strLife));

	// 이동 표시
	Rectangle(g_hDC, g_tPlayerRC.left, g_tPlayerRC.top, g_tPlayerRC.right, g_tPlayerRC.bottom);

	iterEnd = g_MonsterList.end();
	for (iter = g_MonsterList.begin(); iter != iterEnd; ++iter) {
		Rectangle(g_hDC, (*iter).tSphere.x - (*iter).tSphere.r, (*iter).tSphere.y - (*iter).tSphere.r,
			(*iter).tSphere.x + (*iter).tSphere.r, (*iter).tSphere.y + (*iter).tSphere.r);
	}
}

void Run()
{
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

	if (g_bRotate == false) {
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

		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
			// 마우스 위치를 얻어온다.
			POINT ptMouse;

			// 아래 함수는 마우스 위치를 얻어온다.
			// 마우스 위치는 스크린 좌표 기준으로 받아온다.
			GetCursorPos(&ptMouse);

			// 스크린 좌표를 클라이언트 좌표로 변환한다.
			ScreenToClient(g_hWnd, &ptMouse);

			// 플레이어와 충돌처리한다.
			if ((g_tPlayerRC.left <= ptMouse.x && g_tPlayerRC.right >= ptMouse.x) &&
				(g_tPlayerRC.top <= ptMouse.y && g_tPlayerRC.bottom >= ptMouse.y)) {
				MessageBox(NULL, L"플레이어 클릭", L"마우스 클릭", MB_OK);
			}

			float fX = g_tMonster.tSphere.x - ptMouse.x;
			float fY = g_tMonster.tSphere.y - ptMouse.y;
			float fDist = sqrtf(fX * fX + fY * fY);
			if (fDist <= g_tMonster.tSphere.r) {
				MessageBox(NULL, L"몬스터 클릭", L"마우스 클릭", MB_OK);
			}
		}
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
			BULLET tBullet = {};

			//tBullet.tRC.left = g_tPlayerRC.right;
			//tBullet.tRC.right = tBullet.tRC.left + 50.f;
			//tBullet.tRC.top = (g_tPlayerRC.top + g_tPlayerRC.bottom) / 2.f - 25.f;
			//tBullet.tRC.bottom = tBullet.tRC.top + 50.f;
			tBullet.tSphere.x = g_tPlayerRC.right + 25.f;
			tBullet.tSphere.y = (g_tPlayerRC.top + g_tPlayerRC.bottom) / 2.f;
			tBullet.tSphere.r = 25.f;
			tBullet.fDist = 0.f;
			tBullet.fLimitDist = 800.f;
			g_PlayerBulletList.push_back(tBullet);
		}
	}
	else {
		if (GetAsyncKeyState('W') & 0x8000) {
			g_tPlayer.x += fSpeed * cosf(g_fPlayerAngle);
			g_tPlayer.y += fSpeed * sinf(g_fPlayerAngle);
		}
		if (GetAsyncKeyState('S') & 0x8000) {
			g_tPlayer.x -= fSpeed * cosf(g_fPlayerAngle);
			g_tPlayer.y -= fSpeed * sinf(g_fPlayerAngle);
		}
		if (GetAsyncKeyState('A') & 0x8000) {
			g_fPlayerAngle -= PI * g_fDeltaTime * fTimeScale;
		}
		if (GetAsyncKeyState('D') & 0x8000) {
			g_fPlayerAngle += PI * g_fDeltaTime * fTimeScale;
		}

		// 총구 위치를 구한다.
		g_tGunPos.x = g_tPlayer.x + cosf(g_fPlayerAngle) * g_fGunLength;
		g_tGunPos.y = g_tPlayer.y + sinf(g_fPlayerAngle) * g_fGunLength;

		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
			BULLET tBullet = {};
			tBullet.tSphere.x = g_tGunPos.x + cosf(g_fPlayerAngle) * 25.f;
			tBullet.tSphere.y = g_tGunPos.y + sinf(g_fPlayerAngle) * 25.f;
			tBullet.tSphere.r = 25.f;
			tBullet.fDist = 0.f;
			tBullet.fLimitDist = 800.f;
			tBullet.fAngle = g_fPlayerAngle;
			g_PlayerBulletList.push_back(tBullet);
		}

		if (GetAsyncKeyState('1') & 0x8000) {
			/*
				 30
				--- * PI = 0.2617993333...
				360


				PI / 12.f = 0.2617993333...
			 */
			float fAngle = g_fPlayerAngle - PI / 12.f;	// 플레이어 각도 - 30도(30/360)
			// 6.f : 60도
			// 12.f : 30도
			// 24.f : 15도

			for (int i = 0; i < 3; i++) {
				BULLET tBullet = {};
				tBullet.tSphere.x = g_tGunPos.x + cosf(fAngle) * 25.f;
				tBullet.tSphere.y = g_tGunPos.y + sinf(fAngle) * 25.f;
				tBullet.tSphere.r = 25.f;
				tBullet.fDist = 0.f;
				tBullet.fLimitDist = 800.f;
				tBullet.fAngle = fAngle;
				g_PlayerBulletList.push_back(tBullet);

				fAngle += PI / 12.f;
			}
		}
		if (GetAsyncKeyState('2') & 0x8000) {

			float fAngle = 0.f;

			for (int i = 0; i < 36; i++) {
				BULLET tBullet = {};
				tBullet.tSphere.x = g_tPlayer.x + cosf(fAngle) * 25.f;
				tBullet.tSphere.y = g_tPlayer.y + sinf(fAngle) * 25.f;
				tBullet.tSphere.r = 25.f;
				tBullet.fDist = 0.f;
				tBullet.fLimitDist = 800.f;
				tBullet.fAngle = fAngle;
				g_PlayerBulletList.push_back(tBullet);

				fAngle += PI / 18.f;
			}
		}
	}

	RECT rcWindow;
	//GetWindowRect(g_hWnd, &rcWindow);
	GetClientRect(g_hWnd, &rcWindow);
	SetRect(&rcWindow, 0, 0, 800, 600);

	if (g_bRotate == false) {
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
	}
	else {
		if (g_tPlayer.y - g_tPlayer.r <= 0) {
			g_tPlayer.y = g_tPlayer.r;
		}
		else if (g_tPlayer.y + g_tPlayer.r >= rcWindow.bottom) {
			g_tPlayer.y = rcWindow.bottom - g_tPlayer.r;
		}
		if (g_tPlayer.x - g_tPlayer.r <= 0) {
			g_tPlayer.x = g_tPlayer.r;
		}
		else if (g_tPlayer.x + g_tPlayer.r >= rcWindow.right) {
			g_tPlayer.x = rcWindow.right - g_tPlayer.r;
		}
	}

	// 몬스터 이동
	//g_tMonster.tRC.top += g_tMonster.fSpeed * g_fDeltaTime * fTimeScale * g_tMonster.iDir;
	//g_tMonster.tRC.bottom += g_tMonster.fSpeed * g_fDeltaTime * fTimeScale * g_tMonster.iDir;
	g_tMonster.tSphere.y += g_tMonster.fSpeed * g_fDeltaTime * fTimeScale * g_tMonster.iDir;

	//if (g_tMonster.tRC.bottom >= 600) {
	//	g_tMonster.iDir = MD_BACK;
	//	g_tMonster.tRC.top = rcWindow.bottom - 100;
	//	g_tMonster.tRC.bottom = rcWindow.bottom;
	//}
	//else if (g_tMonster.tRC.top <= 0) {
	//	g_tMonster.iDir = MD_FRONT;
	//	g_tMonster.tRC.top = 0;
	//	g_tMonster.tRC.bottom = 100;
	//}
	if ((g_tMonster.tSphere.y + g_tMonster.tSphere.r) >= 600) {
		g_tMonster.iDir = MD_BACK;
		g_tMonster.tSphere.y = 600 - g_tMonster.tSphere.r;
	}
	else if ((g_tMonster.tSphere.y - g_tMonster.tSphere.r) <= 0) {
		g_tMonster.iDir = MD_FRONT;
		g_tMonster.tSphere.y = g_tMonster.tSphere.r;
	}

	// 몬스터 총알 발사 로직
	g_tMonster.fTime += g_fDeltaTime;

	static float fEnemyX = 0.f;
	static float fEnemyY = 0.f;

	if (g_bRotate == false) {
		if (g_tMonster.fTime >= g_tMonster.fLimitTime) {
			g_tMonster.fTime -= g_tMonster.fLimitTime;
			BULLET tBullet = {};

			//tBullet.tRC.left = g_tMonster.tRC.left - 50.f;
			//tBullet.tRC.right = g_tMonster.tRC.left;
			//tBullet.tRC.top = (g_tMonster.tRC.top + g_tMonster.tRC.bottom) / 2.f - 25.f;
			//tBullet.tRC.bottom = tBullet.tRC.top + 50.f;
			tBullet.tSphere.r = 25.f;
			tBullet.tSphere.x = g_tMonster.tSphere.x - g_tMonster.tSphere.r - tBullet.tSphere.r;
			tBullet.tSphere.y = g_tMonster.tSphere.y;
			tBullet.fDist = 0.f;
			tBullet.fLimitDist = 850.f;
			g_MonsterBulletList.push_back(tBullet);

			g_fEnemyAngle = 0.f;
		}
	}
	else {
		float fX = g_tPlayer.x - g_tMonster.tSphere.x;
		float fY = g_tPlayer.y - g_tMonster.tSphere.y;
		float fDist = sqrtf(fX * fX + fY * fY);

		float fAngle = atan2f(fY, fX); // acos(fX / fDist);

		g_fEnemyAngle = fAngle;

		// 총구 위치를 구한다.
		g_tGunEnemyPos.x = g_tMonster.tSphere.x + cosf(g_fEnemyAngle) * g_fGunEnemyLength;
		g_tGunEnemyPos.y = g_tMonster.tSphere.y + sinf(g_fEnemyAngle) * g_fGunEnemyLength;

		if (g_tMonster.fTime >= g_tMonster.fLimitTime) {
			g_tMonster.fTime -= g_tMonster.fLimitTime;

			fEnemyX = fX;
			fEnemyY = fY;
			//TCHAR test[64];
			//_stprintf_s(test, 64, TEXT("%f"), fEnemyAngle);
			//MessageBox(NULL, test, TEXT("test"), MB_OK);
			// acos(x/d), asin(y/d), atan(y/x)

			// 180도 까지만 가능함
			// y축에 따른 예외 처리로 360도에 대한 각을 구하게 만들기

			BULLET tBullet = {};
			tBullet.tSphere.r = 25.f;
			tBullet.tSphere.x = g_tGunEnemyPos.x + cosf(fAngle) * 25.f;
			tBullet.tSphere.y = g_tGunEnemyPos.y + sinf(fAngle) * 25.f;
			tBullet.fDist = 0.f;
			tBullet.fLimitDist = 850.f;
			tBullet.fAngle = fAngle;
			g_MonsterBulletList.push_back(tBullet);
		}
	}

	list<BULLET>::iterator iter;
	list<BULLET>::iterator iterEnd = g_PlayerBulletList.end();

	fSpeed = 600.f * g_fDeltaTime * fTimeScale;

	float fX, fY, fDist;

	if (g_bRotate == false) {
		// 플레이어 총알 이동
		for (iter = g_PlayerBulletList.begin(); iter != iterEnd; ++iter) {
			//(*iter).tRC.left += fSpeed;
			//(*iter).tRC.right += fSpeed;
			(*iter).tSphere.x += fSpeed;

			(*iter).fDist += fSpeed;
		}

		// 몬스터 총알 이동
		iterEnd = g_MonsterBulletList.end();
		for (iter = g_MonsterBulletList.begin(); iter != iterEnd; ++iter) {
			//(*iter).tRC.left -= fSpeed;
			//(*iter).tRC.right -= fSpeed;
			(*iter).tSphere.x -= fSpeed;

			(*iter).fDist += fSpeed;
		}
	}
	else {
		// 플레이어 총알 이동
		for (iter = g_PlayerBulletList.begin(); iter != iterEnd; ++iter) {
			(*iter).tSphere.x += fSpeed * cosf((*iter).fAngle);
			(*iter).tSphere.y += fSpeed * sinf((*iter).fAngle);
			(*iter).fDist += fSpeed;
		}

		// 몬스터 총알 이동
		iterEnd = g_MonsterBulletList.end();
		for (iter = g_MonsterBulletList.begin(); iter != iterEnd; ++iter) {
			(*iter).tSphere.x += fSpeed * cosf((*iter).fAngle);
			(*iter).tSphere.y += fSpeed * sinf((*iter).fAngle);
			(*iter).fDist += fSpeed;
		}
	}

	// 출력
	TCHAR delta[64] = {};
	_stprintf_s(delta, 64, TEXT("Angle = %.2f, %.2f"), g_fEnemyAngle, g_fPlayerAngle);
	TextOut(g_hDC, 600, 60, delta, lstrlen(delta));
	_stprintf_s(delta, 64, TEXT("EnemyXY = (%.2f, %.2f)"), fEnemyX, fEnemyY);
	TextOut(g_hDC, 600, 80, delta, lstrlen(delta));

	//Rectangle(g_hDC, 0, 0, 850, 600);
	if (g_bRotate == false) {
		Rectangle(g_hDC, g_tPlayerRC.left, g_tPlayerRC.top, g_tPlayerRC.right, g_tPlayerRC.bottom);
	}
	else {
		Ellipse(g_hDC, g_tPlayer.x - g_tPlayer.r, g_tPlayer.y - g_tPlayer.r,
			g_tPlayer.x + g_tPlayer.r, g_tPlayer.y + g_tPlayer.r);

		// 캐릭터 총구
		MoveToEx(g_hDC, g_tPlayer.x, g_tPlayer.y, NULL);
		LineTo(g_hDC, g_tGunPos.x, g_tGunPos.y);

		// 몬스터 총구
		MoveToEx(g_hDC, g_tMonster.tSphere.x, g_tMonster.tSphere.y, NULL);
		LineTo(g_hDC, g_tGunEnemyPos.x, g_tGunEnemyPos.y);
	}
	Ellipse(g_hDC, g_tMonster.tSphere.x - g_tMonster.tSphere.r, g_tMonster.tSphere.y - g_tMonster.tSphere.r,
		g_tMonster.tSphere.x + g_tMonster.tSphere.r, g_tMonster.tSphere.y + g_tMonster.tSphere.r);

	iterEnd = g_PlayerBulletList.end();
	for (iter = g_PlayerBulletList.begin(); iter != iterEnd;) {
		Ellipse(g_hDC, (*iter).tSphere.x - (*iter).tSphere.r, (*iter).tSphere.y - (*iter).tSphere.r,
			(*iter).tSphere.x + (*iter).tSphere.r, (*iter).tSphere.y + (*iter).tSphere.r);

		fX = (*iter).tSphere.x - g_tMonster.tSphere.x;
		fY = (*iter).tSphere.y - g_tMonster.tSphere.y;
		fDist = sqrtf(fX * fX + fY * fY);

		// 플레이어 총알 대 몬스터 충돌처리를 한다.
		if (fDist <= ((*iter).tSphere.r + g_tMonster.tSphere.r)) {
			iter = g_PlayerBulletList.erase(iter);
			iterEnd = g_PlayerBulletList.end();
		}
		else if (((*iter).fDist >= (*iter).fLimitDist) ||
			(((*iter).tSphere.x - (*iter).tSphere.r) >= 800.f)) {
			iter = g_PlayerBulletList.erase(iter);
			iterEnd = g_PlayerBulletList.end();
		}
		/*else if (g_tMonster.tRC.left <= (*iter).tRC.right && (*iter).tRC.left <= g_tMonster.tRC.right
		&& g_tMonster.tRC.top <= (*iter).tRC.bottom && (*iter).tRC.top <= g_tMonster.tRC.bottom) {
		iter = g_PlayerBulletList.erase(iter);
		iterEnd = g_PlayerBulletList.end();
		}*/
		else {
			++iter;
		}
	}

	iterEnd = g_MonsterBulletList.end();
	if (g_bRotate == false) {
		for (iter = g_MonsterBulletList.begin(); iter != iterEnd;) {
			Ellipse(g_hDC, (*iter).tSphere.x - (*iter).tSphere.r, (*iter).tSphere.y - (*iter).tSphere.r,
				(*iter).tSphere.x + (*iter).tSphere.r, (*iter).tSphere.y + (*iter).tSphere.r);

			// 몬스터 총알 대 플레이어 충돌처리를 한다.
			if (RectangleCircleCollision(g_tPlayerRC, (*iter)) == true) {
				iter = g_MonsterBulletList.erase(iter);
				iterEnd = g_MonsterBulletList.end();
			}
			else if (((*iter).fDist >= (*iter).fLimitDist) ||
				(((*iter).tSphere.x + (*iter).tSphere.r) <= 0.f)) {
				iter = g_MonsterBulletList.erase(iter);
				iterEnd = g_MonsterBulletList.end();
			}
			/*else if (g_tPlayerRC.left <= (*iter).tRC.right && (*iter).tRC.left <= g_tPlayerRC.right
			&& g_tPlayerRC.top <= (*iter).tRC.bottom && (*iter).tRC.top <= g_tPlayerRC.bottom) {
			iter = g_MonsterBulletList.erase(iter);
			iterEnd = g_MonsterBulletList.end();
			}*/
			else {
				++iter;
			}
		}
	}
	else {
		for (iter = g_MonsterBulletList.begin(); iter != iterEnd;) {
			Ellipse(g_hDC, (*iter).tSphere.x - (*iter).tSphere.r, (*iter).tSphere.y - (*iter).tSphere.r,
				(*iter).tSphere.x + (*iter).tSphere.r, (*iter).tSphere.y + (*iter).tSphere.r);

			fX = g_tPlayer.x - (*iter).tSphere.x;
			fY = g_tPlayer.y - (*iter).tSphere.y;
			fDist = sqrtf(fX * fX + fY * fY);
			// 몬스터 총알 대 플레이어 충돌처리를 한다.
			if (fDist < (*iter).tSphere.r + g_tPlayer.r) {
				iter = g_MonsterBulletList.erase(iter);
				iterEnd = g_MonsterBulletList.end();
			}
			else if (((*iter).fDist >= (*iter).fLimitDist) ||
				(((*iter).tSphere.x + (*iter).tSphere.r) <= 0.f)) {
				iter = g_MonsterBulletList.erase(iter);
				iterEnd = g_MonsterBulletList.end();
			}
			/*else if (g_tPlayerRC.left <= (*iter).tRC.right && (*iter).tRC.left <= g_tPlayerRC.right
			&& g_tPlayerRC.top <= (*iter).tRC.bottom && (*iter).tRC.top <= g_tPlayerRC.bottom) {
			iter = g_MonsterBulletList.erase(iter);
			iterEnd = g_MonsterBulletList.end();
			}*/
			else {
				++iter;
			}
		}
	}
}