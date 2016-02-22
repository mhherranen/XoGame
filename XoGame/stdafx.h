// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <Windowsx.h>


// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <strsafe.h> // only for ErrorExit() function


// TODO: reference additional headers your program requires here

#include <d2d1.h>
#include <array>
#include <vector>
#include <random>
#include <time.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <Eigen/Dense>

// project header files
#include "defines.h"
#include "main.h"
#include "Game.h"
#include "RenderGame.h"
#include "AIBrain.h"
#include "CNeuralNet.h"
#include "utils.h"
#include "CParams.h"