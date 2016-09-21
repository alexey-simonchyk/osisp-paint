#include <windows.h>
#include "Draw.cpp"

class Window
{
private:
	Paint paint;
	POINT mousePosition;
	POINT previousMousePosition;
	bool isNeedUpdateMousePos  = true;
	
public:

	void create(HWND hWnd)
	{
		paint.create(hWnd);
	}

	BOOL sendRedrawMsg(HWND hWnd, BOOL isToClearArea)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		return InvalidateRect(hWnd, &rect, isToClearArea);
	}

	void setMousePosition(HWND hWnd)
	{
		if (isNeedUpdateMousePos)
		{
			previousMousePosition = mousePosition;
		}
		GetCursorPos(&mousePosition);
		ScreenToClient(hWnd, &mousePosition);
	}

	void draw(HDC hdc)
	{
		if (paint.getIsDrawing())
			paint.draw(hdc, mousePosition.x, mousePosition.y, previousMousePosition.x, previousMousePosition.y);
	}

	void setDrawItem(UINT drawItem, HWND hWnd)
	{
		paint.setDrawItem(drawItem);
		isNeedUpdateMousePos = drawItem == MENU_PENCIL ? true : false;
		setMousePosition(hWnd);
		paint.beginDraw();
	}

	void endDraw(HWND hWnd)
	{
		paint.endDraw(hWnd);
		isNeedUpdateMousePos = true;
		setMousePosition(hWnd);
	}

	bool isDrawing()
	{
		return paint.getIsDrawing();
	}
};