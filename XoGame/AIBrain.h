// forward-declare Neural Net class
//class CNeuralNet;

// class for the AI Brain
class AIBrain
{
private:
	// point to the neural net class
	class CNeuralNet					*m_pNeuralNet;

	// neural net class instance
	//CNeuralNet						m_NeuralNet;

	// whose turn	
	int									m_iTurn;
	// scale factor for inputs
	double								m_dInputScale;

	// pointer to game position
	std::array<int, SQUARES_TOT> 		*m_pGamePosition;
	// empty square mask (1 == EMPTY, 0 == NOT EMPTY)
	std::array<int, SQUARES_TOT> 		*m_pEmptySquareMask;
	// Vector of evaluations (number between 0 and 1) for the candidate moves
	std::vector<double>					m_MoveEvaluation;
	//std::vector<double>					m_MoveEvaluationTest;

public:

	AIBrain();
//	~AIBrain();

	// selects move
	int		SelectMove(class Game *pThisGame);
	// selects move with deep evaluation
	//int		SelectDeepMove(Game *pThisGame);
	// make move-evaluation by using Neural Net
	void	Evaluation();
	void	FastEvaluation();
	void	FastEvaluationOLD();
	// make deep (one move depth) move-evaluation by using neural net
	void	DeepEvaluation();
	void	FastDeepEvaluation();
	// make random move-evaluation
	void	RandomEvaluation();
	// destroy the allocated neural net
	void	Destroy();
};