//------------------------------------------------------------------------
//
//	Name: CNeuralNet.h
//
//  Using partially the code by Mat Buckland from the book "AI Techniques
//  for game programming", Premier Press (2002), and 'Eigen' linear algebra
//  library
//
//  Desc: Class for creating a feedforward neural net.
//-------------------------------------------------------------------------

using namespace Eigen;

//typedef Eigen::Matrix<double, Dynamic, Dynamic, 0, SQUARES_TOT+1, SQUARES_TOT+1> WeightMatrix;
typedef Eigen::Matrix<double, Dynamic, Dynamic> WeightMatrix;

const double ActivationResponse = 1.0;


//-------------------------------Sigmoid function-------------------------
//
//------------------------------------------------------------------------
inline double Sigmoid(const double netinput, const double response)
{
	return ( 1 / ( 1 + exp(-netinput / response)));
}


//------------------------------------------------------------------------
//
//  The following code creates a lookup table to speed up the sigmoid 
//  function
//------------------------------------------------------------------------
const double SigmoidRange = 20.0;
const int    NumSamples   = 4000;
const double SampleSlice  = SigmoidRange*2/NumSamples;

extern double SigmoidTable[NumSamples];


static bool CreateLookupTable()
{
  int idx = 0;

  for (double d=-SigmoidRange; d<SigmoidRange; d+=SampleSlice)
  {
    SigmoidTable[idx++] = Sigmoid(d, ActivationResponse);
  }

  return true;
}

static bool Done = CreateLookupTable();

inline double FastSigmoid(const double val)
{
  if (val < -SigmoidRange)
  {
    return 0;
  }

  else if (val > SigmoidRange)
  {
    return 1.0;
  }

  else
  {
    //calculate the index into the sigmoid table
    int idx = (int)((val + SigmoidRange)/SampleSlice);

    return SigmoidTable[idx];
  }
}


//-------------------------------------------------------------------
//	define neuron struct
//-------------------------------------------------------------------
struct SNeuron
{
	//the number of inputs into the neuron
	int				      m_NumInputs;

	//the weights for each input
	std::vector<double>	m_vecWeight;


	//ctor
	SNeuron(int NumInputs);
};


//---------------------------------------------------------------------
//	struct to hold a layer of neurons.
//---------------------------------------------------------------------

struct SNeuronLayer
{
	//the number of neurons in this layer
	int					      m_NumNeurons;

	//the layer of neurons
	std::vector<SNeuron>		m_vecNeurons;

	SNeuronLayer(int NumNeurons, int NumInputsPerNeuron);
};


//----------------------------------------------------------------------
//	neural net class
//----------------------------------------------------------------------

class CNeuralNet
{

private:

	int					m_NumInputs;

	int					m_NumOutputs;

	int					m_NumHiddenLayers;

	int					m_NeuronsPerHiddenLyr;

	//storage for each layer of neurons including the output layer
	std::vector<SNeuronLayer>	m_vecLayers;

  //std::vectors used in the update function. When this class is instantiated
  //these std::vectors are allocated enough memory to store the max amount
  //of elements (m_iMaxBufferSize) required by the update function. This 
  //is simply the larger of the amounts m_NumInputs, m_NumOutputs or
  //m_NeuronsPerHiddenLyr
  std::vector<double> m_Inputs,
                 m_Outputs;

  int CalculateMaxBufferSize();

  // ADDITIONS:
  // weight matrices for each neuron layer
  std::vector<WeightMatrix> m_WeightMatrices;

  // vector for inputs and outputs
  VectorXd m_InputsM;
  VectorXd m_OutputsM;

public:

	CNeuralNet();

	void			      CreateNet();

	//gets the weights from the NN
	std::vector<double>	GetWeights()const;

	//returns total number of weights in net
	int				      GetNumberOfWeights()const;

	//replaces the weights with new ones
	void			      PutWeights(const std::vector<double> &weights);

	//calculates the outputs from a set of inputs
	std::vector<double>	Update(const std::vector<double> &inputs);
	std::vector<double>	FastUpdateOLD(std::array<int,SQUARES_TOT> *pGamePosition, int iTurn);

	//sigmoid response curve
	//inline double	  Sigmoid(const double activation, const double response);

	std::vector<int>     CalculateSplitPoints() const;

	// ADDITIONS:
	// set weight matrices from weights
	void				SetWeightMatrices();
	// fast update method using weight matrices and linear algebra
	VectorXd			FastUpdate(const VectorXd &inputs);
	// method to compute sigmoid-activation for a VectorXd
	VectorXd			FastSigmoidVec(const VectorXd &inputs);
	
};
				