#pragma once
#include <string>
#include <list>
using namespace std;

enum NodeType {
	INPUT = 0,
	OUTPUT,
	BUFF,
	NOT,
	AND,
	NAND,
	OR,
	NOR,
	XOR,
	TYPE_COUNT
};

const static char* NODE_TYPE_STRINGS[] = {
	"INPUT",
	"OUTPUT",
	"BUFF",
	"NOT",
	"AND",
	"NAND",
	"OR",
	"NOR",
	"XOR"
};

class Node
{
public:
	Node(void);
	~Node(void);

	// Function to print node info
	void toString();

	// Function to find transitive fan-in inputs
	list<Node*> trans_fanin();

	// Net ID and type
	int ID;
	NodeType type;

	// Inputs, outputs...
	list<Node*> inputs;
	list<Node*> outputs;

	// Gate characteristics
	//double L;
	double W;

	// Delay calculations
	//  --> Delay at output of this gate
	double delay_so_far;
	//  --> Threshold at output of this gate
	double threshold;
	//  --> Delay of this gate
	double delay;
	//  --> True if this gate is a critical gate
	bool is_critical;
	//  --> True if this is transitive fan-in of critical gate
	bool is_transitive;

	// Leakage calculations
	double leakage;

	// Switching calculations (should not need this per gate)
	double switching;

	// Visited (implemented as counter...increment and compare)
	int visited;
};
