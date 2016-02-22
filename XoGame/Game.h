// forward-declare AIBrain class
//class AIBrain;

// game modes
enum GameMode{ TwoHumans, HumanAI, AIHuman, TwoAIs, TwoAIsFast };
enum Player{Human, AI};

// struct for board points {x-coord, y-coord, color}
struct board_point {
	int x;
	int y;
	int col;
};
typedef struct board_point SMOVE;

// class for the game
class Game
{
private:
	HWND								m_hwnd;
	GameMode							m_Mode;
	std::array<Player, 2>				m_Players;
	std::vector<class AIBrain>			m_AIBrains;
	int									m_iTurn;
	int									m_iMoveCounter;
	int									m_iWinner;
	SMOVE								m_LastMove;
	SMOVE								m_WinnerRowFirst;
	SMOVE								m_WinnerRowLast;

	// game position in an array
	std::array<int, SQUARES_TOT> 		m_GamePosition;
	// empty square mask (1 == EMPTY, 0 == NOT EMPTY)
	std::array<int, SQUARES_TOT> 		m_EmptySquareMask;

	// board rendering class
	class RenderGame					*m_pRenderGame;

public:
	Game(HWND hwnd, GameMode Mode);

	// method to check if given point is valid board point
	int					LegalPoint(struct board_point sMove);
	// methods to make human move
	int					MakeMove(struct board_point sMove);
	// method for making move at Left Mouse Button
	void				OnLButtonDown(D2D1_POINT_2F ptMouse);
	// method on AI move
	void				OnAIMove();
	// method for AI making move
	void				AIMove();
	// method for checking end of game (win or draw)
	BOOL				CheckEndGame();
	// method for checking winner
	BOOL				CheckWinner();
	// method for checking draw position (board full)
	BOOL				CheckDraw();
	// method for initializing a new game
	void				ResetGame();
	// method to destroy the game and release memory
	void				Destroy();
	// method to map (WHITE,BLACK) to indices (0,1) in m_Players
	int					IndexMap(int iTurn);

	// accessors
	HWND							GetHandle() { return m_hwnd; }
	std::array<int, SQUARES_TOT>*	GetpGamePosition() { return &m_GamePosition; }
	std::array<int, SQUARES_TOT>*	GetpEmptySquareMask() { return &m_EmptySquareMask; }
	int								GetTurn() { return m_iTurn; }
	int								GetMoveCounter() { return m_iMoveCounter; }
	int								GetWinner() { return m_iWinner; }
	SMOVE*							GetpLastMove() { return &m_LastMove; }
	SMOVE*							GetpWinnerRowFirst() { return &m_WinnerRowFirst; }
	SMOVE*							GetpWinnerRowLast() { return &m_WinnerRowLast; }
	Player							GetPlayer(int iTurn) { return m_Players[IndexMap(iTurn)]; }
	RenderGame*						GetpRenderGame() { return m_pRenderGame; }

	void							SetMove(int index, int color) { m_GamePosition[index] = color; }
};

