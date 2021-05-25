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

//Имя класса
TCHAR WinName[] = _T("Window2");

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
		_T("SecondWindow"),
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

BOOL messageIsRecieved = false;
BOOL messageShutDownIsRecieved = false;

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

	//Дескриптор первого окна
	static HWND hWnd_1;

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
		//Через вызов функции InvalidateRect() мы даём системе понять, что прямоугольник, 
		//отрисовывающий окно, является "испорченным", т.е. недействительным, и что его надо перерисовать.
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_LBUTTONUP:
		bLButtonIsPressed = false;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_USER + 1:
	{
		hWnd_1 = (HWND)wParam;
		messageIsRecieved = true;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	//Если получено WM_USER + 2, то закрываем это окно
	case WM_USER + 2:
	{
		hWnd_1 = (HWND)wParam;
		messageShutDownIsRecieved = true;
		SendMessage(hWnd_1, WM_USER + 1, (WPARAM)hWnd, NULL);
		//Посылаем сообщение о закрытии окна.
		SendMessage(hWnd, WM_DESTROY, NULL, NULL);
		break;
	}

		// При перерисовке
	case WM_PAINT:
		//Получаем дескриптор контекста устройства.
		hDC = BeginPaint(hWnd, &ps);
		//Получаем прямоугольник - рабочую область окна.
		GetClientRect(hWnd, &rect);

		//Заполняем прямоугольник
		FillRect(hDC, &rect, CreateSolidBrush(RGB(200, 255, 200)));

		//Фон текста при отображении
		SetBkMode(hDC, TRANSPARENT);
		//Цвет текста
		SetTextColor(hDC, RGB(200, 10, 120));

		if (messageShutDownIsRecieved)
		{
			MessageBox(hWnd, L"WM_USER + 2 получено!", L"Статус", NULL);
		}
		else
		//DrawText(hDC, _T("Сообщение получено!"), -1, &rect, DT_SINGLELINE | DT_TOP | DT_RIGHT);
		//Если нажата левая кнопка (переменную сами создали)
		if (bLButtonIsPressed)
		{
			//TextOut(hDC, rect.right-100, 0, (LPCWSTR)"123", 12);
			if (messageIsRecieved)
			{
				//MessageBox(hWnd, (LPCWSTR)hWnd_1, L"Статус", NULL);
				MessageBox(hWnd, L"Сообщение получено!", L"Статус", NULL);
			}
			else
			{
				MessageBox(hWnd, L"Сообщение не получено!", L"Статус", NULL);
			}
			messageIsRecieved = false;
		}

		EndPaint(hWnd, &ps);
		break;

	//	При закрытии окна
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	// Обработка сообщения по умолчанию
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
