#include "stdafx.h"

double SigmoidTable[NumSamples];


//*************************** methods for Neuron **********************
//
//---------------------------------------------------------------------
SNeuron::SNeuron(int NumInputs): m_NumInputs(NumInputs+1)
											
{
	//we need an additional weight for the bias hence the +1
	for (int i=0; i<NumInputs+1; ++i)
	{
		//set up the weights with an initial random value
		m_vecWeight.push_back(RandomClamped());
	}
}




//************************ methods for NeuronLayer **********************

//-----------------------------------------------------------------------
//	ctor creates a layer of neurons of the required size by calling the 
//	SNeuron ctor the rqd number of times
//-----------------------------------------------------------------------
SNeuronLayer::SNeuronLayer(int NumNeurons, 
                           int NumInputsPerNeuron):	m_NumNeurons(NumNeurons)
{
	for (int i=0; i<NumNeurons; ++i)

		m_vecNeurons.push_back(SNeuron(NumInputsPerNeuron));
}




//************************ methods forCNeuralNet ************************

//------------------------------default ctor ----------------------------
//
//	creates a ANN based on the default values in params.ini
//-----------------------------------------------------------------------
CNeuralNet::CNeuralNet()
{
	m_NumInputs				= CParams::iNumInputs;
	m_NumOutputs			= CParams::iNumOutputs;
	m_NumHiddenLayers	    = CParams::iNumHidden;
	m_NeuronsPerHiddenLyr	= CParams::iNeuronsPerHiddenLayer;

  //calculate the max required buffer size and reserve memory accordingly
  
	int BufferSize = CalculateMaxBufferSize();

	m_Inputs.reserve(BufferSize);
	m_Outputs.reserve(BufferSize);

	CreateNet();
	SetWeightMatrices();
}

//----------------------- CalculateMaxBufferSize -------------------------
//
//  calculates the max dimensions required for the vectors used in the
//  update function
//------------------------------------------------------------------------
int CNeuralNet::CalculateMaxBufferSize()
{
  int BufferSize = 0;
  
  BufferSize = m_NumInputs;
  if (m_NumOutputs > BufferSize)
  {
    BufferSize = m_NumOutputs;
  }
  if (m_NeuronsPerHiddenLyr > BufferSize)
  {
    BufferSize = m_NeuronsPerHiddenLyr;
  }

  return BufferSize;
}


//------------------------------createNet()------------------------------
//
//	this method builds the ANN. The weights are all initially set to 
//	random values -1 < w < 1
//------------------------------------------------------------------------
void CNeuralNet::CreateNet()
{
	// seed random number generator from time
	srand(time(NULL));
	
	//create the layers of the network
	if (m_NumHiddenLayers > 0)
	{
		//create first hidden layer
	  m_vecLayers.push_back(SNeuronLayer(m_NeuronsPerHiddenLyr, m_NumInputs));
    
    for (int i=0; i<m_NumHiddenLayers-1; ++i)
    {

			m_vecLayers.push_back(SNeuronLayer(m_NeuronsPerHiddenLyr,
                                         m_NeuronsPerHiddenLyr));
    }

    //create output layer
	  m_vecLayers.push_back(SNeuronLayer(m_NumOutputs, m_NeuronsPerHiddenLyr));
	}

  else
  {
	  //create output layer
	  m_vecLayers.push_back(SNeuronLayer(m_NumOutputs, m_NumInputs));
  }
}

//---------------------------------GetWeights-----------------------------
//
//	returns a vector containing the weights
//
//------------------------------------------------------------------------
vector<double> CNeuralNet::GetWeights() const
{
	//this will hold the weights
	vector<double> weights;
	
	//for each layer
	for (int i=0; i<m_NumHiddenLayers + 1; ++i)
	{

		//for each neuron
		for (int j=0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			//for each weight
			for (int k=0; k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs; ++k)
			{
				weights.push_back(m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k]);
			}
		}
	}

	return weights;
}

//-----------------------------------PutWeights---------------------------
//
//	given a vector of doubles this function replaces the weights in the NN
//  with the new values
//
//------------------------------------------------------------------------
void CNeuralNet::PutWeights(const vector<double> &weights)
{
	int cWeight = 0;
	
	//for each layer
	for (int i=0; i<m_NumHiddenLayers + 1; ++i)
	{

		//for each neuron
		for (int j=0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			//for each weight
			for (int k=0; k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs; ++k)
			{
				m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k] = weights[cWeight++];
			}
		}
	}

	return;
}

//---------------------------------GetNumberOfWeights---------------------
//
//	returns the total number of weights needed for the net
//
//------------------------------------------------------------------------
int CNeuralNet::GetNumberOfWeights() const
{

	int weights = 0;
	
	//for each layer
	for (int i=0; i<m_NumHiddenLayers + 1; ++i)
	{

		//for each neuron
		for (int j=0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			//for each weight
			for (int k=0; k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs; ++k)		
				weights++;			
		}
	}

	return weights;
}

