#include ".\glc.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include "ranlib.h"
#include <algorithm>
using namespace std;

long g_seed = -129;

// the (constant) lookup table for leakage power;
double g_arrLookupTable[MAX_NUM_TYPES_GATES][MAX_NUM_INPUTS][MAX_WIDTH_LOOKUP_TABLE] = 
{
	{
		{37.84, 105.3, 95.17, 254.4},
// 		{37.84, 105.3, 95.17, 454.4}, // for NAND 2
// 		{22.84, 37.84, 37.84, 100.3, 37.01, 95.17, 94.87, 852.4},  // NAND_3
// 		{16.24, 22.84, 22.84, 37.84, 22.84, 37.84, 37.84, 100.3, 22.55, 37.01, 37.01, 95.17, 36.96, 94.87, 94.7, 1137.01}   // NAND_4
		{37.84, 105.3, 95.17, 254.4}, // for NAND 2
		{22.84, 37.84, 37.84, 103.3, 37.01, 95.17, 94.87, 302.4},  // NAND_3
		{16.24, 22.84, 22.84, 37.84, 22.84, 37.84, 37.84, 100.3, 22.55, 37.01, 37.01, 95.17, 36.96, 94.87, 94.7, 313.01}   // NAND_4

	},

	{
		{100.3, 227.2}, // for NOT_1
// 		{150.3, 327.2}, // for NOT_1

	},


	{
		{250.6, 227.2, 213, 92.05},
		{250.6, 227.2, 213, 92.05},  // NOR_2
		{300.9, 227.2, 213, 92.05, 212.5, 92.05, 89.57, 56.92}, // NOR_3
		{401.1, 227.2, 213, 92.02, 212.5, 92.02, 92.02, 56.92, 212.2, 92.02 , 89.57, 56.92, 89.47, 56.92, 55.97, 40.99}, // for NOR_4
	}, 

	{
		{138.14, 200.6, 195.47, 681.7},
		{138.14, 200.6, 195.47, 681.7},  // AND_2
		{250.04, 265.04, 265.04, 327.5, 264.21, 322.37 , 322.07, 952.7},  // AND_3
		{243.44, 250.04, 250.04, 265.04, 250.04, 265.04, 265.04, 327.5, 249.75, 264.21, 264.21, 322.37, 264.16, 322.07, 321.9, 1237.31}, // AND_4
		{87.54,84.25,71.42,62.57,27.57,77.9,58.82,66.6,2.91,31.17,36.36,96.34,3.34,30.61,26.65,87.14,63.33,1.37,18.56,29.46,90.19,35.29,45.44,46.76,93.15,16.02,72.15,46.83,91.59,6.93,94,37.55},// AND_5
		{0}, //AND_6
		{0}, //AND_7
		{86.4,44.87,56.47,90.93,39.13,11.79,76,85,58.52,70.08,87.81,81.02,1.21,74.14,5.99,55.21,33.34,91.23,81.65,45.22,18.73,49.46,21.78,6.37,12.76,68.66,58.9,14.93,15.03,14.96,49.97,84.07,83.89,69.58,32.41,8.2,66.07,52.54,15.56,1.17,44.44,91.85,35.53,44.14,57.05,78.86,51.74,8.55,90.62,100.34,13.09,69.94,1.54,87.96,72.1,78.27,67.53,75.77,74.91,100.24,23.14,6.31,5.45,99.33,82.06,76.01,11.83,69.16,99.62,60.39,93.05,85.81,78.27,9.91,74.23,94.7,69.53,56.78,48.4,97.62,75.26,23.65,13.05,55.7,1.4,74.69,79.49,73.68,67.59,99.55,83.27,81.35,25.32,71.12,73.57,77.63,88.18,67.41,9.97,53.05,27.09,98.38,44.42,56.04,33.67,28.31,16.82,18.93,4.91,34.18,42.1,9.08,52.32,90.98,17.09,20.8,88.48,24.22,25.23,73.78,94.4,20.6,67.21,33.56,20.61,37.24,95.77,19.69,25.11,100.14,82.72,17.2,1.45,32.63,96.26,57.08,99.92,4.19,16.83,81.23,95.04,40.5,69.57,57.69,2.49,85.25,1.43,61.64,98.52,54.25,81.22,87.87,45.07,72.64,50.59,10.33,73.7,35.65,4.1,54.32,31.36,83.72,26.59,97.1,41.27,32.41,65.77,62.32,81.93,47.83,68.44,24.1,37.41,8.28,58.18,86.35,22.09,42.54,82.87,52.75,32.92,85.62,9.98,53.37,32.31,42.49,97.87,43.21,14.87,100.45,21.25,38.51,61.05,80.76,78.71,6.35,30.76,70.1,60.8,73.23,100.23,22.64,6.43,68.17,95.12,73.18,25.93,66.07,23.06,52.01,18.86,46.8,58.7,42.26,64.65,35.84,84.46,70.14,14.18,41.91,40.06,68.89,60.51,57.31,83.94,7,16.85,36.66,19.75,21.21,79.82,19.43,90.98,45.14,6.22,12.2,96.92,74,81.43,96.01,32.96,55.99,7.49,74.58,8.5,51.26,89.02,28.96,45.5,48.66,45.21,94.23,10.8,81.32,25.81,23.95}, //AND_8,
		{85.66,84.76,67.68,83.27,78.31,13.03,63.61,55,79.32,77.26,21.66,70.94,34.03,86.98,26.61,86.83,95.19,74.03,62.63,60.85,5.93,6.16,85.16,100.86,70.41,37.51,81.76,43.95,27.82,49.19,63.1,46.26,95.33,82.94,74.32,11.33,95.55,21.15,67.51,35.05,34.26,16.26,26.04,95.28,43.91,22.69,81.22,64.39,58.63,34.47,11.93,72.75,36.17,81.63,67.96,1.32,92.72,88.54,7.57,31.43,65.94,47.19,89.9,30.02,71.89,8.8,12.98,98.21,71.68,46.55,37.74,61.59,13.19,92.85,88.94,30.2,63.65,4.04,73.3,6.32,98.67,55.88,5.8,68.3,63.35,22.74,84.55,8.44,71.03,43.67,4.84,53.15,55.46,28.65,53.24,43.4,41.81,7.87,72.06,3.69,52.33,77.34,34.99,77.62,12.14,43.96,24.53,58.15,70.9,23.06,51.79,90.77,5.88,91.64,9.23,77.58,22.16,50.39,97.14,60.53,15.41,97.66,14.33,11.86,13.12,68.39,53.65,9.86,59.73,41.29,93.1,28.2,61.26,67.25,23.03,16.75,55.24,22.17,100.14,2.21,21.44,37.39,68.76,86.51,98.94,10.68,40.29,71.33,25.42,51.14,71.11,86.52,64.06,60.88,29.91,50.93,33.77,6.88,94.75,43.49,8.36,85.86,70.74,12.78,100.33,35.64,68.8,3.24,94.78,95.66,36.19,26.06,49.18,70.29,79.43,52.96,24.63,97.63,70.2,60.9,64.07,77.16,74.93,5.97,83.33,59.34,98.57,54.39,90.37,50.17,83.31,49,98.59,33.99,52.76,39.09,49.57,67.49,42.32,51.94,11.14,48.25,6.24,34.03,66.17,10.66,42.1,40.12,57.89,52.69,23.31,98.27,7.18,46.25,80.21,29.05,67.66,34.66,20.44,46.87,34.04,70,29.21,21.72,69.4,93.7,40.08,29.42,67.1,13.42,16.78,68.35,12.84,79.36,36.98,71.07,62.57,13.02,18.24,88.64,1.77,91.71,77.08,76.04,1.94,41.84,61.68,56.57,39.65,44.09,48.76,93.61,93.99,3.67,45.48,47.62,87.9,88.64,86.7,46.68,89.18,89.67,68.47,48.59,36.54,4.08,8.23,22.74,6.65,97.5,72.42,29.82,87.45,46.15,62.11,27.43,73.67,12.96,97.19,79.97,50,60.52,89.91,100.52,77.56,54.15,48.25,3.29,49.33,58.11,99.8,94.42,30.17,78.09,76.8,79.4,47,53.08,52.9,15.26,44.84,49.87,72.34,85.53,74.22,63.12,62.1,26.31,25.81,74.29,67.72,81.29,77.55,72.42,3.38,14.09,70.24,90.01,56.77,55.93,77.06,57.01,39.65,16.8,82.6,26.16,24.32,38.16,37.12,72.68,57.07,15.42,1.66,21.52,88.82,68.31,49.53,5.34,13.9,87.14,39.93,6.14,57.39,33.22,29.24,83.95,1.79,21.45,35.62,45.46,93.44,63.39,22.57,55.88,19.64,98.61,17.08,47.01,96.7,92.84,27.91,24.81,59.91,30.68,79.78,29.48,95.81,81.31,28.91,51.14,7.47,65.04,75.72,54.42,53.43,99.06,40.17,14.84,50.89,50.38,15.43,20.33,83.78,74.12,65.49,40.86,6.31,65.27,82.38,19.36,82.73,2.08,68.53,87.05,53.74,9.12,59.98,56.38,70.4,18.3,56.04,94.5,85.91,55.72,71,30.68,46.19,32.75,73.54,100.47,69.56,69.25,29.51,80.08,56.41,27.22,13.54,13.75,19.32,82.82,5.37,87.57,74.33,15.11,36.24,52.58,84,39.43,1.18,50.48,92.77,63.68,2.73,45.02,45.43,95.54,41.71,12.09,29.15,78.84,20.69,91.85,29.79,87.08,75.84,55.77,43.1,52.39,2.6,91.95,31.44,57.91,91.06,61.63,66.55,71.8,36.44,30.21,49.23,50.92,40.42,25.69,46.26,92.47,68.9,27.91,35.37,61.48,81.91,41.46,98.91,72.51,1.07,55.59,56.7,53.48,30.51,44.11,78.75,69.97,96.65,8.48,68.57,37.85,22.58,17.16,57.55,2.65,2.65,44.38,1.13,13.55,92.84,61.51,17.31,70.33,82.61,84.01,58.2,71.14,63.81,27.19,55.22,39.56,62.27,73.95,28.54,58.73,47.76,7.45,1.35,90.92} //AND_9,

	},

	{
		{0},
// 		{687.81, 750.27, 745.14, 1104.47},  // XOR_2
		{68.781, 75.027, 74.514, 110.447}
	},
	
	{
		{128.14, 192.6, 235.1, 612.3},
		{138.14, 200.6, 195.47, 681.7},     // OR_2
		{528.1 , 327.5, 313.3, 192.35, 312.8, 192.35, 189.87, 157.22}, // OR_3
		{628.3, 327.5, 313.3, 192.32, 312.8, 192.32, 192.32, 157.22, 312.5, 192.35, 189.87, 157.22, 189.77, 157.22, 156.27, 141.29} // for OR_4

	},

	{
		{27.5, 54.5}  // BUF_1
	}

};

double g_arrSWDelayTable[MAX_NUM_TYPES_GATES][MAX_NUM_INPUTS]= 
{
	{0, 0.05, 0.07, 0.10}, // for NAND1,2,3,4
	{0.02, 0, 0, 0}, // for NOT_1
	{0, 0.06, 0.16, 0.23}, // for NOR1,2,3,4
	{0, 0.15, 0.20, 0.28, 0.33, 0.35, 0.38, 0.41,0.45}, // for AND1,2,3,4,5
	{0, 0.30}, // for XOR1,2
	{0, 0.12, 0.12, 0.13},  // for OR 1,2,3,4
	{0.03} // for BUFF
	
};

double g_selectPI[NUM_BENCHMARK][64]=
{
	{0},// 	C17 = 0,
	{1, 4, 8, 11, 14, 17, 21, 24},   // 	C432,
	{0},// 	C499,
	// 	C880,
	{1, 59, 26, 51, 42, 17, 8, 75, 156, 55, 29, 80, 268, 126, 153, 121, 201, 149, 195, 116, 138, 146, 189, 111, 261, 143, 106, 183, 177, 101, 13, 171, 152, 96, 68, 165, 72, 73, 210, 219},
// 	C1355,
// 	C1908, 
// 	C2670, 
// 	C3540,
// 	C5315,
// 	C6288,
// 	C7552,
};


/*
= {1.36724, 1.67106, 2.1543, 0.978719, 2.04993, 1.12298, 1.40362, 1.84068, 1.17976, 1.43171, 1.49361, 1.59066, 1.18405, 0.641351, 0.840514, 0.9382, 1.49599, 1.03574, 
1.05994, 1.19333, 1.06293, 1.68979, 1.17913, 1.18492, 1.28426, 0.812867, 1.22862, 1.54286, 1.36511, 1.36824, 1.2127, 0.720858, 0.961069, 0.546808, 0.83059, 1.37597, 
1.07917, 1.08638, 1.58014, 1.36077, 2.02526, 0.462764, 1.39548, 0.748618, 0.570457, 0.962309, 1.82532, 1.26891, 0.851868, 1.44724, 1.30281, 0.889645, 1.36206, 1.28328, 
1.11171, 1.88107, 1.46855, 1.6431, 1.52313, 1.38837, 1.51577, 1.35786, 1.13893, 0.305244, 0.907629, 1.11407, 0.856321, 0.899549, 1.13795, 1.24361, 0.890199, 1.0096, 
1.90696, 1.99863, 1.92478, 2.15348, 1.19836, 0.874313, 1.43332, 1.14333, 2.09306, 1.61615, 0.778897, 1.42353, 1.36422, 1.5726, 0.932142, 0.643636, 1.0745, 0.730723, 
1.83496, 2.07987, 1.4546, 0.988607, 0.961843, 2.05501, 1.5697, 1.14116, 0.832443, 0.969867, 1.2228, 1.19721, 1.23275, 1.40644, 1.16612, 0.406428, 1.36383, 1.2833, 
1.87577, 1.33141, 1.49052, 0.982361, 2.07417, 1.75332, 0.940294, 1.36921, 1.0618, 1.90391, 1.47841, 1.06095, 1.14188, 1.39995, 1.49442, 0.83314, 0.358155, 0.932298, 
2.1181, 1.82332, 1.00861, 1.14896, 2.04635, 2.22405, 1.76951, 1.12269, 1.56132, 1.52715, 1.22085, 1.26226, 0.922691, 1.34364, 0.987888, 0.96468, 0.356736, 1.05384, 
1.35748, 2.07398, 0.781318, 0.657164, 1.35318, 1.20483, 0.642575, 1.04761, 1.40033, 0.989733, 1.44314, 1.36618, 1.76444, 0.882443, 1.10815, 0.802914, 0.632621, 1.10967, 
};*/


char g_arrBenchmarkName[NUM_BENCHMARK][32] = {"C17", "C432", "C499", "C880", "C1355", "C1908", "C2670", "C3540", "C5315", "C6288", "Saro", "ADDER_CR", /*"Saro12", "Saro13", "Saro21", "Saro31",*/ "C7552", "S17", "S526", "S641", "S713", "S820", "S832"};
char g_arrErrorModelName[NUM_ERRORMODEL][32] = {"Uniform", "Gaussian", "Triangular"};
char g_arrPIStrategyName[NUM_PISTRATEGY][32] = {"withoutS", "withS"};
char g_arrNormName[NUM_NORM][32] = {"SUM", "L1", "MLE"};

// for saro [12/19/2009 shengwei]
const int NUM_SELECT_GATES = MAX_NUM_GATES;
int g_nSelGates = 10;
double g_arrSelectedGates[NUM_SELECT_GATES] = {427, 467, 698, 976, 987, 1148, 1429, 1430, 1439, 1442};

GLC::GLC(int iBenchmark, double dErrorRate, int nInstances, int iPIStrategy, int iNorm, int seedIndex)
{

	// seeds for random number generator
	m_seed[0] = -seedIndex*7-1;
	m_seed[1] = -seedIndex*7-2;  // for setting primary inputs
	m_seed[2] = -seedIndex*7-3;  // for setting primary inputs in segmentation
	m_seed[3] = -seedIndex*7-4;  // for HTH position
	m_seed[4] = -seedIndex*7-5;
	m_seed[5] = -seedIndex*7-6;  // for generate exp distribution
	m_seed[6] = -seedIndex*7-7;  // for generate exp distribution

	// 	m_fixedSeed = -166;


	m_iNorm = iNorm;
	m_iErrorModel = ERROR_EXP;
	m_iSegStrategy = SEG_OUTPUT_CONE;

	m_bFilterErrors = false;

	for(int i = 0; i < nInstances/2; i++)
	{
		double rnd = ranExp3();

		if(m_bFilterErrors)
		{
			if(rnd < 0.001)
			{
				m_vecExpRnd.push_back(rnd);
			}
		}
		else
		{
			m_vecExpRnd.push_back(rnd);
		}

	}

	int size = m_vecExpRnd.size();
	for(int i = 0; i < size; i++)
	{
		m_vecExpRnd.push_back(-m_vecExpRnd[i]);
	}
	

  	m_nInputInstance = m_vecExpRnd.size();
// 	m_nInputInstance = nInstances;
	m_iBenchmark = iBenchmark;
	m_dErrorRate = dErrorRate;
	m_iPIStrategy = iPIStrategy;
  	m_iLPSolver = SOLVER_LP_SOLVE;
// 	m_iLPSolver = SOLVER_CPLEX;
// 	m_iMetric = METRIC_THERMAL_LEAKAGE;
	m_iMetric = METRIC_LEAKAGE_POWER;
// 	m_iMetric = METRIC_SWITCH_POWER;

	m_bHTH = false;
	m_bReallyHaveHTH = false;
	m_bDiagnosis = false;

	m_iHTHInput = 1; // input pin for HTH (consider inverter, only 1 input)
	m_iGuessHTHInput = 2; // guessed input for HTH when doing diagnosis
	m_iGuessHTHType = NOT; // guess type [2/18/2010 shengwei]

	m_bLocationHTH = false;
	m_iHTHLocation = 96344;   // HTH location [2/18/2010 shengwei]
	m_iGuessHTHLocation = 97517;  // HTH location [2/18/2010 shengwei]

	m_bRandomHTH = false;
	m_nEqnsPerSignal = 1;  //  [6/30/2010 shengwei]
	if(m_bRandomHTH)
	{
		m_nEqnsPerSignal = 5;
	}


 	m_bSmallOut = false;


	m_bPostProcessing = false;  // whether to do post-processing or not

	// for saro [12/19/2009 shengwei]
	// for segmentation [3/21/2010 shengwei]
	m_bSelectGates = false;


	// PV model [10/25/2010 shengwei]
	m_iPVModel = PVMODEL_QUADTREE;

	char strErrorRate[32];
	int iErrorRate = (int)(dErrorRate*100);
	_itoa(iErrorRate, strErrorRate, 10);

	char strNumInstances[32];
	_itoa(m_nInputInstance, strNumInstances, 10);



	// output stream
	if(PI_RANDOM == iPIStrategy || PI_READ == iPIStrategy)
	{
		char strOutputLP[512];

// 		switch(m_iLPSolver)
// 		{
// 		case SOLVER_LP_SOLVE:
			sprintf(strOutputLP, "outputGLC_%s_%s_%s_%s_%s_%s.lp", 
				getBenchmarkName(m_iBenchmark).c_str(), strErrorRate, strNumInstances, 
				getErrorModelName(m_iErrorModel).c_str(), getPIStrategyName(m_iPIStrategy).c_str(), getNormName(m_iNorm).c_str());
// 			break;
// 		case SOLVER_CPLEX:
// 			sprintf(strOutputLP, /*"C:\\Workspace\\Gate_LP\\src\\GLC\\CPLEX\\outputGLC_%s_%s_%s_%s_%s_%s.mod"*/ "C:\\Workspace\\Gate_LP\\src\\GLC\\CPLEX\\GLC.mod", 
// 				getBenchmarkName(m_iBenchmark).c_str(), strErrorRate, strNumInstances, 
// 				getErrorModelName(m_iErrorModel).c_str(), getPIStrategyName(m_iPIStrategy).c_str(), getNormName(m_iNorm).c_str());
		
// 			break;
// 		}
// 		m_fLPout.open(strOutputLP);
// 		if(!m_fLPout)
// 		{
// 			cout << "open *.lp failed" << endl;
// 		}
// 		m_fLPout << "///" << "Benchmark: " << m_iBenchmark << endl; 
// 		m_fLPout << "///" << "Norm: " << m_iNorm << endl; 
// 		m_fLPout << "///" << "Error Model: " << m_iErrorModel << endl; 
// 		m_fLPout << "///" << "Error Rate:: " << m_dErrorRate << endl; 
	}
	else if(PI_SEGMENTATION == iPIStrategy)
	{
// 		for(int i = 0; i < MAX_NUM_SEG; i++)
// 		{
// // 			char strSegOutputLP[512];
// // 			sprintf(strSegOutputLP, "outputGLC_%s_%s_%s_%s_%s_Seg%d.lp", 
// // 				getBenchmarkName(m_iBenchmark).c_str(), strErrorRate, strNumInstances, 
// // 				getErrorModelName(m_iErrorModel).c_str(), getPIStrategyName(m_iPIStrategy).c_str(), i+1);
// // 			m_fSegLPout[i].open(strSegOutputLP);
// 		}

	}

	char strOutputRealSize[512];
	sprintf(strOutputRealSize, "RealSize_%s_%s_%s_%s_%s_%s_seed%d.xls", 
		getBenchmarkName(m_iBenchmark).c_str(), strErrorRate, strNumInstances, 
		getErrorModelName(m_iErrorModel).c_str(), getPIStrategyName(m_iPIStrategy).c_str(), getNormName(m_iNorm).c_str(), seedIndex);
	m_fXLSout.open(strOutputRealSize/*, fstream::in | fstream::out | fstream::app*/);
	if(!m_fXLSout)
	{
		cout << "open *.xls failed" << endl;
	}

	m_fTimerOut.open("timer.txt");
	if(!m_fTimerOut)
	{
		cout << "open timer.txt failed" << endl;
	}

	m_fDebugOut.open("debug.txt");
	if(!m_fDebugOut)
	{
		cout << "open debug.txt failed" << endl;
	}

	m_fCorrOut.open("CorrGates.txt");
	if(!m_fCorrOut)
	{
		cout << "open CorrGates.txt failed" << endl;
	}

	// for saro [12/17/2009 shengwei]
	m_heatout.open("heat-saro.xls");
	if(!m_heatout)
	{
		cout << "open heat-saro.xls failed" << endl;
	}

	m_fSWMapOut.open("Tempr.xls");
	if(!m_fSWMapOut)
	{
		cout << "open Tempr.xls" << endl;
	}



// 	m_fCnstout.open("Constraints.lp"); // for split OF/Constraint
// 	if(!m_fCnstout)
// 	{
// 		cout << "open Constraints.lp failed" << endl;
// 	}
// 
// 
// 	m_fOFout.open("OF.lp");   // for split OF/Constraint
// 	if(!m_fOFout)
// 	{
// 		cout << "open OF.lp failed" << endl;
// 	}
// 
// 	m_fCnstout.setf(ios::fixed);//floating-point   numbers   are   not   writen   in   e-notation   
// 	m_fCnstout.setf(ios::showpoint);//a   decimal   and   trailing   zeros   are   always   shown   
// 	m_fCnstout.precision(12);
// 
// 	m_fOFout.setf(ios::fixed);//floating-point   numbers   are   not   writen   in   e-notation   
// 	m_fOFout.setf(ios::showpoint);//a   decimal   and   trailing   zeros   are   always   shown   
// 	m_fOFout.precision(12);
// 
// 	m_fLPout.setf(ios::fixed);//floating-point   numbers   are   not   writen   in   e-notation   
// 	m_fLPout.setf(ios::showpoint);//a   decimal   and   trailing   zeros   are   always   shown   
// 	m_fLPout.precision(12);
// 
	m_fSegLPout[15].setf(ios::fixed);//floating-point   numbers   are   not   writen   in   e-notation   
	m_fSegLPout[15].setf(ios::showpoint);//a   decimal   and   trailing   zeros   are   always   shown   
	m_fSegLPout[15].precision(12);


	// FOR CPLEX
	m_fCPLEXModOut.open("C:\\Workspace\\Gate_LP\\src\\GLC\\CPLEX\\GLC.mod");
	if(!m_fCPLEXModOut)
	{
		cout << "open CPLEX\\GLC.mod failed" << endl;
	}

	m_fCPLEXDataOut.open("C:\\Workspace\\Gate_LP\\src\\GLC\\CPLEX\\GLC.dat");
	if(!m_fCPLEXDataOut)
	{
		cout << "open CPLEX\\GLC.dat failed" << endl;
	}


	//  [1/18/2010 shengwei]
// 	m_fSWMapOut.open("switchMap.txt");
// 	if(!m_fSWMapOut)
// 	{
// 		cout << "open switchMap.txt failed" << endl;
// 	}


	// open matlab engine
// 	m_pEngine = engOpen(NULL);
// 	if(NULL == m_pEngine)
// 	{
// 		cout << "open matlab Engine failed" << endl;
// 	}

	// set PL matlab Engine
//	PL_SetMatEngine(m_pEngine);

// 	setState(time(NULL));

}

