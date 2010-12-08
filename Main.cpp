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
	if (argc != 2) {
		cout << "Usage: "
			 << argv[0] << ": "
			 << "<benchmark_circuit> "
			 //<< "<threshold_delay> "
			 //<< "<stdev> "
			 //<< "<seed>"
			 << endl;
		return 1;
	}
	//double threshold_delay = atof(argv[2]);		// threshold for finding epsilon-critical paths
	//double stdev = atof(argv[4]);				// standard deviation of process variation
	//srand(atoi(argv[5]));						// random number generator for process variability

	// Parse
	Circuit circuit;
	circuit.parse(argv[1]);

	// Analyze
	circuit.analyze();

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
