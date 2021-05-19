// Laba_3_lunohod.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Laba_3_lunohod.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Resource.h"
#include <mmsystem.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

// Глобальные переменные:
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;

Image* backimage;
Image* sheathingImg;
Image* metalImg;

HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

int step = 0;
float distanceX = 0.f;
float distanceY = 0.f;
float angle = 0.f;
float t = 0.f;
UINT frameIndex;
UINT frameCount;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void Display(HDC);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LABA3LUNOHOD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABA3LUNOHOD));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABA3LUNOHOD));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LABA3LUNOHOD);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1300, 900, nullptr, nullptr, hInstance, nullptr);

   sheathingImg = Image::FromFile(L"sheathing.jpg");
   metalImg = Image::FromFile(L"metal.jpg");

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		backimage = Image::FromFile(L"background.gif");
		if (backimage == NULL) return -1;

		frameIndex = 0;
		frameCount = backimage->GetFrameCount(&FrameDimensionTime);

		SetTimer(hWnd, 1, 50, NULL);
		return 0;
	}
	case WM_TIMER:
	{
		frameIndex = (frameIndex + 1) % frameCount;
		backimage->SelectActiveFrame(&FrameDimensionTime, frameIndex);
		step++;
		t += 0.05f;
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
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
	case WM_ERASEBKGND: return 1;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			Display(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		if (sheathingImg != NULL)
		{
			delete sheathingImg;
		}
		if (metalImg != NULL)
		{
			delete metalImg;
		}
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
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

PointF Tween(const PointF& A, const PointF& B, float t)
{
	return PointF(A.X * (1.f - t) + B.X * t, A.Y * (1.f - t) + B.Y * t);
}

void Tween(const PointF* A, const PointF* B, PointF* P, int count, float t)
{
	for (int i = 0; i < count; i++)
	{
		P[i] = Tween(A[i], B[i], t);
	}
}

void DrawComet(Graphics& g)
{
	Rect rect;
	g.GetVisibleClipBounds(&rect);
	Bitmap backbuffer(rect.Width, rect.Height, &g);

	Graphics temp(&backbuffer);
	temp.SetSmoothingMode(SmoothingModeHighQuality);
	Pen blackpen(Color::Black, 2.f);
	SolidBrush body_brush(Color::DarkGray);

	float startxPos = 400;
	float startyPos = -150;

	PointF start_comet_Points[] = {
		PointF(700.f + startxPos, 100.f + startyPos),
		PointF(720.f + startxPos, 50.f + startyPos),
		PointF(750.f + startxPos, 20.f + startyPos),
		PointF(780.f + startxPos, 30.f + startyPos),
		PointF(800.f + startxPos, 10.f + startyPos),
		PointF(840.f + startxPos, 40.f + startyPos),
		PointF(800.f + startxPos, 90.f + startyPos),
		PointF(750.f + startxPos, 85.f + startyPos),
		PointF(720.f + startxPos, 120.f + startyPos),
	};

	float endxPos = -800;
	float endyPos = 50;

	PointF end_comet_Points[] = {
		PointF(700.f + endxPos, 100.f + endyPos),
		PointF(720.f + endxPos, 50.f + endyPos),
		PointF(750.f + endxPos, 20.f + endyPos),
		PointF(780.f + endxPos, 30.f + endyPos),
		PointF(800.f + endxPos, 10.f + endyPos),
		PointF(840.f + endxPos, 40.f + endyPos),
		PointF(800.f + endxPos, 90.f + endyPos),
		PointF(750.f + endxPos, 85.f + endyPos),
		PointF(720.f + endxPos, 120.f + endyPos),
	};

	PointF int_comet_Points[9];

	Tween(start_comet_Points, end_comet_Points, int_comet_Points, 9, t);

	g.FillPolygon(&body_brush, int_comet_Points, 9);
}

void DrawMoonrover(Graphics& g)
{
	Rect rect;
	g.GetVisibleClipBounds(&rect);

	Bitmap backBuffer(rect.Width, rect.Height, &g);

	Graphics temp(&backBuffer);

	temp.SetSmoothingMode(SmoothingModeHighQuality);

	//текстурная кисть картинкой обшивкой корабля
	WrapMode wrapMode = WrapModeTile;

	TextureBrush tb(sheathingImg, wrapMode);

	TextureBrush tbm(metalImg, wrapMode);

	SolidBrush sbBlack(Color::Black);
	SolidBrush sbGray(Color::Gray);
	SolidBrush sbBlue(Color::Blue);
	Pen blackPen(Color::Black);

	LinearGradientBrush lgb(Rect(200, 200, 300, 150), Color::White, Color::Black, 45.f);
	Color colors[6] =
	{
		Color(255, 128, 0, 0),
		Color(255, 255, 0, 0),
		Color(255, 240, 128, 128)
	};

	//ширина областей градиента
	float pos[3] = {
	0.0f,
	0.3f,
	1.0f };

	//комбинированный карандаш 
	Pen CompPen(Color::Black, 6.f);
	float comp[6] = {
		0.0f, 0.2f,
		0.4f, 0.6f,
		0.8f, 1.0f
	};

	//настройка карандаша
	CompPen.SetCompoundArray(comp, 6);

	//стр 77 
	const int i = 12;
	Point moonRover[i] =
	{
		Point(100, 380),
		Point(70, 380),
		Point(90, 270),
		Point(210, 240),
		Point(340, 250),
		Point(380, 310),
		Point(380, 380),
		Point(350, 380),
		Point(310, 380),
		Point(200, 380),
		Point(120, 380),
		Point(100, 380)
	};

	//прорисока кривой
	g.DrawCurve(&CompPen, moonRover, i);
	//заливка содержимого
	g.FillClosedCurve(&tbm, moonRover, i);

	const int ii = 11;
	Point Star[ii] =
	{
		Point(315,330),
		Point(285,345),
		Point(295,310),
		Point(280,280),
		Point(305,300),
		Point(315,260),
		Point(325,300),
		Point(350,280),
		Point(335,310),
		Point(345,345),
		Point(315,330),

	};

	//звезда
	g.DrawLines(&blackPen, Star, ii);

	//заливка звезды
	lgb.SetInterpolationColors(colors, pos, 3);
	g.FillPolygon(&lgb, Star, ii);

	const int iii = 7;
	Point kontur[iii] =
	{
		Point(180,250),
		Point(170,315),
		Point(80,320),
		Point(80,330),
		Point(185,325),
		Point(190,245),
		Point(180,250),
	};

	//прорисока кривой
	g.DrawCurve(&blackPen, kontur, iii);
	//заливка содержимого
	g.FillClosedCurve(&tb, kontur, iii);

	const int iiii = 9;
	Point antena[iiii] =
	{
		Point(190,240),
		Point(190,220),
		Point(175,200),
		Point(200,220),
		Point(240,220),
		Point(265,200),
		Point(250,220),
		Point(250,235),
		Point(190,240),
	};


	//прорисока кривой
	g.DrawLines(&blackPen, antena, iiii);
	//заливка содержимого
	g.FillPolygon(&sbGray, antena, iiii);

	const int iiiii = 8;
	Point tarelka[iiiii] =
	{
		Point(360,260),
		Point(380,230),
		Point(400,240),
		Point(390,220),
		Point(380,200),
		Point(378,225),
		Point(350,253),
		Point(360,260),
	};

	//прорисока кривой
	g.DrawLines(&blackPen, tarelka, iiiii);
	//заливка содержимого
	g.FillPolygon(&sbBlack, tarelka, iiiii);

	Rect HWheelForw = Rect(90, 355, 60, 60);
	Rect HWheelBack = Rect(305, 355, 60, 60);
	Rect LWheelForw = Rect(100, 365, 40, 40);
	Rect LWheelBack = Rect(315, 365, 40, 40);

	//прорисовка колеса 1
	g.DrawEllipse(&blackPen, HWheelForw);
	g.FillEllipse(&sbBlack, HWheelForw);
	g.DrawEllipse(&blackPen, LWheelForw);
	g.FillEllipse(&sbGray, LWheelForw);

	//прорисовка колеса 4
	g.DrawEllipse(&blackPen, HWheelBack);
	g.FillEllipse(&sbBlack, HWheelBack);
	g.DrawEllipse(&blackPen, LWheelBack);
	g.FillEllipse(&sbGray, LWheelBack);

	HWheelForw = Rect(160, 355, 60, 60);
	HWheelBack = Rect(235, 355, 60, 60);
	LWheelForw = Rect(170, 365, 40, 40);
	LWheelBack = Rect(245, 365, 40, 40);

	//прорисовка колеса 2
	g.DrawEllipse(&blackPen, HWheelForw);
	g.FillEllipse(&sbBlack, HWheelForw);
	g.DrawEllipse(&blackPen, LWheelForw);
	g.FillEllipse(&sbGray, LWheelForw);

	//прорисовка колеса 3
	g.DrawEllipse(&blackPen, HWheelBack);
	g.FillEllipse(&sbBlack, HWheelBack);
	g.DrawEllipse(&blackPen, LWheelBack);
	g.FillEllipse(&sbGray, LWheelBack);
}

void Display(HDC hdc)
{
	Graphics g(hdc);
	Rect rect;
	g.GetVisibleClipBounds(&rect);

	Bitmap backBuffer(rect.Width, rect.Height, &g);

	Graphics temp(&backBuffer);
	temp.DrawImage(backimage, 0, 0, rect.Width, rect.Height);

	distanceX = 5.f * (step + 10);
	distanceY = 5.f * (step - 50);
	angle = -90.f * step / (frameCount + 400);

	temp.TranslateTransform(0.f, 150.f);

	if (t < 1)
	{
		DrawComet(temp);
	}
	else
	{
		t = 0;
	}

	if (step < 50)
	{
		temp.TranslateTransform(0.f + distanceX, 0.f);
		DrawMoonrover(temp);
	}
	else if (step < 70)
	{
		//distanceY = 5.f * (step - 50);
		temp.TranslateTransform(0.f + distanceX, 0.f-distanceY);
		temp.RotateTransform(angle + 10);
		DrawMoonrover(temp);
	}
	else if (step < 150)
	{
		//distanceY = 5.f * (step - 50);
		temp.TranslateTransform(0.f + distanceX+20, 0.f - 120);
		DrawMoonrover(temp);
	}
	else
	{
		step = -1;
		temp.ResetTransform();
	}
	g.DrawImage(&backBuffer, rect);
}