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

static const int TRIALS[] = {
	10, 20, 50, 100, 200, 500, 1000, 2000, 5000
};
static const int TRIALS_LEN = 9;

void run(char* file);
void run_circuit(Circuit &circuit, bool trans_freeze);
int greedy_set_cover(Circuit &circuit, multiset<InputPair> &pairs);

int main(int argc, char* argv[]) {
	// Check for command-line arguments
	switch (argc) {
		case 1:
			run("benchmarks/c2670.txt");
			run("benchmarks/c1355.txt");
			run("benchmarks/c17.txt");
			//run("benchmarks/c1908.txt");
			//run("benchmarks/c3540.txt");
			//run("benchmarks/c432.txt");
			//run("benchmarks/c5315.txt");
			//run("benchmarks/c6288.txt");
			//run("benchmarks/c7552.txt");
			//run("benchmarks/c880.txt");
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
	// Print name of file
	cout << file << endl;

	// Parse and analyze
	Circuit circuit;
	circuit.parse(file);
	circuit.analyze();
	circuit.crit_fanin_noncrit_fanout();
	circuit.find_ideal_energy();
	circuit.print_stats();

	// Check parsing...
	//for (list<Node*>::iterator i = circuit.net_inputs.begin(); i != circuit.net_inputs.end(); i++)
	//	(*i)->print_stats();
	//for (list<Node*>::iterator i = circuit.net_gates.begin(); i != circuit.net_gates.end(); i++)
	//	(*i)->print_stats();
	//for (list<Node*>::iterator i = circuit.net_outputs.begin(); i != circuit.net_outputs.end(); i++)
	//	(*i)->print_stats();

	// Run transitive and random tests
	run_circuit(circuit, true);
	circuit.reset_reachable();
	run_circuit(circuit, false);

	// Clean line
	cout << endl;
}

void run_circuit(Circuit &circuit, bool trans_freeze) {
	// Is there a point?
	if (trans_freeze && circuit.ideal_leakage_saved_trans <= 0)
		return;
	if (circuit.ideal_leakage_saved <= 0)
		return;

	// Try vectors
	multiset<InputPair> pairs;
	int total_guesses = 0;
	int bad_guesses = 0;
	for (int trial = 0; trial < TRIALS_LEN; trial++) {
		// Guess
		int guesses = TRIALS[trial]-total_guesses;
		for (int guess = 0; guess < guesses; guess++) {
			// Generate vector inputs
			int *input1 = new int[circuit.freeze_mask_len];
			int *input2 = new int[circuit.freeze_mask_len];
			for (int j = 0; j < circuit.freeze_mask_len; j++) {
				input1[j] = rand();
				input2[j] = rand();
			}
			
			// Transitive fanin freeze
			if (trans_freeze) {
				for (int i = 0; i < circuit.freeze_mask_len; i++) {
					input2[i] = (input2[i]&(~circuit.freeze_mask[i])) |
						(input1[i]&circuit.freeze_mask[i]);
				}
			}

			// Create InputPair
			InputPair *pair = new InputPair;
			pair->input1 = input1;
			pair->input2 = input2;

			// Find new leakage energy and record
			if (circuit.apply_input_pair(pair)) {
				if (trans_freeze)
					assert(false); // For now, never affect critical
				bad_guesses++;
			} else if (circuit.leakage_saved_last == 0)
				bad_guesses++;

			// Add input pair to list
			pair->saved_orig = circuit.leakage_saved_last;
			pair->saved_last = pair->saved_orig;
			pair->visited = 0;
			pairs.insert(*pair);
		}

		// Increment total guesses
		total_guesses += guesses;
		int vectors = greedy_set_cover(circuit, pairs);
		circuit.countCovered();

		// Print stats
		if (trans_freeze) {
			printf("Transitive (%d), Covered=%d, BadGuesses=%d, Vectors=%d\n",
				total_guesses, circuit.covered_count, bad_guesses, vectors);
		} else {
			printf("Random (%d), Covered=%d, CoveredNonFreezeCoverable=%d, BadGuesses=%d, Vectors=%d\n",
				total_guesses, circuit.covered_count, circuit.nonfreeze_reachable_reached(),
				bad_guesses, vectors);
		}

		// Reset covered
		circuit.reset_covered();
	}
}

int greedy_set_cover(Circuit &circuit, multiset<InputPair> &pairs) {
	// Greedy (set cover)
	int visited = 0;
	list<InputPair> greedy;
	multiset<InputPair>::iterator cur = pairs.begin();
	while (!circuit.covered()) {
		while (true) {
			// Sanity
			assert(cur != pairs.end());

			// Check if we have already visited this input pair
			multiset<InputPair>::iterator prev = cur;
			cur++;
			if ((*prev).visited == visited) {
				// Maximum, so add to greedy
				circuit.cover(&*prev);
				greedy.push_back(*prev);
				break; // Check if everything is covered
			} else {
				// Recalculate
				circuit.apply_input_pair(&*prev);
				(*prev).saved_last = circuit.leakage_saved_last;
				(*prev).visited = visited;

				// Check if this is still the maximum
				pairs.erase(prev);
				if (cur == pairs.end() || (*prev).saved_last >= (*cur).saved_last) {
					// Still the maximum, so add to greedy
					circuit.cover(&*prev);
					greedy.push_back(*prev);
					break; // Check if everything is covered
				} else {
					// Reinsert (sorted)
					pairs.insert(*prev);
				}
			}
			
			// Increment visited
			visited++;
		}
	}

	// Reset/resuse input pairs
	for (multiset<InputPair>::iterator i = pairs.begin(); i != pairs.end();) {
		if ((*i).visited != 0) {
			multiset<InputPair>::iterator prev = i;
			(*prev).saved_last = (*prev).saved_orig;
			(*prev).visited = 0;
			i = pairs.erase(prev);
			pairs.insert(*prev);
		} else {
			i++;
		}
	}

	return greedy.size();
	//for (multiset<InputPair>::iterator cur = pairs.begin(); cur != pairs.end(); cur++) {
	//	cout << (*cur).saved_orig << " (" << (*cur).visited << ")";
	//}
	//cout << endl;

}