GLC::~GLC(void)
{
	m_fXLSout.close();
	m_fTimerOut.close();
	m_fDebugOut.close();
// 	m_fCorrOut.close();

	m_fCnstout.close();
	m_fOFout.close();

	m_fOFout.close();


// 	engClose(m_pEngine);

}

void GLC::readBenchmark()
{
	ifstream fin;

	switch(m_iBenchmark)
	{
	case C17:

		fin.open("benchmark/c17.txt");
		break;

	case C432:
		fin.open("benchmark/c432.txt");

		break;

	case C499:
		fin.open("benchmark/c499.txt");

		break;

	case C880:
		fin.open("benchmark/c880.txt");

		break;

	case C1355:
		fin.open("benchmark/c1355.txt");

		break;

	case C1908:

		fin.open("benchmark/c1908.txt");
		break;

	case C2670:
		fin.open("benchmark/c2670.txt");
		break;

	case C3540:
		fin.open("benchmark/c3540.txt");
		break;

	case C5315:
		fin.open("benchmark/c5315.txt");
		break;

	case C6288:
		fin.open("benchmark/c6288.txt");
		break;

	case C7552:
		fin.open("benchmark/c7552.txt");
		break;
	
	case S17:
		fin.open("benchmark/s17.txt");
		break;

	case S526:
		fin.open("benchmark/s526.txt");
		break;

	case S641:
		fin.open("benchmark/s641.txt");
		break;

	case S713:
		fin.open("benchmark/s713.txt");
		break;

	case S820:
		fin.open("benchmark/s820.txt");
		break;

	case S832:
		fin.open("benchmark/s832.txt");
		break;

	case S5378:
		fin.open("benchmark/s5378.txt");
		break;

	case Saro:
		fin.open("benchmark/saro.txt");
		break;

	case ADDER_CR:
		fin.open("benchmark/adder_cr.txt");
		break;

	case S13207:
		fin.open("benchmark/s13207.txt");
		break;

	case S15850:
		fin.open("benchmark/s15850.txt");
		break;

	case S38584:
		fin.open("benchmark/s38584.txt");
		break;

	default:

		break;

	}

	char buf[MAX_WIDTH_BENCHMARK];
	int iGateId = 0;

	while(!fin.eof())
	{		
		fin.getline(buf, MAX_WIDTH_BENCHMARK);

		string strBuf = buf;

		if(0 == strBuf.compare(0, 5, "INPUT"))
		{
			// INPUT
			int startPos = 0;
			int endPos = 0;
			if(m_iBenchmark < C7552)
			{
				startPos = strBuf.find_first_of('(')+1;
				endPos = strBuf.find_first_of(')')-1;
			}
			else
			{
				// s benchmark
				startPos = strBuf.find_first_of(' ')+1;
				endPos = strBuf.find_first_of('\0')-1;
			}
			string strPrimaryInput = strBuf.substr(startPos, endPos-startPos+1);
			int input = atoi(strPrimaryInput.c_str());

			m_mapPrimaryInputSignal.insert(make_pair(input, 0));  // init set signal to 1
																  // change to 0 [3/21/2010 shengwei]

			m_vecPrimaryInput.push_back(input);  // fill the PI vector [9/22/2009 shengwei]
		}
		else if(0 == strBuf.compare(0, 6, "OUTPUT"))
		{
			// OUTPUT
			int startPos = 0;
			int endPos = 0;
			if(m_iBenchmark < C7552)
			{
				startPos = strBuf.find_first_of('(')+1;
				endPos = strBuf.find_first_of(')')-1;
			}
			else
			{
				// s benchmark
				startPos = strBuf.find_first_of(' ')+1;
				endPos = strBuf.find_first_of('\0')-1;
			}
			string strPrimaryOutput = strBuf.substr(startPos, endPos-startPos+1);
			int output= atoi(strPrimaryOutput.c_str());

			m_vecPrimaryOutput.push_back(output);
		}
		else if(isdigit(strBuf[0]))
		{
			// GATE
			// 1. output
			int startPosOutput = 0;
			int endPosOutput = strBuf.find_first_of(' ') - 1;
			string strOutput = strBuf.substr(startPosOutput, endPosOutput-startPosOutput+1);
			int iOutput = atoi(strOutput.c_str());

			GateNode gateNode;

			// 2. gate type
			int startPosGateType = strBuf.find_first_of('=')+2;
			int endPosGateType = 0;
			if(m_iBenchmark <= C7552)
			{
				endPosGateType = strBuf.find_first_of('(')-1;
			}
			else
			{
// 				string typeStartSubStr = strBuf.substr(startPosGateType, strBuf.length()-startPosGateType-1);
				endPosGateType = strBuf.find_first_of(' ', startPosGateType)-1;
			}

			string strGateType = strBuf.substr(startPosGateType, endPosGateType-startPosGateType+1);
			if(0 == strGateType.compare("NAND"))
			{
				gateNode.gateType = NAND;
			}
			else if(0 == strGateType.compare("NOT"))
			{
				gateNode.gateType = NOT;
			}
			else if(0 == strGateType.compare("NOR"))
			{
				gateNode.gateType = NOR;
			}
			else if(0 == strGateType.compare("AND"))
			{
				gateNode.gateType = AND;
			}
			else if(0 == strGateType.compare("XOR"))
			{
				gateNode.gateType = XOR;
			}
			else if(0 == strGateType.compare("OR"))
			{
				gateNode.gateType = OR;
			}
			else if(0 == strGateType.compare("BUFF"))
			{
				gateNode.gateType = BUFF;
			}
			else if(0 == strGateType.compare("DFF"))
			{
				gateNode.gateType = BUFF;
			}

			// 3. input vector
			int startPosInput = 0;
			int endPosInput = 0; 
			if(m_iBenchmark <= C7552)
			{
				startPosInput = strBuf.find_first_of('(')+1;
				endPosInput = strBuf.find_first_of(',', startPosInput)-1;
			}
			else
			{
				startPosInput = endPosGateType + 2;
				endPosInput = strBuf.find_first_of(' ', startPosInput)-1;
			}

			while(string::npos != endPosInput+1)
			{
				string strInput = strBuf.substr(startPosInput, endPosInput-startPosInput+1);
				int input = atoi(strInput.c_str());
				gateNode.mapInputSignal.insert(make_pair(input, -1));  // init set signal to be -1  

				startPosInput = endPosInput + 2;
				if(m_iBenchmark <= C7552)
				{
					endPosInput = strBuf.find_first_of(',', startPosInput)-1;
				}
				else
				{
					endPosInput = strBuf.find_first_of(' ', startPosInput)-1;
				}
			}
			
			if(m_iBenchmark <= C7552)
			{
				endPosInput = strBuf.find_first_of(')', startPosInput)-1;
				string strInput = strBuf.substr(startPosInput, endPosInput-startPosInput+1);
				int input = atoi(strInput.c_str());
				gateNode.mapInputSignal.insert(make_pair(input, -1)); // init set signal to be -1

			}
			else
			{
				if (startPosInput <= strBuf.length()-1)
				{
					// read the last input
					string strInput = strBuf.substr(startPosInput, strBuf.length()-startPosInput);
					int input = atoi(strInput.c_str());
					gateNode.mapInputSignal.insert(make_pair(input, -1)); // init set signal to be -1
				}
			}
			
			// 4. gateId
			gateNode.gateId = ++iGateId;
			gateNode.location = gateNode.gateId; //  [2/18/2010 shengwei]

			// 5. bSwitch (init to false)
			gateNode.bSwitch = false;

			// 6. output signal
			gateNode.iOutputSignal = 0;

			// 7. temperature, for thermal conditioning
			gateNode.dTempr = 0;

			// 8. delay
			gateNode.delay = 0;

 			m_mapGlobalCircuit.insert(make_pair(iOutput, gateNode));

		} // else if(isdigit(strBuf[0]))

	}  // while(!fin.eof())

	m_numPins = m_vecPrimaryOutput[m_vecPrimaryOutput.size()-1];

	m_nGates = m_mapGlobalCircuit.size();

	// for sequential benchmark, consider FFs as inputs and outputs
	if(m_iBenchmark > C7552)
	{
		map<int, GateNode>::iterator mapGCIter;
		for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
		{
			if(BUFF == mapGCIter->second.gateType)
			{
				// add PI
				map<int,int>::iterator mapISIter = mapGCIter->second.mapInputSignal.begin();
				int input = mapISIter->first;
				m_mapPrimaryInputSignal.insert(make_pair(input, 1));

				m_vecPrimaryInput.push_back(input);  //  [4/5/2010 shengwei]

				// add PO
				m_vecPrimaryOutput.push_back(mapGCIter->first);
			}
		}

	}

	

	initMLE(calcRealPower());

	//  [11/5/2009 shengwei]
	m_nEncodeBits = log((double)m_nGates) / log(double(2)) + 1;    // # of encoded bits of gates
	m_nEqPerThermal = 16/*m_nInputInstance / m_nEncodeBits*/;  // # of equations per thermal condition


	fin.close();

}

void GLC::greedyPlacement()
{

}

// search all the gates on the path from specific output
void GLC::listAllGatesOnPath(int iOutput, set<int> & setGates)
{
	setGates.clear();

	map<int, GateNode>::iterator mapIter;
	mapIter = m_mapGlobalCircuit.find(iOutput);
	if(m_mapGlobalCircuit.end() != mapIter)
	{
		// start search on the path
		map<int, int> mapInputSignal = mapIter->second.mapInputSignal;
		map<int, int>::iterator mapIter;

		// add this gate (the lowest level gate)
		setGates.insert(iOutput);

		// add all the upper level gates on the path
		for(mapIter = mapInputSignal.begin(); mapIter != mapInputSignal.end(); mapIter++)
		{
			set<int> setTempGates;
			set<int>::iterator setTempIter;

			// each input is also an output for the upper level gate
			listAllGatesOnPath(mapIter->first, setTempGates);

			for(setTempIter = setTempGates.begin(); setTempIter != setTempGates.end(); setTempIter++)
			{
				setGates.insert(*setTempIter);
			}

		}
	}
}

// set Primary Input Signal
void GLC::setPrimaryInputSignal()
{
	ifstream fPISin;

	int count = 0;

	vector<int> vecSelPI;


	switch(m_iPIStrategy)
	{
	case PI_RANDOM:
		// exhaustive search


		while(count < m_nInputInstance/* && count < pow(2.0, (double)m_mapPrimaryInputSignal.size())*/)
		{
			map<int, int>::iterator mapPISIter;
			int i = 0;
			for(mapPISIter = m_mapPrimaryInputSignal.begin(); mapPISIter != m_mapPrimaryInputSignal.end(); mapPISIter++)
			{
				// right shift i bits and get the last bit, to feed the PI
				mapPISIter->second = (count >> i) & 1;
				i++;
			}

			m_vecMapPISignal.push_back(m_mapPrimaryInputSignal);


			count++;
		}
/*
				// select 8 inputs from the primary inputs
				cout << "Selected PI: "; 
				for(int i = 0; i < 8; i++)
				{
					int rndIndex = ran1(&m_seed[1])*m_vecPrimaryInput.size();
					vecSelPI.push_back(m_vecPrimaryInput[rndIndex]);
					cout << m_vecPrimaryInput[rndIndex] << ", ";
				}
				cout << endl;

				for(int i = 0; i < m_nInputInstance; i++)
				{
					map<int, int>::iterator mapPISIter;

					vector<int>::iterator vecIterSPI;
					for(vecIterSPI = vecSelPI.begin(); vecIterSPI != vecSelPI.end(); vecIterSPI++)
					{
						int rnd = (int)floor(ran1(&m_seed[1])+0.5);

						mapPISIter = m_mapPrimaryInputSignal.find(*vecIterSPI);
						mapPISIter->second = rnd;

					}

					m_vecMapPISignal.push_back(m_mapPrimaryInputSignal);
				}
*/

/*
				// set m_nInputInstance+1 PIs, for switch power metric
				for(int i = 0; i < m_nInputInstance; i++)
				{
					map<int, int>::iterator mapPISIter;
					for(mapPISIter = m_mapPrimaryInputSignal.begin(); mapPISIter != m_mapPrimaryInputSignal.end(); mapPISIter++)
					{
						// select PIs (Freeze some PIs)
// 						if(!isSelectedPI(mapPISIter->first))
// 						{
// 							continue;
// 						}
						int rnd = (int)floor(ran1(&m_seed[1])+0.5);
						mapPISIter->second = rnd;
  						cout << rnd << ", ";
					}
  					cout << endl;
		
					m_vecMapPISignal.push_back(m_mapPrimaryInputSignal);
		
				}*/



		break;

	case PI_READ:

		fPISin.open("benchmark\\readPI.txt");

		if(!fPISin)
		{
			cout << "open benchmark\\readPI.txt failed" << endl;
		}

		while(!fPISin.eof())
		{
			char buf[1024];
			fPISin.getline(buf, 1024);
			int numPI = m_mapPrimaryInputSignal.size();
			int i = 0;
			map<int, int>::iterator mapPISIter;
			for(mapPISIter = m_mapPrimaryInputSignal.begin(); mapPISIter != m_mapPrimaryInputSignal.end(); mapPISIter++)
			{
				mapPISIter->second = buf[i++] - '0';
			}
			m_vecMapPISignal.push_back(m_mapPrimaryInputSignal);

		}

		m_nInputInstance = m_vecMapPISignal.size();

		break;

	case PI_SEGMENTATION:

		vector<int>::iterator vecPOIter;

		// for each primary output (each segment)
		for(vecPOIter = m_vecPrimaryOutput.begin(); vecPOIter != m_vecPrimaryOutput.end(); vecPOIter++)
		{
			// each output cone is a segment
			// find the corresponding primary inputs
			set<int> setPI;
			set<int>::iterator setPIIter;
			findPrimaryInputs(*vecPOIter, setPI);
						
// 			cout << setPI.size() << "  ";

			for(int i = 0; i < m_nInputInstance; i++)
			{
				int j = 0;
				// for each PI
				for(setPIIter = setPI.begin(); setPIIter != setPI.end(); setPIIter++)
				{

					// random choose change or not
					int rnd = (int)(ran1(&m_seed[2])+0.5);

					// change the value in m_mapPrimaryInputSignal
					map<int, int>::iterator mapPISIter;
					mapPISIter = m_mapPrimaryInputSignal.find(*setPIIter);
	/*				if(m_mapPrimaryInputSignal.end() != mapPISIter)
					{
						mapPISIter->second = rnd;

					}*/

					// right shift i bits and get the last bit, to feed the PI
					mapPISIter->second = (i >> j) & 1;
					j++;

				}

				m_vecMapPISignal.push_back(m_mapPrimaryInputSignal);
			}

		}



		break;


	}

	//m_mapPrimaryInputSignal

}

void GLC::setSegPrimaryInputSignal(int segIter)
{
	m_vecMapPISignal.clear();

	vector<int>::iterator vecPOIter;

	// for each primary output (each segment)
	// 	for(vecPOIter = m_vecPrimaryOutput.begin(); vecPOIter != m_vecPrimaryOutput.end(); vecPOIter++)
	// 	{
	// each output cone is a segment
	// find the corresponding primary inputs
	set<int> setPI;
	set<int>::iterator setPIIter;
// 	findPrimaryInputs(m_vecPrimaryOutput[segIter], setPI);
	// tree circuit segmentation [9/26/2009 shengwei]
	findPrimaryInputs(m_vecSegOutput[segIter], setPI);

//   	cout << setPI.size() << ", ";
// 
 	set<int> setGatesInSegment;
 	set<int>::iterator setGISIter;
 	listGatesInSegment(m_vecSegOutput[segIter], setGatesInSegment);
	m_nGatesInSeg[segIter] = setGatesInSegment.size();
 	
//   	cout << m_nGatesInSeg[segIter] << endl;
 
// 	// gate type
// 	for(setGISIter = setGatesInSegment.begin(); setGISIter != setGatesInSegment.end(); setGISIter++)
// 	{
// 		cout << m_mapGlobalCircuit.find(*setGISIter)->second.gateType << " ";
// 	}
// 	cout << endl;

	
	map<int, int>::iterator mapPISIter;
	for(mapPISIter = m_mapPrimaryInputSignal.begin(); mapPISIter != m_mapPrimaryInputSignal.end(); mapPISIter++)
	{
	mapPISIter->second = 1;  // init to 1
	}
	

	for(int i = 0; i < m_nInputInstance; i++)
	{
		// for each PI
		int iPICount = 0;
		if(setPI.size() > 0)
		{
			for(setPIIter = setPI.begin(); setPIIter != setPI.end(); setPIIter++)
			{

				int signal = i >> iPICount++;
				signal &= 1;

				mapPISIter = m_mapPrimaryInputSignal.find(*setPIIter);
				if(m_mapPrimaryInputSignal.end() != mapPISIter)
				{
				mapPISIter->second = signal;
				}


/*
								// random choose change or not
								int rnd = (int)(ran1(&m_seed[2])+0.5);
				

				// change the value in m_mapPrimaryInputSignal
				map<int, int>::iterator mapPISIter;
				mapPISIter = m_mapPrimaryInputSignal.find(*setPIIter);
				if(m_mapPrimaryInputSignal.end() != mapPISIter)
				{
					mapPISIter->second = rnd;

				}*/


			}

		}
		else
		{
			map<int, int>::iterator mapPISIter;
			for(mapPISIter = m_mapPrimaryInputSignal.begin(); mapPISIter != m_mapPrimaryInputSignal.end(); mapPISIter++)
			{
				int rnd = (int)(ran1(&m_seed[1])+0.5);
				mapPISIter->second = rnd;
			}

			m_vecMapPISignal.push_back(m_mapPrimaryInputSignal);


		}

		m_vecMapPISignal.push_back(m_mapPrimaryInputSignal);
	}

	// 	}



/*
	set<int> setPI;
	set<int>::iterator setPIIter;
	findPrimaryInputs(*vecPOIter, setPI);


	// only set the PIs in the segment m_vecPrimaryInput[startIndex] -- m_vecPrimaryInput[endIndex]
	for(int i = 0; i < pow(2.0, (double)(endPIIndex-startPIIndex+1)); i++)
	{
		map<int, int>::iterator mapPISIter;
		for(mapPISIter = m_mapPrimaryInputSignal.begin(); mapPISIter != m_mapPrimaryInputSignal.end(); mapPISIter++)
		{
			mapPISIter->second = 1;  // init to 1
		}
		// changem_vecPrimaryInput[startIndex] -- m_vecPrimaryInput[endIndex]
		for(int index = startPIIndex; index <= endPIIndex; index++)
		{
			int signal = i >> (index-startPIIndex);
			signal &= 1;

			mapPISIter = m_mapPrimaryInputSignal.find(m_vecPrimaryInput[index]);
			if(m_mapPrimaryInputSignal.end() != mapPISIter)
			{
				mapPISIter->second = signal;
			}
		}

		// 	cout << endl;
		m_vecMapPISignal.push_back(m_mapPrimaryInputSignal);
	}
*/

	// for test, output m_vecMapPISignal

/*
	vector< map<int, int> >::iterator vecIter;
	for(vecIter = m_vecMapPISignal.begin(); vecIter != m_vecMapPISignal.end(); vecIter++)
	{
		map<int, int>::iterator mapPISIter;
		for(mapPISIter = vecIter->begin(); mapPISIter != vecIter->end(); mapPISIter++)
		{
			cout << mapPISIter->second << "  ";
		}
		cout << endl;
	}
	*/


	
}


