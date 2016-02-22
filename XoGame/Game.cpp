// Game.cpp: This module handles the xo - game instance

#include "stdafx.h"

// declare global handles as external variables
extern HWND g_hWndFrame;
extern HWND g_hWndMDIClient;


// ctor
Game::Game(HWND hwnd, GameMode Mode) : m_hwnd(hwnd), m_Mode(Mode), m_iTurn(WHITE), m_iMoveCounter(0),
			m_iWinner(EMPTY), m_LastMove({0,0,0}), m_WinnerRowFirst({ 0,0,0 }), m_WinnerRowLast({ 0,0,0 })
{
	// intialize empty initial position
	m_GamePosition.assign(EMPTY);
	m_EmptySquareMask.assign(TRUE);

	// check first for fast (hidden) AI game mode
	if (m_Mode == TwoAIsFast)
	{
		m_Players.assign(AI);
		m_AIBrains.push_back(AIBrain());
		m_AIBrains.push_back(AIBrain());

		while (!CheckEndGame())
			AIMove();
		// destroy AI brains
		Destroy();
	}
	else
	{
		// create rendering unit
		m_pRenderGame = new RenderGame(hwnd);
		m_pRenderGame->Create();

		// assign players depending on the mode
		switch (m_Mode)
		{
		case TwoAIs:
			m_Players.assign(AI);
			m_AIBrains.push_back(AIBrain());
			m_AIBrains.push_back(AIBrain());
			break;
		case TwoHumans:
			m_Players.assign(Human);
			break;
		case HumanAI:
			m_Players.at(0) = Human;
			m_Players.at(1) = AI;
			m_AIBrains.push_back(AIBrain());
			break;
		case AIHuman:
			m_Players.at(0) = AI;
			m_Players.at(1) = Human;
			m_AIBrains.push_back(AIBrain());
			break;
		}
		if (GetPlayer(m_iTurn) == AI)
			PostMessage(m_hwnd, WM_AIMOVE, 0, 0);

	} //end if
}

// method to check if given point is valid board point
int Game::LegalPoint(struct board_point sMove)
{
	if (0 <= sMove.x && sMove.x < BOARD_SQUARES && 0 <= sMove.y && sMove.y < BOARD_SQUARES 
		&& (sMove.col == WHITE || sMove.col == BLACK))
		return TRUE;
	else
		return FALSE;
}

// methods to make human move
int Game::MakeMove(struct board_point Move)
{
	const int index = Move.x + BOARD_SQUARES * Move.y;

	if (!LegalPoint(Move))
		return FALSE;
	else if (m_EmptySquareMask[index] == FALSE)
		return FALSE;
	else
	{
		m_GamePosition[index] = Move.col;
		m_EmptySquareMask[index] = FALSE;
		m_LastMove = Move;
		m_iMoveCounter++;
		m_iTurn *= -1;
		return TRUE;
	}
}

// method for making move at Left Mouse Button
void Game::OnLButtonDown(D2D1_POINT_2F ptMouse)
{
	float Start_x = m_pRenderGame->GetStart_x();
	float Start_y = m_pRenderGame->GetStart_y();
	float SquareWidth = m_pRenderGame->GetSquareWidth();
	// compute the corresponding square indices
	if (GetPlayer(m_iTurn) == Human && ptMouse.x > Start_x && ptMouse.y > Start_y)
	{
		int i = int((ptMouse.x - Start_x) / SquareWidth);
		int j = int((ptMouse.y - Start_y) / SquareWidth);
		SMOVE Move = { i,j,m_iTurn };
		if (MakeMove(Move))
		{
			InvalidateRect(m_hwnd, NULL, FALSE);
			m_pRenderGame->OnPaint(this);
			if (CheckEndGame())
			{
				InvalidateRect(m_hwnd, NULL, FALSE);
				m_pRenderGame->OnPaint(this);
				PostMessage(m_hwnd, WM_ENDGAME, 0, 0);
			}
			else if (GetPlayer(m_iTurn) == AI)
				PostMessage(m_hwnd, WM_AIMOVE, 0, 0);
		}
	}
}


