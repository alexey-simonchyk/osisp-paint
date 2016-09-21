#include <windows.h>
#include "MenuBar.h"

class Paint
{
private:
	HDC drawingArea;
	HDC finalPicture;
	int hdcWidth;
	int hdcHeigth;
	bool isDrawing = false;
	UINT drawItem;

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
		bmDrawingCopy = CreateCompatibleBitmap(drawingArea, hdcWidth, hdcHeigth);
		SelectObject(drawingArea, bmDrawingCopy);
		SelectObject(drawingArea, brush);
		PatBlt(drawingArea, 0, 0, hdcWidth, hdcHeigth, PATCOPY);
		// Result HDC //
		finalPicture = CreateCompatibleDC(hdc);
		bmFinalCopy = CreateCompatibleBitmap(finalPicture, hdcWidth, hdcHeigth);
		SelectObject(finalPicture, bmFinalCopy);
		SelectObject(finalPicture, brush);
		PatBlt(finalPicture, 0, 0, hdcWidth, hdcHeigth, PATCOPY);

		ReleaseDC(hWnd, hdc);
		DeleteObject(brush);
		DeleteObject(bmDrawingCopy);
		DeleteObject(bmFinalCopy);
	}

public:
	void create(HWND hWnd)
	{
		initializeHDC(hWnd);
	}

	void draw(HDC hdc, int x1, int y1, int x2, int y2)
	{
		switch (drawItem)
		{
			case MENU_PENCIL:
				line(finalPicture, x1, y1, x2, y2);
				StretchBlt(hdc, 0, 0, hdcWidth, hdcHeigth, finalPicture, 0, 0, hdcWidth, hdcHeigth, SRCCOPY);
				break;
			case MENU_LINE:
				StretchBlt(drawingArea, 0, 0, hdcWidth, hdcHeigth, finalPicture, 0, 0, hdcWidth, hdcHeigth, SRCCOPY);
				line(drawingArea, x1, y1, x2, y2);
				StretchBlt(hdc, 0, 0, hdcWidth, hdcHeigth, drawingArea, 0, 0, hdcWidth, hdcHeigth, SRCCOPY);
				break;
		}
	}

	void endDraw(HWND hWnd)
	{
		isDrawing = false;
		HDC tempDC = GetDC(hWnd);
		StretchBlt(finalPicture, 0, 0, hdcWidth, hdcHeigth, tempDC, 0, 0, hdcWidth, hdcHeigth, SRCCOPY);
		ReleaseDC(hWnd, tempDC);
	}

	void beginDraw()
	{
		isDrawing = true;
	}

	void setDrawItem(UINT drawItem)
	{
		this->drawItem = drawItem;
	}

	UINT getDrawItem()
	{
		return drawItem;
	}

	BOOL line(HDC hdc, int x1, int y1, int x2, int y2)
	{
		MoveToEx(hdc, x1, y1, NULL);
		return LineTo(hdc, x2, y2);
	}

	bool getIsDrawing()
	{
		return isDrawing;
	}

	void setIsDrawing(bool isDrawing)
	{
		this->isDrawing = isDrawing;
	}

};