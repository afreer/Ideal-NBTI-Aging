#ifndef GLC_H
#define GLC_H
#pragma once
#pragma  warning(disable:4267)

#include <vector>
#include <map>
#include <set>
#include <fstream>
// matlab engine
//#include "Engine.h"
#include <cmath>
#include "PLDLL.h"

#include "lp_lib.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
// piecewise linear approximation



// constant limitations
const int MAX_WIDTH_BENCHMARK = 128;

const int MAX_NUM_PRIMARY_INPUT = 1024;

const int MAX_NUM_TYPES_GATES = 32;

const int MAX_WIDTH_LOOKUP_TABLE = 512;

const int MAX_NUM_GATES = 2048;

const int MAX_NUM_INPUTS = 10;

const int MAX_NUM_SEG = 1024;

const int MAX_NUM_INSTANCE = 1024;

const int MAX_NUM_PINS = 4096;

const int HTH_OUTPUT_PIN = -1;  // output pin for the HTH gate

// maximum possible delay
// const int MAX_DELAY = 10000;


// type definitions and constant numbers


enum EnumBenchmark
{
	C17 = 0,
	C432,
	C499,
	C880,
	C1355,
	C1908, 
	C2670, 
	C3540,
	C5315,
	C6288,
	Saro,
	ADDER_CR,
// 	Saro12,
// 	Saro13,
// 	Saro21,
// 	Saro31,
	C7552,
	S17,
	S526,
	S641,
	S713,
	S820,
	S832,
	S1423,
	S5378,
	S13207,
	S15850,
	S38584,
};

const int NUM_BENCHMARK = 23;


enum EnumGateType
{
	NAND = 0,
	NOT,
	NOR,
	AND,
	XOR,
	OR,
	BUFF,
	HTH,
	DFF
};

const int NUM_GateType = 7;


enum EnumNorm
{
	NORM_SUM = 0,
	NORM_L1,
	NORM_MLE,
};

const int NUM_NORM = 3;

enum EnumErrorModel
{
	ERROR_UNIFORM,
	ERROR_GAUSSIAN,
	ERROR_TRIANGULAR, 
	ERROR_EXP,

};

const int NUM_ERRORMODEL = 4;

enum EnumPIStrategy  // strategy of selecting primary input
{
	PI_RANDOM,
	PI_SEGMENTATION,
	PI_READ,
};

const int NUM_PISTRATEGY = 3;

enum EnumSegStrategy  // strategy of segmentation
{
	SEG_OUTPUT_CONE,   // output cone segmentation
	SEG_TREE_CIRCUIT,  // tree circuit segmentation
};

const int NUM_SEGSTRATEGY = 2;

enum EnumLpSolver
{
	SOLVER_LP_SOLVE,
	SOLVER_CPLEX,
};

enum EnumMetric
{
	METRIC_LEAKAGE_POWER,
	METRIC_SWITCH_POWER,
	METRIC_THERMAL_LEAKAGE,
};

// PV model [10/25/2010 shengwei]
enum PVModel
{
	PVMODEL_GAUSSIAN,
	PVMODEL_QUADTREE,
};

struct GateNode 
{
	int gateId;   //from 1 to nGates
	EnumGateType gateType;
	map<int, int> mapInputSignal;   // <inputPin, signal>
// 	double realSize;                // the real size
	bool bSwitch;                 // whether it is a switched gate (init to 0)
	int iOutputSignal;            // output signal of the gate
	double dTempr;				  // Temperature of the gate (init to 1.0)
	double delay;                     // time stamp (delay) at the output pin of this gate
	map<int, int> ctrlList[2]; // controllability list  m_ctrlList[0] - CC0,  m_ctrlList[1] - CC1 [1/28/2010 shengwei] 
	int iFanout;                 // fanout of this gate
	int location;  // location of this gate: equal to location [2/18/2010 shengwei]
// 	double vdd;
// 	double vth;
};

class GLC
{
public:
	GLC(int iBenchmark, double dErrorRate, int nInstances, int iPIStrategy, int iNorm, int seedIndex);
	~GLC(void);

