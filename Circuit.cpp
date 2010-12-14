#include "Circuit.h"
#include "Node.h"
#include "Math.h"
#include "InputPair.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <map>
using namespace std;

Circuit::Circuit(void)
{
	global_T = 373; // 100 C = 373 K
	global_k_tp = 1; // Arbitrary/Relative Value
	global_k_fit = 1.406; // fig 3 of Markovic reference
	global_L = 65e-9; // 65 nm technology -- used in Markovic reference
	global_n = 1.410; // fig 3 of Markovic reference
	global_u = 0.08; // average of mobility of holes & electrons for 300 K
	global_y = 0.2; // TODO: Saro's code used this ratio --> 0.2 is body factor
	global_W = 1; // TODO: W calculations are relative
	global_Cox = 1730e-6; // eox/tox where tox = 20nm
	global_I_S = 0.998e-6; // fig 3 of Markovic reference
	global_V_T0 = 0.519; // fig 3 (HVT) of Markovic reference
	global_dibl = 0.105; // fig 3 of Markovic reference

	global_phi = calcPhi(global_T);
	global_I_S_noW = global_I_S/global_W;
	global_K_d = calcKd(global_k_tp, global_k_fit, 
		global_L, global_n, global_u, global_phi);

	NBTI_time = 86400; // --> assume one day for aging
}

Circuit::~Circuit(void)
{
}

// Parser for benchmark netlist file
void Circuit::parse(char *file) {	
	// Net ID -> Node map
	map<int,Node*> node_map;

	// Extract netlist from benchmark circuit
	ifstream inf(file);
	while (!inf.eof()) {
		string line;
		getline(inf, line);
		
		// Skip empty and commented lines
		if (line == "" || line[0] == '#') {
			continue;
		}
		
		// Load netlist inputs
		if (line.substr(0,6) == "INPUT(" && line[line.size()-1] == ')') {
			int n = atoi(line.substr(6,line.size()-6-1).c_str());
			assert(n >= 0);

			// Create INPUT node
			Node* tmp = new Node;
			tmp->type = INPUT;
			tmp->ID = n;

			// Add to circuit inputs + map
			node_map[n] = tmp;
			net_inputs.push_back(tmp);
		}
		
		// Load netlist outputs
		else if (line.substr(0,7) == "OUTPUT(" && line[line.size()-1] == ')') {
			int n = atoi(line.substr(7,line.size()-7-1).c_str());
			assert(n >= 0);
			
			// Create OUTPUT node
			Node* tmp = new Node;
			tmp->type = OUTPUT;
			tmp->ID = n;

			// Match output with gate and add to circuit outputs
			assert(node_map.find(n) != node_map.end());
			tmp->inputs.push_back(node_map[n]);	
			node_map[n]->outputs.push_back(tmp);
			net_outputs.push_back(tmp);
		}
		
		// Load netlist gates
		else if (line[line.size()-1] == ')') {
			size_t pos_eq = line.find('=',0);
			assert(pos_eq < line.size());			
			int n = atoi(line.substr(0,pos_eq).c_str());
			assert(n >= 0);
			
			// Create new node
			Node* tmp = new Node;	
			node_map[n] = tmp;
			tmp->ID = n;

			// Get node type
			size_t pos_open = line.find('(',pos_eq); 
			string type = line.substr(pos_eq+2,pos_open-pos_eq-2);	
			if (type.compare("BUFF") == 0)
				tmp->type = BUFF;
			else if (type.compare("NOT") == 0)
				tmp->type = NOT;
			else if (type.compare("AND") == 0)
				tmp->type = AND;
			else if (type.compare("NAND") == 0)
				tmp->type = NAND;
			else if (type.compare("OR") == 0)
				tmp->type = OR;
			else if (type.compare("NOR") == 0)
				tmp->type = NOR;
			else if (type.compare("XOR") == 0)
				tmp->type = XOR;
			else
				assert(false); // Unrecognized gate type

			// Process inputs (comma separated)
			size_t pos_comma = line.find(',',pos_open);			
			while (pos_comma < line.size()) {
				// Match input/output
				n = atoi(line.substr(pos_open+1, pos_comma-pos_open-1).c_str());
				assert(node_map.find(n) != node_map.end());
				tmp->inputs.push_back(node_map[n]);			
				node_map[n]->outputs.push_back(tmp);

				// Next input
				pos_open = pos_comma;
				pos_comma = line.find(',',pos_open+1);
			}
	
			// Last input (terminated by ')')
			pos_comma = line.find(')',pos_open);
			n = atoi(line.substr(pos_open+1, pos_comma-pos_open-1).c_str());
			assert(node_map.find(n) != node_map.end());
			tmp->inputs.push_back(node_map[n]);			
			node_map[n]->outputs.push_back(tmp);

			// Add to circuit gate list
			net_gates.push_back(tmp);
		}
	}

	// Netlist inputs, outputs, and gates are set
	//  --> no longer need file stream or Node map
	inf.close();
}

