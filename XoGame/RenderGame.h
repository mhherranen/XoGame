//#include <windows.h>
//#include "stdafx.h"
//#include <d2d1.h>
#pragma comment(lib, "d2d1")

// template for safely releasing COM-pointer
template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

// game rendering class using ID2D1 methods
class RenderGame
{
private:
	HWND							m_hwnd;
	ID2D1Factory					*pFactory;
	ID2D1HwndRenderTarget			*pRenderTarget;
	ID2D1SolidColorBrush			*pBrush;
	ID2D1SolidColorBrush			*pBrushBack;
	ID2D1SolidColorBrush			*pBrushMove;
	ID2D1SolidColorBrush			*pBrushWinningRow;
//	D2D1_ELLIPSE            ellipse;
	float					m_fStart_x;			// left x-edge of board
	float					m_fStart_y;			// top y-edge of board
	float					m_fEnd_x;			// right x-edge of board
	float					m_fEnd_y;			// bottom y-edge of board
	float					m_fSquareWidth;		// pixel width of a square
	float					m_fPieceRadius;		// radius of the piece (circle)
	D2D1_RECT_F				m_BoardRect;

public:
//	RenderGame(HWND hwnd, std::array<int, SQUARES_TOT> *pGamePosition, SMOVE *pLastMove);
	RenderGame(HWND hwnd);

	// pixels to DIP conversion class 
	class DPIScale	*m_pDPIScale;

	void			SetHwnd(HWND hwnd){m_hwnd = hwnd;}
	void			CalculateLayout();
	HRESULT			CreateGraphicsResources();
	void			DiscardGraphicsResources();
	int				Create();
	void			OnPaint(Game *pThisGame);
	void			Resize();
	void			Destroy();

// accessors
	float			GetStart_x()
	{
		return m_fStart_x;
	}
	float			GetStart_y()
	{
		return m_fStart_y;
	}
	float			GetSquareWidth()
	{
		return m_fSquareWidth;
	}

};


// class to convert pixels to device independent pixels (DIP) drawn by ID2D1
class DPIScale
{
	float m_fScaleX;
	float m_fScaleY;

public:
	// ctor
	DPIScale(ID2D1Factory *pFactory)
	{
		FLOAT dpiX, dpiY;
		pFactory->GetDesktopDpi(&dpiX, &dpiY);
		m_fScaleX = dpiX / 96.0f;
		m_fScaleY = dpiY / 96.0f;
	}

	template <typename T>
	D2D1_POINT_2F PixelsToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / m_fScaleX,
							static_cast<float>(y) / m_fScaleY);
	}
};

