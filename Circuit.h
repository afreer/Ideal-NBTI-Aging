#pragma once
#include <list>
#include "Node.h"
using namespace std;

class Circuit
{
public:
	Circuit(void);
	~Circuit(void);

	// Parser for benchmark netlist file
	void parse(char *file);

	// Circuit analysis function
	void analyze();

	// Find input nodes that are not part of transitive
	//  fan-in of any critical gate
	list<Node*> non_trans_fanin();

	// All inputs
	list<Node*> net_inputs;

	// All outputs
	list<Node*> net_outputs;

	// All gates (not inputs/outputs)
	// BELIEF: assume net_gates is in order evaluatable.
	//  --> e.g. net_gates[i]'s inputs come before net_gate[i].
	//  --> This is currently enforced by the parser.
	list<Node*> net_gates;

	// Calculation optimizations
	double global_T;
	double global_k_tp;
	double global_k_fit;
	double global_L;
	double global_n;
	double global_u;
	double global_y;
	double global_W;
	double global_Cox; 
	double global_I_S;
	double global_V_T0;
	double global_dibl;

	double global_phi;
	double global_I_S_noW;
	double global_K_d;
};