//-------------------------------Update-----------------------------------
//
//	given an input vector this function calculates the output vector
//
//------------------------------------------------------------------------
vector<double> CNeuralNet::Update(const vector<double> &vecIn)
{
  m_Inputs = vecIn;

	int cWeight = 0;
	
	//first check that we have the correct amount of inputs
	if (m_Inputs.size() != m_NumInputs)
  {
		//just return an empty vector if incorrect.
		return m_Outputs;
  }
	
  
	//For each layer....
	for (int i=0; i<m_NumHiddenLayers + 1; ++i)
	{		
		if ( i > 0 )
    {
			m_Inputs = m_Outputs;
    }

		m_Outputs.clear();
		
		cWeight = 0;

		//for each neuron sum the (inputs * corresponding weights).Throw 
		//the total at our sigmoid function to get the output.
		for (int j=0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			double netinput = 0;

			int	NumInputs = m_vecLayers[i].m_vecNeurons[j].m_NumInputs;
			
			//for each weight
			for (int k=0; k<NumInputs - 1; ++k)
			{
				//sum the weights x inputs
				netinput += m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k]
                  * m_Inputs[cWeight++];
			}

			//add in the bias
			netinput += m_vecLayers[i].m_vecNeurons[j].m_vecWeight[NumInputs-1]
        * CParams::dBias;

			//we can store the outputs from each layer as we generate them. 
      //The combined activation is first filtered through the sigmoid 
      //function
			m_Outputs.push_back(FastSigmoid(netinput));

			cWeight = 0;
		}
	}

	return m_Outputs;
}


//-------------------------------FastUpdateOLD-----------------------------------
//
//	given an input vector this function calculates the output vector
//	(old faster version)
//
//------------------------------------------------------------------------
vector<double> CNeuralNet::FastUpdateOLD(array<int, SQUARES_TOT> *pGamePosition, int iTurn)
{
	// convert game position data to vector of doubles for neural net input
	m_Inputs.clear();
	for (int i = 0; i < SQUARES_TOT; ++i)
		m_Inputs.push_back((double)(iTurn*pGamePosition->at(i)));
	
	int cWeight = 0;

	//first check that we have the correct amount of inputs
	if (m_Inputs.size() != m_NumInputs)
	{
		//just return an empty vector if incorrect.
		return m_Outputs;
	}


	//For each layer....
	for (int i = 0; i<m_NumHiddenLayers + 1; ++i)
	{
		if (i > 0)
		{
			m_Inputs = m_Outputs;
		}

		m_Outputs.clear();

		cWeight = 0;

		//for each neuron sum the (inputs * corresponding weights).Throw 
		//the total at our sigmoid function to get the output.
		for (int j = 0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			double netinput = 0;

			int	NumInputs = m_vecLayers[i].m_vecNeurons[j].m_NumInputs;

			//for each weight
			for (int k = 0; k<NumInputs - 1; ++k)
			{
				//sum the weights x inputs
				netinput += m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k]
					* m_Inputs[cWeight++];
			}

			//add in the bias
			netinput += m_vecLayers[i].m_vecNeurons[j].m_vecWeight[NumInputs - 1]
				* CParams::dBias;

			//we can store the outputs from each layer as we generate them. 
			//The combined activation is first filtered through the sigmoid 
			//function
			m_Outputs.push_back(FastSigmoid(netinput));

			cWeight = 0;
		}
	}

	return m_Outputs;
}



//--------------------------- CalculateSplitPoints -----------------------
//
//  this method calculates all points in the vector of weights which 
//  represent the start and end points of individual neurons
//------------------------------------------------------------------------
vector<int> CNeuralNet::CalculateSplitPoints() const
{
	vector<int> SplitPoints;

  int WeightCounter = 0;
	
	//for each layer
	for (int i=0; i<m_NumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for (int j=0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			//for each weight
			for (int k=0; k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs; ++k)
      {
				++WeightCounter;			
      }

      SplitPoints.push_back(WeightCounter - 1);
		}
	}

	return SplitPoints;
}


// ADDITIONS:
// set weight matrices from weights
void CNeuralNet::SetWeightMatrices()
{
	//this will hold the weights
	WeightMatrix WeightMat;

	m_WeightMatrices.clear();

	//for each layer
	for (int i = 0; i<m_NumHiddenLayers + 1; ++i)
	{
		WeightMat.resize(m_vecLayers[i].m_NumNeurons, m_vecLayers[i].m_vecNeurons[0].m_NumInputs);

		//for each neuron
		for (int j = 0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			//for each weight
			for (int k = 0; k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs; ++k)
			{
				WeightMat(j,k) = m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k];
			}
		}
		m_WeightMatrices.push_back(WeightMat);
	}
}


// fast update method using weight matrices and linear algebra
VectorXd CNeuralNet::FastUpdate(const VectorXd &inputs)
{
	m_InputsM = inputs;

	//first check that we have the correct amount of inputs
	if (m_InputsM.rows() != m_NumInputs)
	{
		//just return an empty vector if incorrect.
		return m_InputsM;
	}

	//For each layer....
	for (int i = 0; i < m_NumHiddenLayers + 1; ++i)
	{
		// add bias as the last component of the input vector
		m_InputsM.conservativeResize(m_InputsM.rows() + 1);
		m_InputsM(m_InputsM.rows()-1) = CParams::dBias;

		// compute temporary output vector (before activation)
		m_OutputsM = m_WeightMatrices[i] * m_InputsM;

		// activation
		m_InputsM = FastSigmoidVec(m_OutputsM);
	}
	return m_InputsM;
}


// method to compute sigmoid-activation for a VectorXd
VectorXd CNeuralNet::FastSigmoidVec(const VectorXd &inputs)
{
//	const VectorXi IndexVec = (inputs / SampleSlice).cast<int>();
	VectorXd Activation(inputs.rows());

	for (int i = 0; i < inputs.rows(); ++i)
		Activation(i) = FastSigmoid(inputs(i));
	
	return Activation;
}