void GLC::readPrimaryInputSignal(int index)   // from m_vecMapPISignal  to  m_mapPrimaryInputSignal
{
	m_mapPrimaryInputSignal = m_vecMapPISignal[index];

	// for test
/*
	map<int, int>::iterator mapPISIter;
	for(mapPISIter = m_mapPrimaryInputSignal.begin(); mapPISIter != m_mapPrimaryInputSignal.end(); mapPISIter++)
	{
		cout << mapPISIter->second << ", ";
	}
	cout << endl;
*/

}


// set the real size of each gate (in m_mapGlobalCircuit)
void GLC::setRealSize()
{
	m_fXLSout<<"Real Gate Sizes:\t";

	switch(m_iPVModel)
	{
	case PVMODEL_GAUSSIAN:
		// set real size: Gaussian Distribution
		for(int i = 0; i < m_nGates; i++)
		{
			g_arrRealSize[i] = gngauss(1.0, 0.2);

			//m_fXLSout << g_arrRealSize[i] << "\t";

			// 		// set Vdd
			// 		g_arrVdd[i] = vdd;

			// set Vth
			g_arrVth[i] = abs(gngauss(0.4, 0.10));
			// 		cout << "g_arrVth: " << g_arrVth[i] << ", ";
		}
		break;

	case PVMODEL_QUADTREE:
		{

			// quad tree PV model [10/25/2010 swei]
			// 256-16-4  three-level quad tree model
			// level 1: each gate
			for(int i = 0; i < m_nGates; i++)
			{
				g_arrRealSize[i] = gngauss(0.25, 0.075);
				//g_arrRealSize[i] = gngauss(1.0, 0.075);
			}
			// level 2: 16 grids
			int nGatesPerGrid = m_nGates / 16;
			int gateIndex = 0;
			for(int i = 0; i < 16; i++)
			{
				double rnd = gngauss(0.1, 0.025);
				for(int j = 0; j < nGatesPerGrid; j++)
				{
					g_arrRealSize[gateIndex++] += rnd;
				}
			}

			// level 3: 4 grids
			nGatesPerGrid = m_nGates / 4;
			gateIndex = 0;
			for(int i = 0; i < 4; i++)
			{
				double rnd = gngauss(0.1, 0.025);
				for(int j = 0; j < nGatesPerGrid; j++)
				{
					g_arrRealSize[gateIndex++] += rnd;
				}
			}

			break;
		}
	default:
		break;
	}

	// set HTH size
	// if we really have HTH, set its size to a value, otherwise set it to 0
	if(m_bHTH && m_bReallyHaveHTH)
	{
		// for hth gate
		g_arrRealSize[m_nGates] = 1.85/*ranTriangular(0.9, 1.5, 1.2)*/;
	}
	else
	{
		g_arrRealSize[m_nGates] = 0;
	}

	// output for debug
	for(int i = 0; i < m_nGates; i++)
	{
		m_fXLSout << g_arrRealSize[i] << "\t";

	}

	m_fXLSout<<"End"<<endl;

}


// gate function
int GLC::gateFunction(int gateType, map<int, int> mapInputSignal)
{
	int iRet = 0;

	map<int, int>::iterator mapISIter;

	switch(gateType)
	{
	case NAND:
		iRet = 1;
		for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
		{
			iRet = iRet & mapISIter->second;
		}
		iRet = !iRet;

		break;

	case NOT:
		iRet = !(mapInputSignal.begin()->second);

		break;

	case NOR:

		iRet = 0;

		for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
		{
			iRet = iRet | mapISIter->second;
		}
		iRet = !iRet;

		break;

	case AND:

		iRet = 1;
		for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
		{
			iRet = iRet & mapISIter->second;
		}

		break;

	case XOR:

		iRet = 0;
		for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
		{
			iRet = iRet ^ mapISIter->second;
		}

		break;


	case OR:

		iRet = 0;
		for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
		{
			iRet = iRet | mapISIter->second;
		}

		break;

	case BUFF:

		iRet = mapInputSignal.begin()->second;

		break;

	default:

		break;
	}

	return iRet;
}

// generate LP constraint
double GLC::generateOneLPConstraint(int index, int eqIndex, int segIter)
{
	double measurePower = 0;
	double realPower = 0;

	ofstream *pFCnstout;
	switch(m_iPIStrategy)
	{
	case PI_RANDOM:
	case PI_READ:
		pFCnstout = &m_fCnstout;
		break;

	case PI_SEGMENTATION:
		pFCnstout = &m_fSegLPout[segIter];
		break;

	default:
		break;
	}


	switch(m_iNorm)
	{
	case NORM_SUM:
		// left side
		switch(m_iPIStrategy)
		{
		case PI_RANDOM:
			writeConstraintLeftSide(measurePower, realPower, index);
			break;

		case PI_SEGMENTATION:
			writeConstraintLeftSide(measurePower, realPower, index, segIter);
			break;

		default:
			break;
		}

		// right side
		switch(m_iLPSolver)
		{
		case SOLVER_LP_SOLVE:
			*pFCnstout << " = " << measurePower << " + e" << index << "_" << eqIndex << ";" << endl;   //  [6/30/2010 shengwei]
			break;

		case SOLVER_CPLEX:
			*pFCnstout << " = " << measurePower << endl;

		}

		break;

	case NORM_L1:

		m_iNorm = NORM_SUM;

		switch(m_iPIStrategy)
		{
			// basic constraints
		case PI_RANDOM:
// 			generateOneLPConstraint(index);
			writeConstraintLeftSide(measurePower, realPower, index);
			break;

		case PI_SEGMENTATION:
// 			generateOneLPConstraint(index, segIter);
			writeConstraintLeftSide(measurePower, realPower, index, segIter);
			break;

		default:
			break;
		}


		m_iNorm = NORM_L1;



		// abs related
		switch(m_iLPSolver)
		{
		case SOLVER_CPLEX:
			// right side
			*pFCnstout << " = " << measurePower << endl;

			*pFCnstout << /*"eq" << index << "_2:*/ " +" << (1.0/realPower) << " e" << index << " - abs" << index << " <= 0" << endl;
			*pFCnstout << /*"eq" << index << "_3: " << */ -(1.0/realPower) << " e" << index << " - abs" << index << " <= 0" << endl;

			break;

		case SOLVER_LP_SOLVE:
			// right side
			*pFCnstout << " = " << measurePower << " + e" << index << "_" << eqIndex << ";" << endl;

			*pFCnstout << (1.0/realPower) << " e" << index << "_" << eqIndex << " <= " << "abs" << index << "_" << eqIndex << ";" << endl;  //  [6/30/2010 shengwei]
			*pFCnstout << -(1.0/realPower) << " e" << index << "_" << eqIndex << " <= " << "abs" << index << "_" << eqIndex << ";" << endl; //  [6/30/2010 shengwei]
// 			*pFCnstout << "abs" << index << " < 0.01;" << endl;

// 			if(index <= m_nInputInstance / 2)
// 			{
// 				*pFCnstout << "e" << index << ">0;" << endl;
// 			}
// 			else
// 			{
// 				*pFCnstout << "e" << index << "<0;" << endl;
// 			}

			break;
		}
		break;

	case NORM_MLE:

		// left side
		switch(m_iPIStrategy)
		{
		case PI_RANDOM:
			writeConstraintLeftSide(measurePower, realPower, index);
			break;

		case PI_SEGMENTATION:
			writeConstraintLeftSide(measurePower, realPower, index, segIter);
			break;

		default:
			break;
		}
		// right side
		*pFCnstout << " = " << measurePower << " + e" << index << ";" << endl;

		*pFCnstout << "enorm" << index << ">=" << (1.0/realPower) << " e" << index << ";" << endl; 
		*pFCnstout << "enorm" << index << ">=" << -(1.0/realPower) << " e" << index << ";" << endl; 

		// additional constraints: break points
		// for each break point
		for(int i = 0; i < m_nBreakPoints+1; i++)
		{
 			if(i == 0)
 			{
// 				*pFCnstout << "abs" << index << "_" << i+1 << ">= 0;"<< endl;
// 				*pFCnstout << "abs" << index << "_" << i+1 << "<=" << m_arrBP[i] << ";" << endl;
				*pFCnstout <<  "abs" << index << "_" << i+1 << "=enorm" << index << ";" << endl; 
 			}
			else if(i == m_nBreakPoints)
			{
//  			*pFCnstout << "abs" << index << "_" << i+1 << ">= " << m_arrBP[i-1] << ";" << endl;		
// 				*pFCnstout << "abs" << index << "_" << i+1 << "<= " << m_dMaxError << ";" << endl;
// 				*pFCnstout << "abs" << index << "_" << i << "<=" << "abs" << index << "_" << i+1 << ";" << endl;
				*pFCnstout << "abs" << index << "_" << i+1 << ">=" << "enorm" << index << "-" << m_arrBP[i-1] << ";" << endl;

			}
			else
			{
//   				*pFCnstout << "abs" << index << "_" << i+1 << ">=" << m_arrBP[i-1] << ";" << endl;
// 				*pFCnstout << "abs" << index << "_" << i+1 << "<=" << m_arrBP[i] << ";" << endl;
// 				*pFCnstout << "abs" << index << "_" << i << "<=" << "abs" << index << "_" << i+1  << ";" << endl;
				*pFCnstout << "abs" << index << "_" << i+1 << ">=" << "enorm" << index << "-" << m_arrBP[i-1] << ";" << endl;

			}
//  			*pFCnstout << "e" << index << "_" << i+1 << "+" << m_deltaDP << "<e" << index << "_" << i+2 << ";" << endl;

		}
		break;

	default:

		break;
	}

	return measurePower;
}

void GLC::writeConstraintLeftSide(double &measurePower, double &realPower, int index, int segIter)
{
	int iterGateType = 0;
	int iterNumInputs = 0;
	double hthTempr = 1;
	double hthCoeff = 1;
// 	for(iterGateType = NAND; iterGateType<=BUFF; iterGateType++)
// 	{
// 		for(iterNumInputs = 1; iterNumInputs <= MAX_NUM_INPUTS; iterNumInputs++)
// 		{
// 
// 			m_mapCoeffGates[iterGateType][iterNumInputs].clear();
// 		}
// 	}

	map<int, GateNode>::iterator mapGCIter;
	unsigned int iCount = 0;

	// set output stream (consider segmentation)
	ofstream *pFCnstout;
	switch(m_iPIStrategy)
	{
	case PI_RANDOM:
		pFCnstout = &m_fCnstout;
		break;

	case PI_SEGMENTATION:
		pFCnstout = &m_fSegLPout[segIter];
		break;

	default:
		break;
	}


	// consider segmentation: figure out the set of gates in this segment
	set<int> setGatesInSegment;
	set<int>::iterator setGIter;

	int nGates = m_nGates;
	if(PI_SEGMENTATION == m_iPIStrategy)
	{
		// get the gate set for this segment
		// tree circuit segmentation [9/26/2009 shengwei]
		listGatesInSegment(m_vecSegOutput[segIter], setGatesInSegment);
		// if we consider HTH, add the HTH variable, with output pin 0
		if(m_bHTH)
		{
			setGatesInSegment.insert(HTH_OUTPUT_PIN);
		}

		nGates = setGatesInSegment.size();
	}

	// for CPLEX
	if(SOLVER_CPLEX == m_iLPSolver)
	{
		if(1 == index)
		{
			*pFCnstout << "subject to" << endl; 
		}
		// title of each constraint:
// 		*pFCnstout << "eq" << index << "_1: ";
	}

// 	cout << "{";
	vector<double> vect;
	// left side of the constraint
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		// for saro [12/19/2009 shengwei]
		if(m_bSelectGates)
		{
			if(!isSelectedGate(mapGCIter->second.gateId))
			{
				continue;
			}
		}

		// consider segmentation
		if(PI_SEGMENTATION == m_iPIStrategy)
		{
			if(setGatesInSegment.end() == setGatesInSegment.find(mapGCIter->first))
			{
				// not in segment, continue
				continue;
			}
		}

		// consider switch power [10/11/2009 shengwei]
		if(METRIC_SWITCH_POWER == m_iMetric && !mapGCIter->second.bSwitch)
		{
			// update coeff vector to be 0
			nGates = m_nSwitchedGates;
			vect.push_back(0);
			continue;
		}

		int gateId = mapGCIter->second.gateId;


		// for each gate
		int gateType = mapGCIter->second.gateType;
		map<int, int> mapInputSignal = mapGCIter->second.mapInputSignal;
		map<int, int>::iterator mapISIter;
		if(mapInputSignal.begin()->second < 0)
		{
			mapInputSignal.begin()->second = floor(ran1(&m_seed[3])+0.5);
		}

		int signalIndex = mapInputSignal.begin()->second;
		// if not HTH
		if(gateId != m_nGates+1 || !m_bRandomHTH)
		{
			for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
			{
				if(mapInputSignal.begin() == mapISIter)
				{
					continue;
				}
				if(mapISIter->second < 0)
				{
					mapISIter->second = floor(ran1(&m_seed[3])+0.5);
				}
				signalIndex = signalIndex*2 + mapISIter->second;
			}
		}
		else
		{
			// random HTH
			signalIndex = ran1(&m_seed[4]) * (pow(2.0, (double)mapInputSignal.size()));
			cout << "eqn: " << index << " signalIndex: " << signalIndex << endl;
		}
// 		if(signalIndex < 0)
// 		{
// 			signalIndex = 0;
// 		}

		iCount++;

		double coeff = 0;  // coeff of this x
		// consider switch power [10/11/2009 shengwei]
		// consider thermal [10/13/2009 shengwei]
		switch(m_iMetric)
		{
		case METRIC_SWITCH_POWER:
			coeff = g_arrSWDelayTable[gateType][mapInputSignal.size()-1];

			break;

		case METRIC_LEAKAGE_POWER:
			coeff = g_arrLookupTable[gateType][mapInputSignal.size()-1][signalIndex];

			break;

		case METRIC_THERMAL_LEAKAGE:
			coeff = mapGCIter->second.dTempr*mapGCIter->second.dTempr*exp(-1/mapGCIter->second.dTempr)*g_arrLookupTable[gateType][mapInputSignal.size()-1][signalIndex];
// 			printf("%f, ", coeff);
// 			if(1 == mapInputSignal.size())
// 			{
// 				// for saro's case
// 				coeff = mapGCIter->second.dTempr*g_arrLookupTable[gateType][1][2*signalIndex+signalIndex];
// 			}
			break;
		}

		if(0 == coeff)
		{
			cout << "coeff";
		}

		// set hth temperature
		if(m_nGates+1 == gateId)
		{
			hthTempr = mapGCIter->second.dTempr;
			hthCoeff = coeff;
			// for test placement, set HTH coeff manually [5/24/2010 shengwei]
			coeff *= 1.4;
//			cout << "HTH coeff: " << coeff << endl;
		}


		// consider HTH [11/4/2009 shengwei]
		if(m_nGates+1 == gateId && !m_bReallyHaveHTH)
		{
			coeff = 0;
		}

		if(!m_bSmallOut)
		{
			// consider HTH
 			if(HTH_OUTPUT_PIN != mapGCIter->first)
 			{
				*pFCnstout << coeff << " x";
 			
			}
			else
			{
				*pFCnstout  << " x";
			}

			// 		m_fCnstout<< iCount;
			*pFCnstout<< gateId;  //  [10/11/2009 shengwei]

			if(iCount <= nGates-1)
			{
				*pFCnstout << " + ";
			}
		}
		else
		{
			// take the smallest coeff out
			// don't output yet
			map<double, set<int> >::iterator mapCGIter;
			mapCGIter = m_mapCoeffGates[gateType][mapInputSignal.size()].find(coeff);

			if(m_mapCoeffGates[gateType][mapInputSignal.size()].end() != mapCGIter)
			{
				// already exists
				// insert into the set
				mapCGIter->second.insert(gateId);
			}
			else
			{
				// not exists yet
				// make a new gateId set and insert into the map
				set<int> setGates;
				setGates.insert(gateId);
				m_mapCoeffGates[gateType][mapInputSignal.size()].insert(make_pair(coeff, setGates));
			}
		}

		realPower += coeff * g_arrRealSize[gateId-1];

		if(gateId == m_nGates+1)
		{
// 			cout << coeff * g_arrRealSize[gateId-1] << endl;
		}
/*
		switch(m_iErrorModel)
		{
		case ERROR_UNIFORM:
			measurePower += coeff * g_arrRealSize[gateId-1] * (1+m_dErrorRate);

			break;

		case ERROR_GAUSSIAN:
			rnd = ranGaussian(m_dErrorRate, 0.001);
			measurePower += coeff * g_arrRealSize[gateId-1] * (1+rnd);
			break;

		case ERROR_TRIANGULAR:
			realPower += coeff * g_arrRealSize[gateId-1];
// 			rnd = ranTriangular(-m_dErrorRate, m_dErrorRate, 0);
// 			measurePower += g_arrLookupTable[gateType][mapInputSignal.size()-1][signalIndex] * g_arrRealSize[iCount-1] * (1+rnd);
			break;

		case ERROR_EXP:
// 			realPower += coeff 
			break;

		}*/


		// update coefficient, for correlation detection
		vect.push_back(coeff); 

	}
// 	cout << "}," << endl;

	if(SOLVER_CPLEX == m_iLPSolver)
	{
		*pFCnstout << "-e" << index;
	}

	if(m_bSmallOut)
	{
		outputSmallOutConstraints(index);
	}


	if(m_bDiagnosis)
	{
		// put guessHTH in the equation
		// 		cout << "guess " << guessHTH << ": " << endl;
		double guessHTHCoeff;

		// guess!
		int guessHTHSignal0 = getSignal(m_iGuessHTHInput);
		int guessHTHSignal1 = getSignal(m_iGuessHTHInput);

		// suppose Inverter
		if(NOT != m_iGuessHTHType)
		{
 			guessHTHCoeff = hthTempr * g_arrLookupTable[m_iGuessHTHType][1][guessHTHSignal0*2+guessHTHSignal1];  //  [2/18/2010 shengwei]
		}
		else
		{
			guessHTHCoeff = hthTempr * g_arrLookupTable[m_iGuessHTHType][0][guessHTHSignal0];  //  [2/18/2010 shengwei]
		}
	
		// 		cout << guessHTHCoeff << endl;
// 		cout << guessHTHCoeff << endl;

		if(!m_bSmallOut)
		{
			*pFCnstout << " + " << guessHTHCoeff << " x";
		}

// 		realPower += guessHTHCoeff * 1.35;
	}	

	m_vecCoeff.push_back(vect);

	double rnd = 0;
	int sign = 1;
	switch(m_iErrorModel)
	{
	case ERROR_TRIANGULAR:
		rnd = ranTriangular(-3.0*m_dErrorRate, 3.0*m_dErrorRate, 0);

		break;

	case ERROR_EXP:

//  		rnd = ranExp3(/*&m_seed[4]*/);
// 		// approach 3 [11/7/2009 shengwei]
// 		if(index <= m_nInputInstance / 2)
// 		{
// 			rnd = ranExp3(/*&m_seed[5]*/);
// // 			rnd = 0.01;
// 			if(rnd < 0.01)
// 			{
// 				m_vecExpRnd.push_back(rnd);
// 			}
// 		}
// 		else
// 		{
// 			rnd = -m_vecExpRnd[index - m_nInputInstance/2 - 1];
// 		}

		rnd = 100 * m_dErrorRate * m_vecExpRnd[index-1];
		break;
	}

//   	cout << rnd << ", ";
//   	cout << realPower*rnd << endl;
 	measurePower = realPower *(1.0 + rnd);
// 	cout << measurePower << endl;

}

