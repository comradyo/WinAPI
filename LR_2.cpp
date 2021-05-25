#include "framework.h"
#include <Windows.h>
#include <tchar.h>

/*
Сформировать два приложения, которые открывают по одному окну.

В окне 1 по щелчку левой клавиши мыши (при помощи FindWindow()) найти дескриптор окна 2. Выдать сообщение открыто или нет второе окно.
При помощи функции SendMessage() и поля WPARAM  передать свой дескриптор второму окну. Выдать сообщение об этом.

//WM_USER+n - это n-е полученное сообщение
В окне 2 при получении сообщения WM_USER+1 выдать сообщение об этом.
При получении сообщения WM_USER+2  Shut down.

*/

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR WinName[] = _T("Window1");

//Головная функция (аналог main)
int WINAPI _tWinMain
//Передаваемые параметры
(
	HINSTANCE This,
	HINSTANCE Prev,
	LPTSTR cmd,
	int mode
)
//Сама функция
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;
	// Задаём параметры класса окна
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
		WinName,			
		_T("FirstWindow"), 		
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		HWND_DESKTOP,
		NULL,
		This,
		NULL
	);

	ShowWindow(hWnd, mode);	// Показать окно

	// Цикл обработки сообщений (вся перерисовка и логика происходит уже здесь)
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg); 		// Функция трансляции кодов нажатой клавиши 
		DispatchMessage(&msg); 			// Посылает сообщение функции WndProc() и инициализирует её вызов. 
										// (В WndProc() находится вся логика обработки каких-либо событий)
	}
	return 0;
}

int k = 1;
BOOL bLButtonIsPressed = false;
BOOL bRButtonIsPressed = false;

BOOL secondWindowIsFound = false;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Через DispatchMessage мы передаём системе информацию о том, что у нас появляются новые сообщения, и сама система вызывает WndProc.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Дескриптор контекста устройства - это набор сведений о том, как работать с данным устройством, нужен для самой системы.
	HDC hDC;
	//Вся необходимая информация для перерисовки окна хранится в paintstruct (см. стр 23)
	PAINTSTRUCT ps;
	//Прямоугольник, в нём мы будем хранить рабочую область окна
	RECT rect;

	int x, y;
	//Переменные статические, чтобы изменения были видны везде. 
	//В них будем хранить размеры окна.
	//В первой лабе они пока что не нужны, по крайней мере в моей реализации.
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
	{
		bLButtonIsPressed = true;
		//Левый параметр FindWindow - это имя класса, правый - имя окна. Достаточно выбрать один из двух
		HWND hWnd_2 = FindWindow(L"Window2", NULL); 	//Дескриптор второго окна
		if (hWnd_2)
		{
			MessageBox(hWnd, L"Второе окно найдено!", L"Статус", NULL);
			if (hWnd_2)
			{
				SendMessage(hWnd_2, WM_USER + 1, (WPARAM)hWnd, NULL);
			}
			MessageBox(hWnd, L"Сообщение второму окну отправлено!", L"Статус", NULL);
			//DrawText(hDC, _T("Второе окно найдено!"), -1, &rect, DT_SINGLELINE | DT_TOP | DT_RIGHT);
			bLButtonIsPressed = false;
		}
		else
		{
			MessageBox(hWnd, L"Второе окно не найдено!", L"Статус", NULL);
			//DrawText(hDC, _T("Второе окно не найдено!"), -1, &rect, DT_SINGLELINE | DT_TOP | DT_RIGHT);
			bLButtonIsPressed = false;
		}
		//Через вызов функции InvalidateRect() мы даём системе понять, что прямоугольник, 
	//отрисовывающий окно, является "испорченным", т.е. недействительным, и что его надо перерисовать.
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	case WM_LBUTTONUP:
		bLButtonIsPressed = false;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_RBUTTONDOWN:
	{
		bRButtonIsPressed = true;

		HWND hWnd_2 = FindWindow(L"Window2", NULL); 	//Дескриптор второго окна
		if (hWnd_2)
		{
			SendMessage(hWnd_2, WM_USER + 2, (WPARAM)hWnd, NULL);
			bRButtonIsPressed = false;
		}
		else
		{
			MessageBox(hWnd, L"Второе окно не найдено!", L"Статус", NULL);
			//DrawText(hDC, _T("Второе окно не найдено!"), -1, &rect, DT_SINGLELINE | DT_TOP | DT_RIGHT);
			bRButtonIsPressed = false;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	case WM_USER + 1:
	{
		HWND hWnd_2 = (HWND)wParam;
		MessageBox(hWnd, L"Второе окно закрыто!", L"Статус", NULL);
		break;
	}

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

		//Заполняем прямоугольник
		FillRect(hDC, &rect, CreateSolidBrush(RGB(200, 200, 255)));

		EndPaint(hWnd, &ps);
		break;

		//	При закрытии окна
	case WM_DESTROY:
		PostQuitMessage(0);
		break; 	// Завершение программы 

	// Обработка сообщения по умолчанию
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}