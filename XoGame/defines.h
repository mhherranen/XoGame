/////////////////////////////////////////////////////////////////////////
//
//		File: defines.h
//
//		Desc: defines for the XoGame
//
/////////////////////////////////////////////////////////////////////////


// Number of squares (per side) and the pixel width of board
#define BOARD_SQUARES		19
#define SQUARES_TOT			BOARD_SQUARES * BOARD_SQUARES
#define WINNING_ROW			5
//#define BOARD_WIDTH		200

// the size of created game window compared to client window
#define GAMEWINDOW_SIZE		0.7
// The width of edge in the board window
#define BOARD_WIN_EDGE		10
// the offset factor of piece radius
#define RADIUS_OFFSET		0.7
// the offset factor of move ring radius
#define MOVE_RING_OFFSET	0.9

// codes for squares in game position
#define WHITE				1
#define BLACK				-1
#define EMPTY				0

#define OWN_PIECE			1
#define OPP_PIECE			-1

// code for game ending in a draw
#define DRAW				2

// user defined windows messages
#define        WM_ENDGAME     (WM_USER + 0)
#define        WM_AIMOVE     (WM_USER + 1)