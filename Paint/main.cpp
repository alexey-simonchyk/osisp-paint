#include <windows.h>
#include <iostream>
#include "MenuBar.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // message handler
BOOL Line(HDC hdc, int x1, int y1, int x2, int y2); 
BOOL Redraw(HWND hWnd, BOOL isToClearArea);
void draw(HDC drawingHDC, HDC resultHDC, UINT drawItems, POINT previousMousePosition, POINT mousePosition);
void MenuCommand(HWND hWnd, WPARAM param);
void TrackMouse(HWND hwnd);
UINT CheckDrawItem();
void prepareDrawing(HWND hWnd, POINT *previousMousePostion, POINT *mousePosition, UINT drawItem);
void OpenFileWindow(HWND hWnd);
HMENU drawItemsMenu;
int deviceX, deviceY;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nCmdShow)
{
	TCHAR szClassName[] = L"My class";
	HWND hMainWindow;
	MSG msg;
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc); // size of thre struct
	wc.style = CS_HREDRAW | CS_VREDRAW; // window styles
	wc.lpfnWndProc = WndProc; // processing function
	wc.lpszMenuName = NULL; // Menu name
	wc.lpszClassName = szClassName;
	wc.cbWndExtra = NULL; // number of released bytes in the end of struct
	wc.cbClsExtra = NULL; // number of released bytes on creating application	
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); 
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // cursor image
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // background color
	wc.hInstance = hInstance;
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error", L"Class not found!", MB_OK);
		return NULL;
	}
	hMainWindow = CreateWindow(szClassName, L"My Window",
				WS_OVERLAPPEDWINDOW | WS_VSCROLL,
				CW_USEDEFAULT, NULL,
				CW_USEDEFAULT, NULL,
				NULL, NULL,
				hInstance, NULL);
	if (!hMainWindow)
	{
		MessageBox(NULL, L"Can not create class!", L"Error", MB_OK);
		return NULL;
	}
	ShowWindow(hMainWindow, nCmdShow);
	UpdateWindow(hMainWindow);
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);  
		DispatchMessage(&msg); // transfer message to the handler
	}
	return msg.wParam;
}

void CreateMainMenu(HWND hWnd)
{
	HMENU hMenu = CreateMenu();
	HMENU subMenu = CreateMenu();
	
	AppendMenu(subMenu, MF_STRING, MENU_OPEN, L"Open");
	AppendMenu(subMenu, MF_STRING, MENU_CREATE, L"Create");
	AppendMenu(subMenu, MF_STRING, MENU_SAVE, L"Save");
	AppendMenu(subMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(subMenu, MF_STRING, MENU_EXIT, L"Exit");
	AppendMenu(hMenu, MF_POPUP, (UINT)subMenu, L"File");

	HMENU ColorMenu = CreateMenu();
	AppendMenu(ColorMenu, MF_CHECKED, 0, L"Black");
	AppendMenu(ColorMenu, MF_UNCHECKED, 0, L"Red");
	AppendMenu(ColorMenu, MF_UNCHECKED, 0, L"Green");
	AppendMenu(hMenu, MF_POPUP, (UINT)ColorMenu, L"Color");

	drawItemsMenu = CreateMenu();
	AppendMenu(drawItemsMenu, MF_CHECKED, MENU_PENCIL, L"Pencil");
	AppendMenu(drawItemsMenu, MF_UNCHECKED, MENU_LINE, L"Line");
	AppendMenu(hMenu, MF_POPUP, (UINT)drawItemsMenu, L"Draw Items");
	SetMenu(hWnd, hMenu);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	static bool isDrawing = false;
	HDC hdc;
	PAINTSTRUCT paintStruct;
	static HDC drawingHDC;
	static HDC finalHDC;
	static POINT mousePosition;
	static POINT previousMousePosition;
	static UINT drawItem;
	COLORREF colorText = RGB(255, 0, 0);
	switch (uMsg)
	{
		case WM_CREATE:
		{
			HBITMAP bmFinalCopy;
			HBITMAP bmDrawingCopy;
			HBRUSH brush;

			brush = CreateSolidBrush(0xffffff);
			CreateMainMenu(hWnd);
			hdc = GetDC(hWnd);
			deviceX = GetDeviceCaps(hdc, HORZRES);
			deviceY = GetDeviceCaps(hdc, VERTRES);
			// Drawing HDC //
			drawingHDC = CreateCompatibleDC(hdc);
			bmDrawingCopy = CreateCompatibleBitmap(drawingHDC, deviceX, deviceY);
			SelectObject(drawingHDC, bmDrawingCopy);
			SelectObject(drawingHDC, brush);
			PatBlt(drawingHDC, 0, 0, deviceX, deviceY, PATCOPY);
			// Result HDC //
			finalHDC = CreateCompatibleDC(hdc);
			bmFinalCopy = CreateCompatibleBitmap(finalHDC, deviceX, deviceY);
			SelectObject(finalHDC, bmFinalCopy);
			SelectObject(finalHDC, brush);
			PatBlt(finalHDC, 0, 0, deviceX, deviceY, PATCOPY);

			ReleaseDC(hWnd, hdc);
			DeleteObject(brush);
			DeleteObject(bmDrawingCopy);
			DeleteObject(bmFinalCopy);

			break;
		}

		case WM_LBUTTONDOWN:
			isDrawing = true;
			GetCursorPos(&mousePosition);
			ScreenToClient(hWnd, &mousePosition);
			previousMousePosition = mousePosition;
			break;

		case WM_MOUSELEAVE:
			hdc = GetDC(hWnd);
			StretchBlt(finalHDC, 0, 0, deviceX, deviceY, hdc, 0, 0, deviceX, deviceY, SRCCOPY);
			isDrawing = false;
			ReleaseDC(hWnd, hdc);
			break;

		case WM_LBUTTONUP:
			hdc = GetDC(hWnd);
			StretchBlt(finalHDC, 0, 0, deviceX, deviceY, hdc, 0, 0, deviceX, deviceY, SRCCOPY);
			isDrawing = false;
			ReleaseDC(hWnd, hdc);
			break;

		case WM_MOUSEMOVE:
		{
			TrackMouse(hWnd);
			if (isDrawing)
			{
				drawItem = CheckDrawItem();
				if (drawItem != MENU_PENCIL)
				{
					StretchBlt(drawingHDC, 0, 0, deviceX, deviceY, finalHDC, 0, 0, deviceX, deviceY, SRCCOPY);
				}
				prepareDrawing(hWnd, &previousMousePosition, &mousePosition, drawItem);
			}
			break;
		}
		case WM_COMMAND:
			MenuCommand(hWnd, wParam);
			break;
		case WM_PAINT:
			if (isDrawing)
			{	
				hdc = BeginPaint(hWnd, &paintStruct);
				draw(drawingHDC, hdc, drawItem, previousMousePosition, mousePosition);
				EndPaint(hWnd, &paintStruct);
			}
			break;

		case WM_DESTROY:
			DeleteObject(drawingHDC);
			DeleteObject(finalHDC);
			PostQuitMessage(NULL); // send close message
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam); // transfer message to the standart event handler
	}
	return NULL;
}