/*
void GLC::writeSegConstraintLeftSide(double &measurePower, double &realPower, int segIter, int guessHTH)
{
	map<int, GateNode>::iterator mapGCIter;
	unsigned int iCount = 0;

	vector<double> vect;

	// figure out the set of gates in this segment
	set<int> setGatesInSegment;
	set<int>::iterator setGIter;

	if(PI_SEGMENTATION == m_iPIStrategy)
	{
		// get the gate set for this segment
		// tree circuit segmentation [9/26/2009 shengwei]
		listGatesInSegment(m_vecSegOutput[segIter], setGatesInSegment);
		// if we consider HTH, add the HTH variable, with output pin 0
		if(m_bHTH)
		{
			setGatesInSegment.insert(HTH_OUTPUT_PIN);
		}
	}



	for(setGIter = setGatesInSegment.begin(); setGIter != setGatesInSegment.end(); setGIter++)
	{
		// for each gate
		mapGCIter = m_mapGlobalCircuit.find(*setGIter);
		int gateType = mapGCIter->second.gateType;
		int gateId = mapGCIter->second.gateId;
		map<int, int> mapInputSignal = mapGCIter->second.mapInputSignal;
		map<int, int>::iterator mapISIter;
		int signalIndex = mapInputSignal.begin()->second;
		for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
		{
			if(mapInputSignal.begin() == mapISIter)
			{
				continue;
			}
			signalIndex = signalIndex*2 + mapISIter->second;
		}

		double coeff = 0;  // coeff of this x
		// consider switch power [10/11/2009 shengwei]
		// consider thermal [10/13/2009 shengwei]
		switch(m_iMetric)
		{
		case METRIC_SWITCH_POWER:
			coeff = g_arrSWDelayTable[gateType][mapInputSignal.size()-1];

			break;

		case METRIC_LEAKAGE_POWER:
			coeff = g_arrLookupTable[gateType][mapInputSignal.size()-1][signalIndex];
// 			if(gateId == 384)
// 			{
// 				cout << coeff << endl;
// 			}


			break;

		case METRIC_THERMAL_LEAKAGE:
			coeff = getLeakageCoeffFromTempr(mapGCIter->second.dTempr)*g_arrLookupTable[gateType][mapInputSignal.size()-1][signalIndex];
// 			if(gateId == 384)
// 			{
// 				cout << coeff << endl;
// 			}

			break;
		}

		switch(m_iErrorModel)
		{
		case ERROR_UNIFORM:
			realPower += coeff * g_arrRealSize[gateId-1];

			break;

		case ERROR_GAUSSIAN:
			//  				rnd = m_dErrorRate + gasdev(&m_seed[4]);
			//  				cout << rnd << "  ";
			realPower += coeff * g_arrRealSize[gateId-1];
			break;

		case ERROR_TRIANGULAR:
			// 				cout << rnd << "  ";
			if(0 != mapGCIter->first || (0 == mapGCIter->first && m_bReallyHaveHTH))
			{
				realPower += coeff * g_arrRealSize[gateId-1];  // consider positive error only
// 				cout << "coeff: " << coeff << "   ";
//  				cout << coeff * g_arrRealSize[gateId-1] << "  ";
			}
			break;

		}

		iCount++;
		//////////////////////////////////////////////////////////////////////////
		// left side

		// consider HTH [10/17/2009 shengwei]
		if(HTH_OUTPUT_PIN != mapGCIter->first)
		{
			m_fSegLPout[segIter] << coeff;
		}

		m_fSegLPout[segIter] << " x" << mapGCIter->second.gateId;
		if(iCount < setGatesInSegment.size())
		{
			m_fSegLPout[segIter] << " + ";
		}

		// update coefficient, for correlation detection
		vect.push_back(g_arrLookupTable[gateType][mapInputSignal.size()-1][signalIndex]); 
	}

// 	cout << endl;
	if(m_bDiagnosis)
	{
		// put guessHTH in the equation
// 		cout << "guess " << guessHTH << ": " << endl;
		double guessHTHCoeff;

		int guessHTHSignal = getSignal(290);
		// suppose Inverter
		guessHTHCoeff = g_arrLookupTable[NOT][0][guessHTHSignal];
// 		cout << guessHTHCoeff << endl;

		m_fSegLPout[segIter] << " + " << guessHTHCoeff << " x";
	}

	m_vecCoeff.push_back(vect);

	double rnd = ranTriangular(-3.0*m_dErrorRate, 3.0*m_dErrorRate, 0);
// 	cout << rnd << endl;
// 	cout << realPower*rnd << endl;
	measurePower = realPower *(1.0 + rnd);
// 	cout << "realPower: " << realPower << endl; 

//  	cout << measurePower << endl;




}



void GLC::generateSegOneLPConstraint(int cnstIndex, int segIter)
{
	double measurePower = 0;
	double realPower = 0;

	switch(m_iNorm)
	{
	case NORM_SUM:

		writeSegConstraintLeftSide(measurePower, realPower, segIter);
		//////////////////////////////////////////////////////////////////////////
		// right side

		// right side of the constraint
		m_fSegLPout[segIter] << " = " << measurePower << " + e" << cnstIndex+1 << ";" << endl;

		break;

	case NORM_L1:

		m_iNorm = NORM_SUM;
		// basic constraints
		generateSegOneLPConstraint( cnstIndex, segIter);
		m_iNorm = NORM_L1; 

		// abs related
		m_fSegLPout[segIter] << "e" << cnstIndex+1 << " <= " << "abs" << cnstIndex+1 << ";" << endl;
		m_fSegLPout[segIter] << "-e" << cnstIndex+1 << " <= " << "abs" << cnstIndex+1 << ";" << endl;

		break;

	case NORM_MLE:

		writeSegConstraintLeftSide(measurePower, realPower, segIter);
		// additional constraints: break points
		// for each break point
		for(int i = 0; i < m_nBreakPoints; i++)
		{
			if(i == 0)
			{
				m_fSegLPout[segIter] << "e" << cnstIndex+1 << "_" << i+1 << ">=" << 0 << ";" << endl;
				m_fSegLPout[segIter] << "e" << cnstIndex+1 << "_" << i+1 << "<=" << m_arrBP[i+1]*measurePower << ";" << endl;
			}
			else if(i == m_nBreakPoints-1)
			{
				m_fSegLPout[segIter] << "e" << cnstIndex+1 << "_" << i+1 << ">=" << m_arrBP[i-1]*measurePower << ";" << endl;
				m_fSegLPout[segIter] << "e" << cnstIndex+1 << "_" << i+1 << "<=" << m_dMaxError*measurePower << ";" << endl;
			}
			else
			{
				m_fSegLPout[segIter] << "e" << cnstIndex+1 << "_" << i+1 << ">=" << m_arrBP[i-1]*measurePower << ";" << endl;
				m_fSegLPout[segIter] << "e" << cnstIndex+1 << "_" << i+1 << "<=" << m_arrBP[i+1]*measurePower << ";" << endl;

			}
		}

		break;

	default:

		break;
	}
}
*/

// generate Objective Function
void GLC::generateOF(int segIter)
{
	ofstream *pFOFout;
	switch(m_iPIStrategy)
	{
	case PI_RANDOM:
	case PI_READ:
		pFOFout = &m_fOFout;

		break;

	case PI_SEGMENTATION:

		pFOFout = &m_fSegLPout[segIter];
		break;


	}

	switch(m_iNorm)
	{
	case NORM_SUM:

		m_fLPout << "min: ";

		for(int i = 0; i < m_nInputInstance; i++)
		{
			*pFOFout << "e" << (i+1);
			if(m_nInputInstance-1 != i)
			{
				*pFOFout << "+";
			}
		}

		*pFOFout << ";" << endl << endl;
		break;

	case NORM_L1:

		switch(m_iLPSolver)
		{
		case SOLVER_LP_SOLVE:
			*pFOFout << "min: ";
			break;

		case SOLVER_CPLEX:
			*pFOFout << "minimize cost: ";
			break;

		}


		for(int i = 0; i < m_nInputInstance; i++)
		{
			for(int j = 0; j < m_nEqnsPerSignal; j++)
			{
				*pFOFout << "abs" << (i+1) << "_" << (j+1);
				if(m_nInputInstance-1 != i)
				{
					*pFOFout << "+";
				}
			}			
		}

		*pFOFout << ";" << endl << endl;

		break;

	case NORM_MLE:

		*pFOFout << "min: ";


		// list optimal break points for the MLE function
		listOptBreakPoints(m_arrBP);

		// calculate a b for each piece (m_nBreakPoints+1 pieces in total)
		// then the function for each piece is y = bx + a
		double a[MAX_N+1], b[MAX_N+1];
		// first piece, from m_dLowerDP to arrBP[0];
		//calcAB(m_dLowerDP, m_arrBP[0], a[0], b[0]);
		// middle pieces
		for(int i = 0; i < m_nBreakPoints-1; i++)
		{
			//calcAB(m_arrBP[i], m_arrBP[i+1], a[i+1], b[i+1]);
		}


		// last piece, from arrBP[m_nBreakPoints-1] to m_dUpperDP
		//calcAB(m_arrBP[m_nBreakPoints-1], m_dUpperDP, a[m_nBreakPoints], b[m_nBreakPoints]);

// 		// for test
//   		b[0] = -m_debugSlop[0];
//   		b[1] = -m_debugSlop[1];
//   		b[2] = -m_debugSlop[2];
	
		// for each instance
		for(int i = 0; i < m_nInputInstance; i++)
		{
/*
			if((i+1)>=81 && (i+1)<=120)
			{
				continue;
			}
*/

			// for each piece, generate OF
			for(int j = 0; j < m_nBreakPoints+1; j++)
			{
				// the linear function for this piece is y = b[i] * x + a[i], for each e, we need to sum them up over e
				// f(X1) -> k1X1_1 + k2 X1_2
// 				if(j==0)
// 				{
					*pFOFout << b[j]/b[0] << " abs" << i+1 << "_" << j+1; 
// 				}
// 				else
// 				{
// 					*pFOFout << -(b[j]-b[j-1]) << " abs" << i+1 << "_" << j+1;
// 				}
				if(j != m_nBreakPoints || i != m_nInputInstance-1)
				{
					*pFOFout << "+";
				}
			}
		}

		if(SOLVER_CPLEX != m_iLPSolver)
		{
			*pFOFout << ";" << endl << endl;
		}


		break;

	}
}

void GLC::generateSegOF(int segIter)
{

// 	set<int> setPI;
// 	findPrimaryInputs(m_vecPrimaryOutput[segIter], setPI);
	// tree circuit segmentation[9/26/2009 shengwei]
// 	findPrimaryInputs(m_vecSegOutput[segIter], setPI);

// 	int nVars = setPI.size();

	m_fSegLPout[segIter] << "/* OF */" << endl;

	switch(m_iNorm)
	{
	case NORM_SUM:
		m_fSegLPout[segIter] << "min: ";

		for(int i = 0; i < m_nInputInstance; i++)
		{
			m_fSegLPout[segIter] << "e" << (i+1);
			if(m_nInputInstance-1 != i)
			{
				m_fSegLPout[segIter] << "+";
			}
		}

		m_fSegLPout[segIter] << ";" << endl << endl;
		break;

	case NORM_L1:
		m_fSegLPout[segIter] << "min: ";

		for(int i = 0; i < m_nInputInstance; i++)
		{
			m_fSegLPout[segIter] << "abs" << (i+1);
			if(m_nInputInstance-1 != i)
			{
				m_fSegLPout[segIter] << "+";
			}
		}

		m_fSegLPout[segIter] << ";" << endl << endl;

		break;

	case NORM_MLE:

		m_fSegLPout[segIter] << "max: ";

		// list optimal break points for the MLE function
		listOptBreakPoints(m_arrBP);

		// calculate a b for each piece (m_nBreakPoints+1 pieces in total)
		// then the function for each piece is y = bx + a
		double a[MAX_N+1], b[MAX_N+1];
		// first piece, from m_dLowerDP to arrBP[0];
		//calcAB(m_dLowerDP, m_arrBP[0], a[0], b[0]);
		// middle pieces
		for(int i = 0; i < m_nBreakPoints-1; i++)
		{
			//calcAB(m_arrBP[i], m_arrBP[i+1], a[i+1], b[i+1]);
		}

		// last piece, from arrBP[m_nBreakPoints-1] to m_dUpperDP
		//calcAB(m_arrBP[m_nBreakPoints-1], m_dUpperDP, a[m_nBreakPoints], b[m_nBreakPoints]);

		// for each instance
		for(int i = 0; i < m_nInputInstance; i++)
		{
			// for each piece, generate OF
			for(int j = 0; j < m_nBreakPoints+1; j++)
			{
				// the linear function for this piece is y = b[i] * x + a[i], for each e, we need to sum them up over e
				// f(X1) -> k1X1_1 + k2 X1_2
				m_fSegLPout[segIter] << b[j] << " e" << i+1 << "_" << j+1; 
				if(j != m_nBreakPoints || i != m_nInputInstance-1)
				{
					m_fSegLPout[segIter] << "+";
				}
			}
		}
		m_fSegLPout[segIter] << ";" << endl << endl;
		break;
	}


}

// for weighted L1-norm
/*
void GLC::generateWeight(double abs[MAX_NUM_INSTANCE], double weight[MAX_NUM_INSTANCE])
{
	// calculate mad of the abs
	double madResult = mad(abs);
	for(int i = 0; i < m_nInputInstance; i++)
	{
		weight[i] = exp(-(abs[i]*abs[i])/(2*(abs[i] - madResult)*(abs[i] - madResult)));
		if(weight[i] < 0.0001)
		{
			weight[i] = 0;
		}
	}

// 	double sum = 0;
// 	for(int i = 0; i < m_nInputInstance; i++)
// 	{
// 		sum += weight[i];
// 	}
// 
// 	for(int i = 0; i < m_nInputInstance; i++)
// 	{
// 		weight[i] = weight[i] / sum;
// 	}


}
*/


// generate Free variables declaration
void GLC::generateFreeVariables(int segIter)
{
	switch(m_iPIStrategy)
	{
	case PI_RANDOM:

		switch(m_iNorm)
		{
		case NORM_L1:

			// free e
			m_fLPout << "free ";
			for(int i = 0; i < m_nInputInstance; i++)
			{
				for(int j = 0; j < m_nEqnsPerSignal; j++)
				{
					m_fLPout << "e" << (i+1) << "_" << (j+1);
					if(i < m_nInputInstance - 1)
					{
						m_fLPout << ", ";
					}
				}
			}
			m_fLPout << ";" << endl;


			break;

		case NORM_MLE:

			// free e
			m_fLPout << "free ";
			for(int i = 0; i < m_nInputInstance; i++)
			{
// 				for(int j = 0; j < m_nBreakPoints+1; j++)
// 				{
					m_fLPout << "e" << (i+1);
					if(i != m_nInputInstance - 1)
					{
						m_fLPout << ", ";
					}
// 				}

			}
			m_fLPout << ";" << endl;

			break;
		}

		break;

	case PI_SEGMENTATION:
		set<int> setGatesInSegment;
		set<int>::iterator setGISIter;
		// get the gate set for this segment
		// tree cirtuit segmentation [9/26/2009 shengwei]
		// 	listAllGatesOnPath(m_vecPrimaryOutput[segIter], setGatesInSegment);
		listGatesInSegment(m_vecSegOutput[segIter], setGatesInSegment);

		int iCount = 0;

		// free e
		m_fSegLPout[segIter] << "free ";
		for(int i = 0; i < m_nInputInstance; i++)
		{
			m_fSegLPout[segIter] << "e" << (i+1);
			if(i < m_nInputInstance - 1)
			{
				m_fSegLPout[segIter] << ", ";
			}
		}
		m_fSegLPout[segIter] << ";" << endl;

		break;
	}



}

void GLC::generateSOS()
{
	m_fCnstout << "sos" << endl;
	for(int i = 0; i < m_nInputInstance; i++)
	{
		m_fCnstout << "SOS: ";
		for(int j = 0; j < m_nBreakPoints+1; j++)
		{
			m_fCnstout << "abs" << (i+1) << "_" << (j+1);
			if(j != m_nBreakPoints)
			{
				m_fCnstout << ",";
			}
		}
		m_fCnstout << " <= 1;" << endl;
	}
}

void GLC::generateSec()
{
	m_fCnstout << "sec ";

	for(int i = 0; i < m_nInputInstance; i++)
	{
		for(int j = 0; j < m_nBreakPoints+1; j++)
		{
			m_fCnstout << "abs" << (i+1) << "_" << (j+1);
			if(j != m_nBreakPoints)
			{
				m_fCnstout << ",";
			}
		}
	}
	m_fCnstout << ";" << endl;
}


// merge OF and constraints
void GLC::mergeOFCnst()
{
	// for CPLEX
// 	if(SOLVER_CPLEX == m_iLPSolver)
// 	{
// 		// declare all the variables
// 		for(int i = 0; i < m_nInputInstance; i++)
// 		{
// 			// x
// 			m_fLPout << "var x" << (i+1) << ";" << endl;
// 
// 			// e
// 			m_fLPout << "var e" << (i+1) << ";" << endl;
// 
// 
// 			switch(m_iNorm)
// 			{
// 			case NORM_L1:
// 				// abs
// 				m_fLPout << "var abs" << (i+1) << ";" << endl;
// 				break;
// 			}
// 			// 
// 		}
// 	}

	// merge OF.lp and Constraints.lp to m_fLPout
	ifstream finOF, finCnst;

	// read OF
	finOF.open("OF.lp");
	while(finOF.good())
	{
		char * buf = new char[4096000];
		finOF.getline(buf, 4096000);
		m_fLPout << buf << endl;
		delete []buf;
	}
	finOF.close();

	// read Constraints
	finCnst.open("Constraints.lp");
	while(finCnst.good())
	{
		char * buf = new char[4096000];
		finCnst.getline(buf, 4096000);
		m_fLPout << buf << endl;
		delete []buf;
	}
	finCnst.close();

	m_fLPout.flush();
}


// for debug
void GLC::debugCircuit()
{
	map<int, int>::iterator mapPIIter;
	m_fDebugOut << "primary input: ";
	for(mapPIIter = m_mapPrimaryInputSignal.begin(); mapPIIter != m_mapPrimaryInputSignal.end(); mapPIIter++)
	{
		m_fDebugOut << mapPIIter->first << ", " << mapPIIter->second << "   ";
	}
	m_fDebugOut << endl;

	m_fDebugOut << "primary output: ";
	vector<int>::iterator vecIter;
	for(vecIter = m_vecPrimaryOutput.begin(); vecIter != m_vecPrimaryOutput.end(); vecIter++)
	{
		m_fDebugOut << *vecIter << ", ";
	}
	m_fDebugOut << endl;

	m_fDebugOut << "map circuit: " << endl;
	map<int, GateNode>::iterator mapIter;
	for(mapIter = m_mapGlobalCircuit.begin(); mapIter != m_mapGlobalCircuit.end(); mapIter++)
	{
		// output
		m_fDebugOut << mapIter->first << ",  ";

		// gate type
		m_fDebugOut << mapIter->second.gateType << ", ";

		// input
		map<int, int> mapInputSignal = mapIter->second.mapInputSignal;
		map<int, int>::iterator mapIter;
		for(mapIter = mapInputSignal.begin(); mapIter != mapInputSignal.end(); mapIter++)
		{
			m_fDebugOut << mapIter->first << "," << mapIter->second << "  ";
		}

		m_fDebugOut << endl;

	}

	m_fDebugOut << endl;
}

// random number generator
double GLC::ran1(long *idum)
{
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

	int j;
	long k;
	static long iy=0;
	static long iv[NTAB];
	double temp;

	if (*idum <= 0 || !iy) 
	{ 
		if (-(*idum) < 1) *idum=1; 
		else *idum = -(*idum);
		for (j=NTAB+7;j>=0;j--) { 
			k=(*idum)/IQ;
			*idum=IA*(*idum-k*IQ)-IR*k;
			if (*idum < 0) *idum += IM;
			if (j < NTAB) iv[j] = *idum;
		}
		iy=iv[0];
	}

	k=(*idum)/IQ; 
	*idum=IA*(*idum-k*IQ)-IR*k; 
	if (*idum < 0) *idum += IM; 
	j=iy/NDIV; 
	iy=iv[j]; 
	iv[j] = *idum; 
	if ((temp=AM*iy) > RNMX) return RNMX; 
	else return temp;

}

// Gaussian Distribution
double GLC::gasdev(long *idum)
{
// 	float ran1(long *idum);
	static int iset=0;
	static double gset;
	double fac,rsq,v1,v2;
	if (iset == 0) 
	{ 
		do 
		{
			v1=2.0*ran1(idum)-1.0; 
			v2 = 2.0*ran1(idum)-1.0; 
			rsq=v1*v1+v2*v2; 
		}while (rsq >= 1.0 || rsq == 0.0);

		fac=sqrt(-2.0*log(rsq)/rsq);

		gset=v1*fac;
		iset=1; 
		return v2*fac;
	} 
	else 
	{ 
		iset=0; 
		return gset; 
	}
}

double GLC::gngauss(double mean, double sigma) 
{ 
	double u, r; /* uniform and Rayleigh random variables */ 
	u=(double)rand()/(double) RAND_MAX; /* uniform random number in [0 and 1) */ 
	if (u==1.0) u = 0.999999999; 
	r=sigma*sqrt(2.0*log(1.0/(1.0 - u))); /* generate a Rayleigh-distributed random number r using u */ 
	u=(double)rand()/(double) RAND_MAX; /* uniform random number in [0 and 1) */ 
	if (u==1.0) u = 0.999999999; 
	return((double)(mean+r*cos(2*3.14159265*u))); /* return Gaussian-distributed random number using r and u */ 
}


// Triangular Distribution
double GLC::ranTriangular(double min, double max, double mode)
{
// 	double r = 0;
// 	double u = ran1(&m_seed[0]);
// 	double v = ran1(&m_seed[0]);

	double r = 0;
	double u=ran1(&m_fixedSeed);
// 	cout << u << " ";

	if (u <= (mode-min)/(max-min))
	{
		r=min+sqrt(u*(max-min)*(mode-min));
	}
	else
	{
		r=max-sqrt((1-u)*(max-min)*(max-mode));
	}

	return r;
}

// Gaussian Distribution
/*
double GLC::ranGaussian(double mean, double var)
{

	setState(time(NULL));

	// generate Gaussian Distribution
	// input value to matlab, probablility p
	mxArray *pMxArr1, *pMxArr2;
	int dims[2] = {1,1};

	pMxArr1 = mxCreateDoubleMatrix(1, 1, mxREAL);
	memcpy((void *)mxGetData(pMxArr1), (void*)&mean, sizeof(double));
	engPutVariable(m_pEngine, "mu", pMxArr1);

	pMxArr2 = mxCreateDoubleMatrix(1, 1, mxREAL);
	memcpy((void *)mxGetData(pMxArr2), (void*)&var, sizeof(double));
	engPutVariable(m_pEngine, "sigma", pMxArr2);


	// generate a random number from discrete uniform distribution with parameter n.
	engEvalString(m_pEngine, "num = normrnd(mu, sigma)");

	// output value to C++
	double *pCresult;
	mxArray *pMresult;

	pMresult = engGetVariable(m_pEngine, "num");
	pCresult = (double *)mxGetData(pMresult);

	// 	cout << "*pCresult = " << *pCresult << endl;

	// 	m_fout << "Simulator::myUnidrnd(): " << timer.stopTimer() << endl;


	return *pCresult;

}*/


