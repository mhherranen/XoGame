#include "stdafx.h"

//the parameters that are loaded in the settings from an ini file
//are set to zero

int CParams::iDeep					= 0;
int	CParams::iNumInputs				= SQUARES_TOT;
int CParams::iNumHidden             = 0;
int CParams::iNeuronsPerHiddenLayer = 0;
int CParams::iNumOutputs			= SQUARES_TOT;
double CParams::dActivationResponse = 0;
double CParams::dBias               = 0;

//this function loads in the parameters from a given file name. Returns
//false if there is a problem opening the file.
bool CParams::LoadInParameters(char* szFileName)
{
  ifstream grab(szFileName);

  //check file exists
  if (!grab)
  {
    return false;
  }

  //load in from the file
  char ParamDescription[40];

  grab >> ParamDescription;
  grab >> iDeep;
  grab >> ParamDescription;
  grab >> iNumHidden;
  grab >> ParamDescription;
  grab >> iNeuronsPerHiddenLayer;
  grab >> ParamDescription;
  grab >> dActivationResponse;
  grab >> ParamDescription;
  grab >> dBias;

  // choose neural net number of outputs based on iDeep value
  if (CParams::iDeep)
	  CParams::iNumOutputs = 1;
    
  return true;
}
 




  
  
