#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <map>
#include <string>
#include <cmath>
#include <cassert>
#include "Node.h" 
#include "Math.h"
#include "Circuit.h"
#include "InputPair.h"
using namespace std;

void run(char* file);

int main(int argc, char* argv[]) {
	// Check for command-line arguments
	switch (argc) {
		case 1:

			break;
		case 2:
			run(argv[1]);
			break;
		default:
			cout << "Usage: "
				 << argv[0] << ": "
				 << "<benchmark_circuit> "
				 //<< "<delay_threshold> "
				 //<< "<stdev> "
				 //<< "<seed>"
				 << endl;
			return 1;
			break;
	}
	//double delay_threshold = atof(argv[2]);		// threshold for finding epsilon-critical paths
	//double stdev = atof(argv[4]);				// standard deviation of process variation
	//srand(atoi(argv[5]));						// random number generator for process variability
	return 0;
}

void run(char* file) {
	// Parse and analyze
	Circuit circuit;
	circuit.parse(file);
	circuit.analyze();
	circuit.crit_fanin_noncrit_fanout();
	circuit.find_ideal_energy();
	circuit.print_stats();

	// Try vectors
	multiset<InputPair*> pairs;
	if (circuit.ideal_leakage_saved_trans > 0) {
		int maxtries = 1; // TODO: input
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
				input2[i] = (input2[i]&(~circuit.freeze_mask[i])) |
					(input1[i]&circuit.freeze_mask[i]);
			}

			// Create InputPair
			InputPair *pair = new InputPair;
			pair->input1 = input1;
			pair->input2 = input2;

			// Find new leakage energy and record
			if (circuit.apply_input_pair(pair)) 
				assert(false); // For now, never affect critical

			// Add input pair to list
			pair->saved_orig = circuit.leakage_saved_last;
			pair->saved_last = pair->saved_orig;
			pair->visited = 0;
			pairs.insert(pair);
		}
	}

	// Hamming distance on top vectors

	// Greedy (set cover)
	int visited = 0;
	list<InputPair*> greedy;
	multiset<InputPair*>::iterator i = pairs.begin();
	while (!circuit.covered()) {
		while (i != pairs.end()) {
			multiset<InputPair*>::iterator temp = i;
			
			// Check if we have already visited this input pair
			if ((*i)->visited == visited) {
				// Maximum, so add to greedy
				circuit.cover(*temp);
				greedy.push_back(*temp);
				i = pairs.erase(temp);
			} else {
				// Recalculate
				circuit.apply_input_pair(*temp);
				(*temp)->saved_last = circuit.leakage_saved_last;
				(*temp)->visited = visited;

				// Check if this is still the maximum
				i++;
				i = pairs.erase(temp);
				if ((*temp)->saved_last >= (*i)->saved_last) {
					// Still the maximum, so add to greedy
					circuit.cover(*temp);
					greedy.push_back(*temp);
				} else {
					// Reinsert (sorted)
					pairs.insert(*temp);
				}
			}
		}
	}
	circuit.countCovered();
	cout << "Greedy: Covered = " << circuit.covered_count 
		<< " Vectors = " << greedy.size()
		<< endl;
	
	// Linear programming (set cover)

	// Convex programming (true optimality)

	// Output for testing...
	for (list<Node*>::iterator i = circuit.net_inputs.begin(); i != circuit.net_inputs.end(); i++)
		(*i)->print_stats();
	for (list<Node*>::iterator i = circuit.net_gates.begin(); i != circuit.net_gates.end(); i++)
		(*i)->print_stats();
	for (list<Node*>::iterator i = circuit.net_outputs.begin(); i != circuit.net_outputs.end(); i++)
		(*i)->print_stats();
}