// method on AI move
void Game::OnAIMove()
{
	if (GetPlayer(m_iTurn) == AI)
	{
		AIMove();
		InvalidateRect(m_hwnd, NULL, FALSE);
		m_pRenderGame->OnPaint(this);
		if (CheckEndGame())
		{
			InvalidateRect(m_hwnd, NULL, FALSE);
			m_pRenderGame->OnPaint(this);
			PostMessage(m_hwnd, WM_ENDGAME, 0, 0);
		}
		else if (GetPlayer(m_iTurn) == AI)
		{
			MSG msg;

			// check for urgent messages during lengthy calculation
			while (m_iMoveCounter > 2 && PeekMessage(&msg, m_hwnd, 0, 0, PM_REMOVE))
			{
//				if(msg.hwnd == m_hwnd)
					DefMDIChildProc(m_hwnd, msg.message, msg.wParam, msg.lParam);
//				else if (msg.hwnd == g_hWndFrame)
//					DefFrameProc(g_hWndFrame, g_hWndMDIClient, msg.message, msg.wParam, msg.lParam);
			}

			PostMessage(m_hwnd, WM_AIMOVE, 0, 0);
		}
	}	
}


// method for AI making move
void Game::AIMove()
{
	SMOVE Move;
	int index;

	if (m_iTurn == WHITE)
		index = m_AIBrains.front().SelectMove(this);
	else
		index = m_AIBrains.back().SelectMove(this);

	// if the move is still illegal (all evaluation values were 0),
	// choose 'first' empty square
	if (m_EmptySquareMask[index] == FALSE)
		for (index = 0; m_EmptySquareMask[index] == FALSE; index++);

	m_GamePosition[index] = m_iTurn;
	m_EmptySquareMask[index] = FALSE;

	m_LastMove.y = int(index / BOARD_SQUARES);
	m_LastMove.x = index - m_LastMove.y*BOARD_SQUARES;
	m_LastMove.col = m_iTurn;

	m_iMoveCounter++;
	m_iTurn *= -1;
}


// method for checking end of game (win or draw)
BOOL Game::CheckEndGame()
{
	if (CheckWinner())
		return TRUE;
	if (CheckDraw())
		return TRUE;
	return FALSE;
}


// method for checking winner
BOOL Game::CheckWinner()
{
	// return if not enough moves to win
	if (m_iMoveCounter < WINNING_ROW)
		return FALSE;

	// local variables
	int ThisCol = m_LastMove.col;
	int ThisX = m_LastMove.x;
	int ThisY = m_LastMove.y;
	int CounterPlus, CounterMinus;

	// check horizontal winning row through the last move
	CounterPlus = 1;
	CounterMinus = 1;
	while (CounterPlus < WINNING_ROW && ThisX + CounterPlus < BOARD_SQUARES
		&& m_GamePosition[ThisX + CounterPlus + BOARD_SQUARES * ThisY] == ThisCol)
		CounterPlus++;
	CounterPlus--;
	while (CounterMinus < WINNING_ROW - CounterPlus && ThisX - CounterMinus >= 0
		&& m_GamePosition[ThisX - CounterMinus + BOARD_SQUARES * ThisY] == ThisCol)
		CounterMinus++;
	CounterMinus--;
	
	if (CounterPlus + CounterMinus + 1 >= WINNING_ROW)
	{
		m_iWinner = ThisCol;
		m_WinnerRowFirst = { ThisX + CounterPlus,ThisY,ThisCol };
		m_WinnerRowLast = { ThisX - CounterMinus,ThisY,ThisCol };
		return TRUE;
	}

	// check vertical winning row through the last move
	CounterPlus = 1;
	CounterMinus = 1;
	while (CounterPlus < WINNING_ROW && ThisY + CounterPlus < BOARD_SQUARES
		&& m_GamePosition[ThisX + BOARD_SQUARES * (ThisY + CounterPlus)] == ThisCol)
		CounterPlus++;
	CounterPlus--;
	while (CounterMinus < WINNING_ROW - CounterPlus && ThisY - CounterMinus >= 0
		&& m_GamePosition[ThisX + BOARD_SQUARES * (ThisY - CounterMinus)] == ThisCol)
		CounterMinus++;
	CounterMinus--;

	if (CounterPlus + CounterMinus + 1 >= WINNING_ROW)
	{
		m_iWinner = ThisCol;
		m_WinnerRowFirst = { ThisX, ThisY + CounterPlus,ThisCol };
		m_WinnerRowLast = { ThisX, ThisY - CounterMinus,ThisCol };
		return TRUE;
	}

	// check diagonal winning rows through the last move
	// first positive diagonal (xStep = 1, yStep = 1)
	CounterPlus = 1;
	CounterMinus = 1;
	while (CounterPlus < WINNING_ROW && ThisX + CounterPlus < BOARD_SQUARES
		&& ThisY + CounterPlus < BOARD_SQUARES
		&& m_GamePosition[ThisX + CounterPlus + BOARD_SQUARES * (ThisY + CounterPlus)] == ThisCol)
		CounterPlus++;
	CounterPlus--;
	while (CounterMinus < WINNING_ROW - CounterPlus && ThisX - CounterMinus >= 0 
		&& ThisY - CounterMinus >= 0
		&& m_GamePosition[ThisX - CounterMinus + BOARD_SQUARES * (ThisY - CounterMinus)] == ThisCol)
		CounterMinus++;
	CounterMinus--;

	if (CounterPlus + CounterMinus + 1 >= WINNING_ROW)
	{
		m_iWinner = ThisCol;
		m_WinnerRowFirst = { ThisX + CounterPlus, ThisY + CounterPlus,ThisCol };
		m_WinnerRowLast = { ThisX - CounterMinus, ThisY - CounterMinus,ThisCol };
		return TRUE;
	}

	// then 'negative' diagonal (xStep = 1, yStep = -1)
	CounterPlus = 1;
	CounterMinus = 1;
	while (CounterPlus < WINNING_ROW && ThisX + CounterPlus < BOARD_SQUARES
		&& ThisY - CounterPlus >= 0
		&& m_GamePosition[ThisX + CounterPlus + BOARD_SQUARES * (ThisY - CounterPlus)] == ThisCol)
		CounterPlus++;
	CounterPlus--;
	while (CounterMinus < WINNING_ROW - CounterPlus && ThisX - CounterMinus >= 0
		&& ThisY + CounterMinus < BOARD_SQUARES
		&& m_GamePosition[ThisX - CounterMinus + BOARD_SQUARES * (ThisY + CounterMinus)] == ThisCol)
		CounterMinus++;
	CounterMinus--;

	if (CounterPlus + CounterMinus + 1 >= WINNING_ROW)
	{
		m_iWinner = ThisCol;
		m_WinnerRowFirst = { ThisX + CounterPlus, ThisY - CounterPlus,ThisCol };
		m_WinnerRowLast = { ThisX - CounterMinus, ThisY + CounterMinus,ThisCol };
		return TRUE;
	}
	return FALSE;
}