// Exponential Distribution [11/5/2009 shengwei]
// Returns an exponentially distributed, positive, random deviate of unit mean, using
// ran1(idum) as the source of uniform deviates.
double GLC::ranExp(long *idum)
{
	double dum;
	do
	{
		dum=ran1(&m_seed[4]);
	}
	while (-log(dum) == 0.0);
	
	return (-log(dum));
}

double GLC::ranExp2()
{
	{
		static float q[8] = {
			0.6931472,0.9333737,0.9888778,0.9984959,0.9998293,0.9999833,0.9999986,1.0
		};
		static long i;
		static float sexpo,a,u,ustar,umin;
		static float *q1 = q;
		a = 0.0;
		u = ran1(&m_seed[4]);
		goto S30;
S20:
		a += *q1;
S30:
		u += u;
		if(u <= 1.0) goto S20;
		u -= 1.0;
		if(u > *q1) goto S60;
		sexpo = a+u;
		return sexpo;
S60:
		i = 1;
		ustar = ran1(&m_seed[4]);
		umin = ustar;
S70:
		ustar = ran1(&m_seed[4]);
		if(ustar < umin) umin = ustar;
		i += 1;
		if(u > *(q+i-1)) goto S70;
		sexpo = a+umin**q1;
		return sexpo;
	}

}

double GLC::ranExp3()
{
	double x = 0.0;
	double y = 0.0;

// 	double d = 2;
	double coeff = -500/*log(0.0005)/log(d)/0.10*/;

	do
	{
			x = ran1(&m_seed[5]) * 0.20;
			y = ran1(&m_seed[6]) * 0.2;
	}
//	while(y > 0.2 * pow(d, coeff * x));
	while(y > 0.2 * exp(coeff * x));

	return 5 * x;

}




// mad
/*
double GLC::mad(double x[MAX_NUM_INSTANCE])
{
	mxArray *pMxArr1, *pMxArr2;

	pMxArr1 = mxCreateDoubleMatrix(1, m_nInputInstance, mxREAL);
	memcpy((void *)mxGetPr(pMxArr1), (void*)x, m_nInputInstance * sizeof(double));
	engPutVariable(m_pEngine, "x", pMxArr1);

	// generate a random number from discrete uniform distribution with parameter n.
// 	engEvalString(m_pEngine, "x = (1:1:10);" );
	engEvalString(m_pEngine, "madresult = mad(x, 0);");

	// output value to C++
	double *pCresult;
	mxArray *pMresult;

	pMresult = engGetVariable(m_pEngine, "madresult");
	pCresult = (double *)mxGetData(pMresult);

	// 	cout << "*pCresult = " << *pCresult << endl;

	// 	m_fout << "Simulator::myUnidrnd(): " << timer.stopTimer() << endl;


	return *pCresult;

}*/



/*
void GLC::setState(int state)
{
	mxArray *pMxArr;
	pMxArr = mxCreateDoubleMatrix(1, 1, mxREAL);
	memcpy((void *)mxGetPr(pMxArr), (void*)&state, sizeof(double));
	engPutVariable(m_pEngine, "state", pMxArr);

	// generate a random number from binomial distribution with parameter 1 and p. i.e. do bernoulli experiment
	engEvalString(m_pEngine, "rand('state', state);");
	engEvalString(m_pEngine, "randn('state', state);");

}*/


// detect correlation
void GLC::detectCorrelation(vector< pair<int, int> > & vecCorrelation)
{
// 	m_vecCoeff;
	int nGates = m_nGates;
	if(m_bSelectGates)
	{
		nGates = g_nSelGates;
	}

	// for each pair
	for(int i = 0; i < nGates; i++)
	{
		for(int j = i+1; j < nGates; j++)
		{

			double resRatio = 0;  // reserved ratio
			bool bCorr = true;
			vector< vector<double> >::iterator vecIter;
			set<double> setRatio;

			bool bBreakCorr = false;

			for(vecIter = m_vecCoeff.begin(); (vecIter+1) != m_vecCoeff.end(); vecIter++)
			{
				if(0 == (*vecIter)[i] && 0 == (*vecIter)[j])
				{
					continue;
				}
				else if(0 == (*vecIter)[i] || 0 == (*vecIter)[j])
				{
					// break correlation
					bBreakCorr = true;
				}

				double ratio = (*vecIter)[i] / (*vecIter)[j];

				if(setRatio.end() == setRatio.find(ratio))
				{
					setRatio.insert(ratio);
				}


			}

			if(setRatio.size() <= 1 && !bBreakCorr)
			{
				// this i,j pair is correlated
				vecCorrelation.push_back(make_pair(i+1,j+1));
			}
/*
			// check whether Gate i and Gate j are correlated
			// if so, push into the vecCorrelation
			double resRatio = 0;  // reserved ratio
			bool bCorr = true;
			vector< vector<double> >::iterator vecIter;
			for(vecIter = m_vecCoeff.begin(); (vecIter+1) != m_vecCoeff.end(); vecIter++)
			{
				if(0 == resRatio)
				{
					resRatio = (*vecIter)[i] / (*vecIter)[j];
				}

				double ratio = (*vecIter)[i] / (*vecIter)[j];

				if(fabs(ratio-resRatio) >= 0.05)
				{
					// not correlated!
					bCorr = false;
					break;
				}
			}

			if(bCorr)
			{
				// this i,j pair is correlated
				vecCorrelation.push_back(make_pair((i+1),(j+1)));
			}*/


		}
	}

	vector< pair<int, int> >::iterator vecIter;	

	set<int> setCorr;
	set<int>::iterator setIter;
	// for test
	for(vecIter = vecCorrelation.begin(); vecIter != vecCorrelation.end(); vecIter++)
	{
// 		cout << (*vecIter).first << "  " << (*vecIter).second << " are correlated!" << endl;
		setCorr.insert((*vecIter).first);
		setCorr.insert((*vecIter).second);
	}

	cout << "# corr gates: " << setCorr.size() << endl;
	cout << "corr gates:";

	for(setIter = setCorr.begin(); setIter != setCorr.end(); setIter++)
	{
		cout << *setIter << " ";
	}

	cout << endl;

}

void GLC::detectSegCorrelation(set<int> &setCorr, int segIter)
{
	set<int> segGatesInSegment;
	set<int>::iterator setSGIter;
	// tree circuit segmentation [9/26/2009 shengwei]
// 	listAllGatesOnPath(m_vecPrimaryOutput[segIter], segGatesInSegment);
	listGatesInSegment(m_vecSegOutput[segIter], segGatesInSegment);

	vector<int> vecGatesInSegment;  // with gate id
	for(setSGIter = segGatesInSegment.begin(); setSGIter != segGatesInSegment.end(); setSGIter++)
	{
		vecGatesInSegment.push_back(getGateId(*setSGIter));
	}


	vector< pair<int, int> >  vecCorrelation;

	// count the number of different ratios

	for(unsigned int i = 0; i < segGatesInSegment.size(); i++)
	{
		for(unsigned int j = i+1; j < segGatesInSegment.size(); j++)
		{
			// check whether Gate i and Gate j are correlated
			// if so, push into the vecCorrelation
			double resRatio = 0;  // reserved ratio
			bool bCorr = true;
			vector< vector<double> >::iterator vecIter;
			set<double> setRatio;

			for(vecIter = m_vecCoeff.begin(); (vecIter+1) != m_vecCoeff.end(); vecIter++)
			{
/*
				//////////////////////////////////////////////////////////////////////////
				// aggresive correlation detection
				//////////////////////////////////////////////////////////////////////////
				
				if(0 == resRatio)
				{
					resRatio = (*vecIter)[i] / (*vecIter)[j];
				}

				double ratio = (*vecIter)[i] / (*vecIter)[j];
				if(fabs(ratio-resRatio) >= 0.05)
				{
					// not correlated!
					bCorr = false;
					break;
				}

				if(bCorr)
				{
					// this i,j pair is correlated
					vecCorrelation.push_back(make_pair(vecGatesInSegment[i],vecGatesInSegment[j]));
				}
*/

				
				//////////////////////////////////////////////////////////////////////////
				// conservative correlation detection
				//////////////////////////////////////////////////////////////////////////

				double ratio = (*vecIter)[i] / (*vecIter)[j];

				if(setRatio.end() == setRatio.find(ratio))
				{
					setRatio.insert(ratio);
				}


			}

			if(/*bCorr*/setRatio.size() <= 2)
			{
				// this i,j pair is correlated
				vecCorrelation.push_back(make_pair(vecGatesInSegment[i],vecGatesInSegment[j]));
			}


		}
	}

	vector< pair<int, int> >::iterator vecIter;	

// 	set<int> setCorr;
	set<int>::iterator setIter;
	// for test
	for(vecIter = vecCorrelation.begin(); vecIter != vecCorrelation.end(); vecIter++)
	{
		// 		cout << (*vecIter).first << "  " << (*vecIter).second << " are correlated!" << endl;
		setCorr.insert((*vecIter).first);
		setCorr.insert((*vecIter).second);
	}

	m_fCorrOut << "corr gates - Seg" << segIter+1 << ":" << endl;;

	for(setIter = setCorr.begin(); setIter != setCorr.end(); setIter++)
	{
		m_fCorrOut << *setIter << " ";
	}

	m_fCorrOut << endl;



}

// detect bad segment (insufficient number of equations)
// put bad segments into m_arrBadSeg
bool GLC::isBadSeg(int segIter)
{
	vector<string> vecCoeffComb;
	vector<string>::iterator vecStrIter;

	// convert coefficient to a string
	vector< vector<double> >::iterator iterVecCoeff;
	for(iterVecCoeff = m_vecCoeff.begin(); iterVecCoeff != m_vecCoeff.end(); iterVecCoeff++)
	{
		string strCoeffComb;
		vector<double>::iterator iterVecDouble;
		for(iterVecDouble = iterVecCoeff->begin(); iterVecDouble != iterVecCoeff->end(); iterVecDouble++)
		{
			// put the double(only the integer part) into the string
			char str[32];
			itoa((int)(*iterVecDouble), str, 10);
			strCoeffComb += str;
		}

		bool bIn = false;
		for(vecStrIter = vecCoeffComb.begin(); vecStrIter != vecCoeffComb.end(); vecStrIter++)
		{
			if(0 == strcmp((*vecStrIter).c_str(), strCoeffComb.c_str()))
			{
				bIn = true;
				break;
			}
		}

		if(!bIn)
		{
 			vecCoeffComb.push_back(strCoeffComb);
		}
	}
	
	int nCoeffComb = vecCoeffComb.size();
	int nVar = m_nGatesInSeg[segIter];

	if(nCoeffComb<nVar)
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

// find the corresponding PI for the specific PO
void GLC::findPrimaryInputs(int iOutput, set<int> &setPI)
{
	set<int> setGates;
	set<int>::iterator setIter;
	listAllGatesOnPath(iOutput, setGates);

	// for each gate
	for(setIter = setGates.begin(); setIter != setGates.end(); setIter++)
	{
		map<int, GateNode>::iterator mapGCIter = m_mapGlobalCircuit.find(*setIter);
		if(m_mapGlobalCircuit.end() != mapGCIter)
		{
			map<int, int> mapInputSignal = mapGCIter->second.mapInputSignal;
			map<int, int>::iterator mapISIter;
			for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
			{
				if(isPrimaryInput(mapISIter->first))
				{
					// insert the PI into vecPI
					setPI.insert(mapISIter->first);
				}
			}
		}
	}
}

bool GLC::isPrimaryInput(int iInput)
{
	if(m_mapPrimaryInputSignal.end() != m_mapPrimaryInputSignal.find(iInput))
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

// check whether a pin is a primary output
bool GLC::isPrimaryOutput(int pin)
{
	vector<int>::iterator mapPOIter = find(m_vecPrimaryOutput.begin(), m_vecPrimaryOutput.end(), pin);

	if(m_vecPrimaryOutput.end() != mapPOIter)
	{
		return true;
	}
	else
	{
		return false;
	}
}



// getNames
string GLC::getBenchmarkName(int iBenchmark)
{
	return g_arrBenchmarkName[iBenchmark];
}

string GLC::getErrorModelName(int iErrorModel)
{
	return g_arrErrorModelName[iErrorModel];
}
string GLC::getPIStrategyName(int iPIStrategy)
{
	return g_arrPIStrategyName[iPIStrategy];
}
string GLC::getNormName(int iNorm)
{
	return g_arrNormName[iNorm];
}



// set global signal for the specific primary input
void GLC::setGlobalSignal()
{
	m_mapGlobalSignal.clear();

	// read primary input
	map<int, int>::iterator mapIter;
	for(mapIter = m_mapPrimaryInputSignal.begin(); mapIter != m_mapPrimaryInputSignal.end(); mapIter++)
	{
		// update global signal
		m_mapGlobalSignal.insert(make_pair(mapIter->first, mapIter->second));
	}

	set<int> setGates;
	set<int>::iterator setIter;


	while(!allOutputSatisfied())  // while not all output pins are done
	{
		// set signal for the gates
		// try to set the signals using known inputs (m_mapGlobalSignal)
		map<int, GateNode>::iterator mapGCIter;
		for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
		{
			int gateId = mapGCIter->second.gateId;
			map<int, int> *pMapInputSignal = &(mapGCIter->second.mapInputSignal);
			map<int, int>::iterator mapISIter; // for mapInputSignal
			for(mapISIter = pMapInputSignal->begin(); mapISIter != pMapInputSignal->end(); mapISIter++)
			{
				if(m_mapGlobalSignal.end() != m_mapGlobalSignal.find(mapISIter->first))
				{
					mapISIter->second = m_mapGlobalSignal.find(mapISIter->first)->second;
				}
			}

			// if this gate is fully satisfied (all the input signals are available), calculate output and update m_mapGlobalSignal
			bool bFullSatisfied = true;
			for(mapISIter = pMapInputSignal->begin(); mapISIter != pMapInputSignal->end(); mapISIter++)
			{
				if(-1 == mapISIter->second)
				{
					bFullSatisfied = false;
				}

			}

			if(bFullSatisfied)
			{
				// for debug
				int iTemp = mapGCIter->second.gateType;

				int outputSignal = gateFunction(mapGCIter->second.gateType, *pMapInputSignal);

				// set output signal into gateNode [10/11/2009 shengwei]
				mapGCIter->second.iOutputSignal = outputSignal;

				// set the output signal to the output pin (m_mapGlobalSignal)
				m_mapGlobalSignal[mapGCIter->first] = outputSignal;

				// add delay [1/14/2010 shengwei]
				// the largest delay among all the inputs
				double maxDelay = 0;
				map<int, int>::iterator mapISIter;
				for(mapISIter = mapGCIter->second.mapInputSignal.begin(); mapISIter != mapGCIter->second.mapInputSignal.end(); mapISIter++)
				{
					double delay = getDelay(mapISIter->first);
					if(delay > maxDelay)
					{
						maxDelay = delay;
					}					
				}
				// TODO: set delay for this gate
				double delayThisGate = g_arrRealSize[gateId-1] * g_arrVdd[gateId-1] / pow((g_arrVdd[gateId-1]-g_arrVth[gateId-1]), 1.4);/*(g_arrVdd[gateId-1]-g_arrVth[gateId-1]) / (g_arrVdd[gateId-1]-g_arrVth[gateId-1]);*/
				// mapGCIter->second.delay is the accumlated delay up to this gate (the output of this gate)
				mapGCIter->second.delay = maxDelay+delayThisGate;
			}			
		}


	}




}

// get signal for the specific pin [10/19/2009 shengwei]
int GLC::getSignal(int pin)
{
	int ret = 1;

	// check primary input
	map<int, int>::iterator mapPIIter;
	mapPIIter = m_mapPrimaryInputSignal.find(pin);
	if(m_mapPrimaryInputSignal.end() != mapPIIter)
	{
		ret = mapPIIter->second;
	}
	else
	{
		// not in the primary input
		// check output signal of each gate
		map<int, GateNode>::iterator mapGCIter;
		mapGCIter = m_mapGlobalCircuit.find(pin);
		if(m_mapGlobalCircuit.end() != mapGCIter)
		{
			ret = mapGCIter->second.iOutputSignal;
		}
	}

	return ret;
}



// for segmentation
void GLC::setPIGatesMap()
{
	// set up PI-Gates map

	map<int, GateNode>::iterator mapGCIter;
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		// for each gate
		// search, unitl find all the primary inputs associating with it
		
		// setPI for this gate
		set<int> setPI;
		set<int>::iterator setPIIter;
		findPrimaryInputs(mapGCIter->first, setPI);		

		// for each PI in the setPI
		for(setPIIter = setPI.begin(); setPIIter != setPI.end(); setPIIter++)
		{

			// insert into m_mapPIGates
			map<int, set<int> >::iterator mapPIGIter;
			mapPIGIter = m_mapPIGates.find(*setPIIter);
			if(m_mapPIGates.end() != mapPIGIter)
			{
				// already has the PI
				// insert mapGCIter->first into the set
				mapPIGIter->second.insert(mapGCIter->first);
			}
			else
			{
				// doesn't have the PI yet
				set<int> setGates;
				setGates.insert(mapGCIter->first);
				m_mapPIGates.insert(make_pair(*setPIIter, setGates));
			}

		}

	}

	// for test

	m_fDebugOut << "///////////////PI-Gate Map: //////////////////" << endl;
	map<int, set<int> >::iterator mapPIGIter;
	for(mapPIGIter = m_mapPIGates.begin(); mapPIGIter != m_mapPIGates.end(); mapPIGIter++)
	{
		m_fDebugOut /*<< "PI " */<< mapPIGIter->first << ", ";
		m_fDebugOut << mapPIGIter->second.size() << ", ";
		set<int>::iterator setIter;
		for(setIter = mapPIGIter->second.begin(); setIter != mapPIGIter->second.end(); setIter++)
		{
			m_fDebugOut << getGateId(*setIter) << "  ";
		}
		m_fDebugOut << endl;
	}

}

// check if all the output signals have been set
bool GLC::allOutputSatisfied()
{
	bool bRet = true;
	vector<int>::iterator vecPOIter;
	for(vecPOIter = m_vecPrimaryOutput.begin(); vecPOIter != m_vecPrimaryOutput.end(); vecPOIter++)
	{
		if(m_mapGlobalSignal.end() == m_mapGlobalSignal.find(*vecPOIter))
		{
			bRet = false;
			break;
		}
	}

	return bRet;
}

// get the gate set for this segment
void GLC::getSegGates(set<int> &setGatesInSegment, int startPIIndex, int endPIIndex)
{
	// for each PI
	for(int pIIndex = startPIIndex; pIIndex <= endPIIndex; pIIndex++)
	{
		int pi = m_vecPrimaryInput[pIIndex];
		map<int, set<int> >::iterator mapPIGIter;
		mapPIGIter = m_mapPIGates.find(pi);
		if(m_mapPIGates.end() != mapPIGIter)
		{
			set<int> tempSetGates = mapPIGIter->second;
			setGatesInSegment.insert(tempSetGates.begin(), tempSetGates.end());
		}
	}


}

// get gate id from the gate output (the key of the gatd)
int GLC::getGateId(int iGateOutput)
{
	int iRet = 0;
	map<int, GateNode>::iterator mapGCIter;
	mapGCIter = m_mapGlobalCircuit.find(iGateOutput);
	if(m_mapGlobalCircuit.end() != mapGCIter)
	{
		iRet = mapGCIter->second.gateId;
	}

	return iRet;
}

// output real gate sizes for segmentation
void GLC::outputSegRealGateSizes(int segIter)
{
	set<int> setGatesInSegment;
	set<int>::iterator setIter;
	listAllGatesOnPath(m_vecPrimaryOutput[segIter], setGatesInSegment);

// 	cout << "Seg" << segIter+1 << "\n"; 

	for(setIter = setGatesInSegment.begin(); setIter != setGatesInSegment.end(); setIter++)
	{
		int gateId = getGateId(*setIter);
		double realGateSize = g_arrRealSize[gateId-1];

		cout << realGateSize << ", "; 
	}

	cout << endl;

}

// do segmentation (fill up m_vecSegOutput)
void GLC::doSegmentation()
{

	vector<int>::iterator vecPOIter;
	int iSegIter = 0;

	switch(m_iSegStrategy)
	{
	case SEG_OUTPUT_CONE:

		// test: output cone information
		m_fDebugOut << "///////////////Output Cone: //////////////////" << endl;
		// for each primary output (output cone)
		for(vecPOIter = m_vecPrimaryOutput.begin(); vecPOIter != m_vecPrimaryOutput.end(); vecPOIter++)
		{
			iSegIter++;
			// find all the associated PIs

// 			// 1. # of PIs
// 			set<int> setPI;
// 			findPrimaryInputs(*vecPOIter, setPI);
// 			m_fDebugOut << setPI.size() << ", ";
// 
// 			// 2. PIs
// 			set<int>::iterator setPIter;
// 			for(setPIter = setPI.begin(); setPIter != setPI.end(); setPIter++)
// 			{
// 				m_fDebugOut << *setPIter << "  ";
// 			}
// 			m_fDebugOut << ", ";
			
			// 3. # of gates
			set<int> setGates;
			listAllGatesOnPath(*vecPOIter, setGates);
// 			m_fDebugOut << setGates.size() << ", ";
// 
// 
// 			// 4. gate ids
			set<int>::iterator setGIter;
// 			for(setGIter = setGates.begin(); setGIter != setGates.end(); setGIter++)
// 			{
// 				m_fDebugOut << m_mapGlobalCircuit.find(*setGIter)->second.gateId << "  ";
// 			}
// 			m_fDebugOut << endl;
// 

			// 5. gate outputs
			m_fDebugOut << "Gate Outputs - Seg " << iSegIter << endl;
			for(setGIter = setGates.begin(); setGIter != setGates.end(); setGIter++)
			{
				m_fDebugOut << m_mapGlobalCircuit.find(*setGIter)->second.gateId << ",  " << m_mapGlobalCircuit.find(*setGIter)->first << endl;
			}
			m_fDebugOut << endl;

// 			// test seg16 [10/9/2009 shengwei]
// 			if(segI)
		}

		// try segmentation using each output cone
		m_vecSegOutput = m_vecPrimaryOutput;


		break;

	case SEG_TREE_CIRCUIT:

		// init m_vecSegOutput [9/26/2009 shengwei]
		// PO
		vector<int>::iterator vecPOIter;
		for(vecPOIter = m_vecPrimaryOutput.begin(); vecPOIter != m_vecPrimaryOutput.end(); vecPOIter++)
		{
			m_vecSegOutput.push_back(*vecPOIter);
		}

		// MF(Multiple Fanout) Gates
		// for each gate
		map<int, GateNode>::iterator mapGCIter;
		for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
		{
			set<int> setFanoutGates;
			listFanoutGates(mapGCIter->first, setFanoutGates);
			if(setFanoutGates.size() > 1)
			{
				// this gate is MF gate, insert into m_vecSegOutput
				m_vecSegOutput.push_back(mapGCIter->first);
			}
		}
		break;

	}

}

// find the fanout gates for specific output
void GLC::listFanoutGates(int iOutput, set<int> &setFanoutGates)
{
	map<int, GateNode>::iterator mapGCIter;
	
	// for each gate
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		// if one of the inputs is the iOutput, then insert it into the setFanoutGates
		map<int, int> mapInputSignal = mapGCIter->second.mapInputSignal;
		map<int, int>::iterator mapISIter;
		for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
		{
			if(iOutput == mapISIter->first)
			{
				setFanoutGates.insert(mapGCIter->first);
				break;
			}
		}
	}
}