	// read the specified benchmark file
	void readBenchmark();

	// gate placement (greedy) -- not implemented yet, seems of no use
	void greedyPlacement();

	// search all the gates on the path from specific output
	void listAllGatesOnPath(int iOutput, set<int> & setGates);

	// set Primary Input Signal (for all the instances) -> m_vecMapPISignal
	void setPrimaryInputSignal();
	void setSegPrimaryInputSignal(int segIter);

	// read PIs, from m_vecMapPISignal  to  m_mapPrimaryInputSignal
	void readPrimaryInputSignal(int index);   

	// set global signal for the specific primary input, according to the current PI signals in m_mapPrimaryInputSignal
	void setGlobalSignal();

	// get signal for the specific pin [10/19/2009 shengwei]
	int getSignal(int pin);

	// set the real size of each gate (in m_mapGlobalCircuit)
	void setRealSize();

	// get gates from input
	//void getGatesFromInput();

	// gate function
 	int gateFunction(int gateType, map<int, int> mapInputSignal);

	// generate LP constraint
	double generateOneLPConstraint(int index, int eqIndex = 0, int segIter=0);
	void generateSegOneLPConstraint(int cnstIndex, int segIter);

	void writeConstraintLeftSide(double &measurePower, double &realPower, int index, int segIter=0);
	void writeSegConstraintLeftSide(double &measurePower, double &realPower, int segIter, int guessHTH = 0);

	// generate Objective Function
	void generateOF(int segIter=0);
	void generateSegOF(int segIter);

	// for weighted L1-norm
	void generateWeight(double abs[MAX_NUM_INSTANCE], double weight[MAX_NUM_INSTANCE]);

	// generate Free variables declaration
	void generateFreeVariables(int segIter=0);

	void generateSOS();

	void generateSec();


	// merge OF and constraints
	void mergeOFCnst();

	// for debug
	void debugCircuit();

	// random number generator
	// Uniform Distribution from 0 to 1
	double ran1(long *idum);

	// Gaussian Distribution
	double gasdev(long *idum);

	// Gaussian distribution generator, provided by Saro
	double gngauss(double mean, double sigma);

	// Triagnle Distribution
	double ranTriangular(double min, double max, double mode);

	// Gaussian Distribution
	double ranGaussian(double mean, double var);

	// Exponential Distribution [11/5/2009 shengwei]
	double ranExp(long *idum);

	double ranExp2();

	double ranExp3();

	// mad
	double mad(double x[MAX_NUM_INSTANCE]);

	// set the seed for matlab RNG
	void setState(int state);

	// detect correlation
	void detectCorrelation(vector< pair<int, int> > & vecCorrelation);
	void detectSegCorrelation(set<int> &setCorr, int segIter);

	// detect bad segment (insufficient number of equations)
	// put bad segments into m_arrBadSeg
	bool isBadSeg(int segIter);




	// find the corresponding PI for the specific PO
	void findPrimaryInputs(int iOutput, set<int> &setPI);

	// check whether an input is a primary input
	bool isPrimaryInput(int iInput);
	// check whether a pin is a primary output
	bool isPrimaryOutput(int pin);

	// getNames
	string getBenchmarkName(int iBenchmark);
	string getErrorModelName(int iErrorModle);
	string getPIStrategyName(int iPIStrategy);
	string getNormName(int iNorm);

	// for segmentation
	void setPIGatesMap();

	// check if all the output signals have been set
	bool allOutputSatisfied();

	// get the gate set for the segment
	void getSegGates(set<int> &setGatesInSegment,  int startPIIndex, int endPIIndex);


	// get gate id from the gate output (the key of the gate)
	int getGateId(int iGateOutput);

	// output real gate sizes for segmentation
	void outputSegRealGateSizes(int segIter);

	// do segmentation (fill up m_vecSegOutput)
	void doSegmentation();

	//////////////////////////////////////////////////////////////////////////
	// tree circuit segmentation [9/26/2009 shengwei]
	
