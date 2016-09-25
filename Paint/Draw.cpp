#include <windows.h>
#include "MenuBar.h"

class Paint
{
private:
	HDC drawingArea;
	HDC finalPicture;
	int hdcWidth;
	int hdcHeigth;
	HPEN currentPen;
	DWORD currentColor;

	void initializeHDC(HWND hWnd)
	{
		HBITMAP bmFinalCopy;
		HBITMAP bmDrawingCopy;
		HBRUSH brush;
		HDC hdc;

		brush = CreateSolidBrush(0xffffff);
		hdc = GetDC(hWnd);
		hdcWidth = GetDeviceCaps(hdc, HORZRES);
		hdcHeigth = GetDeviceCaps(hdc, VERTRES);
		// Drawing HDC //
		drawingArea = CreateCompatibleDC(hdc);
		bmDrawingCopy = CreateCompatibleBitmap(hdc, hdcWidth, hdcHeigth);
		SelectObject(drawingArea, bmDrawingCopy);
		SelectObject(drawingArea, brush);
		PatBlt(drawingArea, 0, 0, hdcWidth, hdcHeigth, PATCOPY);

		// Result HDC //
		finalPicture = CreateCompatibleDC(hdc);
		bmFinalCopy = CreateCompatibleBitmap(hdc, hdcWidth, hdcHeigth);
		SelectObject(finalPicture, bmFinalCopy);
		SelectObject(finalPicture, brush);
		PatBlt(finalPicture, 0, 0, hdcWidth, hdcHeigth, PATCOPY);

		ReleaseDC(hWnd, hdc);
		DeleteObject(brush);
		DeleteObject(bmDrawingCopy);
		DeleteObject(bmFinalCopy);
	}

public:
	Paint(HWND hWnd)
	{
		initializeHDC(hWnd);
	}

	void drawLine(HDC hdc, int x1, int y1, int x2, int y2)
	{
		StretchBlt(drawingArea, 0, 0, hdcWidth, hdcHeigth, finalPicture, 0, 0, hdcWidth, hdcHeigth, SRCCOPY);
		line(drawingArea, x1, y1, x2, y2);
		StretchBlt(hdc, 0, 0, hdcWidth, hdcHeigth, drawingArea, 0, 0, hdcWidth, hdcHeigth, SRCCOPY);
	}

	void drawPencil(HDC hdc, int x1, int y1, int x2, int y2)
	{
		line(finalPicture, x1, y1, x2, y2);
		StretchBlt(hdc, 0, 0, hdcWidth, hdcHeigth, finalPicture, 0, 0, hdcWidth, hdcHeigth, SRCCOPY);
	}

	void endDraw(HWND hWnd)
	{
		HDC tempDC = GetDC(hWnd);
		StretchBlt(finalPicture, 0, 0, hdcWidth, hdcHeigth, tempDC, 0, 0, hdcWidth, hdcHeigth, SRCCOPY);
		ReleaseDC(hWnd, tempDC);
	}

	BOOL line(HDC hdc, int x1, int y1, int x2, int y2)
	{
		MoveToEx(hdc, x1, y1, NULL);
		return LineTo(hdc, x2, y2);
	}

	void setColor(DWORD color)
	{
		currentColor = color;
		currentPen = CreatePen(PS_SOLID, 10, color);
		SelectObject(finalPicture, currentPen);
		SelectObject(drawingArea, currentPen);
	}

	void onClose()
	{
		DeleteObject(currentPen);
		DeleteObject(drawingArea);
		DeleteObject(finalPicture);
	}

};