// list all the gates in one segment  (segOutput = m_vecSegOutput[segIter])
void GLC::listGatesInSegment(int segOutput, set<int> & setGates)
{
	setGates.clear();

	map<int, GateNode>::iterator mapIter;
	mapIter = m_mapGlobalCircuit.find(segOutput);
	if(m_mapGlobalCircuit.end() != mapIter)
	{
		// start search on the path
		map<int, int> mapInputSignal = mapIter->second.mapInputSignal;
		map<int, int>::iterator mapIter;

		// add this gate (the lowest level gate)
		setGates.insert(segOutput);

		// add all the upper level gates on the path, stop at MF gates
		for(mapIter = mapInputSignal.begin(); mapIter != mapInputSignal.end(); mapIter++)
		{
			// for each input signal
			set<int> setTempGates;
			set<int>::iterator setTempIter;

			// if it is not a MF gate  (equivalent to checking whether it is in m_vecSegOutput)
			bool bMFGate = false;
			// if found, set true
			vector<int>::iterator vecSOIter;
			for(vecSOIter = m_vecSegOutput.begin(); vecSOIter != m_vecSegOutput.end(); vecSOIter++)
			{
				if(mapIter->first == *vecSOIter)
				{
					bMFGate = true;
					break;
				}
			}

			if(!bMFGate)
			{

				// each input is also an output for the upper level gate
				listGatesInSegment(mapIter->first, setTempGates);

				for(setTempIter = setTempGates.begin(); setTempIter != setTempGates.end(); setTempIter++)
				{
					setGates.insert(*setTempIter);
				}

			}

		}
	}

}

