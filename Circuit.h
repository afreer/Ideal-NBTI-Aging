#pragma once
#include <list>
#include "Node.h"
using namespace std;

class Circuit
{
public:
	// >>> CIRCUIT FUNCTIONS <<<
	Circuit(void);
	~Circuit(void);
	void parse(char *file);
	void analyze();
	void printStats();
	void non_trans_fanin();
	void find_ideal_energy();
	bool apply_input_pair(); 

	// >>> CIRCUIT VARIABLES <<<

	// Total ideal leakage energy
	double ideal_leakage_energy;

	// Critical gate transitive fanin freeze bit mask
	int *freeze_mask;
	int freeze_mask_len;

	// All inputs
	list<Node*> net_inputs;

	// All outputs
	list<Node*> net_outputs;

	// All gates (not inputs/outputs)
	// BELIEF: assume net_gates is in order evaluatable.
	//  --> e.g. net_gates[i]'s inputs come before net_gate[i].
	//  --> This is currently enforced by the parser.
	list<Node*> net_gates;

	// Original totals
	double total_leakage;
	double total_switching;
	double critical_delay;

	// Temporary
	double last_leakage_energy;

	// Input parameters
	double V_DD;
	double T_clk;
	double aging_time;

	// Amount of time for aging via NBTI degradation
	double NBTI_time;

	// Parameters
	double global_T;		// temperature
	double global_k_tp;		// delay-fitting parameter
	double global_k_fit;	// model-fitting parameter
	double global_L;		// length of channel
	double global_n;		// subthreshold slope
	double global_u;		// mobility of charge carrier
	double global_y;		// TODO: Is this the ratio of gate parasitic to input capacitance or is it body factor?
	double global_W;		// width of channel
	double global_Cox;		// oxide capacitance
	double global_V_T0;		// initial threshold voltage
	double global_dibl;		// drain induced barrier lowering (sigma character)

	// Derived parameters
	double global_phi;		// thermal voltage (kT/q)

	// Calculation optimizations
	double global_I_S;		// current when Vgs = Vt
	double global_I_S_noW;	// calculated term (global_I_S/global_W)
	double global_K_d;		// calculated term (ktp*kfit*L^2 / 2*n*u*phi)
};
