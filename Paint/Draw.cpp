#include <windows.h>
#include "MenuBar.h"

#define START_PEN_WIDTH 2
#define START_PEN_COLOR RGB(0, 0, 0)
#define MAX_TEXT_LENGHT 29

class Paint
{
private:
	HDC drawingArea;
	HDC finalPicture;
	int *hdcWidth;
	int *hdcHeight;
	HPEN currentPen;
	DWORD currentColor;
	int currentPenWidth;
	POINT *points;
	wchar_t bufferedText[MAX_TEXT_LENGHT + 1];
	int textLenght;

	void initializeHDC(HWND hWnd)
	{
		HBITMAP bmFinalCopy;
		HBITMAP bmDrawingCopy;
		HBRUSH brush;
		HDC hdc;
		HBRUSH temp;
		

		brush = CreateSolidBrush(0xffffff);
		hdc = GetDC(hWnd);

		// Drawing HDC //
		drawingArea = CreateCompatibleDC(hdc);
		bmDrawingCopy = CreateCompatibleBitmap(hdc, *hdcWidth, *hdcHeight);
		SelectObject(drawingArea, bmDrawingCopy);
		temp = (HBRUSH)SelectObject(drawingArea, brush);
		DeleteObject(temp);
		PatBlt(drawingArea, 0, 0, *hdcWidth, *hdcHeight, PATCOPY);

		// Result HDC //
		finalPicture = CreateCompatibleDC(hdc);
		bmFinalCopy = CreateCompatibleBitmap(hdc, *hdcWidth, *hdcHeight);
		SelectObject(finalPicture, bmFinalCopy);
		temp = (HBRUSH)SelectObject(finalPicture, brush);
		DeleteObject(temp);
		PatBlt(finalPicture, 0, 0, *hdcWidth, *hdcHeight, PATCOPY);

		ReleaseDC(hWnd, hdc);
		DeleteObject(brush);
		DeleteObject(bmDrawingCopy);
		DeleteObject(bmFinalCopy);
		temp = (HBRUSH)SelectObject(drawingArea, GetStockObject(HOLLOW_BRUSH));
		DeleteObject(temp);

		SetBkMode(drawingArea, TRANSPARENT);
	}

public:
	Paint(HWND hWnd, int *hdcWidth, int *hdcHeight)
	{
		this->hdcHeight = hdcHeight;
		this->hdcWidth = hdcWidth;
		initializeHDC(hWnd);
		currentPenWidth = START_PEN_WIDTH;
		currentColor = START_PEN_COLOR;
		currentPen = CreatePen(PS_SOLID, currentPenWidth, currentColor);
		setPen();
		textLenght = 0;
	}

	void drawLine(HDC hdc, int x1, int y1, int x2, int y2, int offsetX, int offsetY, double currentZoom)
	{
		realCoordinates(&x1, &y1, &x2, &y2, offsetX, offsetY, currentZoom);
		StretchBlt(drawingArea, 0, 0, *hdcWidth, *hdcHeight, finalPicture, 0, 0, *hdcWidth, *hdcHeight, SRCCOPY);
		line(drawingArea, x1, y1, x2, y2);
		StretchBlt(hdc, 0, 0, *hdcWidth, *hdcHeight, drawingArea, offsetX, offsetY, *hdcWidth / currentZoom, *hdcHeight / currentZoom, SRCCOPY);
	}

	void drawPencil(HDC hdc, int x1, int y1, int x2, int y2, int offsetX, int offsetY, double currentZoom)
	{
		realCoordinates(&x1, &y1, &x2, &y2, offsetX, offsetY, currentZoom);
		StretchBlt(drawingArea, 0, 0, *hdcWidth, *hdcHeight, finalPicture, 0, 0, *hdcWidth, *hdcHeight, SRCCOPY);
		line(drawingArea, x1, y1, x2, y2);
		StretchBlt(hdc, 0, 0, *hdcWidth, *hdcHeight, drawingArea, offsetX, offsetY, *hdcWidth / currentZoom, *hdcHeight / currentZoom, SRCCOPY);
		StretchBlt(finalPicture, 0, 0, *hdcWidth, *hdcHeight, drawingArea, 0, 0, *hdcWidth, *hdcHeight, SRCCOPY);
	}

	void drawEllipse(HDC hdc, int x1, int y1, int x2, int y2, int offsetX, int offsetY, double currentZoom)
	{
		realCoordinates(&x1, &y1, &x2, &y2, offsetX, offsetY, currentZoom);
		StretchBlt(drawingArea, 0, 0, *hdcWidth, *hdcHeight, finalPicture, 0, 0, *hdcWidth, *hdcHeight, SRCCOPY);
		Ellipse(drawingArea, x1, y1, x2, y2);
		StretchBlt(hdc, 0, 0, *hdcWidth, *hdcHeight, drawingArea, offsetX, offsetY, *hdcWidth / currentZoom, *hdcHeight / currentZoom, SRCCOPY);
	}

