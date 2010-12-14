#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <cmath>
#include "Node.h" 
#include "Math.h"
#include "Circuit.h"
#include "InputPair.h"
using namespace std;

int main(int argc, char* argv[]) {
	// Check for command-line arguments
	if (argc != 2) {
		cout << "Usage: "
			 << argv[0] << ": "
			 << "<benchmark_circuit> "
			 //<< "<delay_threshold> "
			 //<< "<stdev> "
			 //<< "<seed>"
			 << endl;
		return 1;
	}
	//double delay_threshold = atof(argv[2]);		// threshold for finding epsilon-critical paths
	//double stdev = atof(argv[4]);				// standard deviation of process variation
	//srand(atoi(argv[5]));						// random number generator for process variability

	// Parse and analyze
	Circuit circuit;
	circuit.parse(argv[1]);
	circuit.analyze();
	circuit.find_ideal_energy();
	circuit.non_trans_fanin();
	circuit.printStats();

	// Try vectors
	list<InputPair*> pairs;
	int maxtries = 0; // TODO: input
	for (int guess = 0; guess < maxtries; guess++) {
		// Generate vector inputs
		int *input1 = new int[circuit.freeze_mask_len];
		int *input2 = new int[circuit.freeze_mask_len];
		for (int j = 0; j < circuit.freeze_mask_len; j++) {
			input1[j] = rand();
			input2[j] = rand();
		}

		// Transitive fanin freeze
		for (int i = 0; i < circuit.freeze_mask_len; i++) {
			input2[i] = (input2[i]&circuit.freeze_mask[i]) |
				(input1[i]&circuit.freeze_mask[i]);
		}

		// Apply to inputs
		int *input1_i = input1;
		int *input2_i = input2;
		int input_i = 0;
		for (list<Node*>::iterator j = circuit.net_inputs.begin(); j != circuit.net_inputs.end(); j++) {
			(*j)->output1 = (*input1_i & (1<<input_i)) == 0;
			(*j)->output2 = (*input2_i & (1<<input_i)) == 0;
			input_i++;
			if (input_i >= 32) {
				input1_i++;
				input2_i++;
				input_i = 0;
			}
		}

		// Find new leakage energy and record
		if (!circuit.apply_input_pair()) 
			continue; // TODO: support critical gate aging
		InputPair *pair = new InputPair;
		pair->input1 = input1;
		pair->input2 = input2;
		pair->leakage_energy = circuit.last_leakage_energy;
		pairs.push_front(pair);
	}

	// Convex programming

	// Output for testing...
	for (list<Node*>::iterator i = circuit.net_inputs.begin(); i != circuit.net_inputs.end(); i++)
		(*i)->print_stats();
	for (list<Node*>::iterator i = circuit.net_gates.begin(); i != circuit.net_gates.end(); i++)
		(*i)->print_stats();
	for (list<Node*>::iterator i = circuit.net_outputs.begin(); i != circuit.net_outputs.end(); i++)
		(*i)->print_stats();

	return 0;
}
