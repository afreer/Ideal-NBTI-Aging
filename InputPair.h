#pragma once

class InputPair {
public:
	int *input1, *input2;
	double leakage_energy;
	double remaining;
	int visited;

	bool operator < (const InputPair& refParam) const {
		return remaining < refParam.remaining;
	}
};