#include <windows.h>
#include "Draw.cpp"

class Window
{
private:
	Paint *paint;
	POINT mousePosition;
	POINT previousMousePosition;
	bool isNeedUpdateMousePos  = true;
	bool isDrawing;
	UINT currentDrawItem;
	
public:

	Window(HWND hWnd)
	{
		isDrawing = false;
		paint = new Paint(hWnd);
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
		if (!isDrawing)
		{
			return;
		}
		switch (currentDrawItem)
		{
			case MENU_LINE:
				paint->drawLine(hdc, mousePosition.x, mousePosition.y, previousMousePosition.x, previousMousePosition.y);
				break;
			case MENU_PENCIL:
				paint->drawPencil(hdc, mousePosition.x, mousePosition.y, previousMousePosition.x, previousMousePosition.y);
				break;
		}
	}

	void setDrawItem(UINT drawItem, HWND hWnd)
	{
		currentDrawItem = drawItem;
		isNeedUpdateMousePos = drawItem == MENU_PENCIL ? true : false;
		setMousePosition(hWnd);
		isDrawing = true;
	}

	void endDraw(HWND hWnd)
	{
		paint->endDraw(hWnd);
		isDrawing = false;
		isNeedUpdateMousePos = true;
		setMousePosition(hWnd);
	}

	bool getDrawing()
	{
		return isDrawing;
	}

	void setColorPen(DWORD color)
	{
		paint->setColor(color);
	}

	void setPenWidth(int penWidth)
	{
		paint->setPenWidth(penWidth);
	}

	void onClose()
	{
		paint->onClose();
	}
};