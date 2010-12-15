#pragma once

class InputPair {
public:
	int *input1, *input2;
	double saved_orig;
	double saved_last;
	int visited;

	bool operator < (const InputPair& refParam) const {
		return saved_last > refParam.saved_last;
	}
};