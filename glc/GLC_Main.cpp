#include <iostream>
using namespace std;

#include "GLC.h"
#include <ctime>

ofstream g_resultOut;

extern double g_arrSWDelayTable[MAX_NUM_TYPES_GATES][MAX_NUM_INPUTS];
// extern double g_arrRealSize[MAX_NUM_GATES];

void main(int argc, char *argv[])
{
	g_resultOut.open("result.txt");

	//////////////////////////////////////////////////////////////////////////
	// 0. Read Input and Set Simulation Parameters
	//////////////////////////////////////////////////////////////////////////

	// command format: 
	// GLC <iBenchmark> <dErrorRate> <nInstances> <noS/S>
	if(argc < 6)  
	{
		cout << "wrong arguments!" << endl;
		cout << "GLC <iBenchmark> <dErrorRate> <nInstances> <noS/S> <L1/MLE>" << endl;
		exit(0);
	}

	// argv[1]: benchmark
	int iBenchmark = C17;
	if(0 == strcmp(argv[1], "C17"))
	{
		iBenchmark = C17;
	}
	else if(0 == strcmp(argv[1], "C432"))
	{
		iBenchmark = C432;
	}
	else if(0 == strcmp(argv[1], "C499"))
	{
		iBenchmark = C499;
	}
	else if(0 == strcmp(argv[1], "C880"))
	{
		iBenchmark = C880;
	}
	else if(0 == strcmp(argv[1], "C1355"))
	{
		iBenchmark = C1355;
	}
	else if(0 == strcmp(argv[1], "C1908"))
	{
		iBenchmark = C1908;
	}
	else if(0 == strcmp(argv[1], "C2670"))
	{
		iBenchmark = C2670;
	}
	else if(0 == strcmp(argv[1], "C3540"))
	{
		iBenchmark = C3540;
	}
	else if(0 == strcmp(argv[1], "C5315"))
	{
		iBenchmark = C5315;
	}
	else if(0 == strcmp(argv[1], "C6288"))
	{
		iBenchmark = C6288;
	}
	else if(0 == strcmp(argv[1], "C7552"))
	{
		iBenchmark = C7552;
	}
	else if(0 == strcmp(argv[1], "S17"))
	{
		iBenchmark = S17;
	}
	else if(0 == strcmp(argv[1], "S526"))
	{
		iBenchmark = S526;
	}
	else if(0 == strcmp(argv[1], "S641"))
	{
		iBenchmark = S641;
	}
	else if(0 == strcmp(argv[1], "S713"))
	{
		iBenchmark = S713;
	}
	else if(0 == strcmp(argv[1], "S820"))
	{
		iBenchmark = S820;
	}
	else if(0 == strcmp(argv[1], "S832"))
	{
		iBenchmark = S832;
	}
	else if(0 == strcmp(argv[1], "Saro"))
	{
		iBenchmark = Saro;
	}
	else if(0 == stricmp(argv[1], "Adder_CR"))
	{
		iBenchmark = ADDER_CR;
	}
	else if(0 == strcmp(argv[1], "S13207"))
	{
		iBenchmark = S13207;
	}
	else if(0 == strcmp(argv[1], "S15850"))
	{
		iBenchmark = S15850;
	}
	else if(0 == strcmp(argv[1], "S38584"))
	{
		iBenchmark = S38584;
	}

	// argv[2]: errorRate
	double dErrorRate = atof(argv[2]);

	// argv[3]: nInstances
	int nInstances = atoi(argv[3]);

	// argv[4]: noS/S
	int iPIStrategy = PI_RANDOM;
	if(0 == strcmp(argv[4], "noS"))
	{
		iPIStrategy = PI_RANDOM;
	}
	else if(0 == strcmp(argv[4], "S"))
	{
		iPIStrategy = PI_SEGMENTATION;
	}
	else if(0 == strcmp(argv[4], "Read"))
	{
		iPIStrategy = PI_READ;
	}

	// arg[5]: L1/MLE
	int iNorm = NORM_L1;
	if(0 == strcmp(argv[5], "L1"))
	{
		iNorm = NORM_L1;
	}
	else if(0 == strcmp(argv[5], "MLE"))
	{
		iNorm = NORM_MLE;
	}

	// run 50 times, using different seeds 
	for(int seedIndex = 51; seedIndex < 52; seedIndex+=1)
	{
 		srand(0);
		cout << "seed, iter" << seedIndex << endl;
		GLC glc(iBenchmark, dErrorRate, nInstances, iPIStrategy, iNorm, seedIndex);

		//////////////////////////////////////////////////////////////////////////
		// 1. Read Benchmark and Set Circuit Information
		//////////////////////////////////////////////////////////////////////////

		// read benchmark file (output to m_mapGloabalCircuit, m_vecPrimaryInput, m_vecPrimaryOutput)
		glc.readBenchmark();

		// set the real size of each gate
		glc.setRealSize();

// 		for(int vddIter = 0; vddIter < 1; vddIter++)
// 		{

			// set real size, vdd, vth (using global variables)
// 			double vdd = 1.1 + vddIter * 0.01;
			// set Vdd
// 			for(int i = 0; i < glc.m_nGates; i++)
// 			{
// 				glc.g_arrVdd[i] = vdd;
// 			}

			// 		glc.calcCtrlList();

			//  [1/16/2010 shengwei]
			// 		if(METRIC_SWITCH_POWER == glc.m_iMetric)
			// 		{
			// 			glc.printSwitchPI();
			// 		}

			// set PIGatesMap, for segmentation
			// 		glc.setPIGatesMap();


			//////////////////////////////////////////////////////////////////////////
			// 2. LP Generation
			//////////////////////////////////////////////////////////////////////////

			if(PI_RANDOM == iPIStrategy || PI_READ == iPIStrategy)
			{
				char strErrorRate[32];
				int iErrorRate = (int)(dErrorRate*100);
				_itoa(iErrorRate, strErrorRate, 10);

				char strNumInstances[32];
				_itoa(glc.m_nInputInstance, strNumInstances, 10);

				glc.setPrimaryInputSignal();

				int iMaxHTHInput = 1;
				if(glc.m_bHTH && glc.m_bReallyHaveHTH)
				{
					switch(glc.m_iBenchmark)
					{
					case C17:
						iMaxHTHInput = 23;
						break;

					case C432:
						iMaxHTHInput = 432;
						break;
					case C499:
						iMaxHTHInput = 755;
						break;

					case C880:
						iMaxHTHInput = 880;
						break;
					case C1355:
						iMaxHTHInput = 1355;
						break;
					}
				}

				//  Auto HTH [11/2/2009 shengwei]  
				int iterHTHInput = 1;
				for(iterHTHInput = 1; iterHTHInput <= 1/*iMaxHTHInput*/; iterHTHInput++)
				{
					char strHTH[128];
					if(glc.m_bReallyHaveHTH)
					{
						sprintf(strHTH, "hth%d", iterHTHInput);
					}
					else
					{
						sprintf(strHTH, "no, hth");
					}

					char strOutputLP[512];
					sprintf(strOutputLP, "outputGLC_%s_%s_%s_%s_%s_%s_%s_seed%d.lp", 
						glc.getBenchmarkName(glc.m_iBenchmark).c_str(), strErrorRate, strNumInstances, 
						glc.getErrorModelName(glc.m_iErrorModel).c_str(), glc.getPIStrategyName(glc.m_iPIStrategy).c_str(), glc.getNormName(glc.m_iNorm).c_str(), strHTH, seedIndex);

					glc.m_fLPout.open(strOutputLP);
					if(!glc.m_fLPout)
					{
						cout << "open *.lp failed" << endl;
					}

					glc.m_fCnstout.open("Constraints.lp"); // for split OF/Constraint
					if(!glc.m_fCnstout)
					{
						cout << "open Constraints.lp failed" << endl;
					}


					glc.m_fOFout.open("OF.lp");   // for split OF/Constraint
					if(!glc.m_fOFout)
					{
						cout << "open OF.lp failed" << endl;
					}

					glc.m_fCnstout.setf(ios::fixed);//floating-point   numbers   are   not   writen   in   e-notation   
					glc.m_fCnstout.setf(ios::showpoint);//a   decimal   and   trailing   zeros   are   always   shown   
					glc.m_fCnstout.precision(6);

					glc.m_fOFout.setf(ios::fixed);//floating-point   numbers   are   not   writen   in   e-notation   
					glc.m_fOFout.setf(ios::showpoint);//a   decimal   and   trailing   zeros   are   always   shown   
					glc.m_fOFout.precision(6);

					glc.m_fLPout.setf(ios::fixed);//floating-point   numbers   are   not   writen   in   e-notation   
					glc.m_fLPout.setf(ios::showpoint);//a   decimal   and   trailing   zeros   are   always   shown   
					glc.m_fLPout.precision(6);



					// set HTH
					if(glc.m_bHTH)
					{
						glc.addHTH(NOT, iterHTHInput);
					}

					// 				cout << strHTH << endl;

					// generate objective function
					glc.generateOF();

					double arrMeasurePower[128];

					for(int i = 0; i < glc.m_nInputInstance; i++)
					{
						// consider switch power [10/11/2009 shengwei]
						if(METRIC_SWITCH_POWER == glc.m_iMetric)
						{
							if(glc.m_nInputInstance / 2 == i)
							{
								break;
							}
							glc.getSwitchedGates(2*i);
						}
						else if(METRIC_LEAKAGE_POWER == glc.m_iMetric || METRIC_THERMAL_LEAKAGE == glc.m_iMetric)
						{
							// set signal for each pin according to primary input
							glc.readPrimaryInputSignal(i);

							glc.setGlobalSignal();  // result in m_mapGlobalCircuit


// 							cout << vdd << ", ";
// 							glc.outputDelay();
// 							cout << ", ";
// 							glc.outputPower();
// 							cout << endl;

							// set temperature
							if(glc.m_bLocationHTH)  //  [2/18/2010 shengwei]
							{
								glc.setTemperatureForLocation(6);
							}
							else
							{
								// for the 2nd half of the constraints, change the temperature
								if(METRIC_THERMAL_LEAKAGE == glc.m_iMetric && i % glc.m_nEqPerThermal == 0)
								{
									// iterBit  1 - m_nEncodeBits
									int iterBit = i / glc.m_nEqPerThermal + 1;
									// set temperature
									glc.setTemperature(iterBit);
								}

							}
						}

						// generate LP constraints
						// for random HTs, generate multiple equations for each signal [6/30/2010 shengwei]
						//int nEqnsPerSignal = 1;
						for(int j = 0; j < glc.m_nEqnsPerSignal; j++)
						{
							arrMeasurePower[i] = glc.generateOneLPConstraint(i+1, j+1);
						}

						//  					cout << "instance " << i << " done!" << endl;
					}

					int iterMeasure;
					// 				for(iterMeasure = 0; iterMeasure < glc.m_nInputInstance-1; iterMeasure++)
					// 				{
					// 					cout << arrMeasurePower[iterMeasure] << ", ";
					// 				}
					// 				cout << arrMeasurePower[iterMeasure] << endl;

					glc.m_fOFout.flush();
					glc.m_fCnstout.flush();

					glc.mergeOFCnst();

					if(SOLVER_LP_SOLVE == glc.m_iLPSolver)
					{
						glc.generateFreeVariables();
					}
					else if(SOLVER_CPLEX == glc.m_iLPSolver)
					{
						glc.generateCPLEXBounds();
					}



					if(METRIC_SWITCH_POWER == glc.m_iMetric)
					{

						// 					cout << "switched, gates: " << endl;
						// output switch gate/count
						cout << "switching gates: " << glc.m_mapSwitchCount.size() << endl;
						map<int, int>::iterator mapSCIter;
						for(int i = 1; i <= glc.m_nGates; i++)
						{
							mapSCIter = glc.m_mapSwitchCount.find(i);
							if(glc.m_mapSwitchCount.end() == mapSCIter)
							{
// 								cout << i << ", ";
// 								cout << "0" << endl;
							}
							else
							{
								cout << mapSCIter->first << ", ";
// 								cout << mapSCIter->second << endl;
							}
						}




						// output switching coeff
						map<int, GateNode>::iterator mapGCIter;
						for(mapGCIter = glc.m_mapGlobalCircuit.begin(); mapGCIter != glc.m_mapGlobalCircuit.end(); mapGCIter++)
						{
							int gateId = mapGCIter->second.gateId;

							// switch count
							int nSwitchCount = 0;
							mapSCIter = glc.m_mapSwitchCount.find(gateId);
							if(glc.m_mapSwitchCount.end() == mapSCIter)
							{
								// 							cout << mapGCIter->first << ", ";
								// 							cout << "0 ";

								nSwitchCount = 0;
							}
							else
							{
								// 							cout << mapSCIter->first << ", ";
								// 							cout << mapSCIter->second << " ";
								nSwitchCount = mapSCIter->second;
							}


							// switch coeff
							int gateType = mapGCIter->second.gateType;
							int inputSize = mapGCIter->second.mapInputSignal.size();
							double coeff = g_arrSWDelayTable[gateType][inputSize-1];

							// gate size
							double gateSize = glc.g_arrRealSize[gateId - 1];

							// output generated heat for this gate
							double genHeat = nSwitchCount * coeff * gateSize;
							// 						cout << genHeat << "\t";

							// 						cout << gateId << ", " << coeff << endl;
						}

						cout << endl;

					}



// 					vector< pair<int, int> > vecCorrelation;
// 					glc.detectCorrelation(vecCorrelation);
					/*
					if(glc.m_iLPSolver == SOLVER_LP_SOLVE)
					{
					// solve the LP , using lp_solve API
					int nVars = 0;
					REAL *var = new REAL[MAX_NUM_GATES*10];
					map<int, double> mapCalcGateSize;
					map<int, double>::iterator mapCGSIter;
					int nGates = 0;
					glc.solveLP(strOutputLP, var, nVars, mapCalcGateSize);

					if(glc.m_bHTH)
					{
					mapCGSIter = mapCalcGateSize.find(glc.m_nGates+1);
					if(mapCalcGateSize.end() != mapCGSIter)
					{
					cout << "x" << mapCGSIter->first << ", ";
					cout << mapCGSIter->second << endl;
					}
					}
					else
					{
					for(mapCGSIter = mapCalcGateSize.begin(); mapCGSIter != mapCalcGateSize.end(); mapCGSIter++)
					{
					cout << "x" << mapCGSIter->first << ", ";
					cout << mapCGSIter->second << endl;
					}
					}

					delete []var;

					}*/


					glc.m_fLPout.close();
					glc.m_fCnstout.close();
					glc.m_fOFout.close();

				} // for(int iterHTHInput = 1; iterHTHInput < 23; iterHTHInput++)


			}
			else if(PI_SEGMENTATION == iPIStrategy)
			{

				// do segmentation (fill up m_vecSegOutput)
				glc.doSegmentation();

				// each output cone is a segment
				for(size_t segIter = 15; segIter < 16/*glc.m_vecSegOutput.size()*/; segIter++)
				{
					// 			cout << "Segment " << (segIter+1) << ": " << endl;
					glc.m_vecCoeff.clear();

					char strErrorRate[32];
					int iErrorRate = (int)(dErrorRate*100);
					_itoa(iErrorRate, strErrorRate, 10);

					char strNumInstances[32];
					_itoa(glc.m_nInputInstance, strNumInstances, 10);

					char strSegOutputLP[512];
					sprintf(strSegOutputLP, "outputGLC_%s_%s_%s_%s_%s_Seg%d.lp",  /*"outputGLC_%s_%s_%s_%s_%s_Seg%d_slope%f_%f_%f.lp"*/
						glc.getBenchmarkName(glc.m_iBenchmark).c_str(), strErrorRate, strNumInstances, 
						glc.getErrorModelName(glc.m_iErrorModel).c_str(), glc.getPIStrategyName(glc.m_iPIStrategy).c_str(), segIter+1/*, glc.m_debugSlop[0], glc.m_debugSlop[1], glc.m_debugSlop[2]*/);
					glc.m_fSegLPout[segIter].open(strSegOutputLP);

					glc.m_fSegLPout[segIter].setf(ios::fixed);//floating-point   numbers   are   not   written   in   e-notation   
					glc.m_fSegLPout[segIter].setf(ios::showpoint);//a   decimal   and   trailing   zeros   are   always   shown   
					glc.m_fSegLPout[segIter].precision(12);

					// OF
					glc.generateOF(segIter);

					// Input Signals
					glc.setSegPrimaryInputSignal(segIter);

					// set HTH
					if(glc.m_bHTH)
					{
						glc.addHTH(NOT, glc.m_iHTHInput);
					}

					// for each constraint
					for(int cnstIter = 0; cnstIter < glc.m_nInputInstance; cnstIter++)
					{

						glc.readPrimaryInputSignal(cnstIter);

						// setGlobalSignal
						glc.setGlobalSignal();

						// generateOneSegConstraint
						glc.generateOneLPConstraint(cnstIter+1, segIter);

						// for the 2nd half of the constraints, change the temperature
						if(METRIC_THERMAL_LEAKAGE == glc.m_iMetric && cnstIter % 2 == 1)
						{
							// set temperature
							glc.setTemperature(cnstIter);
						}
						// 					cout << "instance " << cnstIter+1 << " done!" << endl;
					}

					if(glc.m_bHTH)
					{
						glc.m_fSegLPout[segIter] << "x384 > 0;" << endl;
					}

					// generate free variables declaration
					glc.generateFreeVariables(segIter);

					// 				set<int> setCorr;
					// 				glc.detectSegCorrelation(setCorr, segIter);



					// solve the LP for this segment, using lp_solve API
					int nVars = 0;
					REAL *var = new REAL[MAX_NUM_GATES];
					map<int, double> mapCalcGateSize;
					map<int, double>::iterator mapCGSIter;
					int nGates = 0;
					glc.solveLP(strSegOutputLP, var, nVars, mapCalcGateSize, segIter);

					for(mapCGSIter = mapCalcGateSize.begin(); mapCGSIter != mapCalcGateSize.end(); mapCGSIter++)
					{
						cout << "x" << mapCGSIter->first << ", ";
						cout << mapCGSIter->second << endl;
					}

					delete []var;

					// 				if(glc.isBadSeg(segIter))
					// 				{
					// 					glc.m_vecBadSeg.push_back(segIter+1);
					// 				}


					glc.m_fSegLPout[segIter].close();

				} // for(size_t segIter = 0; segIter < glc.m_vecSegOutput.size(); segIter++)

				// 			cout << "bad Seg: ";
				// 			vector<int>::iterator vecIter;
				// 			for(vecIter = glc.m_vecBadSeg.begin(); vecIter != glc.m_vecBadSeg.end(); vecIter++)
				// 			{
				// 				cout << *vecIter << "  ";
				// 			}
				// 			cout << endl;
			}
		//}

	}  // run 50 times, using different seeds

}