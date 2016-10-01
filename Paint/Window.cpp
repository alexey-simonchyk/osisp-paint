#include <windows.h>
#include "Draw.cpp"

#define ZOOM_STEP 0.5
#define ZOOM_MAX 9
#define ZOOM_MIN 1
#define SCROOL_STEP 10

class Window
{
private:
	Paint *paint;
	POINT mousePosition;
	POINT previousMousePosition;
	bool isNeedUpdateMousePos  = true;
	bool isDrawing;
	UINT currentDrawItem;

	int yMaxScroll;

	int xMaxScroll;

	double currentZoom;
	int offsetX;
	int offsetY;
	int hdcWidth;
	int hdcHeight;
	SCROLLINFO scrollInfo;

	
public:

	Window(HWND hWnd)
	{
		isDrawing = false;
		currentZoom = ZOOM_MIN;
		offsetX = 0;
		offsetY = 0;
		HDC hdc = GetDC(hWnd);
		hdcWidth = GetDeviceCaps(hdc, HORZRES);
		hdcHeight = GetDeviceCaps(hdc, VERTRES);
		ReleaseDC(hWnd, hdc);
		
		paint = new Paint(hWnd, &hdcWidth, &hdcHeight);

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
			paint->updateWindow(hdc, offsetX, offsetY, currentZoom);
			return;
		}
		switch (currentDrawItem)
		{
			case MENU_LINE:
				paint->drawLine(hdc, mousePosition.x, mousePosition.y, previousMousePosition.x, previousMousePosition.y, offsetX, offsetY, currentZoom);
				break;
			case MENU_PENCIL:
				paint->drawPencil(hdc, mousePosition.x, mousePosition.y, previousMousePosition.x, previousMousePosition.y, offsetX, offsetY, currentZoom);
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

	void endDraw(HWND hWnd, LPARAM lParam)
	{
		paint->endDraw();
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

	/// SCROLL ///

	void setScrollSize(HWND hWnd, int windowWidth, int windowHeight)
	{
		int pageSizeX, pageSizeY;
		if (currentZoom == 1)
		{
			pageSizeX = windowWidth;
			pageSizeY = windowHeight;
			xMaxScroll = hdcWidth - windowWidth;
			yMaxScroll = hdcHeight - windowHeight;
		}
		else
		{
			pageSizeX = hdcWidth / currentZoom;
			pageSizeY = hdcHeight / currentZoom;
			xMaxScroll = hdcWidth - hdcWidth / currentZoom;
			yMaxScroll = hdcHeight - hdcHeight / currentZoom;
		}
		
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		scrollInfo.nMin = 0;
		scrollInfo.nMax = hdcWidth;
		scrollInfo.nPage = pageSizeX;
		scrollInfo.nPos = offsetX;
		SetScrollInfo(hWnd, SB_HORZ, &scrollInfo, TRUE);
		
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		scrollInfo.nMin = 0;
		scrollInfo.nMax = hdcHeight;
		scrollInfo.nPage = pageSizeY;
		scrollInfo.nPos = offsetY;
		SetScrollInfo(hWnd, SB_VERT, &scrollInfo, TRUE);
		
	}

	void scrollY(WPARAM wParam, HWND hWnd)
	{
		int deltaTemp = offsetY;
		switch (LOWORD(wParam))
		{
		case SB_LINEDOWN:
			updateSrollY(false, hWnd);
			break;

		case SB_LINEUP:
			updateSrollY(true, hWnd);
			break;
		}
	}

	void scrollY(HWND hWnd, bool isUp)
	{
		updateSrollY(isUp, hWnd);
	}

	void scrollX(WPARAM wParam, HWND hWnd)
	{
		
		switch (LOWORD(wParam))
		{
		case SB_LINEDOWN:
			updateScrollX(false, hWnd);
			break;
		case SB_LINEUP:
			updateScrollX(true, hWnd);
			break;
		}
		
	}

	/// ZOOM ///

	void changeZoom(bool isIncrease)
	{
		if (isIncrease)
		{
			increaseZoom();
		}
		else
		{
			decreaseZoom();
		}
	}

	void increaseZoom()
	{
		if (currentZoom < ZOOM_MAX)
		{
			currentZoom += ZOOM_STEP;
		}
	}

	void decreaseZoom()
	{
		if (currentZoom > ZOOM_MIN)
		{
			currentZoom -= ZOOM_STEP;
		}
	}

	void onClose()
	{
		paint->onClose();
	}


	void saveToFile(HWND hWnd, LPWSTR filePath)
	{

		HDC hdc = GetDC(hWnd);
		RECT rect;
		GetClientRect(hWnd, &rect);
		
		int iWidthMM = GetDeviceCaps(hdc, HORZSIZE);
		int iHeightMM = GetDeviceCaps(hdc, VERTSIZE);
		int iWidthPels = GetDeviceCaps(hdc, HORZRES);
		int iHeightPels = GetDeviceCaps(hdc, VERTRES);
		rect.left = (rect.left * iWidthMM * 100) / iWidthPels;
		rect.top = (rect.top * iHeightMM * 100) / iHeightPels;
		rect.right = (rect.right * iWidthMM * 100) / iWidthPels;
		rect.bottom = (rect.bottom * iHeightMM * 100) / iHeightPels;

		HDC enhHDC = CreateEnhMetaFile(hdc, filePath, &rect, L"iPaint EMF file\0");
		if (!enhHDC)
		{
			MessageBox(NULL, L"Error creating file", L"Error", MB_OK);
		}
		paint->saveBufferHDC(enhHDC);
		SetMapMode(enhHDC, MM_ANISOTROPIC);
		HENHMETAFILE enhFile = CloseEnhMetaFile(enhHDC);
		ReleaseDC(hWnd, hdc);
	}

	void openFile(HWND hWnd, LPWSTR filePath)
	{
		HENHMETAFILE henHMetaFile = GetEnhMetaFile(filePath);
		RECT rect;
		GetClientRect(hWnd, &rect);
		paint->setBufferHDC(henHMetaFile, &rect);
		DeleteEnhMetaFile(henHMetaFile);
		HDC hdc = GetDC(hWnd);
		paint->endDraw();
		sendRedrawMsg(hWnd, FALSE);
		ReleaseDC(hWnd, hdc);
	}

private: 


	void updateScrollX(bool isRight, HWND hWnd)
	{
		int deltaX = changeScrollPosition(&offsetX, xMaxScroll, isRight);
		displayScrollBar(hWnd, offsetX, deltaX, 0, SB_HORZ);
	}


	void updateSrollY(bool isUp, HWND hWnd)
	{	
		int deltaY = changeScrollPosition(&offsetY, yMaxScroll, isUp);
		displayScrollBar(hWnd, offsetY, 0, deltaY, SB_VERT);
	}


	int changeScrollPosition(int *currentPos, int maxValue, bool checkDirection)
	{
		int delta = *currentPos;
		if (checkDirection)
		{
			*currentPos -= SCROOL_STEP;
			if (*currentPos < 0)
			{
				*currentPos = 0;
			}
		}
		else
		{
			*currentPos += SCROOL_STEP;
			if (*currentPos > maxValue)
			{
				*currentPos = maxValue;
			}
		}

		delta = *currentPos - delta;
		return delta;
	}



	void displayScrollBar(HWND hWnd, int scrollPosition,int deltaX, int deltaY, int param)
	{
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_POS;
		scrollInfo.nPos = scrollPosition;
		SetScrollInfo(hWnd, param, &scrollInfo, TRUE);

		ScrollWindowEx(hWnd, -deltaX, -deltaY, (CONST RECT *) NULL,
			(CONST RECT *) NULL, (HRGN)NULL, (PRECT)NULL,
			SW_INVALIDATE);
		sendRedrawMsg(hWnd, FALSE);
	}

};