// method for checking draw position (board full)
BOOL Game::CheckDraw()
{
	if (m_iMoveCounter >= SQUARES_TOT)
	{
		m_iWinner = DRAW;
		return TRUE;
	}
	return FALSE;
}

// method for initializing a new game
void Game::ResetGame()
{
	//	int msgboxID = MessageBox(
	//						NULL,
	//						L"Current game will be lost. Continue?",
	//						L"Confirm New game",
	//						MB_OKCANCEL
	//						);
	//
	//	if (msgboxID == IDOK)
	//		pGame->ResetGame();
	//

	// intialize empty initial position
	m_GamePosition.assign(EMPTY);
	m_EmptySquareMask.assign(TRUE);

	// for TwoAI game choose new random brains
	if (m_Mode == TwoAIs)
	{
		m_AIBrains.front().Destroy();
		m_AIBrains.back().Destroy();
		m_AIBrains.clear();
		m_AIBrains.push_back(AIBrain());
		m_AIBrains.push_back(AIBrain());
	}

	m_iTurn = WHITE;
	m_iMoveCounter = 0;
	m_iWinner = EMPTY;
	m_LastMove = { 0,0,0 };
	m_WinnerRowFirst = { 0,0,0 };
	m_WinnerRowLast = { 0,0,0 };
	
	// update the window
	InvalidateRect(m_hwnd, NULL, FALSE);
	m_pRenderGame->OnPaint(this);

	if (GetPlayer(m_iTurn) == AI)
		PostMessage(m_hwnd, WM_AIMOVE, 0, 0);
}

// method to map (WHITE,BLACK) to indices (0,1) in m_Players
int	Game::IndexMap(int iTurn)
{
	if (iTurn == WHITE)
		return 0;
	else
		return 1;
}


// method to destroy the game and release memory
void Game::Destroy()
{
	if (m_Mode != TwoAIsFast)
	{
		// destroy rendering methods
		m_pRenderGame->Destroy();
		delete m_pRenderGame;		
	}
	// destroy AI brains
	for (int i = 0; i < m_AIBrains.size(); ++i)
		m_AIBrains[i].Destroy();
	m_AIBrains.clear();
}

