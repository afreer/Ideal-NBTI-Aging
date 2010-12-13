#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <cmath>
#include "Node.h" 
#include "Math.h"
#include "Circuit.h"
using namespace std;

int main(int argc, char* argv[]) {
	// Check for command-line arguments
	if (argc != 3) {
		cout << "Usage: "
			 << argv[0] << ": "
			 << "<benchmark_circuit> "
			 << "<delay_threshold> "
			 //<< "<stdev> "
			 //<< "<seed>"
			 << endl;
		return 1;
	}
	double delay_threshold = atof(argv[2]);		// threshold for finding epsilon-critical paths
	//double stdev = atof(argv[4]);				// standard deviation of process variation
	//srand(atoi(argv[5]));						// random number generator for process variability

	// Parse
	Circuit circuit;
	circuit.parse(argv[1], delay_threshold);

	// Analyze
	circuit.analyze();

	// Transitive fanin freeze method
	circuit.non_trans_fanin();

	// Try vectors
	int maxtries = 10000;
	int inputs = circuit.net_inputs.size();
	int inputRands = ((inputs-1)/32)+1;
	for (int i = 0; i < maxtries; i++) {
		// Generate vector inputs
		int[] input1 = new int[inputRands];
		int[] input2 = new int[inputRands];
		for (int j = 0; j < inputRands; j++) {
			input1[j] = rand();
			input2[j] = rand();
		}

		// Transitive fanin freeze
		for (int j = 0; j < inputRands; j++) {
			
		}

		// Apply to inputs
		int *input1_i = input1;
		int *input2_i = input2;
		int input_i = 0;
		for (list<Node*>::iterator j = circuit.net_inputs.begin(); j != circuit.net_inputs.end(); j++) {
			(*j)->input1 = (*input1_i & (1<<input_i)) == 0;
			(*j)->input2 = (*input2_i & (1<<input_i)) == 0;
			input_i++;
			if (input_i >= 32) {
				input1_i++;
				input2_i++;
				input_i = 0;
			}
		}

		// Evaluate and record

	}

	// Convex programming

	// Output for testing...
	for (list<Node*>::iterator i = circuit.net_inputs.begin(); i != circuit.net_inputs.end(); i++) {
		(*i)->toString();
	}

	for (list<Node*>::iterator i = circuit.net_gates.begin(); i != circuit.net_gates.end(); i++) {
		(*i)->toString();
	}

	for (list<Node*>::iterator i = circuit.net_outputs.begin(); i != circuit.net_outputs.end(); i++) {
		(*i)->toString();
	}

	return 0;
}
