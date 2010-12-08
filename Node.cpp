#include "Node.h"
#include <iostream>
#include <limits>
using namespace std;

Node::Node(void)
{
	// Set visited counter to -1.
	visited = -1;

	// Initialize critical delay calculations
	delay_so_far = 0;
	delay = 0;
	is_critical = false;
	threshold = numeric_limits<double>::infinity();
}

Node::~Node(void)
{
	// BELIEF: we do not free Nodes
}

void Node::toString() {
	// Print node type and id
	cout << NODE_TYPE_STRINGS[type] << 
		"(" << ID << ")" << endl;

	// Print inputs
	cout << "\tInputs:";
	for (list<Node*>::iterator i = inputs.begin(); i != inputs.end(); i++) {
		cout << " " << (*i)->ID;
	}
	cout << endl;

	// Print outputs
	cout << "\tOutputs:";
	for (list<Node*>::iterator i = outputs.begin(); i != outputs.end(); i++) {
		cout << " " << (*i)->ID;
	}
	cout << endl;

	// Print delay, delay_so_far, threshold, is_critical
	cout << "\tDelay: " << delay
		<< ", Delay_so_far: " << delay_so_far
		<< ", Threshold: " << threshold
		<< ", Is_critical " << is_critical;
	cout << endl;
}