// Circuit analysis function
void Circuit::analyze() {
	critical_delay = 0;
	leakage_energy = 0;
	switching_energy = 0;

	// Calculate each gate's delay
	for (list<Node*>::iterator i = net_gates.begin(); i != net_gates.end(); i++) {
		// Calculate sum of output width
		double output_width = 0;
		for (list<Node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); j++) {
			output_width += (*j)->W;
		}

		// Calculate delay, leakage, switching
		//(*i)->delay = markovicDelay(global_K_d, V_DD,
		//	calcIC(global_dibl, V_DD, global_V_T0,
		//	global_n, global_phi), global_y, global_W,
		//	global_W); 
		(*i)->delay = 1; // FUTURE: real circuit
	}

	// Iterate and allow for delay values to propagate through paths
	for (list<Node*>::iterator i = net_gates.begin(); i != net_gates.end(); i++) {
		// Get longest delay (inputs+gate)
		for (list<Node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); j++) {
			if ((*i)->delay_so_far < (*j)->delay_so_far + (*i)->delay) {
				(*i)->delay_so_far = (*j)->delay_so_far + (*i)->delay;
			}
		}
	}

	// Get critical delay
	for (list<Node*>::iterator i = net_outputs.begin(); i != net_outputs.end(); i++) {
		// Get longest delay (inputs)
		for (list<Node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); j++) {
			if ((*j)->delay_so_far > critical_delay) 
				critical_delay = (*j)->delay_so_far;
		}		
	}

	// Set threshold for all the outputs
	// --> TODO: Calculate delay_threshold in terms of worst case NBTI 
	double delay_threshold = critical_delay; 
	for (list<Node*>::iterator i = net_outputs.begin(); i != net_outputs.end(); i++) {
		(*i)->threshold = delay_threshold;
		if ((*i)->delay_so_far >= delay_threshold) {
			(*i)->is_critical = true;
		}
	}

	// Calculate thresholds for all the remaining gates in reverse order
	for (list<Node*>::reverse_iterator i = net_gates.rbegin(); i != net_gates.rend(); i++) {
		// For all outputs, calculate the lowest threshold
		for (list<Node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); j++) {
			if ((*i)->threshold > (*j)->threshold - (*j)->delay) {
				(*i)->threshold = (*j)->threshold - (*j)->delay;
			}
		}

		// Check if delay exceeds threshold
		if ((*i)->delay_so_far >= (*i)->threshold)
			(*i)->is_critical = true;
	}

	// Set clock period of circuit as critical delay
	T_clk = critical_delay;

	// Calculate leakage and switching energy
	for (list<Node*>::iterator i = net_gates.begin(); i != net_gates.end(); i++) {
		// Calculate for each gate
		//(*i)->leakage_energy = T_clk*V_DD*markovicLeakageCurrent(global_I_S_noW, global_W,
		//	global_dibl, V_DD, global_V_T0, global_n, global_phi); 
		(*i)->leakage_energy = 1; // TODO: real circuit
		(*i)->switching_energy = 1; // TODO: real circuit

		// Total leakage & switching
		leakage_energy += (*i)->leakage_energy;
		switching_energy += (*i)->switching_energy;
	}
}

// Find input nodes that are not part of transitive
//  fan-in of any critical gate
void Circuit::crit_fanin_noncrit_fanout() {
	// Iterate in reverse through gate nodes to mark transitive inputs
	for (list<Node*>::reverse_iterator i = net_gates.rbegin(); i != net_gates.rend(); i++) {
		// If gate is critical or transitive:
		//	--> iterate through its inputs & mark them as transitive
		if ((*i)->is_critical || (*i)->crit_trans_fanin) {
			(*i)->crit_trans_fanin = true;
			for (list<Node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); j++) {
				(*j)->crit_trans_fanin = true;
			}
		}
	}

	// Transitive fanout (inputs)
	for (list<Node*>::iterator i = net_inputs.begin(); i != net_inputs.end(); i++) {
		if (!(*i)->crit_trans_fanin) {
			(*i)->noncrit_trans_fanout = true;
			for (list<Node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); j++) {
				(*j)->noncrit_trans_fanout = true;
			}
		}
	}

	// Transitive fanout (gates)
	for (list<Node*>::iterator i = net_gates.begin(); i != net_gates.end(); i++) {
		if ((*i)->noncrit_trans_fanout) {
			for (list<Node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); j++) {
				(*j)->noncrit_trans_fanout = true;
			}
		}
	}
	
	// Generate freeze mask
	freeze_mask_len = (((int)net_inputs.size()-1)/32)+1;
	freeze_mask = new int[freeze_mask_len];
	int *mask = freeze_mask;
	int mask_i = 0;
	*mask = 0;
	for (list<Node*>::iterator i = net_inputs.begin(); i != net_inputs.end(); i++) {
		if ((*i)->crit_trans_fanin) 
			*mask |= 1 << mask_i;
		mask_i++;
		if (mask_i >= 32) {
			mask++;
			*mask = 0;
			mask_i = 0;
		}
	}
}