	// list all the gates in one segment  (segOutput = m_vecSegOutput[segIter])
	void listGatesInSegment(int segOutput, set<int> & setGates);

	// list all the PIs in one segment  (segOutput = m_vecSegOutput[segIter])
	void listPIInSegment(int segOutput, set<int> & setPI);

	// find the fanout gates for specific output
	void listFanoutGates(int iOutput, set<int> &setFanoutGates);


	// output of each segment (PO + Gates with multiple fanout gates (MF gates))
	// init in readBenchmark
	vector<int> m_vecSegOutput;

	int m_nGatesInSeg[MAX_NUM_SEG];
 	vector<int> m_vecBadSeg;

	//////////////////////////////////////////////////////////////////////////
	// piecewise linear approximation
	void listOptBreakPoints(double u[MAX_N]);

	// set m_dMinError, m_dMaxError, m_dModeError error for piecewise linear approximation
	void setErrorForPL();

	// calculate measured power for all the input instances
	double calcRealPower();

	double density(double x);

	// init, called after readBenchmark
	void initMLE(double realPower);

	// do post-processing
	REAL * doPostProcessing(REAL *var);

	// for switching power
	// set bSwitch bit in the m_mapGlobalCircuit
	void getSwitchedGates(int index);

	// get switching PI vector pair for switching gate i [1/15/2010 shengwei]
	void getPIPairForSwitching(int iGate);

	// print switch PI for all the gates
	void printSwitchPI();

	map<int, int> m_mapSwitchCount;
	int m_nSwitchedGates;

	//////////////////////////////////////////////////////////////////////////
	// Thermal Conditioning

	// set temperature for each gate (in m_mapGlobalCircuit), keep it there until next change
	// to the temperature we need to fight correlated gates
	void setTemperature(int iterBit);

	//  [2/18/2010 shengwei]
	void setTemperatureForLocation(int location);

	// leakage energy model
	// get leakage power coefficient from the given temperature
	double getLeakageCoeffFromTempr(double dTempr);

	// for temperature selection [11/5/2009 shengwei]
	int m_nEncodeBits;    // # of encoded bits of gates
	int m_nEqPerThermal;  // # of equations per thermal condition


	// End of Thermal Conditioning
	//////////////////////////////////////////////////////////////////////////

	// HTH detection
	void addHTH(int gateType, int iHTHInput);

	// solve LP
	void solveLP(char *lpFileName, REAL* var, int nVars, map<int, double> &mapCalcGateSize, int segIter=0);

	// get out the smallest
	double getSmallestCoeff(int gateType, int nInputs);

	void outputSmallOutConstraints(int index);

	void getGatesFromType(int gateType, int nInputs, set<int> &setGates);

	bool m_bSmallOut;  // whether to get the smallest coefficient out
	// Coeff-Gates map for each gate type
	map< double, set<int> > m_mapCoeffGates[MAX_NUM_TYPES_GATES][MAX_NUM_INPUTS];


	// select PI for swithcing power [10/28/2009 shengwei]
	bool isSelectedPI(int iPI);

	// generate Bounds section for CPLEX [11/4/2009 shengwei]
	void generateCPLEXBounds();

	// for Saro [12/19/2009 shengwei]
	bool isSelectedGate(int gateid);
	bool m_bSelectGates;

	// delay related [1/14/2010 shengwei]
	// get the delay on a specific pin
	double getDelay(int pin);
	// output delay at each primary output
	bool outputDelay();

	// output power
	void outputPower();

	// controllability list [1/28/2010 shengwei]
	void calcCtrlList();

	// do "and" operation of two ctrl lists
	void andCtrlList(map<int, int> & destCtrlList, map<int, int> & srcCtrlList);

	bool m_bHTH;   // whether to consider HTH
	bool m_bReallyHaveHTH;   // whether HTH is really in the circuit
	bool m_bDiagnosis;
	int m_iHTHInput; // input pin for HTH (consider inverter, only 1 input)
	int m_iGuessHTHInput; // guessed input for HTH when doing diagnosis
	int m_iGuessHTHType;  // guess type [2/18/2010 shengwei]