// list all the PIs in one segment  (segOutput = m_vecSegOutput[segIter])
void GLC::listPIInSegment(int segOutput, set<int> & setPI)
{
	set<int> setGates;
	set<int>::iterator setIter;
	listGatesInSegment(segOutput, setGates);

	// for each gate
	for(setIter = setGates.begin(); setIter != setGates.end(); setIter++)
	{
		map<int, GateNode>::iterator mapGCIter = m_mapGlobalCircuit.find(*setIter);
		if(m_mapGlobalCircuit.end() != mapGCIter)
		{
			map<int, int> mapInputSignal = mapGCIter->second.mapInputSignal;
			map<int, int>::iterator mapISIter;
			for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
			{
				if(isPrimaryInput(mapISIter->first))
				{
					// insert the PI into vecPI
					setPI.insert(mapISIter->first);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// piecewise linear approximation
void GLC::listOptBreakPoints(double u[MAX_N])
{
//  	findOptBreakPoints(m_dLowerDP, m_dUpperDP, m_nBreakPoints, u);
// 	tryArbitrary(m_nBreakPoints, u);
	double val = 0;
// 	for(int i = 0; i < m_nBreakPoints; i++)
// 	{
// 		val += m_dErrorRate*3/(m_nBreakPoints+1);
// 		u[i] = val;
// 	}

// 2 break points
// 	if(0.01 == m_dErrorRate)
// 	{
// 		u[0] = 0.0187;
// 		u[1] = 0.0255;
// 	}
// 	else if(0.05 == m_dErrorRate)
// 	{
// 		u[0] = 0.09;
// 		u[1] = 0.1275;
// 	}
// 	else if(0.10 == m_dErrorRate)
// 	{
// 		u[0] = 0.18;
// 		u[1] = 0.255;
// 	}
	// 0.01 error
//     	u[0] = 0.0187;
//     	u[1] = 0.0255;

	// 0.05 error

	// 0.10 error
	// 3 break pionts
  	u[0] = 0.0135;
  	u[1] = 0.021;
  	u[2] = 0.0255;

// 	u[0] = 0.01;
// 	u[1] = 0.02;
}

// set m_dMinError, m_dMaxError, m_dModeError error for piecewise linear approximation
void GLC::setErrorForPL()
{
	
}

// calculate real power for all the gates (assume all the input signals are 0...)
double GLC::calcRealPower()
{

	double realPower = 0;
	map<int, GateNode>::iterator mapGCIter;
	unsigned int iCount = 0;
	double measurePower = 0;

	vector<double> vect;


	// left side of the constraint (exactly the same with NORM_SUM)
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		iCount++;

		// for each gate
		int gateType = mapGCIter->second.gateType;
		map<int, int> mapInputSignal = mapGCIter->second.mapInputSignal;
// 		map<int, int>::iterator mapISIter;
// 		int signalIndex = mapInputSignal.begin()->second;
// 		for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
// 		{
// 			if(mapInputSignal.begin() == mapISIter)
// 			{
// 				continue;
// 			}
// 			signalIndex = signalIndex*2 + mapISIter->second;
// 		}

		switch(m_iErrorModel)
		{
		case ERROR_UNIFORM:
			realPower += g_arrLookupTable[gateType][mapInputSignal.size()-1][0] * g_arrRealSize[iCount-1];

			break;

		case ERROR_GAUSSIAN:
			realPower += g_arrLookupTable[gateType][mapInputSignal.size()-1][0] * g_arrRealSize[iCount-1];
			break;

		case ERROR_TRIANGULAR:
			realPower += g_arrLookupTable[gateType][mapInputSignal.size()-1][0] * g_arrRealSize[iCount-1];
			break;

		}
	}

	return realPower;

}

double GLC::density(double x)
{
	double iRet = 0;
	switch(m_iErrorModel)
	{
	case ERROR_TRIANGULAR:
		iRet = 4*(m_dMaxError-x) / (2*m_dMaxError*m_dMaxError);

		break;
	}

	return iRet;
}

void GLC::initMLE(double realPower)
{

// 	m_realPower = calcRealPower();


	// init 
	m_dMinError = 0/*-3 * m_dErrorRate*/;
	m_dModeError = 0;
	m_dMaxError = 3*m_dErrorRate;

	m_nBreakPoints = 3;

	// piecewise linear related
	m_deltaDP = 0.05*m_dMaxError;

	m_dLowerDP = m_dMinError + m_deltaDP;
	m_dUpperDP = m_dMaxError - m_deltaDP;

	//PL_Init(m_dMinError, m_dMaxError, m_dModeError, m_deltaDP);

}

// do post-processing
/*
REAL * GLC::doPostProcessing(REAL *var)
{
	// post processing: weighted L1 norm
	// generate weight


		double weight[MAX_NUM_INSTANCE];
		generateWeight(var, weight);
	
		m_fLPout.close();
		m_fLPout.open("newOF.lp");
	
		m_fLPout << "min: ";
		for(int i = 0; i < m_nInputInstance; i++)
		{
			m_fLPout << weight[i] << " abs" << (i+1);
			if(i != m_nInputInstance-1)
			{
				m_fLPout << " + ";
			}
		}
		m_fLPout << ";" << endl;
	
		//	glc.m_newOFOut.close();
	
		for(int i = 0; i < m_nInputInstance; i++)
		{
			// set signal for each pin according to primary input
			readPrimaryInputSignal(i);
	
			setGlobalSignal();  // result in m_mapGlobalCircuit
	
			// generate LP constraints
			generateOneLPConstraint(i+1);
		}

		// solve the new LP
		// read lp			
		m_plp = read_LP("newOF.lp", NORMAL, NULL);

		// redirect lp_solve generated info to a log file
		set_outputfile(m_plp, "lp_report.txt");

		// solve lp
		int ret = solve(m_plp);

		// retrieve the value of variables
		int nVars = 0;
		if(NORM_L1 == m_iNorm)
		{
			nVars = m_nInputInstance * 2 + m_nGates;
		}
		else if(NORM_MLE == m_iNorm)
		{
			nVars = m_nInputInstance * (m_nBreakPoints+1) + m_nGates;
		}
		REAL *newvar = new REAL[nVars];
		get_variables(m_plp, newvar);

// 		// retrieve the gate variables from all the variables
// 		// all the gates
// 		map<int, GateNode>::iterator mapGCIter;
// 		int iGateVarIndex = m_nInputInstance;
// 		if(NORM_L1 == m_iNorm)
// 		{
// 			iGateVarIndex = m_nInputInstance;
// 		}
// 		else if(NORM_MLE == m_iNorm)
// 		{
// 			iGateVarIndex = (m_nBreakPoints+1) * m_nInputInstance;
// 		}
// 
// 		for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
// 		{
// 			int gateId = getGateId(mapGCIter->first);
// 			cout << "x" << gateId << ", ";
// 			cout << var[iGateVarIndex++] << endl;
// 		}
// 
// 		delete []var;

		return newvar;

}*/


// for switching power
// set bSwitch bit in the m_mapGlobalCircuit
void GLC::getSwitchedGates(int index)
{
	// 
	m_nSwitchedGates = 0;
	readPrimaryInputSignal(index);
	setGlobalSignal();
	map<int, GateNode> mapGC1 = m_mapGlobalCircuit;

	readPrimaryInputSignal(index+1);
	setGlobalSignal();
	map<int, GateNode> mapGC2 = m_mapGlobalCircuit;

	// for saro [12/17/2009 shengwei]
/*
	double *pGenHeat = new double [m_nGates];  // generated heat for this execution
	for(int i = 0; i < m_nGates; i++)
	{
		pGenHeat[i] = 0.0;
	}

*/
	// compare mapGC1 and mapGC2, get the switched gates (set the bSwitch bit in m_mapGlobalCircuit)
	map<int, GateNode>::iterator mapGCIter1, mapGCIter2, mapGCIter;
	
	for(mapGCIter1 = mapGC1.begin(), mapGCIter2 = mapGC2.begin(), mapGCIter = m_mapGlobalCircuit.begin(); 
		mapGCIter1 != mapGC1.end(), mapGCIter2 != mapGC2.end(), mapGCIter != m_mapGlobalCircuit.end(); 
		mapGCIter1++, mapGCIter2++, mapGCIter++)
	{
		// reset bSwitch
		mapGCIter->second.bSwitch = false;

		int gateId = mapGCIter->second.gateId;
		int iOutputSignal1 = mapGCIter1->second.iOutputSignal;
		int iOutputSignal2 = mapGCIter2->second.iOutputSignal;
// 		cout << iOutputSignal1 << " " << iOutputSignal2 << endl;
		// only 1->0 counted for switch
		// temp change: 1->0 & 0->1 both counted for switch [1/15/2010 shengwei]
		if(/*iOutputSignal1 != iOutputSignal2*/1 == iOutputSignal1 && 0 == iOutputSignal2)
		{
			// label the bSwitch bit to be 1
			mapGCIter->second.bSwitch = true;

			m_nSwitchedGates++;
			
			// update m_mapSwitchCount
			map<int, int>::iterator mapSCIter;
			mapSCIter = m_mapSwitchCount.find(gateId);

			if(m_mapSwitchCount.end() == mapSCIter)
			{
				m_mapSwitchCount.insert(make_pair(gateId, 1));
			}
			else
			{
				// increase switch count
				mapSCIter->second++;
			}

			// for saro [12/17/2009 shengwei]
/*
			double swCoeff = g_arrSWDelayTable[mapGCIter->second.gateType][mapGCIter->second.mapInputSignal.size()-1];
			double gateSize = g_arrRealSize[mapGCIter->second.gateId-1];
 			pGenHeat[mapGCIter->second.gateId-1] +=  swCoeff * gateSize;
			if(swCoeff == 0)
			{
				cout << "!" << endl;
			}
			if(gateSize == 0)
			{
				cout << "!!" << endl;
			}
*/
		}
	}

	// for Saro [12/17/2009 shengwei]
	// output the generated heat for each gate in this execution
/*
	for(int i = 0; i < m_nGates; i++)
	{
		m_heatout << pGenHeat[i] << "\t";
	}
	m_heatout << endl;
	
	m_heatout.flush();

	delete [] pGenHeat;
*/

	// consider HTH [11/3/2009 shengwei]
	// set HTH variable to be switched
	if(m_bHTH)
	{
		mapGCIter = m_mapGlobalCircuit.find(0);
		if(m_mapGlobalCircuit.end() != mapGCIter)
		{
			mapGCIter->second.bSwitch = true;
			m_nSwitchedGates++;
		}
	}

	// output switching map for this switching
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		m_fSWMapOut << mapGCIter->second.bSwitch;
	}
	m_fSWMapOut << endl;

	m_fSWMapOut.flush();


}

// get switching PI vector pair for switching gate i [1/15/2010 shengwei]
void GLC::getPIPairForSwitching(int iGate)
{
	m_pairPI.first.clear();
	m_pairPI.second.clear();

	// first PI vector
	map<int, int> *pMapPI1 = &m_pairPI.first;
	map<int, int>::iterator mapPIIter1;
	// second PI vector
	map<int, int> *pMapPI2 = &m_pairPI.second;
	map<int, int>::iterator mapPIIter2;

	// PIs
	map<int, int>::iterator mapPIIter;

	// init to 0
	for(mapPIIter = m_mapPrimaryInputSignal.begin(); mapPIIter != m_mapPrimaryInputSignal.end(); mapPIIter++)
	{
		pMapPI1->insert(make_pair(mapPIIter->first, 0));
		pMapPI2->insert(make_pair(mapPIIter->first, 0));
	}

	// this gate
	map<int, GateNode>::iterator mapGCIter;
	mapGCIter = m_mapGlobalCircuit.find(iGate);	
	map<int, int> mapInputSignal = mapGCIter->second.mapInputSignal;
	map<int, int>::iterator mapISIter;

	// suppose iGate is a NAND_2
	// simplest case: both inputs are PI
	bool bAllPIs = true;  // whether all inputs are PIs
	for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
	{
		if(!isPrimaryInput(mapISIter->first))
		{
			bAllPIs = false;
			break;
		}
	}

	if(bAllPIs)
	{
		// change from 0 to 1
		for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
		{
			mapPIIter2 = pMapPI2->find(mapISIter->first);
			mapPIIter2->second = 1;
			//pMapPI2[mapISIter->first] = 1;
		}		
	}
	
	// output
	for(mapPIIter1 = pMapPI1->begin(); mapPIIter1 != pMapPI1->end(); mapPIIter1++)
	{
		cout << mapPIIter1->second;
	}
	cout << endl;

	for(mapPIIter2 = pMapPI2->begin(); mapPIIter2 != pMapPI2->end(); mapPIIter2++)
	{
		cout << mapPIIter2->second;
	}
	cout << endl;

	//TODO: 
	// 1. if not all inputs are PIs
	// 2. if other gate types

}

// print switch PI for all the gates
void GLC::printSwitchPI()
{
	getPIPairForSwitching(10);
	getPIPairForSwitching(11);
}


// set temperature for each gate (in m_mapGlobalCircuit), keep it there until next change
void GLC::setTemperature(int iterBit)
{
	// calculating temperature using another LP formulation
	// output: the dTempr field in m_mapGlobalCircuit

	//////////////////////////////////////////////////////////////////////////
	// Todo: calculate the temperature...
	// for now, set some constants [10/13/2009 shengwei]
	// C499, set the temperature of gate 81-120, in the precision of 0.1
// 	double rnd1 = 2*ran1(&m_seed[2]);
// 	double rnd2 = 2*ran1(&m_seed[2]);
	// read temperature [2/2/2010 shengwei]
// 	double arrTemp[MAX_NUM_GATES] = {111,124,126,133,118,72,81,76,70,81,24,23,25,25,27,129,123,119,129,126,78,71,79,77,77,26,24,21,19,24,130,133,119,130,130,64,66,59,83,78,28,17,24,26,27,111,124,126,133,118,72,81,76,70,81,24,23,25,25,27,129,123,119,129,126,78,71,79,77,77,26,24,21,19,24,79,76,75,58,66,45,57,54,55,61,22,28,33,25,22,83,76,71,71,65,59,48,52,52,51,26,27,22,22,29,82,98,76,67,69,54,49,48,51,48,26,27,25,25,25,79,76,75,58,66,45,57,54,55,61,22,28,33,25,22,83,76,71,71,65,59,48,52,52,51,26,27,22,22,29,23,24,26,24,31,22,22,28,26,26,32,27,29,27,19,22,23,28,31,22,28,18,22,24,34,32,30,27,29,24,25,27,26,27,17,28,24,28,22,24,30,25,22,22,25,23,24,26,24,31,22,22,28,26,26,32,27,29,27,19,22,23,28,31,22,28,18,22,24,34,32,30,27,29,24};
// 	double arrTemp[MAX_NUM_GATES] = {19,25,24,23,27,47,56,44,50,45,22,17,23,21,27,19,26,22,20,21,56,51,48,53,50,20,22,25,23,24,20,29,26,21,30,47,55,56,53,49,22,33,22,20,23,19,25,24,23,27,47,56,44,50,45,22,17,23,21,27,19,26,22,20,21,56,51,48,53,50,20,22,25,23,24,46,46,49,46,47,70,73,78,74,65,41,56,50,44,47,41,55,49,46,41,76,69,73,73,75,52,47,48,63,50,41,49,48,52,40,74,82,71,77,82,49,49,52,53,43,46,46,49,46,47,70,73,78,74,65,41,56,50,44,47,41,55,49,46,41,76,69,73,73,75,52,47,48,63,50,22,19,24,26,21,43,51,50,45,45,32,20,27,25,24,28,23,19,29,23,47,48,54,44,53,28,23,26,24,23,22,24,25,25,22,44,51,48,53,52,18,23,23,29,23,22,19,24,26,21,43,51,50,45,45,32,20,27,25,24,28,23,19,29,23,47,48,54,44,53,28,23,26,24,23};

	// chip 1
// 	double arrTemp[MAX_NUM_GATES] = {141.083397,138.923272,157.875744,159.176178,150.19299,76.2774,81.6156,67.82398,60.440185,68.9733,25.101,25.8685,22.28184,26.69674,32.25525,142.176177,139.7493,148.0701,161.36292,185.5635,48.363966,41.189005,40.15674,43.25185,43.636901,15.235846,13.266725,13.992056,11.37996,15.265096,44.379252,48.82416,40.85552,53.35892,61.146909,21.4329,24.282132,33.770808,17.53479,18.80944,16.52563,16.38846,21.805794,21.266532,18.47199,108.257415,103.791995,125.867144,130.871862,126.576402,71.649076,77.890086,61.8709,60.36433,59.688552,22.242875,22.598275,20.931141,26.619292,27.502672,107.670277,105.39909,123.31665,116.58182,135.68363,60.272424,47.489715,60.621054,54.601985,62.485115,20.318966,18.736025,17.521712,12.88446,18.104912,48.09952,52.392725,45.42825,44.766078,57.172748,56.78493,60.21964,54.8035,60.95898,56.69594,24.88134,28.99368,26.53398,32.5008,21.00258,69.932018,82.8456,66.413964,72.48657,77.512468,45.5199,51.974725,43.4511,38.419941,50.833272,14.586792,17.52741,14.339808,12.64214,14.517998,44.086172,57.264273,56.826614,42.822225,39.868264,35.425677,35.035784,37.264864,32.557558,34.347236,21.65926,14.99796,18.087862,14.7216,16.895944,28.03381,29.88498,20.465025,22.092478,21.903808,15.37229,14.016912,18.0856,13.986054,17.50231,13.140424,13.311168,13.00706,16.89285,9.950904,44.470921,42.7044,39.4272,40.561443,41.86756,51.9005,63.8418,45.0975,51.22461,64.46944,26.14296,34.2183,25.81704,21.1158,25.58864,78.20065,61.37325,63.81672,73.90614,78.98696,132.49582,150.8724,159.453,142.57782,144.20214,29.970507,29.2491,36.11685,22.528001,36.209085,29.378102,30.437326,24.79796,22.241352,31.427825,22.803664,29.42544,22.123679,34.112409,30.787484,10.122756,11.58533,11.310158,14.8203,13.055439,16.7863,14.43402,20.075692,17.392705,13.241144,38.506326,30.477415,33.152296,34.559616,29.127895,26.95686,23.37885,22.841577,21.216396,22.94776,30.759245,26.092125,20.98129,30.15528,26.88319,63.650032,61.666815,66.014865,63.246546,57.397713,37.574097,28.53173,37.12332,31.566128,36.56312,26.437923,24.448034,24.076624,25.0425,33.707408,71.79952,52.432416,60.694295,64.321503,53.453414,20.735274,20.138984,21.929396,25.18318,18.6099};


	// chip 2
// 	double arrTemp[MAX_NUM_GATES] = {201.12246,191.99653,233.79967,228.19968,213.96948,109.2994,123.67485,99.9096,98.14675,91.35258,23.630575,24.21485,22.62078,26.163452,28.427308,142.474904,129.24117,166.0545,151.03992,169.79039,49.106007,33.831265,43.269798,33.55829,50.730988,19.640569,13.317325,15.589002,13.69688,18.296152,121.343346,110.12352,109.49231,96.323496,106.665174,44.5344,58.051224,65.940816,40.66671,39.55458,13.260984,9.08928,12.411387,16.51734,11.514513,81.792417,72.157402,82.438707,92.207484,97.892007,63.772208,70.802667,53.6823,50.577345,47.29197,20.91655,18.23765,17.979465,23.08278,21.461392,100.25587,100.059165,121.8306,123.44236,134.62504,54.763614,45.509945,54.57192,50.221793,58.208535,35.92781,27.4575,30.4785,23.4242,32.01632,78.785,104.64775,81.705,81.77296,93.993,34.892988,29.405272,32.98285,35.714952,30.536653,14.489552,16.027344,13.167418,22.194,12.363858,38.764083,46.17544,44.79146,41.179407,40.46741,30.92455,27.98873,20.311788,24.266362,32.649064,18.448872,25.05675,19.344216,16.27278,17.799342,55.272222,64.024042,64.182904,66.344625,59.714132,23.646268,29.449245,28.473088,25.296528,28.423185,15.346128,9.09302,12.391626,11.015976,12.219792,33.32833,52.30968,38.413875,43.744434,46.235208,25.732547,23.826972,30.8492,26.619192,26.819464,14.604194,12.793632,11.722502,18.0246,10.192608,41.085641,49.92504,40.198576,46.757298,57.37704,32.02945,43.277135,31.95108,33.729974,40.011384,16.985592,21.63576,16.50372,13.0626,15.276536,34.550615,25.300575,28.416454,30.931704,35.700906,57.131132,74.67096,60.687165,57.641364,67.263486,61.3632,52.5665,56.83932,47.53478,61.05385,49.37726,47.55434,47.27932,50.75742,67.32018,123.41784,113.81088,116.34674,135.53877,115.39108,52.48614,51.3705,55.08914,59.76905,59.27577,42.77925,42.62223,46.360184,50.278745,42.273728,76.600776,100.47918,77.61768,94.187808,76.12471,39.337872,39.81325,36.208753,36.465626,34.74262,45.304875,29.81844,35.19253,42.186129,38.584901,75.561524,101.86386,88.370625,89.100198,92.703482,43.96863,38.174894,43.601033,41.253813,45.3354,40.810711,40.489936,39.11116,40.610514,54.369503,95.334408,99.5568,104.37196,104.74404,91.049644,37.067646,45.34726,40.84731,55.64515,46.475841};

	// chip 3
// 	double arrTemp[MAX_NUM_GATES] = {99.751848,94.796332,107.633396,112.603122,98.676036,47.114528,55.640763,48.88667,44.64876,45.920094,14.511725,15.866575,12.749268,17.573892,18.721646,99.526141,91.709955,97.48755,89.844732,100.8304,40.056606,35.568325,42.245268,35.587586,33.4719,14.455891,10.993425,13.321542,10.66828,13.401164,75.971682,76.20512,73.468915,74.516848,71.982522,42.543375,43.579032,55.424756,35.19999,31.893875,20.354818,19.5562,27.23058,25.069884,20.833055,135.447564,135.904192,160.368494,155.278296,155.419161,56.710136,67.852566,52.38387,52.86333,57.260544,20.931975,20.317875,17.656716,21.413672,22.562029,80.303652,72.58842,77.7501,96.301232,109.27685,51.020199,36.608325,43.938258,35.475763,45.474429,13.236528,12.210075,13.734422,10.29324,13.658008,37.85376,49.065825,32.996325,35.719578,34.572248,53.19648,55.76792,53.077,57.45114,51.48283,22.6996,24.18384,23.46036,32.4729,19.03878,95.09811,109.488,95.62428,94.04907,101.64474,62.35,71.20245,52.83684,59.10006,72.42704,19.252944,22.63608,18.765048,16.04888,16.842254,58.186914,67.94297,66.91377,55.888875,47.150384,40.557614,41.78377,44.367624,37.012641,44.038995,22.0717,15.29702,19.83293,20.81904,20.775586,39.78107,55.655025,47.5413,45.66392,40.127848,26.25467,33.550348,29.40385,35.118738,29.824487,9.065716,10.256328,12.164394,15.67413,8.438058,29.791245,39.228,31.131726,29.390826,38.683582,32.0901,37.55928,28.069986,26.30955,36.187704,16.333464,19.17093,16.515288,13.18916,13.199824,28.824455,26.534205,25.277598,28.439742,33.396519,53.501698,53.456025,57.95769,60.595623,51.746881,59.38899,59.23466,67.53154,53.91727,70.4957,59.64347,53.8683,52.0388,54.42612,70.37756,129.4748,113.35104,128.61095,135.20799,117.20702,49.28868,51.3636,54.49574,69.1999,61.75998,49.7651,56.54982,53.55064,58.575,60.72528,101.61495,120.8949,97.98372,108.64568,107.00849,43.482313,45.7443,39.01423,39.081186,36.532804,48.31255,41.86746,39.328666,47.828922,44.73942,97.27684,107.0622,111.888,107.60508,103.35447,54.72147,48.15924,54.21423,44.96467,59.301,51.94628,46.6969,48.94648,41.641782,58.056413,104.11648,93.742848,107.5217,115.93728,97.410824,25.049304,31.992264,31.891892,31.97095,32.328747};

// chip 4
// 	double arrTemp[MAX_NUM_GATES] = {252.59304,261.24216,305.05007,305.54334,289.13196,140.14628,145.91259,126.4746,120.69655,123.79554,44.872,43.27725,37.12065,45.28496,52.35979,245.70689,215.79345,240.69,267.91996,290.564,145.74654,116.32465,145.04646,121.41472,136.27999,52.99199,43.809,49.96368,38.7786,49.5544,248.38164,218.6624,236.6788,217.05736,244.01052,109.6515,123.70344,136.66876,98.2478,89.0682,32.61786,31.6334,42.55821,41.86532,36.59714,252.88998,212.76452,263.06371,247.78062,259.61361,122.47856,118.30212,113.0983,104.60255,110.2728,40.22325,38.358,34.61955,44.75044,45.47055,171.62002,145.7784,167.9805,178.9764,203.9269,98.14851,64.657775,89.49564,79.02583,77.78925,30.55614,25.94525,25.158874,19.23082,30.18624,60.85387,86.36935,74.7057,71.372926,78.2002,79.83843,94.23804,90.0765,95.68476,79.51867,36.00278,39.41376,35.64264,48.8304,29.01528,113.37919,122.2968,118.14914,120.35877,139.2647,86.047,87.59025,69.0627,69.39641,96.0708,17.071344,21.27963,14.312232,10.10348,15.582798,48.166968,52.656528,42.165328,41.586975,38.589592,32.322209,36.056993,30.60148,28.48811,26.047273,16.881312,13.25704,14.521832,17.28,17.565652,93.2848,104.2729,101.97825,107.37918,91.7472,56.87,63.44988,61.862,74.90124,66.0569,24.29152,22.597128,24.5135,30.2427,19.61856,81.27867,87.2192,72.582826,77.5491,85.09878,51.6815,57.3452,43.37844,42.400365,58.26856,25.06248,28.96281,21.708192,19.27334,23.29976,49.576835,41.994045,34.665508,48.306333,39.327484,91.592566,76.906305,94.33053,94.479165,93.243578,57.43161,48.42052,55.70565,48.8437,60.3647,46.60661,45.977552,51.95036,45.55404,61.81784,98.899632,95.498784,89.689722,86.867601,74.386802,37.002588,35.723094,41.684234,44.706915,40.754508,46.63425,40.927296,40.399216,54.159435,43.035944,78.088956,107.964415,77.08799,98.573696,85.240162,56.42062,62.401,53.17204,56.33666,49.87796,62.2534,52.8786,51.72378,53.86365,62.08632,169.76306,179.9469,182.38395,174.79638,157.22633,91.43535,81.98948,83.9891,71.2983,92.70965,83.47952,78.47232,75.88768,74.61846,96.55645,184.49704,156.5328,170.16321,198.0573,164.4979,44.56662,52.70772,56.32102,60.9433,60.64206};

// chip 5
// 	double arrTemp[MAX_NUM_GATES] = {213.5616,201.39988,247.03744,216.90018,212.09487,107.45184,113.91678,101.2452,89.2736,95.16078,30.7475,27.937,23.51601,33.92004,35.583,158.84726,158.2524,165.5985,166.4852,191.6801,98.44335,71.00145,86.76018,72.63403,85.61707,32.96749,30.51175,31.1064,23.2964,33.99452,168.45084,164.2928,153.170535,141.02384,163.36116,76.54725,86.53848,90.308948,75.3662,71.63845,15.260058,13.75946,17.895573,19.802944,16.984373,110.753622,102.901144,125.406788,106.108218,106.630011,19.514672,27.195831,20.54724,21.48055,19.99866,7.967425,7.8099,6.815802,8.168384,10.769962,25.972661,33.34824,20.35575,35.245016,31.29904,15.961293,17.38698,17.51217,12.48076,16.039947,5.676547,5.5678,5.30842,4.72304,7.056784,16.89534,18.658435,18.043725,17.784198,17.77184,39.618885,47.509852,45.44815,46.241496,40.073425,18.421084,22.816224,18.097838,27.67035,15.370182,51.294376,50.11448,47.366586,47.838666,50.206796,37.2981,33.0605,30.010764,27.788879,34.610688,34.45176,41.4639,34.09368,27.5544,29.7605,97.26219,117.72139,120.62141,103.39725,97.71736,56.23088,51.74547,64.51928,53.96117,51.65237,32.38284,22.2898,28.73962,27.9348,27.97054,52.03681,52.802935,46.934775,56.080234,56.758624,30.738893,32.647264,37.10375,32.435478,35.00119,11.220946,10.99452,12.833634,15.30504,8.014572,32.687406,37.09656,34.617422,37.78992,45.121648,42.707,49.71659,41.210358,38.324653,52.624488,19.946592,28.41981,23.190504,17.55396,19.905358,27.1568,23.057595,26.829822,27.214518,30.026037,46.266048,60.140535,60.989145,52.711164,56.570788,30.926349,33.468588,38.806547,26.092099,35.24873,34.761482,33.873112,26.405456,25.272828,39.016759,68.926728,61.467264,63.792844,66.862848,55.615196,27.465942,23.37375,29.494096,33.726825,29.763447,50.175,44.873676,53.38788,56.4146,52.839248,88.857912,108.076655,85.58841,107.60152,103.1917,57.3831,55.4895,53.74356,54.09462,47.62296,64.89505,50.77485,53.98836,59.75058,62.80871,105.48586,114.50145,105.1974,108.38817,103.57563,53.60406,44.20163,53.94764,41.758074,61.5087,36.871594,31.923678,28.537784,31.78875,40.59023,82.156152,65.991936,67.300921,78.939093,66.876278,28.070448,27.924438,25.015214,33.307065,29.016909};

// chip 6
// 	double arrTemp[MAX_NUM_GATES] = {248.33004,253.90852,317.99885,298.8981,302.53089,133.88236,131.74569,128.6152,122.3599,120.28302,33.491,39.16725,29.58312,38.82564,39.75494,191.85738,185.99625,217.6245,219.13768,266.3254,117.85824,98.04015,113.32308,85.60054,118.14418,39.62154,34.9075,37.67426,29.9442,35.75936,248.88708,215.672,234.51035,195.65912,218.93076,116.3895,129.03828,145.53296,109.7264,87.31385,27.72176,22.5754,31.20714,31.57252,31.58153,202.3749,174.31156,194.44633,211.36626,206.94327,114.50084,108.21357,91.8974,92.4144,84.88392,35.44075,37.9875,32.25495,36.83708,41.12838,85.579637,83.443365,80.1141,111.844932,95.05125,51.84891,34.545225,50.925732,37.771518,57.029742,41.9282,39.27125,36.89712,29.9516,42.79996,93.9169,118.88525,100.97325,113.89784,109.09116,90.02051,95.0092,85.789,99.9864,89.86159,43.01858,45.06336,41.60464,55.9683,31.21488,94.23901,99.6536,110.72398,88.23927,115.0173,65.387,79.3199,61.73916,53.10801,73.52912,16.261152,24.55995,19.645104,17.87928,18.897208,63.21627,63.443208,55.824804,54.736425,61.350144,32.143387,34.07558,31.9312,27.228275,33.16908,16.95106,9.58712,17.36397,11.423448,16.793244,69.80036,105.638,75.62325,86.5763,84.4132,51.48239,61.52172,53.6565,60.9687,53.3904,13.944018,14.432184,14.529328,23.8761,14.23566,45.330447,47.10952,56.701242,53.919774,56.815422,50.3535,57.90015,41.984838,41.318399,56.64848,25.23192,27.49239,25.52184,18.84702,24.93326,20.14133,11.72817,12.258264,8.227626,19.048359,36.243674,28.917525,19.23495,27.759204,32.649812,70.79565,63.8618,67.25594,62.36032,77.37125,59.55746,60.71172,53.10492,52.62138,77.65993,120.99256,109.31808,133.4674,119.23398,104.32688,49.18032,59.48858,57.46182,62.9332,55.58337,114.6625,116.79051,120.1954,132.37785,126.49336,225.20274,255.6841,228.28934,245.2632,229.79621,119.08146,112.639,100.80372,103.84868,97.63336,116.17815,101.6289,105.71398,117.55755,121.48289,116.70852,144.26475,145.5825,139.39299,141.08359,65.09538,65.1429,76.24421,53.18541,68.55805,69.99005,66.76118,56.23508,63.07098,89.87824,138.84104,134.5344,131.21479,153.83268,142.3842,24.860682,16.449324,26.949238,29.95751,22.359828};

// chip 7
// 	double arrTemp[MAX_NUM_GATES] = {296.47695,267.44434,360.15928,337.40388,315.3288,158.36728,169.32969,146.755,133.75115,125.96892,59.09375,63.1295,54.58383,72.3478,70.37778,356.20871,319.5585,360.0735,365.57924,413.0473,135.88722,103.33245,141.81492,119.97288,135.93272,45.78868,39.39225,43.22578,36.854,48.99944,322.44318,312.3536,334.7287,263.40344,310.24575,141.59325,163.21284,183.98252,137.7509,135.4067,30.93838,29.5792,37.57995,37.62332,32.69749,198.98949,202.6342,220.18841,205.97814,213.27306,84.8958,94.27104,72.163,78.54145,72.68844,26.84675,26.98125,25.40013,26.892544,28.520514,307.8075,290.8791,318.441,310.36488,344.8739,177.10407,132.93085,154.71924,142.46679,161.61453,61.74013,56.75375,54.32778,45.6914,63.30688,150.7513,192.9245,153.6255,154.39878,157.53508,33.769547,46.626684,39.1159,50.062914,37.806832,19.01636,17.253456,16.379638,26.42496,16.03782,82.4949,92.6112,81.53468,68.974953,77.911316,51.614,53.24132,50.03292,50.4218,60.16192,19.209168,25.54242,24.51456,17.52242,21.911758,61.110978,86.78065,74.432076,77.57775,66.6196,79.72896,77.78652,98.1652,81.1361,83.36615,47.94104,32.8324,42.53366,36.0012,41.5103,56.63294,65.512815,62.780475,56.739944,72.17872,38.163436,49.952916,37.65115,39.307194,40.737767,19.681464,22.476288,21.56495,24.20808,16.147818,66.310166,75.50632,69.751734,54.778824,62.264568,62.8735,62.73685,53.11194,47.0076,71.18384,29.78736,40.596,29.84208,21.4586,23.50348,52.8033,54.1044,48.07874,56.92875,55.85246,113.46552,121.36845,104.59764,107.76942,116.17011,102.36924,82.45684,107.53011,87.86835,102.2758,87.29921,93.47016,87.83192,82.3179,106.96877,177.91072,178.73088,167.08763,202.85028,160.88072,68.5146,83.01758,78.8624,100.00705,89.59068,86.7965,83.3901,93.40084,99.3773,92.56856,183.43374,186.05045,148.21638,158.39408,154.51182,65.05697,54.5705,57.95899,47.33814,44.71632,56.90355,47.25585,47.82988,52.33314,67.91049,122.1201,157.30365,146.17575,123.88761,121.20344,71.61165,62.08528,64.96687,55.34702,75.92255,79.15035,86.11568,74.9232,73.78224,94.45369,168.27408,157.24704,165.64048,207.47454,180.08186,82.28472,79.18256,85.5945,100.5829,98.48814};

// chip 8
// 	double arrTemp[MAX_NUM_GATES] = {174.58161,173.72498,212.47863,179.44578,198.10446,90.70372,101.79837,88.2154,74.24235,77.04774,32.49775,35.1955,27.10428,37.38336,36.77461,188.26522,190.48095,202.212,195.83656,223.3392,69.80499,47.59469,57.141396,56.19893,55.302478,25.188617,20.405925,23.078432,15.90622,20.205024,132.560874,132.61792,128.67511,96.93468,118.171233,56.871,55.427988,65.166912,54.7253,43.16234,3.769568,3.67708,5.377968,3.663912,3.350617,19.14675,24.91436,19.058637,19.757196,17.942832,38.658996,52.101468,39.17228,40.62747,40.87479,13.0411,13.174775,12.099465,14.059052,14.260344,127.341214,113.45238,133.7652,123.245816,136.95455,74.728656,58.9693,65.576004,64.744512,73.39178,22.60927,23.257,23.50751,17.17724,25.117232,52.80947,70.365635,67.001775,56.11568,60.267088,59.87471,65.88608,66.119,71.52354,62.8523,26.67632,32.19936,27.05142,40.3506,21.33342,92.22048,98.0352,93.39318,84.41184,109.82424,68.3645,72.358,56.18676,53.56983,70.88424,10.000128,16.74432,9.724272,9.92088,9.283934,31.699157,46.814241,35.591894,35.65875,37.292152,36.331924,32.601513,43.78276,38.981236,33.311229,21.456652,14.89392,18.710874,19.247928,17.463498,61.89848,75.906445,70.701975,66.44645,71.161384,44.73601,50.46158,42.0742,51.862734,42.800324,26.66752,28.3068,24.60634,34.2807,21.36636,85.91568,97.9392,81.63236,74.2095,98.34998,22.6612,27.663735,18.847668,21.559125,32.203472,12.707544,13.38321,10.665912,9.71012,11.778888,20.412205,18.70668,19.085354,22.140783,24.446568,38.907916,56.7567,42.840945,40.375467,47.352393,61.64625,56.99768,63.93655,46.99384,68.09385,54.93407,53.0012,52.7076,50.43402,70.21354,120.91456,117.81408,136.12892,145.64754,123.20658,52.60584,56.36978,56.37898,63.81155,62.77437,40.35255,38.370462,44.513924,44.714395,42.612976,90.363126,95.70898,72.285624,76.092952,76.164201,53.424,52.6975,48.59001,48.139,40.852372,54.173075,45.70425,46.97198,53.36079,54.32977,80.2008,78.975225,96.388425,87.645492,82.669802,46.720029,36.640334,40.869413,36.774847,48.990975,40.457976,44.299794,41.20622,42.2226,51.000426,102.018176,95.728896,88.902081,105.804978,94.462298,50.169,58.42506,55.16274,64.2807,61.62585};

// chip 9
// 	double arrTemp[MAX_NUM_GATES] = {192.44505,158.24733,201.32125,206.30376,202.0254,95.60648,106.27929,82.6679,75.4143,83.28672,31.12525,32.43125,28.70574,35.19628,39.59109,195.28795,191.44755,203.9565,196.51292,219.3238,160.00254,128.7754,155.36586,126.69164,134.4574,53.51196,44.04175,47.762,36.0012,50.24124,248.0949,240.4128,260.68985,222.05536,228.15972,128.35125,123.38844,154.45512,104.8831,99.3369,37.6332,33.1154,41.35455,44.1546,39.08482,244.78734,239.15089,272.02071,246.79566,278.87646,98.27332,92.74743,80.8584,80.95555,74.80176,30.962,32.173,23.69703,32.543,35.39943,108.964999,98.12259,135.24435,128.857532,129.46979,57.332367,52.676975,74.764794,57.54161,59.79512,23.810943,26.584,25.958166,17.28174,29.84632,61.16579,83.91489,69.36015,73.558738,76.77596,98.12754,112.57896,101.847,107.1873,102.52858,44.20482,49.70016,45.66606,59.1966,35.85744,152.60882,175.9584,164.0432,143.47377,182.40572,110.797,121.2079,90.2958,89.00312,122.7604,45.40416,57.4737,42.93408,38.6176,39.79096,141.81469,157.96809,158.31918,149.1615,130.62936,97.13257,94.19221,104.0704,87.15586,97.59869,53.16976,37.3062,47.45988,43.04544,48.60778,135.6726,167.05305,150.4065,145.43072,140.8356,90.61083,97.65184,93.2265,103.4937,91.95144,48.51616,55.97472,47.4969,65.1735,41.18616,170.09896,177.7184,168.74146,165.78561,182.82802,68.8785,86.21085,64.31334,67.39046,84.9436,32.784,42.3744,32.55864,30.5494,31.75678,108.97645,85.98375,89.39962,101.36607,107.32765,175.1173,212.4885,205.1259,187.89507,185.87916,21.976002,29.61848,22.531201,19.287521,34.280125,24.146673,20.176428,28.200084,17.899056,25.91398,43.37216,34.426656,36.06236,41.685495,42.138922,15.505686,15.78881,22.44823,23.29492,17.665431,43.4127,47.908482,44.752084,46.18724,43.504776,92.988912,107.90381,86.495698,97.661616,83.199861,68.76273,60.946,52.87171,58.21208,53.33328,66.0242,51.9975,59.04974,61.19133,59.62182,70.817908,92.77233,77.05278,82.51155,72.168582,34.292043,33.504192,44.110734,37.760536,43.65768,61.74813,54.57578,53.21932,56.05992,70.92508,140.25648,126.62016,141.89074,143.74833,127.32748,83.13648,99.4704,96.24534,114.15415,112.08168};

// chip 10 	double arrTemp[MAX_NUM_GATES] = {100.2834,106.094391,119.746978,122.372046,118.487913,56.037916,58.298535,54.21745,43.46004,52.47231,21.1014,19.664375,17.575467,22.308888,22.804788,111.850882,104.29776,114.12795,107.50942,137.05043,43.665561,36.29483,42.964428,40.746921,48.126386,17.100575,15.217725,14.613196,11.54002,14.893088,65.787714,70.79248,64.46512,58.613688,65.892357,33.218775,38.659908,41.67738,32.21365,25.870325,17.257328,17.68886,21.590847,24.416924,18.842083,129.230787,124.499798,137.300501,146.35485,140.526903,48.64798,52.710183,44.36607,42.243435,40.11084,15.7946,14.714125,11.992659,16.108988,19.346799,114.239411,118.07937,129.91485,132.6857,148.15772,70.323633,55.8558,69.92466,58.022603,62.165257,30.22989,26.87075,26.63466,20.3626,29.51256,70.8197,90.66695,76.19625,72.399528,76.96368,42.885667,51.929852,47.77075,53.02098,48.269753,19.6295,22.80612,20.724264,26.35056,17.356626,69.268736,75.36016,75.66352,70.2006,86.42062,47.78935,51.25956,42.76608,43.84194,58.66168,14.543472,20.37603,17.05512,11.96014,15.709122,44.699328,54.620391,54.866818,49.1562,43.619008,31.784418,27.552651,31.082576,23.94133,29.080275,16.663556,10.5532,15.007954,14.132352,13.768274,49.81361,68.178755,62.40885,57.035944,57.172596,33.778289,42.450668,41.5394,38.202354,36.979663,21.64734,23.976048,21.417396,29.88216,17.835426,66.666799,69.72464,70.854112,64.064292,77.858672,10.83155,11.96448,13.780158,12.033636,13.21656,7.406088,6.66477,5.615472,5.5212,5.25063,29.54127,21.002895,23.916504,23.602086,25.123272,50.924782,46.735185,51.869685,53.123301,54.994538,55.72464,47.47982,53.96672,43.44333,58.9985,50.95928,49.66942,48.55092,44.3058,61.76297,91.879632,81.391584,94.545554,93.439134,84.121534,40.367712,41.902964,38.980538,53.4369,46.553157,33.9951,37.307673,38.371112,39.284575,37.316552,76.877502,74.785765,61.5418,73.456656,67.069353,38.085217,32.3226,33.286716,33.698956,28.161848,36.739065,33.47919,32.636816,33.694068,35.987318,75.939592,84.066255,83.43888,87.467688,79.224944,39.502203,37.069606,41.430206,37.796527,42.43173,41.565108,38.277612,40.472696,37.63053,50.855817,86.128328,80.06304,90.724872,101.463213,81.895758,23.746044,22.008516,20.685464,25.313805,24.869334};



	int i = 0;
	map<int, GateNode>::iterator mapGCIter;
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		int gateId = mapGCIter->second.gateId;
		int gateType = mapGCIter->second.gateType;

		// temperature selection [11/5/2009 shengwei]
		// the iterBit  bit
		// move to the least significant bit and & 0000001
		int ind = (gateId >> (iterBit-1)) & 1;	
		if(1 == ind)
		{
			mapGCIter->second.dTempr = 1.3 + ran1(&m_seed[4]);
		}
		else
		{
			mapGCIter->second.dTempr = 1.1 + ran1(&m_seed[4]);
		}

// 		cout << mapGCIter->second.dTempr << "\t";

		i++;
	}
}

//  [2/18/2010 shengwei]
void GLC::setTemperatureForLocation(int location)
{
	// location is just gate id
	map<int, GateNode>::iterator mapGCIter;
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		int iGateLocation = mapGCIter->second.location;
		if(location == iGateLocation)
		{
			// set temperature
			mapGCIter->second.dTempr = 4;
		}
	}
}


// leakage energy model
// get leakage power coefficient from the given temperature
double GLC::getLeakageCoeffFromTempr(double dTempr)
{
// 	double coeff = dTempr*dTempr*(1-exp(-1/(dTempr*dTempr))) * exp(-1/dTempr);

	double coeff = exp(dTempr);

	return coeff;
}


// HTH detection

// add HTH to the circuit
void GLC::addHTH(int gateType, int iHTHInput)
{
	// set input to be primary input (randomly selected)
	// NOT gate
	double rnd1 = ran1(&m_seed[3]);
	set<int> setPI;
	set<int>::iterator setPIIter1, setPIIter2;

/*
	switch(m_iPIStrategy)
	{
	case PI_RANDOM:
		{
			// all the PIs should be in setPI
			vector<int>::iterator vecPIIter;
			for(vecPIIter = m_vecPrimaryInput.begin(); vecPIIter != m_vecPrimaryInput.end(); vecPIIter++)
			{
				setPI.insert(*vecPIIter);
			}

			break;
		}
	case PI_SEGMENTATION:
		findPrimaryInputs(m_vecSegOutput[0], setPI);
		break;
	}



	int indexPI1 = (int)setPI.size() * rnd1;
	int indexPI2 = (indexPI1 + 1) % setPI.size();

	int i = 0;
	setPIIter1 = setPI.begin();
	while(i < indexPI1)
	{
		setPIIter1++;
		i++;
	}

	i = 0;
	setPIIter2 = setPI.begin();
	while(i < indexPI2)
	{
		setPIIter2++;
		i++;
	}
*/	
//	int signal = m_vecMapPISignal[indexPI].begin()->first;

	// insert into m_mapGlobleCircuit
	GateNode gateNode;
	gateNode.gateId = m_nGates+1;
	gateNode.gateType = EnumGateType(gateType);
	gateNode.iOutputSignal = 0;
	gateNode.dTempr = 0;
// 	gateNode.gateType = (EnumGateType)gateType;
	gateNode.location = m_iHTHLocation; // HTH location [2/18/2010 shengwei]

	switch(gateType)
	{
	case NOT:
		gateNode.mapInputSignal.insert(make_pair(m_iHTHInput, 1));
		break;

 	case NAND:
	case AND:
	case OR:
		gateNode.mapInputSignal.insert(make_pair(m_iHTHInput, 1));
// 		gateNode.mapInputSignal.insert(make_pair(m_iHTHInput+1, 1));
		break;
 	case NOR:
 		gateNode.mapInputSignal.insert(make_pair(m_iHTHInput, 1));
// 		gateNode.mapInputSignal.insert(make_pair(*setPIIter2, 1));
// 		break;
	}

		m_mapGlobalCircuit.insert(make_pair(HTH_OUTPUT_PIN, gateNode));

}

// solve LP
void GLC::solveLP(char *lpFileName, REAL* var, int nVars, map<int, double> &mapCalcGateSize, int segIter)
{
	// solve the LP for this segment, using lp_solve API
	// read lp			
/*	m_plp = read_LP(lpFileName, NORMAL, NULL);

	// redirect lp_solve generated info to a log file
	set_outputfile(m_plp, "lp_report.txt");

	// solve lp
	int ret = solve(m_plp);

	// retrieve the value of variables
	if(NORM_L1 == m_iNorm)
	{
		nVars = m_nInputInstance * 2 + m_nGates;
	}
	else if(NORM_MLE == m_iNorm)
	{
		nVars = m_nInputInstance * (m_nBreakPoints+1+2) + m_nGates;
	}
	
	get_variables(m_plp, var);

	// fill up the array of gate sizes
	int iGateVarIndex = 0;
	if(NORM_L1 == m_iNorm)
	{
		iGateVarIndex = m_nInputInstance;
	}
	else if(NORM_MLE == m_iNorm)
	{
		iGateVarIndex = (m_nBreakPoints+1) * m_nInputInstance;
	}

// 	arrCalcGateSize = &var[iGateVarIndex];

	set<int> setGatesInSegment;
	set<int>::iterator setGISIter;

	if(PI_SEGMENTATION == m_iPIStrategy)
	{
		listGatesInSegment(m_vecSegOutput[segIter], setGatesInSegment);
		if(m_bHTH)
		{
			setGatesInSegment.insert(0);
		}

	}

	// for each gate
	map<int, GateNode>::iterator mapGCIter;
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		if(PI_SEGMENTATION == m_iPIStrategy && setGatesInSegment.end() == setGatesInSegment.find(mapGCIter->first))
		{
			// not in the segment, continue
			continue;
		}
		mapCalcGateSize.insert(make_pair(mapGCIter->second.gateId, var[iGateVarIndex++]));
	}
	*/
}

// get out the smallest
double GLC::getSmallestCoeff(int gateType, int nInputs)
{
	double smallest = g_arrLookupTable[gateType][nInputs-1][0];
	for(int i = 0; i < pow(2.0, nInputs); i++)
	{
		//double coeff = getLeakageCoeffFromTempr()
		if(smallest > g_arrLookupTable[gateType][nInputs-1][i])
		{
			smallest = g_arrLookupTable[gateType][nInputs-1][i];
		}
	}

	return smallest;
}

void GLC::outputSmallOutConstraints(int index)
{
	ofstream *pFCnstout;
	switch(m_iPIStrategy)
	{
	case PI_RANDOM:
		pFCnstout = &m_fCnstout;
		break;

	case PI_SEGMENTATION:
// 		pFCnstout = &m_fSegLPout[segIter];
		break;

	default:
		break;
	}

	// type-setGates map
// 	map< int, set<int> > mapTypeSetGates;
// 	map< int, set<int> >::iterator mapTSGIter;
	string strMBuf;

	// do this for each type of gates (type, nInput)
	int iterGateType = 0;
	int iterNumInputs = 0;
	for(iterGateType = NAND; iterGateType<=BUFF; iterGateType++)
	{
		for(iterNumInputs = 1; iterNumInputs <= MAX_NUM_INPUTS; iterNumInputs++)
		{
			set<int> setGates;
			getGatesFromType(iterGateType, iterNumInputs, setGates);
			if(0 == setGates.size())
			{
				continue;
			}



// 			mapTypeSetGates.insert(make_pair(iterGateType, setGates));

			double smallest = getSmallestCoeff(iterGateType, iterNumInputs);

			char tempBuf[1024];
			sprintf(tempBuf, "%f y%d%d+", smallest, iterGateType, iterNumInputs);
			strMBuf += tempBuf;

			map<int, GateNode>::iterator mapGCIter;

			int iMapCount = 0;
			if(1 == index)
			{
				// the sum variable y
				*pFCnstout << "y" << iterGateType << iterNumInputs << " = ";
				for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
				{
					if(setGates.end() == setGates.find(mapGCIter->second.gateId))
					{
						continue;
					}
					iMapCount++;


					*pFCnstout << "x" << mapGCIter->second.gateId;
					if(iMapCount < setGates.size())
					{
						*pFCnstout << "+";
					}
				}
				*pFCnstout << ";" << endl;
			}
		}
	}


	if(1 == index)
	{
 		*pFCnstout << "M = " << strMBuf.c_str() << ";" << endl;
	}
// 	for(mapTSGIter = mapTypeSetGates.begin(); mapTSGIter != mapTypeSetGates.end(); mapTSGIter++)
// 	{
// 		*pFCnstout << 
// 	}

	*pFCnstout << "M + ";
	for(iterGateType = NAND; iterGateType<=HTH; iterGateType++)
	{
		for(iterNumInputs = 1; iterNumInputs <= MAX_NUM_INPUTS; iterNumInputs++)
		{
			set<int> setGates;
			getGatesFromType(iterGateType, iterNumInputs, setGates);
			if(0 == setGates.size())
			{
				continue;
			}

			double smallest = getSmallestCoeff(iterGateType, iterNumInputs);

			map<int, GateNode>::iterator mapGCIter;

			int iMapCount = 0;


// 			*pFCnstout << smallest << "y" << iterGateType << iterNumInputs << " +";

			// other parts after taking out the smallest coeff
			map<double, set<int> >::iterator mapCGIter;
			//mapCGIter = m_mapCoeffGates.find(smallest);
			iMapCount = 0;
			for(mapCGIter = m_mapCoeffGates[iterGateType][iterNumInputs].begin(); mapCGIter != m_mapCoeffGates[iterGateType][iterNumInputs].end(); mapCGIter++)
			{
				iMapCount++;
				if(0 == setGates.size())
				{
					continue;
				}
				if(smallest == mapCGIter->first)
				{
					continue;
				}
				else
				{

					// new coefficient after taking out the smallest coefficient
					double newCoeff = mapCGIter->first - smallest;
					set<int>::iterator setGIter;
					int iSetCount = 0;
					for(setGIter = mapCGIter->second.begin(); setGIter != mapCGIter->second.end(); setGIter++)
					{
						iSetCount++;
						*pFCnstout << newCoeff << " x" << *setGIter;
// 						if(iSetCount < mapCGIter->second.size() || iMapCount < m_mapCoeffGates[iterGateType][iterNumInputs].size())
// 						{
							*pFCnstout << "+";
// 						}
					}
				}
			}

			
		}
	}




}

void GLC::getGatesFromType(int gateType, int nInputs, set<int> &setGates)
{
	map<int, GateNode>::iterator mapGCIter;
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		if(gateType == mapGCIter->second.gateType && nInputs == mapGCIter->second.mapInputSignal.size())
		{
			// insert gate id into setGates
			setGates.insert(mapGCIter->second.gateId);
		}
	}
}


