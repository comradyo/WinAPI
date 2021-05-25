#include "framework.h"
#include <Windows.h>
#include <tchar.h>
#include <vector>
#include <string>
using std::vector;
using std::string;
/*
Составьте программу БД "Библиотека". 
С возможностью вывода списка книг по разделам, добавлением, изменением и удалением записей.
*/

struct Genre
{
	string name;
	Genre(string _name) : name(_name) {}
};

struct Book
{
	string name;
	Genre genre;
	Book(string _name, Genre _genre) : name(name), genre(_genre) {}
};

vector<Genre> genres = 
{
	Genre("Horror"),
	Genre("Detective"),
	Genre("Biography"),
	Genre("Comedy"),
	Genre("Poem"),
	Genre("Adventure")
};

vector<Book> books =
{
	Book("Capitan Nemo", genres[5]),
	Book("Steve Jobs", genres[2]),
	Book("Sherlock Holmes", genres[1]),
	Book("It", genres[0]),
	Book("Evgeniy Onegin", genres[4]),
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR WinName[] = _T("MainFrame");

//Головная функция (аналог main)
int WINAPI _tWinMain
(
	HINSTANCE This,
	HINSTANCE Prev,
	LPTSTR cmd,
	int mode
)
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

	// Регистрация класса окна (в системе Windows, наверное)
	if (!RegisterClass(&wc)) return 0;

	// Создание окна 
	hWnd = CreateWindow
	(
		WinName,			// Имя класса окна 
		_T("LR_1"), 		// Заголовок окна 
		WS_OVERLAPPEDWINDOW, 		// Стиль окна 
		CW_USEDEFAULT,				// x 
		CW_USEDEFAULT, 				// y	 Размеры окна 
		CW_USEDEFAULT, 				// width 
		CW_USEDEFAULT, 				// Height 
		HWND_DESKTOP, 				// Дескриптор родительского окна 
		NULL, 						// Нет меню 
		This, 						// Дескриптор приложения 
		NULL
	);

	ShowWindow(hWnd, mode);			// Показать окно

	// Цикл обработки сообщений (вся перерисовка и логика происходит уже здесь)
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

int k = 1;
BOOL bLButtonIsPressed = false;
BOOL bRButtonIsPressed = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;

	int x, y;
	//Размеры окна
	static int sx, sy;
	//Кисть, которой рисуем
	HBRUSH hBrush;

	// Обработчик сообщений
	switch (message)
	{
		// При создании окна:
	case WM_CREATE:
		break;

		// При изменении размеров окна:
	case WM_SIZE:
		//В lparam хранятся дополнительные параметры сообщения, в данном случае - размеры перерисованного окна.
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		break;

	case WM_LBUTTONDOWN:
		bLButtonIsPressed = true;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_LBUTTONUP:
		bLButtonIsPressed = false;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_RBUTTONDOWN:
		bRButtonIsPressed = true;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_RBUTTONUP:
		bRButtonIsPressed = false;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

		// При перерисовке
	case WM_PAINT:
		//Получаем дескриптор контекста устройства.
		hDC = BeginPaint(hWnd, &ps);
		//Получаем прямоугольник - рабочую область окна.
		GetClientRect(hWnd, &rect);

		FillRect(hDC, &rect, CreateSolidBrush(RGB(200, 240, 215)));

		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(200, 10, 120));

		if (bLButtonIsPressed)
		{
			DrawText(hDC, _T("Нажата левая кнопка мыши!"), -1, &rect, DT_SINGLELINE | DT_TOP | DT_RIGHT);
		}
		else
			if (bRButtonIsPressed)
			{
				DrawText(hDC, _T("Нажата правая кнопка мыши!"), -1, &rect, DT_SINGLELINE | DT_TOP | DT_RIGHT);
			}
		EndPaint(hWnd, &ps);
		break;

		//	При закрытии окна
	case WM_DESTROY:
		PostQuitMessage(0);
		break; 

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
