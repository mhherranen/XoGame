//------------------------------------------------------------------------
//
//	Name: CParams.h
//
//  Author: Mat Buckland 2002
//
//  Desc: Class to hold all the parameters used in this project. The values
//        are loaded in from an ini file when an instance of the class is
//        created.
//       
//
//------------------------------------------------------------------------

using namespace std;	



class CParams
{

public:

  //------------------------------------general parameters
//  static double dPi;
//  static double dHalfPi;
//  static double dTwoPi;

//  static int    WindowWidth;
//  static int    WindowHeight;

//  static int    iFramesPerSecond;
  
  //-------------------------------------used for the neural network
	static int	iDeep;			// 1 for deep evaluation, 0 for normal evaluation
	static int	iNumInputs;
	static int	iNumHidden;
	static int	iNeuronsPerHiddenLayer;
	static int	iNumOutputs;

  //for tweeking the sigmoid function
  static double dActivationResponse;
  //bias value
  static double dBias;

  //---------------------------------------GA parameters
//  static double dCrossoverRate;
//  static double dMutationRate;

  //the maximum amount the ga may mutate each weight by
//  static double dMaxPerturbation;

  //used for elitism
//  static int    iNumElite;
//  static int    iNumCopiesElite;

  //ctor
  CParams()
  {
    if(!LoadInParameters("params.ini"))
    {
      MessageBox(NULL, (LPCWSTR)L"Cannot find ini file!", (LPCWSTR)L"Error", 0);
    }
  }

  bool LoadInParameters(char* szFileName);
};