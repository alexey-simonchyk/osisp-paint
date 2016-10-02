#include <windows.h>
#include <iostream>
#include "MenuBar.h"
#include "Window.cpp"
#include "Dialog.cpp"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // message handler
void MenuCommand(HWND hWnd, WPARAM param, int drawItem);
void setChoosenWidth(int choosenWidth);
void TrackMouse(HWND hwnd);
UINT CheckDrawItem();
void OpenFileDialog(HWND hwnd, bool isOpenFile);
void chooseColor(HWND hWnd);
void setDrawItem(int newDrawItem, int drawItem);
HMENU drawItemsMenu;
HMENU widthPenMenu;
Window *window = NULL;

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
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
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
	HMENU colorMenu;
	
	AppendMenu(subMenu, MF_STRING, MENU_OPEN, L"Open");
	AppendMenu(subMenu, MF_STRING, MENU_CREATE, L"Create");
	AppendMenu(subMenu, MF_STRING, MENU_SAVE, L"Save");
	AppendMenu(subMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(subMenu, MF_STRING, MENU_EXIT, L"Exit");
	AppendMenu(hMenu, MF_POPUP, (UINT)subMenu, L"File");

	drawItemsMenu = CreateMenu();
	AppendMenu(drawItemsMenu, MF_CHECKED, MENU_PENCIL, L"Pencil");
	AppendMenu(drawItemsMenu, MF_UNCHECKED, MENU_LINE, L"Line");
	AppendMenu(drawItemsMenu, MF_UNCHECKED, MENU_RECTANGLE, L"Rectangle");
	AppendMenu(drawItemsMenu, MF_UNCHECKED, MENU_ELLIPSE, L"Ellipse");
	AppendMenu(drawItemsMenu, MF_UNCHECKED, MENU_POLY_LINE, L"Poly line");
	AppendMenu(drawItemsMenu, MF_UNCHECKED, MENU_TEXT, L"Text");
	AppendMenu(hMenu, MF_POPUP, (UINT)drawItemsMenu, L"Draw Items");

	widthPenMenu = CreateMenu();
	AppendMenu(widthPenMenu, MF_UNCHECKED, MENU_WIDTH_1, L"1");
	AppendMenu(widthPenMenu, MF_CHECKED, MENU_WIDTH_2, L"2");
	AppendMenu(widthPenMenu, MF_UNCHECKED, MENU_WIDTH_3, L"3");
	AppendMenu(widthPenMenu, MF_UNCHECKED, MENU_WIDTH_4, L"4");
	AppendMenu(widthPenMenu, MF_UNCHECKED, MENU_WIDTH_5, L"5");
	AppendMenu(widthPenMenu, MF_UNCHECKED, MENU_WIDTH_6, L"6");
	AppendMenu(hMenu, MF_POPUP, (UINT)widthPenMenu, L"Width");

	colorMenu = CreateMenu();
	AppendMenu(colorMenu, MF_STRING, MENU_COLOR, L"Color");
	AppendMenu(hMenu, MF_POPUP, (UINT)colorMenu, L"Color");
	SetMenu(hWnd, hMenu);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT paintStruct;
	static UINT drawItem;
	static int windowWidth;
	static int windowHeight;
	
	switch (uMsg)
	{
		case WM_CREATE:
		{
			CreateMainMenu(hWnd);
			window = new Window(hWnd);
			window->setScrollSize(hWnd, windowWidth, windowHeight);
			window->sendRedrawMsg(hWnd, FALSE);
			drawItem = MENU_PENCIL;
			break;
		}

		case WM_SIZE:
			windowWidth = LOWORD(lParam);
			windowHeight = HIWORD(lParam);
			if (window != NULL)
				window->setScrollSize(hWnd, windowWidth, windowHeight);
			break;

		case WM_MOUSEWHEEL:
			if ((short)HIWORD(GetKeyState(VK_CONTROL)))
			{
				if ((short)HIWORD(wParam) < 0)
				{
					window->changeZoom(false);
					window->setScrollSize(hWnd, windowWidth, windowHeight);
				}
				else
				{
					window->changeZoom(true);
					window->setScrollSize(hWnd, windowWidth, windowHeight);
				}
			}
			else
			{
				if ((short)HIWORD(wParam) < 0)
				{
					window->scrollY(hWnd, false);
				}
				else
				{
					window->scrollY(hWnd, true);
				}
			}
			window->sendRedrawMsg(hWnd, FALSE);
			break;

		case WM_LBUTTONDOWN:
			
			drawItem = CheckDrawItem();
			if (!window->getDrawing())
			{
				window->setDrawItem(drawItem, hWnd);
			}
			else
			{
				if (drawItem == MENU_TEXT)
				{
					window->endDraw(hWnd);
					window->saveMousePosition(hWnd);
					window->setDrawItem(MENU_TEXT, hWnd);
					window->endPrintText();
				}
			}
			break;

		case WM_MOUSELEAVE:
			window->endDraw(hWnd);
			break;

		case WM_KEYDOWN:
			if (window->getDrawing())
			{
				hdc = GetDC(hWnd);
				window->printText(hdc, (wchar_t)wParam);
				ReleaseDC(hWnd, hdc);
			}
			break;

		case WM_LBUTTONUP:
			if (drawItem != MENU_TEXT)
				window->endDraw(hWnd);
			break;

		case WM_RBUTTONDOWN:
			if (drawItem == MENU_TEXT)
			{
				window->endDraw(hWnd);
				window->endPrintText();
			}
			break;

		case WM_MOUSEMOVE:
		{
			if (drawItem != MENU_TEXT)
			{
				TrackMouse(hWnd);
				window->setMousePosition(hWnd);
				if (window->getDrawing())
					window->sendRedrawMsg(hWnd, FALSE);
			}
			break;
		}
		case WM_COMMAND:
			MenuCommand(hWnd, wParam, drawItem);
			window->endDraw(hWnd);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &paintStruct);
			window->draw(hdc);
			EndPaint(hWnd, &paintStruct);
			break;

		case WM_DESTROY:
			window->onClose();
			PostQuitMessage(NULL); // send close message
			break;

		case WM_HSCROLL:
			window->scrollX(wParam, hWnd);
			window->sendRedrawMsg(hWnd, FALSE);
			break;
		case WM_VSCROLL:
			window->scrollY(wParam, hWnd);
			window->sendRedrawMsg(hWnd, FALSE);
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam); // transfer message to the standart event handler
	}
	return NULL;
}