	void drawText(HDC hdc, int x1, int y1, int offsetX, int offsetY, int currentZoom, wchar_t symbol)
	{
		wchar_t *text;
		if (symbol == VK_BACK)
		{
			text = new wchar_t[--textLenght + 1];
			if (textLenght < 0)
			{
				textLenght = 0;
			}
		}
		else
		{
			text = new wchar_t[++textLenght + 1];
			if (textLenght < MAX_TEXT_LENGHT)
			{
				bufferedText[textLenght - 1] = symbol;
				bufferedText[textLenght] = '\0';
			}
			else
			{
				textLenght--;
			}
		}
		for (int i = 0; i <= textLenght ; i++)
		{
			text[i] = bufferedText[i];
		}
		realCoordinates(&x1, &y1, offsetX, offsetY, currentZoom);
		SetTextColor(drawingArea, currentColor);
		StretchBlt(drawingArea, 0, 0, *hdcWidth, *hdcHeight, finalPicture, 0, 0, *hdcWidth, *hdcHeight, SRCCOPY);
		TextOut(drawingArea, x1, y1, text, textLenght);
		StretchBlt(hdc, 0, 0, *hdcWidth, *hdcHeight, drawingArea, offsetX, offsetY, *hdcWidth / currentZoom, *hdcHeight / currentZoom, SRCCOPY);
	}
	
	void drawRectangle(HDC hdc, int x1, int y1, int x2, int y2, int offsetX, int offsetY, double currentZoom)
	{
		realCoordinates(&x1, &y1, &x2, &y2, offsetX, offsetY, currentZoom);
		StretchBlt(drawingArea, 0, 0, *hdcWidth, *hdcHeight, finalPicture, 0, 0, *hdcWidth, *hdcHeight, SRCCOPY);
		Rectangle(drawingArea, x1, y1, x2, y2);
		StretchBlt(hdc, 0, 0, *hdcWidth, *hdcHeight, drawingArea, offsetX, offsetY, *hdcWidth / currentZoom, *hdcHeight / currentZoom, SRCCOPY);
	}

	void endDraw()
	{
		StretchBlt(finalPicture, 0, 0, *hdcWidth, *hdcHeight, drawingArea, 0, 0, *hdcWidth, *hdcHeight, SRCCOPY);
	}

	void clearTextBuffer()
	{
		textLenght = 0;
	}

	BOOL line(HDC hdc, int x1, int y1, int x2, int y2)
	{
		MoveToEx(hdc, x1, y1, NULL);
		return LineTo(hdc, x2, y2);
	}

	void setColor(DWORD color)
	{
		currentColor = color;
		currentPen = CreatePen(PS_SOLID, currentPenWidth, currentColor);
		setPen();
	}

	void setPenWidth(int penWidth)
	{
		currentPenWidth = penWidth;
		currentPen = CreatePen(PS_SOLID, currentPenWidth, currentColor);
		setPen();
	}

	void onClose()
	{
		DeleteObject(currentPen);
		DeleteObject(drawingArea);
		DeleteObject(finalPicture);
	}

	void updateWindow(HDC hdc, int offsetX, int offsetY, double currentZoom)
	{
		StretchBlt(hdc, 0, 0, *hdcWidth, *hdcHeight, finalPicture, offsetX, offsetY, *hdcWidth / currentZoom, *hdcHeight / currentZoom, SRCCOPY);
	}

	void setBufferHDC(HENHMETAFILE henHMetaFile, RECT *rect)
	{
		PlayEnhMetaFile(drawingArea, henHMetaFile, rect);
	}

	void saveBufferHDC(HDC hdc)
	{
		StretchBlt(hdc, 0, 0, *hdcWidth, *hdcWidth, drawingArea, 0, 0, *hdcWidth, *hdcHeight, SRCCOPY);
	}

	void realCoordinates(int *x1, int *y1, int *x2, int *y2, int offsetX, int offsetY, double currentZoom)
	{
		*x1 /= currentZoom;
		*y2 /= currentZoom;
		*x2 /= currentZoom;
		*y1 /= currentZoom;

		*x1 += offsetX;
		*y2 += offsetY;
		*x2 += offsetX;
		*y1 += offsetY;
	}

	void realCoordinates(int *x1, int *y1, int offsetX, int offsetY, double currentZoom)
	{
		*x1 /= currentZoom;
		*y1 /= currentZoom;

		*x1 += offsetX;
		*y1 += offsetY;
	}

private:
	void setPen()
	{
		HPEN temp;
		temp = (HPEN)SelectObject(drawingArea, currentPen);
		DeleteObject(temp);
		temp = (HPEN)SelectObject(finalPicture, currentPen);
		DeleteObject(temp);
	}
};
