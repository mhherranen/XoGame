//AIBrain.cpp: This module handles AI decision making using neural nertwork.

#include "stdafx.h"

// ctor
AIBrain::AIBrain() : m_pNeuralNet(NULL)
{
	m_pNeuralNet = new CNeuralNet();
	m_MoveEvaluation.assign(SQUARES_TOT, 0.0);
//	m_dInputScale = 10.0 / double(SQUARES_TOT);
	m_dInputScale = 1.0;
	if (m_dInputScale > 1.0)
		m_dInputScale = 1.0;
}

// d-tor
//AIBrain::~AIBrain()
//{
//	delete m_pNeuralNet;
//	delete &m_MoveEvaluation;
//}


// selects move
int AIBrain::SelectMove(Game *pThisGame)
{
	m_iTurn = pThisGame->GetTurn();
	m_pGamePosition = pThisGame->GetpGamePosition();
	m_pEmptySquareMask = pThisGame->GetpEmptySquareMask();
	
	// check for normal or deep evaluation (testing all candidate moves)
	if (CParams::iDeep)
	{
		FastDeepEvaluation();
		//DeepEvaluation();
	}
	else
	{
		FastEvaluation();
		//Evaluation();

		// screen away non-legal moves (occupied squares)
		for (int i = 0; i < SQUARES_TOT; ++i)
			m_MoveEvaluation[i] *= m_pEmptySquareMask->at(i);
	}

	// get iterator to GamePosition corresponding to the selected (legal)
	// move with maximum evaluation value
	std::vector<double>::iterator it = std::max_element(m_MoveEvaluation.begin(), m_MoveEvaluation.end());
	//std::vector<double>::iterator itTest = std::max_element(m_MoveEvaluationTest.begin(), m_MoveEvaluationTest.end());

	// return the corresponding index
	const int index = int(it - m_MoveEvaluation.begin());
	//const int indexTest = int(itTest - m_MoveEvaluationTest.begin());

	return index;
}


// selects move with deep evaluation
//int AIBrain::SelectDeepMove(Game *pThisGame)
//{
//	m_iTurn = pThisGame->GetTurn();
//	m_pGamePosition = pThisGame->GetpGamePosition();
//	m_pEmptySquareMask = pThisGame->GetpEmptySquareMask();

//	FastDeepEvaluation();
	//DeepEvaluation();

	// get iterator to GamePosition corresponding to the selected (legal)
	// move with maximum evaluation value
//	std::vector<double>::iterator it = std::max_element(m_MoveEvaluation.begin(), m_MoveEvaluation.end());
	//std::vector<double>::iterator itTest = std::max_element(m_MoveEvaluationTest.begin(), m_MoveEvaluationTest.end());
	
	// return the corresponding index
//	const int index = int(it - m_MoveEvaluation.begin());
	//const int indexTest = int(itTest - m_MoveEvaluationTest.begin());

//	return index;
//}


// make move-evaluation by using Neural Net
void AIBrain::Evaluation()
{
	// convert game position data to vector of doubles for neural net input
	std::vector<double> inputs;
	for (int i = 0; i < SQUARES_TOT; ++i)
		inputs.push_back((double)(m_iTurn*m_pGamePosition->at(i)));

	// get move evaluation from Neural Net update (forward propagation)
	m_MoveEvaluation = m_pNeuralNet->Update(inputs);
}


// make move-evaluation by using Neural Net -- faster method
void AIBrain::FastEvaluation()
{
	// convert game position data to vector of doubles for neural net input
	VectorXd inputs(SQUARES_TOT);
	VectorXd outputs(SQUARES_TOT);

	for (int i = 0; i < SQUARES_TOT; ++i)
		inputs(i) = double(m_iTurn*m_pGamePosition->at(i));

	// get move evaluation from Neural Net update (forward propagation)
	outputs = m_pNeuralNet->FastUpdate(inputs);

	m_MoveEvaluation.clear();
	for (int i = 0; i < SQUARES_TOT; ++i)
		m_MoveEvaluation.push_back(outputs(i));
}




// make move-evaluation by using Neural Net -- faster method
void AIBrain::FastEvaluationOLD()
{
	// get move evaluation from Neural Net update (forward propagation)
	m_MoveEvaluation = m_pNeuralNet->FastUpdateOLD(m_pGamePosition, m_iTurn);
}


// make deep (one move depth) move-evaluation by using neural net
// (move evaluation for each candidate move separately)
void AIBrain::DeepEvaluation()
{
	// convert game position data to vector of doubles for neural net input
	std::vector<double> inputs;

	for (int i = 0; i < SQUARES_TOT; ++i)
		inputs.push_back(double(m_iTurn*m_pGamePosition->at(i))*m_dInputScale);

	m_MoveEvaluation.assign(SQUARES_TOT, 0.0);
	
	// get move evaluation for each candidate move separately from neural net update (forward propagation)
	for (int i = 0; i < SQUARES_TOT; ++i)
	{
		if (m_pEmptySquareMask->at(i) == TRUE)
		{
			inputs[i]=double(OWN_PIECE)*m_dInputScale;
			m_MoveEvaluation[i] = (m_pNeuralNet->Update(inputs)).front();
			inputs[i]=(double)EMPTY;
		}
	}
}

// faster version using Eigen Matrix class
void AIBrain::FastDeepEvaluation()
{
	// convert game position data to vector of doubles for neural net input
	VectorXd inputs(SQUARES_TOT);

	for (int i = 0; i < SQUARES_TOT; ++i)
		inputs(i)=(double(m_iTurn*m_pGamePosition->at(i))*m_dInputScale);

	m_MoveEvaluation.assign(SQUARES_TOT, 0.0);

	// get move evaluation for each candidate move separately from neural net update (forward propagation)
	for (int i = 0; i < SQUARES_TOT; ++i)
	{
		if (m_pEmptySquareMask->at(i) == TRUE)
		{
			inputs(i) = double(OWN_PIECE)*m_dInputScale;
			m_MoveEvaluation[i] = (m_pNeuralNet->FastUpdate(inputs))(0);
			inputs(i) = (double)EMPTY;
		}
	}
}


// make random move-evaluation
void AIBrain::RandomEvaluation()
{
	// seed random number generator from time
	srand(time(NULL));
	// generate a random number 'evaluation' for each square
	for (int i = 0; i < SQUARES_TOT; ++i)
		m_MoveEvaluation[i]= (rand() / (double)(RAND_MAX + 1.0));
}


// destroy the allocated neural network
void AIBrain::Destroy()
{
	delete m_pNeuralNet;
}