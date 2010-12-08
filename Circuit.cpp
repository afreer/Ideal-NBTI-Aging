#include "Circuit.h"
#include "Node.h"
#include "Math.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <map>
using namespace std;

Circuit::Circuit(void)
{
	global_T = 373; // 100 C = 373 K
	global_k_tp = 1; // TODO: Random Number
	global_k_fit = 1.406; // fig 3
	global_L = 90e-9; // 90 nm technology reference???
	global_n = 1.410; // fig 3
	global_u = 1; // TODO: Random number
	global_y = 0.2; // TODO: Saro's code used this ratio I believe
	global_W = 1; // TODO: W calculations are relative
	global_Cox = 1; // TODO: Random number
	global_I_S = 0.998e-6; // fig 3
	global_V_T0 = 0.519; // fig 3 (HVT)
	global_dibl = 0.105; // fig 3

	global_phi = calcPhi(global_T);
	global_I_S_noW = global_I_S/global_W;
	global_K_d = calcKd(global_k_tp, global_k_fit, 
		global_L, global_n, global_u, global_phi);
}

Circuit::~Circuit(void)
{
}

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

void Circuit::analyze() {
	int visited = 0;
	double critical_delay = 0;
	double total_leakage = 0;
	double total_switching = 0;

	// Calculate delay, leakage, switching
	for (list<Node*>::iterator i = net_gates.begin(); i != net_gates.end(); i++) {
		// Calculate sum of output width
		double output_width = 0;
		for (list<Node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); j++) {
			output_width += (*j)->W;
		}

		// Calculate delay, leakage, switching
		(*i)->delay = 1; // TODO: Calculate
		(*i)->leakage = 1; // TODO: Calculate
		(*i)->switching = 1; // TODO: Calculate

		// Total leakage & switching
		total_leakage += (*i)->leakage;
		total_switching += (*i)->switching;
	}

	// Iterate
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
	double threshold_delay = 5; // TODO: Circuit input
	for (list<Node*>::iterator i = net_outputs.begin(); i != net_outputs.end(); i++) {
		(*i)->threshold = threshold_delay;
		if ((*i)->delay_so_far >= threshold_delay) {
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
}

list<Node*> Circuit::non_trans_fanin() {
	// Iterate in reverse through gate nodes to mark transitive inputs
	for (list<Node*>::reverse_iterator i = net_gates.rbegin(); i != net_gates.rend(); i++) {
		// If gate is critical or transitive:
		//	--> iterate through its inputs & mark them as transitive
		if ((*i)->is_critical || (*i)->is_transitive) {
			for (list<Node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); j++) {
				(*j)->is_transitive = true;
			}
		}
	}

	// List of non-transitive gates
	list<Node*> non_trans_gates;

	// Iterate through input gates and add non-transitive ones to list
	for (list<Node*>::iterator x = net_inputs.begin(); x != net_inputs.end(); x++) {
		if (!(*x)->is_transitive) {
			non_trans_gates.push_back(*x);
		}
	}

	return non_trans_gates;
}