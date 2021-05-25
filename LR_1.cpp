#include "framework.h"
#include <Windows.h>
#include <tchar.h>

/*
Составьте программу, в которой в главное окно голубого цвета в правом верхнем углу эхо-печать.
При нажатии левой клавиши мыши - Сообщение о нажатии.
При нажатии правой клавиши мыши - изменить цвет фона.
*/

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR WinName[] = _T("MainFrame");

//Головная функция (аналог main)
int WINAPI _tWinMain
//Передаваемые параметры
(
	HINSTANCE This,		 // Дескриптор текущего приложения 
	HINSTANCE Prev, 	// В современных системах всегда 0 
	LPTSTR cmd, 		// Командная строка 
	int mode	// Режим отображения окна
)
//Сама функция
{
	/*
	* ЗДЕСЬ ВСЁ УНИВЕРСАЛЬНО ДЛЯ ЛЮБОЙ ПРОГРАММЫ (Ну, наверное, бывают исключения)
	*/
	HWND hWnd;		// Дескриптор главного окна программы 
	MSG msg; 		// Структура для хранения сообщения
	// Класс окна
	WNDCLASS wc;
	// Задаём параметры (или не параметры, а другой термин, но суть та же) класса окна
	wc.hInstance = This;
	wc.lpszClassName = WinName; 				// Имя класса окна 
	wc.lpfnWndProc = WndProc; 					// Функция окна (то есть отрисовки окна, самая важная часть)
	wc.style = CS_HREDRAW | CS_VREDRAW; 			// Стиль окна 
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); 		// Стандартная иконка 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); 		// Стандартный курсор 
	wc.lpszMenuName = NULL; 					// Нет меню 
	wc.cbClsExtra = 0; 						// Нет дополнительных данных класса 
	wc.cbWndExtra = 0; 						// Нет дополнительных данных окна 
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); 	// Заполнение окна белым цветом (Так же есть явное преобразование к HBRUSH)


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
	); 					// Дополнительной информации нет 

	ShowWindow(hWnd, mode);			// Показать окно

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
int r = 0, g = 0, b = 0;

//ДЛЯ СЕБЯ
BOOL bSpaceIsPressed = false;
int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
//Счётчик количества нажатий на пробел
int numberOfTimesSpaceWasPressed = 0;

// Оконная функция вызывается операционной системой
// и получает сообщения из очереди для данного приложения
// WndProc - функция обратного вызова, то есть её вызывает сама система.
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
		r = 171;
		g = 255;
		b = 251;
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
		//InvalidateRect, видимо, генерирует сообщение WM_PAINT
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_LBUTTONUP:
		bLButtonIsPressed = false;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_RBUTTONDOWN:
		bRButtonIsPressed = true;
		r = rand() % 255;
		g = rand() % 255;
		b = rand() % 255;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_RBUTTONUP:
		bRButtonIsPressed = false;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	//================================
	//ДЛЯ СЕБЯ!!!
	//Если пробел, то рисуем эллипс
	case WM_KEYDOWN:
		if ((TCHAR)wParam == ' ')
		{
			if (numberOfTimesSpaceWasPressed == 0)
			{
				x1 = rand() % sx;
				y1 = rand() % sy;
				numberOfTimesSpaceWasPressed = 1;
			}
			bSpaceIsPressed = true;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_KEYUP:
		if ((TCHAR)wParam == ' ')
		{
			numberOfTimesSpaceWasPressed = 0;
			bSpaceIsPressed = false;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	//================================

	// При перерисовке
	case WM_PAINT:
		//Получаем дескриптор контекста устройства.
		hDC = BeginPaint(hWnd, &ps);
		//Получаем прямоугольник - рабочую область окна.
		GetClientRect(hWnd, &rect);

		//Заполняем прямоугольник
		FillRect(hDC, &rect, CreateSolidBrush(RGB(r, g, b)));

		//Фон текста при отображении
		SetBkMode(hDC, TRANSPARENT);
		//Цвет текста
		SetTextColor(hDC, RGB(200, 10, 120));

		//====================================================
		//ДЛЯ СЕБЯ:
		if (bSpaceIsPressed)
		{
			//Инициализируем кисть
			hBrush = CreateSolidBrush(RGB(255-r, 255-g, 255-b));
			//Указываем, что рисовать будем кистью.
			SelectObject(hDC, hBrush);
			Ellipse(hDC, x1, y1, 40, 40);
			//TextOut(hDC, rect.right-100, 0, (LPCWSTR)"123", 12);
			DrawText(hDC, _T("ПРОБЕЛ НАЖАТ!"), -1, &rect, DT_SINGLELINE | DT_TOP | DT_LEFT);
		}
		//====================================================

		//Если нажата левая кнопка (переменную сами создали)
		if (bLButtonIsPressed)
		{
			//TextOut(hDC, rect.right-100, 0, (LPCWSTR)"123", 12);
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
		break; 	// Завершение программы 

	// Обработка сообщения по умолчанию
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