// select PI for swithcing power [10/28/2009 shengwei]
bool GLC::isSelectedPI(int iPI)
{
	bool bRet = false;
	for(int i = 0; i < m_vecPrimaryInput.size(); i++)
	{
		if(iPI == g_selectPI[m_iBenchmark][i])
		{
			bRet = true;
			break;
		}
	}

	return bRet;
}

// generate Bounds section for CPLEX [11/4/2009 shengwei]
void GLC::generateCPLEXBounds()
{
	m_fLPout << "Bounds" << endl;
	for(int i = 0; i < m_nInputInstance; i++)
	{
		m_fLPout << "e" << (i+1) << " free" << endl;
	}
}

// for Saro [12/19/2009 shengwei]
bool GLC::isSelectedGate(int gateid)
{
	bool ret = false;

	for(int i = 0; i < NUM_SELECT_GATES; i++)
	{
		if(gateid == g_arrSelectedGates[i] || gateid == m_nGates+1)
		{
			ret = true;
			break;
		}
	}

	return ret;

}

double GLC::getDelay(int pin)
{
	double ret = 0;
	map<int, GateNode>::iterator iterMapGCIter = m_mapGlobalCircuit.find(pin);
	if(m_mapGlobalCircuit.end() != iterMapGCIter)
	{
		ret = iterMapGCIter->second.delay;
	}

	return ret;

}

// output delay at each primary output
// return whether critical path changes
// true: changes false: not
bool GLC::outputDelay()
{
	bool bRet = false;

	double maxDelay = 0;
	int maxDelayPO = 0;
	map<int, GateNode>::iterator mapGCIter;
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		if(isPrimaryOutput(mapGCIter->first))
		{
			if(mapGCIter->second.delay > maxDelay)
			{
				maxDelay = mapGCIter->second.delay;
				maxDelayPO = mapGCIter->first;
			}
			//cout << mapGCIter->first << ", " << mapGCIter->second.iOutputSignal << ", " << mapGCIter->second.delay << endl;
		}
	}

	// check maxDelayPO to observe critical path changes
	if(maxDelayPO != m_maxDelayPO)
	{
		bRet = true;
		m_maxDelayPO = maxDelayPO;
	}

	cout << bRet << ", ";
	cout << 100/maxDelay;

	return bRet;
}

// output power
void GLC::outputPower()
{
	double sumLeakagePower = 0;
	double sumSwitchingPower = 0;
	for(int i = 0; i < m_nGates; i++)
	{
		sumLeakagePower += g_arrRealSize[i] * g_arrVdd[i] * exp(g_arrVdd[i]-g_arrVth[i]);
		sumSwitchingPower += g_arrRealSize[i] * 6 * g_arrVdd[i] * g_arrVdd[i];
	}

	cout << sumLeakagePower + sumSwitchingPower << ", ";
	cout << sumLeakagePower << ", ";
	cout << sumSwitchingPower;
}


// controllability list [1/28/2010 shengwei]
void GLC::calcCtrlList()
{
	// level by level, from first level

	// if (not all gates done)
	//	for each gate
	//		if (all input pins/nodes primary or already done)
	//			calculate CtrlList
	//		endif
	//  endfor
	// endif

	int nGatesDone = 0;
	map<int, GateNode>::iterator mapGCIter;

	while(nGatesDone < m_nGates)
	{
		for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
		{
			// if the gate is already done, skip it
			if(mapGCIter->second.ctrlList[0].size() != 0)
			{
				continue;
			}

			// determine (all input pins/nodes primary or already done)
			bool bAllSet = true;
			map<int, int>::iterator mapISIter;
			map<int, int> mapInputSignal = mapGCIter->second.mapInputSignal;
			for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
			{
				// is primary input?
				if(isPrimaryInput(mapISIter->first))
				{
					continue;
				}
				else
				{
					// the input node
					map<int, GateNode>::iterator tempGCIter;
					tempGCIter = m_mapGlobalCircuit.find(mapISIter->first);

					if(tempGCIter->second.ctrlList[0].size() == 0)
					{
						// not set yet
						bAllSet = false;
						break;
					}

				}
			}

			if(bAllSet)
			{
				// calculate CtrlList

				// consider NAND_2 gates only
				// 1. to reach state 0
				// only 11 works
				mapGCIter->second.ctrlList[0] = m_mapPrimaryInputSignal;
				for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
				{
					map<int, int>::iterator tempMapCLIter;
					if(isPrimaryInput(mapISIter->first))
					{
						// primary inputs
						// andCtrlList(mapGCIter->second.ctrlList[0], )
						tempMapCLIter = mapGCIter->second.ctrlList[0].find(mapISIter->first);
						tempMapCLIter->second = 1;
					}
					else
					{
						// input nodes
						map<int, GateNode>::iterator tempGCIter;
						tempGCIter = m_mapGlobalCircuit.find(mapISIter->first);

						andCtrlList(mapGCIter->second.ctrlList[0], tempGCIter->second.ctrlList[1]);
					}
				}

				// 2. to reach state 1
				// 00, 01, or 10  -->  0x  x0
				// fix the least number of fanout
				int i = 0;
				mapGCIter->second.ctrlList[1] = m_mapPrimaryInputSignal;
				for(mapISIter = mapInputSignal.begin(); mapISIter != mapInputSignal.end(); mapISIter++)
				{
					int inputState = 0;
					if(0 == i)
					{
						inputState = 0;
					}
					else
					{
						inputState = -1;
					}

					map<int, int>::iterator tempMapCLIter;
					if(isPrimaryInput(mapISIter->first))
					{
						// primary inputs
						// andCtrlList(mapGCIter->second.ctrlList[0], )
						tempMapCLIter = mapGCIter->second.ctrlList[1].find(mapISIter->first);
						tempMapCLIter->second = inputState;
					}
					else
					{
						// input nodes
						map<int, GateNode>::iterator tempGCIter;
						tempGCIter = m_mapGlobalCircuit.find(mapISIter->first);

						int state;
						if(0 == inputState)
						{
							state = inputState;
						}
						else
						{
							// select a state for which ctrlList has more x
							state = 1;
						}

						andCtrlList(mapGCIter->second.ctrlList[1], tempGCIter->second.ctrlList[state]);
					}

					i++;
				}


				nGatesDone++;
			}

		}
	}

	// for debug
	for(mapGCIter = m_mapGlobalCircuit.begin(); mapGCIter != m_mapGlobalCircuit.end(); mapGCIter++)
	{
		cout << "CC0: ";
		map<int, int>::iterator mapCLIter;
		for(mapCLIter = mapGCIter->second.ctrlList[0].begin(); mapCLIter != mapGCIter->second.ctrlList[0].end(); mapCLIter++)
		{
			if(-1 == mapCLIter->second)
			{
				cout << "x";
			}
			else
			{
				cout << mapCLIter->second;
			}
		}
		cout << " ";

		cout << "CC1: ";
		for(mapCLIter = mapGCIter->second.ctrlList[1].begin(); mapCLIter != mapGCIter->second.ctrlList[1].end(); mapCLIter++)
		{
			if(-1 == mapCLIter->second)
			{
				cout << "x";
			}
			else
			{
				cout << mapCLIter->second;
			}
		}
		cout << endl;
	}
}

// do "and" operation of two ctrl lists
void GLC::andCtrlList(map<int, int> & mapDestCtrlList, map<int, int> & mapSrcCtrlList)
{
	map<int, int>::iterator destIter;
	map<int, int>::iterator srcIter;

	for(destIter = mapDestCtrlList.begin(), srcIter = mapSrcCtrlList.begin(); 
		destIter != mapDestCtrlList.end(), srcIter != mapSrcCtrlList.end();
		destIter++, srcIter++)
	{
		if(-1 == destIter->second)
		{
			destIter->second = srcIter->second;
		}
		else if((1 == destIter->second && 0 == srcIter->second) || (0 == destIter->second && 1 == srcIter->second))
		{
			// conflict -2
			destIter->second = -2;
		}
	}
}




