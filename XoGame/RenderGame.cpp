// RenderGame.cpp: This module renders the board and game position for Game-module

#include "stdafx.h"

//extern HWND g_hWndMDIClient;

// ctor
RenderGame::RenderGame(HWND hwnd)
	: m_hwnd(hwnd),
//	m_pThisGame(pThisGame),
	pFactory(NULL),
	pRenderTarget(NULL),
	pBrush(NULL),
	pBrushBack(NULL),
	pBrushMove(NULL),
	pBrushWinningRow(NULL)
{
}

// Recalculate drawing layout when the size of the window changes.
void RenderGame::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		const float board_width = max(0, min(size.width - 2 * BOARD_WIN_EDGE,
											size.height - 2 * BOARD_WIN_EDGE));
		m_fSquareWidth = board_width / BOARD_SQUARES;
		m_fPieceRadius = RADIUS_OFFSET * m_fSquareWidth / 2;
		m_fStart_x = (size.width - board_width) / 2;
		m_fStart_y = (size.height - board_width) / 2;
		m_fEnd_x = m_fStart_x + board_width;
		m_fEnd_y = m_fStart_y + board_width;
		m_BoardRect = D2D1::RectF(m_fStart_x, m_fStart_y, m_fEnd_x, m_fEnd_y);
		//ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	}
}

HRESULT RenderGame::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

//		RECT rctest;
//		GetClientRect(g_hWndMDIClient, &rctest);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

			if (SUCCEEDED(hr))
			{
				const D2D1_COLOR_F color_bgk = D2D1::ColorF(1, 1, 1);
				hr = pRenderTarget->CreateSolidColorBrush(color_bgk, &pBrushBack);

				if (SUCCEEDED(hr))
				{
					const D2D1_COLOR_F color_move = D2D1::ColorF(D2D1::ColorF::LimeGreen);
					hr = pRenderTarget->CreateSolidColorBrush(color_move, &pBrushMove);

					if (SUCCEEDED(hr))
					{
						const D2D1_COLOR_F color_row = D2D1::ColorF(1, 0, 0);
						hr = pRenderTarget->CreateSolidColorBrush(color_row, &pBrushWinningRow);

						if (SUCCEEDED(hr))
						{
							CalculateLayout();
						}
					}
				}
			}			
		}
	}
	return hr;
}

void RenderGame::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
	SafeRelease(&pBrushBack);
	SafeRelease(&pBrushMove);
	SafeRelease(&pBrushWinningRow);
}

int RenderGame::Create()
{
	if (FAILED(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
	{
		return -1;  // Fail CreateWindowEx.
	}
	m_pDPIScale = new DPIScale(pFactory);
	return 0;
}

void RenderGame::OnPaint(Game *pThisGame)
{
	std::array<int, SQUARES_TOT> *pGamePosition = pThisGame->GetpGamePosition();
	SMOVE *pLastMove = pThisGame->GetpLastMove();

	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();

		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Gainsboro));
		
		// draw board background rectangle
		pRenderTarget->FillRectangle(m_BoardRect, pBrushBack);

		// draw the squares of the board
		for (int n = 0; n < BOARD_SQUARES+1; n++)
		{
			// vertical lines
			pRenderTarget->DrawLine(
				D2D1::Point2F(static_cast<FLOAT>(m_fStart_x + n * m_fSquareWidth), m_fStart_y),
				D2D1::Point2F(static_cast<FLOAT>(m_fStart_x + n * m_fSquareWidth), m_fEnd_y),
				pBrush,
				1.0f
				);
			// horizontal lines
			pRenderTarget->DrawLine(
				D2D1::Point2F(m_fStart_x, static_cast<FLOAT>(m_fStart_y + n * m_fSquareWidth)),
				D2D1::Point2F(m_fEnd_x, static_cast<FLOAT>(m_fStart_y + n * m_fSquareWidth)),
				pBrush,
				1.0f
				);
		}
		
		// draw last move indicator ring
		if (pLastMove->col != EMPTY)
		{
			const float x = m_fStart_x + (pLastMove->x + 0.5)*m_fSquareWidth;
			const float y = m_fStart_y + (pLastMove->y + 0.5)*m_fSquareWidth;
			const float radius = MOVE_RING_OFFSET / RADIUS_OFFSET*m_fPieceRadius;
			D2D1_ELLIPSE move_ring = D2D1::Ellipse(D2D1::Point2F(x, y),
				radius,
				radius);
			D2D1_ELLIPSE move_ring_inner = D2D1::Ellipse(D2D1::Point2F(x, y),
				m_fPieceRadius,
				m_fPieceRadius);
			pRenderTarget->FillEllipse(move_ring, pBrushMove);
			pRenderTarget->FillEllipse(move_ring_inner, pBrushBack);
		}

		// draw pieces
		for (int i = 0; i < BOARD_SQUARES; i++)
		{
			for (int j = 0; j < BOARD_SQUARES; j++)
			{
				int color = pGamePosition->at(i + BOARD_SQUARES * j);
				if (color != EMPTY)
				{
					const float x = m_fStart_x + (i + 0.5)*m_fSquareWidth;
					const float y = m_fStart_y + (j + 0.5)*m_fSquareWidth;
					D2D1_ELLIPSE piece = D2D1::Ellipse(D2D1::Point2F(x, y),
															m_fPieceRadius,
															m_fPieceRadius);
					if (color == WHITE)
						pRenderTarget->DrawEllipse(piece, pBrush, 1.0f, NULL);
					else if (color == BLACK)
						pRenderTarget->FillEllipse(piece, pBrush);
				}
			}
		}

		// draw winner row
		if (pThisGame->GetWinner() != EMPTY)
		{
			int i1 = pThisGame->GetpWinnerRowFirst()->x;
			int j1 = pThisGame->GetpWinnerRowFirst()->y;
			int i2 = pThisGame->GetpWinnerRowLast()->x;
			int j2 = pThisGame->GetpWinnerRowLast()->y;

			const float x1 = m_fStart_x + (i1 + 0.5)*m_fSquareWidth;
			const float y1 = m_fStart_y + (j1 + 0.5)*m_fSquareWidth;
			const float x2 = m_fStart_x + (i2 + 0.5)*m_fSquareWidth;
			const float y2 = m_fStart_y + (j2 + 0.5)*m_fSquareWidth;
		
			pRenderTarget->DrawLine(D2D1::Point2F(x1,y1), D2D1::Point2F(x2,y2),
				pBrushWinningRow, 5.0f);
		}

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
	}
}

void RenderGame::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void RenderGame::Destroy()
{
	DiscardGraphicsResources();
	SafeRelease(&pFactory);
}

