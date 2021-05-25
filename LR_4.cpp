#include <Windows.h>
#include <tchar.h>
#include <string>

//Номера таймеров
#define TIMER_1 1 //square
#define TIMER_2 2 //text

#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1280

//Идентификаторы элементов меню
#define EDIT_1 2

/*
Первый поток выводит в левую половину окна цветными символами состояние клавиш Shift, которое можно получить с помощью функции GetAsyncKeyState().
Состояние анализируемых клавиш индицируйте, выводя на экран соответствующие надписи.
Второй поток выводит в правую половину окна две толстые цветные линии, расходящиеся из одной точки под некоторым углом к горизонтали, 
и периодически то увеличивает, то уменьшает угол между ними.
*/

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI FirstThread(LPVOID param);
DWORD WINAPI SecondThread(LPVOID param);
//Создаём мьютекс
HANDLE MutexHwnd = CreateMutex(0, 0, 0);

TCHAR WinName[] = _T("MainFrame");

struct Color 
{
	int r = 0;
	int g = 0;
	int b = 0;
};

void PrintText(HDC hdc, const std::wstring text, int x, int y) 
{
	TextOut(hdc, x, y, text.c_str(), text.size());
}

void DrawLine(int x1, int y1, int x2, int y2, int width, HWND hWnd, Color& color, HDC& hdc)
{
	HPEN hPen = CreatePen(PS_SOLID, width, RGB(color.r, color.g, color.b));
	//HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
	SelectObject(hdc, hPen);
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

int WINAPI _tWinMain(HINSTANCE This, HINSTANCE Prev, LPTSTR cmd, int mode)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;
	wc.hInstance = This;
	wc.lpszClassName = WinName;
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	// Регистрация класса окна
	if (!RegisterClass(&wc)) return 0;

	// Создание окна 
	hWnd = CreateWindow(
		WinName,			// Имя класса окна 
		_T("LR_4"), 		// Заголовок окна 
		WS_OVERLAPPEDWINDOW, 		// Стиль окна 
		CW_USEDEFAULT,				// x 
		CW_USEDEFAULT, 				// y	 Размеры окна 
		WINDOW_WIDTH, 				// width 
		WINDOW_HEIGHT, 				// Height 
		HWND_DESKTOP, 				// Дескриптор родительского окна 
		NULL, 						// Нет меню 
		This, 						// Дескриптор приложения 
		NULL
	); 					// Дополнительной информации нет 

	ShowWindow(hWnd, mode);

	// Цикл обработки сообщений 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

//Параметры нужны, чтобы при отрисовке одной половины экрана не приходилось отрисовывать вторую.
static bool shiftPushing = true;
static bool linesMooving = true;
//Угол первой линии.
static int alpha = 0;
//Угол второй линии.
static int beta = 120;

static int firstDirection = 1;
static int secondDirection = 1;

static bool leftShiftIsDown = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_CREATE:
	{
		//Третий параметр - это временной интервал в миллисекундах
		SetTimer(hWnd, TIMER_1, 1, NULL);
		SetTimer(hWnd, TIMER_2, 60, NULL);
		return 0;
	}

	//Посылается, когда пользователь выбирает элемент команды из меню, 
	//когда элемент управления отправляет сообщение уведомления родительскому окну или при преобразовании нажатия клавиши быстрого вызова.
	//case WM_COMMAND:

	case WM_SIZE:
	{
		linesMooving = true;
		shiftPushing = true;
		return 0;
	}

	case WM_KEYDOWN:
	{
		if (GetAsyncKeyState(VK_LSHIFT))
		{
			leftShiftIsDown = true;
			//InvalidateRect(hWnd, NULL, 0);
		}
		break;
	}

	case WM_KEYUP:
	{
		if (GetAsyncKeyState(VK_LSHIFT) < 0)
		{
			leftShiftIsDown = false;
		}
		break;
	}

	//Отправляется в очередь сообщений установки потока по истечении времени ожидания (то есть по истечению времени, указанному в таймере)
	case WM_TIMER:
	{
		//Таймер для линий
		if (LOWORD(wParam) == TIMER_1)
		{
			if (alpha > 90)
			{
				firstDirection = -1;
			}
			if (alpha < 0)
			{
				firstDirection = 1;
			}
			alpha += firstDirection * 1;

			if (beta > 270)
			{
				secondDirection = -1;
			}
			if (beta < 120)
			{
				secondDirection = 1;
			}
			beta += secondDirection * 2;

			linesMooving = true;
			//Прямоугольник, в котором будет рисоваться фигура.
			//Его мы будем перерисовывать (чтобы не перерисовывать всё окно)
			RECT rectangle;
			rectangle.top = 0;
			rectangle.right = WINDOW_WIDTH;
			rectangle.bottom = WINDOW_HEIGHT;
			rectangle.left = (WINDOW_WIDTH)/2;
			RECT* lpRect = &rectangle;
			InvalidateRect(hWnd, lpRect, TRUE);
		}
		//Таймер для обработки нажатия Shift
		else if (LOWORD(wParam) == TIMER_2)
		{
			shiftPushing = true;
			InvalidateRect(hWnd, NULL, 0);
		}
		return 0;
	}

	case WM_PAINT:
	{
		if (shiftPushing)
		{
			shiftPushing = false;
			//Третий параметры: A pointer to the application-defined function to be executed by the thread.
			CreateThread(NULL, 0, SecondThread, hWnd, 0, NULL);
		}
		if (linesMooving)
		{
			linesMooving = false;
			CreateThread(NULL, 0, FirstThread, hWnd, 0, NULL);
		}
		return 0;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

const float PI = 3.14;

DWORD __stdcall FirstThread(LPVOID param) 
{
	HWND hWnd = static_cast<HWND>(param);
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	WaitForSingleObject(MutexHwnd, INFINITY);

	Color color_1 = { 255, 40, 100 };
	int x1 = (WINDOW_WIDTH) / 2 + 400;
	int y1 = WINDOW_HEIGHT/2;
	int lineLength1 = 200;
	int width1 = alpha / 5;
	DrawLine(x1, y1, x1 + lineLength1 * sin(alpha * PI / 180.f), y1 + lineLength1 * cos(alpha * PI / 180.f), width1, hWnd, color_1, hdc);

	Color color_2 = { 0, 200, 115 };
	HBRUSH hbrush_2 = CreateSolidBrush(RGB(color_2.r, color_2.g, color_2.b));
	int x2 = (WINDOW_WIDTH) / 2 + 400;
	int y2 = WINDOW_HEIGHT / 2;
	int lineLength2 = 300;
	int width2 = beta / 10;
	DrawLine(x2, y2, x2 + lineLength2 * sin(beta * PI / 180.f), y2 + lineLength2 * cos(beta * PI / 180.f), width2, hWnd, color_2, hdc);

	EndPaint(hWnd, &ps);
	ReleaseMutex(MutexHwnd);
	return 0;
}

const TCHAR* str1 = _T("Левый shift нажат!");
const TCHAR* str2 = _T("Левый shift не Нажат!");

DWORD __stdcall SecondThread(LPVOID param) 
{
	HWND hWnd = static_cast<HWND>(param);
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	WaitForSingleObject(MutexHwnd, INFINITY);
	//Эхо-печать результата ввода в текстовое поле
	if (leftShiftIsDown)
	{
		leftShiftIsDown = false;
		SetTextColor(hdc, RGB(alpha, beta, 120));
		TextOut(hdc, WINDOW_WIDTH/4, WINDOW_HEIGHT / 2, str1, _tcsclen(str1));
	}
	else
	{
		SetTextColor(hdc, RGB(beta, alpha, 120));
		TextOut(hdc, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2, str2, _tcsclen(str2));
	}
	EndPaint(hWnd, &ps);
	ReleaseMutex(MutexHwnd);
	return 0;
}