UINT CheckDrawItem()
{
	UINT temp = GetMenuState(drawItemsMenu, MENU_PENCIL, MF_BYCOMMAND);
	if (temp & MF_CHECKED)
		return MENU_PENCIL;
	temp = GetMenuState(drawItemsMenu, MENU_LINE, MF_BYCOMMAND);
	if (temp & MF_CHECKED)
		return MENU_LINE;
	temp = GetMenuState(drawItemsMenu, MENU_RECTANGLE, MF_BYCOMMAND);
	if (temp & MF_CHECKED)
		return MENU_RECTANGLE;
	temp = GetMenuState(drawItemsMenu, MENU_TEXT, MF_BYCOMMAND);
	if (temp & MF_CHECKED)
		return MENU_TEXT;
	temp = GetMenuState(drawItemsMenu, MENU_ELLIPSE, MF_BYCOMMAND);
	if (temp & MF_CHECKED)
		return MENU_ELLIPSE;
	temp = GetMenuState(drawItemsMenu, MENU_POLY_LINE, MF_BYCOMMAND);
	if (temp & MF_CHECKED)
		return MENU_POLY_LINE;
	return 0;
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

void MenuCommand(HWND hWnd, WPARAM param, int drawItem)
{
	switch (param)
	{
		case MENU_OPEN:
			OpenFileDialog(hWnd, true);
			break;
		case MENU_COLOR:
			chooseColor(hWnd);
			break;
		case MENU_CREATE:
			break;
		case MENU_RECTANGLE:
			setDrawItem(MENU_RECTANGLE, drawItem);
			break;
		case MENU_SAVE:
			OpenFileDialog(hWnd, false);
			break;
		case MENU_EXIT:
			window->onClose();
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
		case MENU_PENCIL:
			setDrawItem(MENU_PENCIL, drawItem);
			break;
		case MENU_LINE:
			setDrawItem(MENU_LINE, drawItem);
			break;
		case MENU_ELLIPSE:
			setDrawItem(MENU_ELLIPSE, drawItem);
			break;
		case MENU_TEXT:
			setDrawItem(MENU_TEXT, drawItem);
			window->setDrawItem(MENU_TEXT, hWnd);
			window->saveMousePosition(hWnd);
			break;
		case MENU_POLY_LINE:
			setDrawItem(MENU_POLY_LINE, drawItem);
			break;
		default:
			if (param <= MENU_WIDTH_6 && param >= MENU_WIDTH_1)
			{
				setChoosenWidth(param);
				window->setPenWidth(param);
			}
	}
}

void setDrawItem(int newDrawItem, int drawItem)
{
	CheckMenuItem(drawItemsMenu, newDrawItem, MF_CHECKED);
	CheckMenuItem(drawItemsMenu, drawItem, MF_UNCHECKED);
}

void setChoosenWidth(int choosenWidth)
{
	static int previousWidth = MENU_WIDTH_2;
	CheckMenuItem(widthPenMenu, choosenWidth, MF_CHECKED);
	CheckMenuItem(widthPenMenu, previousWidth, MF_UNCHECKED);
	previousWidth = choosenWidth;
}

void chooseColor(HWND hWnd)
{
	CHOOSECOLOR chooseColor;
	static COLORREF choosenColors[16];
	static DWORD rgbCurrent;

	ZeroMemory(&chooseColor, sizeof(CHOOSECOLOR));
	chooseColor.lStructSize = sizeof(CHOOSECOLOR);
	chooseColor.hwndOwner = hWnd;
	chooseColor.lpCustColors = choosenColors;
	chooseColor.rgbResult = rgbCurrent;

	chooseColor.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&chooseColor) == TRUE)
	{
		rgbCurrent = chooseColor.rgbResult;
		window->setColorPen(rgbCurrent);
	}

}

void OpenFileDialog(HWND hWnd, bool isOpenFile)
{
	Dialog *dialog = new Dialog();
	bool temp = false;
	char szFileName[MAX_PATH] = "";
	LPWSTR filePath;
	if (isOpenFile)
	{
		filePath = dialog->OpenFileDialog(szFileName);
		window->openFile(hWnd, filePath);
	}
	else
	{
		filePath = dialog->OpenSaveDialog(szFileName);
		window->saveToFile(hWnd, filePath);
	}
	delete dialog;

	
}