void draw(HDC drawingHDC, HDC resultHDC, UINT drawItems, POINT previousMousePosition, POINT mousePosition)
{
	switch (drawItems)
	{
		case MENU_PENCIL:
			Line(resultHDC, previousMousePosition.x, previousMousePosition.y, mousePosition.x, mousePosition.y);
			break;
		case MENU_LINE:
			Line(drawingHDC, previousMousePosition.x, previousMousePosition.y, mousePosition.x, mousePosition.y);
			StretchBlt(resultHDC, 0, 0, deviceX, deviceY, drawingHDC, 0, 0, deviceX, deviceY, SRCCOPY);
			break;

	}
}

void prepareDrawing(HWND hWnd, POINT *previousMousePosition, POINT *mousePosition, UINT drawItem)
{
	HDC drawRegion;
	switch (drawItem)
	{
		case MENU_PENCIL:
		{
			POINT temp = *mousePosition;
			if (GetCursorPos(mousePosition))
			{
				*previousMousePosition = temp;
				ScreenToClient(hWnd, mousePosition);
				Redraw(hWnd, FALSE);
			}
			break;
		}
		case MENU_LINE:
			if (GetCursorPos(mousePosition))
			{
				ScreenToClient(hWnd, mousePosition);
				Redraw(hWnd, FALSE);
				//UpdateWindow(hWnd);
			}
			break;
	}
}

UINT CheckDrawItem()
{
	UINT drawItem;
	UINT temp = GetMenuState(drawItemsMenu, MENU_PENCIL, MF_BYCOMMAND);
	if (temp & MF_CHECKED)
	{
		drawItem = MENU_PENCIL;
	}
	else
	{
		drawItem = MENU_LINE;
	}
	return drawItem;
}

BOOL Redraw(HWND hWnd, BOOL isToClearArea)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	return InvalidateRect(hWnd, &rect, isToClearArea);
}

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	return LineTo(hdc, x2, y2);
}

void TrackMouse(HWND hWnd) // detect mouse left the window
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = 1;
	tme.hwndTrack = hWnd;
	TrackMouseEvent(&tme);
}

void MenuCommand(HWND hWnd, WPARAM param)
{
	switch (param)
	{
		case MENU_OPEN:
			//OpenFileWindow(hWnd);
			break;
		case MENU_CREATE:
			break;
		case MENU_SAVE:
			break;
		case MENU_EXIT:
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
		case MENU_PENCIL:
			CheckMenuItem(drawItemsMenu, MENU_PENCIL, MF_CHECKED);
			CheckMenuItem(drawItemsMenu, MENU_LINE, MF_UNCHECKED);
			break;
		case MENU_LINE:
			CheckMenuItem(drawItemsMenu, MENU_PENCIL, MF_UNCHECKED);
			CheckMenuItem(drawItemsMenu, MENU_LINE, MF_CHECKED);
			break;

	}
}

void OpenFileWindow(HWND hWnd)
{
	TCHAR szFile[260];
	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	HANDLE handle;

	if (GetOpenFileName(&ofn) == TRUE)
		handle = CreateFile(ofn.lpstrFile, GENERIC_READ,
		0, (LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
}