// Find circuit's leakage energy after ideal aging
// --> assume all critical gates do not get aged
// --> assume all non-critical gates get aged
// --> assume constant Vdd (only change in leakage energy)
void Circuit::find_ideal_energy() {
	// Calculate ideal leakage energy saved
	ideal_leakage_saved = 0;
	ideal_leakage_saved_trans = 0;

	// BELIEF: New Vth will always be greater than global Vth and so ideal
	//         will always be less than circuit's actual leakage energy

	// Iterate through gate nodes
	for (list<Node*>::iterator i = net_gates.begin(); i != net_gates.end(); i++) {
		// If gate is non-critical:
		//	--> find "aged" Vth and calculate gate's resulting leakage energy
		//  --> add new leakage energy to ideal_energy
		if (!(*i)->is_critical) {
			// Assume switching duty cycle of 0.5 for these gates
			//double Vth_new = global_V_T0 + wangDeltaV_th(WANG_B, 0.5, NBTI_time);
			//double leakage_energy_new = T_clk*V_DD*markovicLeakageCurrent(global_I_S_noW, global_W,
			//	global_dibl, V_DD, Vth_new, global_n, global_phi);
			double leakage_energy_new = 0; // FUTURE: real circuit

			// Subtract old leakage and add new
			ideal_leakage_saved += (*i)->leakage_energy;
			ideal_leakage_saved -= leakage_energy_new;
			if ((*i)->noncrit_trans_fanout) {
				ideal_leakage_saved_trans += (*i)->leakage_energy;
				ideal_leakage_saved_trans -= leakage_energy_new;
			}
		}	
	}
}

// Print critical delay, total leakage, ideal leakage
void Circuit::print_stats() {
	cout << "Critical delay: " << critical_delay 
		<< "\nLeakage energy: " << leakage_energy
		<< "\nIdeal saved leakage energy: " << ideal_leakage_saved
		<< "\nIdeal saved transitive leakage energy: " << ideal_leakage_saved_trans 
		<< "\nInputs=" << net_inputs.size()
		<< ", Gates=" << net_gates.size() 
		<< ", Outputs=" << net_outputs.size()
		<< "\nTrans. Mask = ";
	for (int i = 0; i < freeze_mask_len; i++) {
		printf("%08x ", freeze_mask[i]);
	}
	cout << endl;
}

// Apply input pair.  Calculate the new leakage energy.
//  Return true if critical gates are switched.
//  Gates are marked as reachable.
bool Circuit::apply_input_pair(InputPair *pair) {
	// Propagate through gates
	apply_input_pair_to_inputs(pair);
	bool ret = false;
	leakage_saved_last = 0;
	for (list<Node*>::iterator i = net_gates.begin(); i != net_gates.end(); i++) {
		// Calculate node output
		(*i)->calc_output();
		if ((*i)->output1 != (*i)->output2) {
			// Switched
			if ((*i)->is_critical)
				ret = true;
			(*i)->reachable = true;

			// Calc new leakage energy
			//double Vth_new = global_V_T0 + wangDeltaV_th(WANG_B, 0.5, NBTI_time);
			//double leakage_energy_new = T_clk*V_DD*markovicLeakageCurrent(global_I_S_noW, global_W,
			//	global_dibl, V_DD, Vth_new, global_n, global_phi);
			double leakage_energy_new = 0; // FUTURE: real circuit

			// Subtract old leakage and add new
			leakage_saved_last += (*i)->leakage_energy;
			leakage_saved_last -= leakage_energy_new;
		}
	}
	return ret;
}

// Return true if some gates are not covered
bool Circuit::covered() {
	for (list<Node*>::iterator i = net_gates.begin(); i != net_gates.end(); i++) {
		if ((*i)->reachable && !(*i)->covered)
			return false;
	}
	return true;
}

void Circuit::countCovered() {
	covered_count = 0;
	for (list<Node*>::iterator i = net_gates.begin(); i != net_gates.end(); i++) {
		if ((*i)->covered)
			covered_count++;
	}
}

// Apply input pair to just the inputs
void Circuit::apply_input_pair_to_inputs(InputPair *pair) {
	// Apply to inputs
	int *input1_i = pair->input1;
	int *input2_i = pair->input2;
	int input_i = 0;
	for (list<Node*>::iterator j = net_inputs.begin(); j != net_inputs.end(); j++) {
		(*j)->output1 = (*input1_i & (1<<input_i)) == 0;
		(*j)->output2 = (*input2_i & (1<<input_i)) == 0;
		input_i++;
		if (input_i >= 32) {
			input1_i++;
			input2_i++;
			input_i = 0;
		}
	}
}

// Mark gates affected by this input pair as covered
void Circuit::cover(InputPair *pair) {
	apply_input_pair_to_inputs(pair);
	for (list<Node*>::iterator i = net_gates.begin(); i != net_gates.end(); i++) {
		(*i)->calc_output();
		if ((*i)->output1 != (*i)->output2) {
			(*i)->covered = true;
		}
	}
}
