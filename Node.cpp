#include "Node.h"
#include <iostream>
#include <limits>
#include <cassert>
using namespace std;

Node::Node(void)
{
	// Gate characteristics
	W = 1; // TODO: Model this
	L = 1; // TODO: Model this

	// Initialize calculation parameters
	visited = -1;
	delay_so_far = 0;
	delay = 0;
	is_critical = false;
	crit_trans_fanin = false;
	noncrit_trans_fanout = false;
	threshold = numeric_limits<double>::infinity();
	reachable = false;
	covered = false;
}

Node::~Node(void)
{
	// BELIEF: we do not free Nodes
}

void Node::print_stats() {
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
		<< ", Is_critical " << is_critical
		<< ", crit_trans_fanin " << crit_trans_fanin
		<< endl;
}

void Node::calc_output() {
	Node *input;
	switch (type) {
		case INPUT:
			assert(false);
			break;
		case OUTPUT:
			assert(false);
			break;
		case BUFF:
			input = inputs.front();
			output1 = input->output1;
			output2 = input->output2;
			break;
		case NOT:
			input = inputs.front();
			output1 = !input->output1;
			output2 = !input->output2;
			break;
		case AND:
		case NAND:
			output1 = true;
			output2 = true;		
			for (list<Node*>::iterator i = inputs.begin(); i != inputs.end(); i++) {
				output1 = output1 && (*i)->output1;
				output2 = output2 && (*i)->output2;
			}
			if (type == NAND) {
				output1 = !output1;
				output2 = !output2;
			}
			break;
		case OR:
		case NOR:
			output1 = false;
			output2 = false;		
			for (list<Node*>::iterator i = inputs.begin(); i != inputs.end(); i++) {
				output1 = output1 || (*i)->output1;
				output2 = output2 || (*i)->output2;
			}
			if (type == NOR) {
				output1 = !output1;
				output2 = !output2;
			}
			break;
		case XOR:
			output1 = false;
			output2 = false;		
			for (list<Node*>::iterator i = inputs.begin(); i != inputs.end(); i++) {
				if ((*i)->output1)
					output1 = !output1;
				if ((*i)->output2)
					output2 = !output2;
			}
			break;
		default:
			assert(false);
	}
}