	int m_bLocationHTH;
	int m_iHTHLocation; // HTH location [2/18/2010 shengwei]
	int m_iGuessHTHLocation; // HTH location [2/18/2010 shengwei]

	bool m_bRandomHTH;    // whether the HTH has random inputs [6/29/2010 shengwei]


	double m_realPower;  // real power when all the signals are 1

	int m_nBreakPoints;  // # of break points
	double m_arrBP[MAX_N];  // the break points

	double m_arrBreakPoints[MAX_NUM_INSTANCE][MAX_N];
	double m_a[MAX_NUM_INSTANCE][MAX_N+1];
	double m_b[MAX_NUM_INSTANCE][MAX_N+1];

	// errors based on the triangular distribution
	double m_dMinError;
	double m_dModeError;
	double m_dMaxError; 

	double m_dLowerDP;
	double m_dUpperDP;

	double m_deltaDP;  // delta in discrete search in dynamic programming

	// end of piecewise linear approximation
	//////////////////////////////////////////////////////////////////////////

	// global circuit map
	map<int, GateNode> m_mapGlobalCircuit;

	// primary input signal
	map<int, int> m_mapPrimaryInputSignal;
	vector<int> m_vecPrimaryInput;
	// PI pair for switching [1/15/2010 shengwei]
	pair< map<int, int>, map<int, int> > m_pairPI;

	// primary output
	vector<int> m_vecPrimaryOutput;

	// num of pins
	int m_numPins;

	// num of gates
	int m_nGates;

	// num of input instances
	int m_nInputInstance;

	// global signal map (based on the signal of primary input vector) <pinIndex, signal>
	map<int, int> m_mapGlobalSignal;

	// seed for random number generator
	long m_seed[7];
	long m_fixedSeed;

	// lp output stream
	ofstream m_fLPout;

	// for segmentation
	ofstream m_fSegLPout[MAX_NUM_SEG];
	ofstream m_fLPSolveout;
	ofstream m_fXLSout;
	ofstream m_fTimerOut;
	ofstream m_fDebugOut;
	ofstream m_fCorrOut;
	ofstream m_newOFOut;  // for weighted L1 norm
	ofstream m_fOFout;   // for split OF/Constraint
	ofstream m_fCnstout; // for split OF/Constraint
	
	ofstream m_fSWMapOut;  // for switching map [1/18/2010 shengwei]
	
	ofstream m_fTemprOut; // for temperatures [2/26/2010 shengwei]


	// FOR CPLEX
	ofstream m_fCPLEXModOut;
	ofstream m_fCPLEXDataOut;

	// for saro [12/17/2009 shengwei]
	ofstream m_heatout;

	// matlab engine
	//Engine *m_pEngine;

	// parameters
	int m_iBenchmark;
	int m_iNorm;
	int m_iErrorModel;
	double m_dErrorRate;
	int m_iPIStrategy;
	int m_iSegStrategy;
	bool m_bPostProcessing;  // whether to do post-processing or not
	int m_iLPSolver; 
	int m_iMetric; // leakage power/ switch power
	bool m_bFilterErrors;
	// PV model [10/25/2010 shengwei]
	int m_iPVModel;

	// coefficients in LP constraints, for correlation detection
	vector< vector<double> > m_vecCoeff;

	vector< map <int, int>  > m_vecMapPISignal;

	// PI-setGates map, for segmentation, set by setPIGatesMap()
	map<int, set<int> > m_mapPIGates;

	// lp_solve
	lprec *m_plp;

	// temp
	set<int> m_vecGatesInSeg17;

	double g_arrRealSize[MAX_NUM_GATES]; 

	double g_arrVdd[MAX_NUM_GATES];
	double g_arrVth[MAX_NUM_GATES];

	double g_arrTempr[MAX_NUM_GATES];

	double m_debugSlop[3];

	vector<double> m_vecExpRnd;

	// critical path
	int m_maxDelayPO; 

	// repetition of each equation  [6/30/2010 shengwei]
	int m_nEqnsPerSignal;

};
	

#endif