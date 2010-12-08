#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

double gngauss(double mean, double sigma) {
	double u, r;
	u=(double)rand()/(double) RAND_MAX;
	if (u==1.0) u = 0.999999999;
	r=sigma*sqrt(2.0*log(1.0/(1.0 - u)));
	u=(double)rand()/(double) RAND_MAX;
	if (u==1.0) u = 0.999999999;
	return((double)(mean+r*cos(2*3.14159265*u)));
}

double generateDelay(double V = 1.2, double Vth0 = 0.1, double Vth_init = 0.3)
{
	double Vth = V;
	while (Vth == V) {
		Vth = Vth_init * fabs(gngauss(0,1));
	}

	double delay = V*(1+Vth/Vth0)/((V-Vth)*(V-Vth));
	return delay;
}

void generateSizes(const int& num_gates, double* sizes, double stdev) {
	for (int i = 0; i < num_gates; ++i) {
		// sizes[i] = gngauss(0.25, 0.05);
		sizes[i] = fabs(gngauss(1, stdev));
	}
	int gates_per_grid = num_gates / 16;
	for (int i = 0; i < 16; ++i) {
		double rnd = gngauss(0.1, 0.025);
		for (int j = 0; j < gates_per_grid; ++j) {
			sizes[i*gates_per_grid+j] += rnd;
		}
	}
	gates_per_grid = num_gates / 4;
	for (int i = 0; i < 4; ++i) {
		double rnd = gngauss(0.1, 0.025);
		for (int j = 0; j < gates_per_grid; ++j) {
			sizes[i*gates_per_grid+j] += rnd;
		}
	}
	for (int i = 0; i < num_gates; ++i) {
		sizes[i] = fabs(sizes[i]);
	}
}

class node {
 public:
	void init(const string& gate_type,
			  const int& num_circuits,
			  const double& threshold_delay) {
		type = gate_type;
		transitive_fanin = 0;
		transitive_fanout = 0;
		times_critical = 0;
		times_modified = 0;

		visited = false;

		L = new double[num_circuits];
		W = new double[num_circuits];
		delay = new double[num_circuits];
		leakage = new double[num_circuits];
		switching = new double[num_circuits];

		delay_so_far = new double[num_circuits];
		threshold = new double[num_circuits];

		is_critical = new bool[num_circuits];

		for (int n = 0; n < num_circuits; ++n) {
			L[n] = 1;
			W[n] = 1;
			delay[n] = 0;
			leakage[n] = 0;
			switching[n] = 0;
			delay_so_far[n] = 0;
			threshold[n] = threshold_delay;
			is_critical[n] = false;
		}
	}

	string type;
	list<node*> inputs;
	list<node*> outputs;
	int transitive_fanin;
	int transitive_fanout;
	int times_critical;
	int times_modified;

	bool visited;

	double* L;
	double* W;
	double* delay;
	double* leakage;
	double* switching;

	double* delay_so_far;
	double* threshold;

	bool* is_critical;
};

void unvisit(list<node*>* inputs, list<node*>* gates, list<node*>* outputs) {
	for (list<node*>::iterator i = inputs->begin(); i != inputs->end(); ++i) {
		(*i)->visited = false;
	}
	for (list<node*>::iterator i = gates->begin(); i != gates->end(); ++i) {
		(*i)->visited = false;
	}
	for (list<node*>::iterator i = outputs->begin(); i != outputs->end(); ++i) {
		(*i)->visited = false;
	}
}

void reset(const int& threshold_delay,
		   const int& num_circuits,
		   list<node*>* inputs,
		   list<node*>* gates,
		   list<node*>* outputs) {
	for (list<node*>::iterator i = inputs->begin(); i != inputs->end(); ++i) {
		(*i)->times_critical = 0;
		(*i)->times_modified = 0;
		(*i)->visited = false;
		for (int n = 0; n < num_circuits; ++n) {
			(*i)->delay[n] = 0;
			(*i)->leakage[n] = 0;
			(*i)->switching[n] = 0;
			(*i)->delay_so_far[n] = 0;
			(*i)->threshold[n] = threshold_delay;
			(*i)->is_critical[n] = false;
		}
	}
	for (list<node*>::iterator i = gates->begin(); i != gates->end(); ++i) {
		(*i)->times_critical = 0;
		(*i)->visited = false;
		for (int n = 0; n < num_circuits; ++n) {
			(*i)->delay[n] = 0;
			(*i)->leakage[n] = 0;
			(*i)->switching[n] = 0;
			(*i)->delay_so_far[n] = 0;
			(*i)->threshold[n] = threshold_delay;
			(*i)->is_critical[n] = false;
		}
	}
	for (list<node*>::iterator i = outputs->begin(); i != outputs->end(); ++i) {
		(*i)->times_critical = 0;
		(*i)->visited = false;
		for (int n = 0; n < num_circuits; ++n) {
			(*i)->delay[n] = 0;
			(*i)->leakage[n] = 0;
			(*i)->switching[n] = 0;
			(*i)->delay_so_far[n] = 0;
			(*i)->threshold[n] = threshold_delay;
			(*i)->is_critical[n] = false;
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc != 5) {
		cout << "usage: "
			 << argv[0] << ": "
			 << "<netlist> "
			 << "<stdev> "
			 << "<num_circuits> "
			 << "<seed>\n";
		return 1;
	}

	double stdev = atof(argv[2]);
	int num_circuits = atoi(argv[3]);

	srand(atoi(argv[4]));

	map<int,node*> node_map;
	list<node*> inputs;
	list<node*> outputs;
	list<node*> gates;

	ifstream inf(argv[1]);
	while (!inf.eof()) {
		string line;
		getline(inf, line);
		if (line[0] == '#') {
			continue;
		}
		if (line.substr(0,6) == "INPUT(" && line[line.size()-2] == ')') {
			// load inputs
			int n = atoi(line.substr(6,line.size()-8).c_str());
			if (n < 0) {
				cout << "node less than zero\n";
				return 1;
			}
			node* tmp = new node;
			tmp->init("INPUT", num_circuits+num_test_circuits, threshold_delay);
			node_map[n] = tmp;

			inputs.push_back(tmp);
		} else if (line.substr(0,7) == "OUTPUT(" && line[line.size()-2] == ')') {
			// load outputs
			int n = atoi(line.substr(7,line.size()-9).c_str());
			if (n < 0) {
				cout << "node less than zero\n";
				return 1;
			}
			node* tmp = new node;
			tmp->init("OUTPUT", num_circuits+num_test_circuits, threshold_delay);
			tmp->inputs.push_back(node_map[n]);
			node_map[n]->outputs.push_back(tmp);

			outputs.push_back(tmp);
		} else if (line[line.size()-2] == ')') {
			// load gates
			size_t pos_eq = line.find('=',0);
			if (pos_eq >= line.size()) {
				cout << "malformed input\n";
				return 1;
			}
			int n = atoi(line.substr(0,pos_eq-1).c_str());
			if (n < 0) {
				cout << "node less than zero\n";
				return 1;
			}

			node* tmp = new node;
			node_map[n] = tmp;

			size_t pos_open = line.find('(',pos_eq);
			string type = line.substr(pos_eq+2,pos_open-pos_eq-2);
			tmp->init(type, num_circuits+num_test_circuits, threshold_delay);
			if (type == "NOT" || type == "BUFF") {
				// unary gates
				n = atoi(line.substr(pos_open+1,line.size()-pos_open-2).c_str());
				tmp->inputs.push_back(node_map[n]);
				node_map[n]->outputs.push_back(tmp);
			} else {
				// binary gates
				size_t pos_comma = pos_open;
				pos_comma = line.find(',',pos_open);
				n = atoi(line.substr(pos_open+1,pos_comma-pos_open-1).c_str());
				tmp->inputs.push_back(node_map[n]);
				node_map[n]->outputs.push_back(tmp);
				size_t pos_comma_old = pos_comma;
				while (1) {
					pos_comma_old = pos_comma;
					pos_comma = line.find(',',pos_comma_old+1);
					if (pos_comma >= line.size()) {
						break;
					}
					n = atoi(line.substr(pos_comma_old+2,pos_comma-pos_comma_old-2).c_str());
					tmp->inputs.push_back(node_map[n]);
					node_map[n]->outputs.push_back(tmp);
				}
				n = atoi(line.substr(pos_comma_old+2,line.size()-pos_comma_old-3).c_str());
				tmp->inputs.push_back(node_map[n]);
				node_map[n]->outputs.push_back(tmp);
			}

			gates.push_back(tmp);
		}
	}
	node_map.clear();

	// transitive fan-in
	for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
		unvisit(&inputs, &gates, &outputs);

		list<node*> queue;
		for (list<node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); ++j) {
			queue.push_back(*j);
		}
		for (list<node*>::iterator j = queue.begin(); j != queue.end(); ++j) {
			if ((*j)->visited) {
				continue;
			}
			(*j)->visited = true;
			(*i)->transitive_fanin++;
			for (list<node*>::iterator k = (*j)->inputs.begin(); k != (*j)->inputs.end(); ++k) {
				queue.push_back(*k);
			}
		}
	}

	// transitive fan-out
	for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
		unvisit(&inputs, &gates, &outputs);

		list<node*> queue;
		for (list<node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); ++j) {
			queue.push_back(*j);
		}
		for (list<node*>::iterator j = queue.begin(); j != queue.end(); ++j) {
			if ((*j)->visited) {
				continue;
			}
			(*j)->visited = true;
			(*i)->transitive_fanout++;
			for (list<node*>::iterator k = (*j)->outputs.begin(); k != (*j)->outputs.end(); ++k) {
				queue.push_back(*k);
			}
		}
	}

	// process variation
	for (int n = 0; n < num_circuits+num_test_circuits; ++n) {
		double* sizes = new double[gates.size()];
		generateSizes(gates.size(), sizes, stdev);
		int size_index = 0;
		for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
			(*i)->L[n] = sizes[size_index++];
		}
		delete[] sizes;
	}

	/*

	CALCULATE DELAY, LEAKAGE, SWITCHING

	phi = k*T/q
	tmp = ln(exp(((1+o)*V_dd-V_th)/(2*n*phi)+1))
	delay = k_tp*C_L*V_dd/(2*n*u*C_ox*W/L*phi*phi)*k_fit/(tmp*tmp)
	leakage = 2*n*mu*C_ox*W/L*phi*phi*D*V_dd*exp((o*V_dd-V_th)/(n*phi))
	switching = a*C_ox*W*L*V_dd*V_dd

	*/

	int prev_delay_below = 0;
	int prev_leakage_below = 0;
	int prev_switching_below = 0;
	int prev_all_below = 0;

	int max_delay_below = 0;
	int max_leakage_below = 0;
	int max_switching_below = 0;
	int max_all_below = 0;

	int times = 0;
	int times_same = 0;

	double* W_opt = new double[gates.size()];

	char next = 'f';
	while (times_same != 100) {
		double* critical_delay = new double[num_circuits];
		double* total_leakage = new double[num_circuits];
		double* total_switching = new double[num_circuits];

		reset(threshold_delay, num_circuits, &inputs, &gates, &outputs);
		for (int n = 0; n < num_circuits; ++n) {
			unvisit(&inputs, &gates, &outputs);
			critical_delay[n] = 0;
			total_leakage[n] = 0;
			total_switching[n] = 0;

			for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
				double gain = 0;
				for (list<node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); ++j) {
					gain += (*j)->W[n]*(*j)->L[n] / ((*i)->W[n]*(*i)->L[n]);
				}
				(*i)->delay[n] = (*i)->L[n] / (*i)->W[n] * (1 + 0.2*gain);
				(*i)->leakage[n] = (*i)->W[n] / (*i)->L[n];
				(*i)->switching[n] = (*i)->W[n] * (*i)->L[n];

				total_leakage[n] += (*i)->leakage[n];
				total_switching[n] += (*i)->switching[n];
			}

			list<node*> queue;
			for (list<node*>::iterator i = inputs.begin(); i != inputs.end(); ++i) {
				queue.push_back(*i);
			}
			for (list<node*>::iterator i = queue.begin(); i != queue.end(); ++i) {
				if ((*i)->visited) {
					continue;
				}

				bool visit_node = true;
				for (list<node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); ++j) {
					if (!(*j)->visited) {
						visit_node = false;
						break;
					}
				}
				if (!visit_node) {
					continue;
				}
				(*i)->visited = true;
				for (list<node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); ++j) {
					if ((*i)->delay_so_far[n] < (*j)->delay_so_far[n] + (*i)->delay[n]) {
						(*i)->delay_so_far[n] = (*j)->delay_so_far[n] + (*i)->delay[n];
					}
				}

				if ((*i)->type == "OUTPUT" && (*i)->delay_so_far[n] > critical_delay[n]) {
					critical_delay[n] = (*i)->delay_so_far[n];
				}

				for (list<node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); ++j) {
					queue.push_back(*j);
				}
			}

			unvisit(&inputs, &gates, &outputs);
			queue.clear();
			for (list<node*>::iterator i = outputs.begin(); i != outputs.end(); ++i) {
				(*i)->visited = true;
				(*i)->threshold[n] = threshold_delay;
				if ((*i)->delay_so_far[n] >= (*i)->threshold[n]) {
					(*i)->is_critical[n] = true;
				}
				for (list<node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); ++j) {
					queue.push_back(*j);
				}
			}
			for (list<node*>::iterator i = queue.begin(); i != queue.end(); ++i) {
				if ((*i)->visited) {
					continue;
				}

				bool visit_node = true;
				for (list<node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); ++j) {
					if (!(*j)->visited) {
						visit_node = false;
						break;
					}
				}
				if (!visit_node) {
					continue;
				}
				(*i)->visited = true;

				for (list<node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); ++j) {
					if ((*i)->threshold[n] > (*j)->threshold[n] - (*j)->delay[n]) {
						(*i)->threshold[n] = (*j)->threshold[n] - (*j)->delay[n];
					}
				}
				if ((*i)->delay_so_far[n] >= (*i)->threshold[n]) {
					(*i)->is_critical[n] = true;
				}
				for (list<node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); ++j) {
					queue.push_back(*j);
				}
			}
		}

		int delay_below = 0;
		int leakage_below = 0;
		int switching_below = 0;
		int all_below = 0;
		for (int i = 0; i < num_circuits; ++i) {
			if (critical_delay[i] <= threshold_delay) {
				++delay_below;
			}
			if (total_leakage[i] <= threshold_leakage) {
				++leakage_below;
			}
			if (total_switching[i] <= threshold_switching) {
				++switching_below;
			}
			if (critical_delay[i] <= threshold_delay &&
				total_leakage[i] <= threshold_leakage &&
				total_switching[i] <= threshold_switching) {
				++all_below;
			}
		}

		int gate_index = 0;
		if (all_below > max_all_below) {
			max_delay_below = delay_below;
			max_leakage_below = leakage_below;
			max_switching_below = switching_below;
			max_all_below = all_below;
			for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
				W_opt[gate_index++] = (*i)->W[0];
			}
		}

		if (next == 'f') {
			next = 'd';
			/*cout << delay_below << " "
				 << leakage_below << " "
				 << switching_below << " "
				 << all_below << "\n";*/
			reps_learn_init_delay_below += delay_below;
			reps_learn_init_leakage_below += leakage_below;
			reps_learn_init_switching_below += switching_below;
			reps_learn_init_all_below += all_below;
		} else if (next == 'd') {
			if (all_below <= prev_all_below &&
				(leakage_below != num_circuits ||
				 switching_below != num_circuits)) {
				next = 'e';
			}
		} else if (next == 'e') {
			if (all_below <= prev_all_below &&
				delay_below != num_circuits) {
				next = 'd';
			}
		}
		/*cerr << delay_below << " "
			 << leakage_below << " "
			 << switching_below << " "
			 << all_below << "\t"
			 << max_delay_below << " "
			 << max_leakage_below << " "
			 << max_switching_below << " "
			 << max_all_below << "\t"
			 << next << "\n";*/

		if (prev_delay_below == delay_below &&
			prev_leakage_below == leakage_below &&
			prev_switching_below == switching_below) {
			++times_same;
		} else{
			times_same = 0;
		}

		prev_delay_below = delay_below;
		prev_leakage_below = leakage_below;
		prev_switching_below = switching_below;
		prev_all_below = all_below;

		int max_times_critical = 0;
		int min_times_critical = -1;
		for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
			for (int n = 0; n < num_circuits; ++n) {
				/*if (critical_delay[n] > threshold_delay &&
					(*i)->is_critical[n]) {
					(*i)->times_critical += 1;
				}*/
				if ((*i)->is_critical[n]) {
					(*i)->times_critical += 1;
				}
			}
			/*if ((*i)->W[0] >= 3) {
				continue;
			}*/
			if (max_times_critical < (*i)->times_critical) {
				max_times_critical = (*i)->times_critical;
			}
			/*if ((*i)->W[0] <= 0.3 || (*i)->times_modified > 0) {
				continue;
			}*/
			if (min_times_critical == -1 || min_times_critical > (*i)->times_critical) {
				min_times_critical = (*i)->times_critical;
			}
		}

		if (next == 'd') {
			list<node*> queue;
			int max_transitive_fanout = 0;
			for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
				if ((*i)->W[0] >= 3) {
					continue;
				}
				if ((*i)->times_critical == max_times_critical) {
					if (max_transitive_fanout < (*i)->transitive_fanout) {
						max_transitive_fanout = (*i)->transitive_fanout;
					}
					queue.push_back(*i);
				}
			}
			list<node*> queue2;
			int min_times_modified = -1;
			for (list<node*>::iterator i = queue.begin(); i != queue.end(); ++i) {
				if ((*i)->transitive_fanout == max_transitive_fanout) {
					if (min_times_modified == -1 || min_times_modified > (*i)->times_modified) {
						min_times_modified = (*i)->times_modified;
					}
					queue2.push_back(*i);
				}
			}
			for (list<node*>::iterator i = queue2.begin(); i != queue2.end(); ++i) {
				if ((*i)->times_modified == min_times_modified) {
					for (int n = 0; n < num_circuits; ++n) {
						(*i)->W[n] *= 1.1;
					}
					(*i)->times_modified++;
				}
			}
		} else if (next == 'e') {
			list<node*> queue;
			int min_transitive_fanout = -1;
			for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
				if ((*i)->W[0] <= 0.3 || (*i)->times_modified > 0) {
					continue;
				}
				if ((*i)->times_critical == min_times_critical) {
					if (min_transitive_fanout == -1 || min_transitive_fanout > (*i)->transitive_fanout) {
						min_transitive_fanout = (*i)->transitive_fanout;
					}
					queue.push_back(*i);
				}
			}
			for (list<node*>::iterator i = queue.begin(); i != queue.end(); ++i) {
				if ((*i)->transitive_fanout == min_transitive_fanout) {
					for (int n = 0; n < num_circuits; ++n) {
						(*i)->W[n] *= 0.9;
					}
					break;
				}
			}
		}
		if (max_all_below == num_circuits) {
			break;
		}
		times++;
	}

	/*cout << max_delay_below << " "
		 << max_leakage_below << " "
		 << max_switching_below << " "
		 << max_all_below << "\n";*/

	/*cout << times << "\n";*/

	reps_learn_delay_below += max_delay_below;
	reps_learn_leakage_below += max_leakage_below;
	reps_learn_switching_below += max_switching_below;
	reps_learn_all_below += max_all_below;

	reps_times += times;

	for (times = 0; times < 2; times++) {
		if (times == 1) {
			int gate_index = 0;
			for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
				for (int n = num_circuits; n < num_circuits+num_test_circuits; ++n) {
					(*i)->W[n] = W_opt[gate_index];
				}
				++gate_index;
			}
		}

		double* critical_delay = new double[num_test_circuits];
		double* total_leakage = new double[num_test_circuits];
		double* total_switching = new double[num_test_circuits];

		for (int n = num_circuits; n < num_circuits+num_test_circuits; ++n) {
			unvisit(&inputs, &gates, &outputs);
			critical_delay[n-num_circuits] = 0;
			total_leakage[n-num_circuits] = 0;
			total_switching[n-num_circuits] = 0;

			for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
				double gain = 0;
				for (list<node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); ++j) {
					gain += (*j)->W[n]*(*j)->L[n] / ((*i)->W[n]*(*i)->L[n]);
				}
				(*i)->delay[n] = (*i)->L[n] / (*i)->W[n] * (1 + 0.2*gain);
				(*i)->leakage[n] = (*i)->W[n] / (*i)->L[n];
				(*i)->switching[n] = (*i)->W[n] * (*i)->L[n];

				total_leakage[n-num_circuits] += (*i)->leakage[n];
				total_switching[n-num_circuits] += (*i)->switching[n];
			}

			list<node*> queue;
			for (list<node*>::iterator i = inputs.begin(); i != inputs.end(); ++i) {
				queue.push_back(*i);
			}
			for (list<node*>::iterator i = queue.begin(); i != queue.end(); ++i) {
				if ((*i)->visited) {
					continue;
				}

				bool visit_node = true;
				for (list<node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); ++j) {
					if (!(*j)->visited) {
						visit_node = false;
						break;
					}
				}
				if (!visit_node) {
					continue;
				}
				(*i)->visited = true;
				for (list<node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); ++j) {
					if ((*i)->delay_so_far[n] < (*j)->delay_so_far[n] + (*i)->delay[n]) {
						(*i)->delay_so_far[n] = (*j)->delay_so_far[n] + (*i)->delay[n];
					}
				}

				if ((*i)->type == "OUTPUT" && (*i)->delay_so_far[n] > critical_delay[n]) {
					critical_delay[n-num_circuits] = (*i)->delay_so_far[n];
				}

				for (list<node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); ++j) {
					queue.push_back(*j);
				}
			}

			unvisit(&inputs, &gates, &outputs);
			queue.clear();
			for (list<node*>::iterator i = outputs.begin(); i != outputs.end(); ++i) {
				(*i)->visited = true;
				(*i)->threshold[n] = threshold_delay;
				if ((*i)->delay_so_far[n] >= (*i)->threshold[n]) {
					(*i)->is_critical[n] = true;
				}
				for (list<node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); ++j) {
					queue.push_back(*j);
				}
			}
			for (list<node*>::iterator i = queue.begin(); i != queue.end(); ++i) {
				if ((*i)->visited) {
					continue;
				}

				bool visit_node = true;
				for (list<node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); ++j) {
					if (!(*j)->visited) {
						visit_node = false;
						break;
					}
				}
				if (!visit_node) {
					continue;
				}
				(*i)->visited = true;

				for (list<node*>::iterator j = (*i)->outputs.begin(); j != (*i)->outputs.end(); ++j) {
					if ((*i)->threshold[n] > (*j)->threshold[n] - (*j)->delay[n]) {
						(*i)->threshold[n] = (*j)->threshold[n] - (*j)->delay[n];
					}
				}
				if ((*i)->delay_so_far[n] >= (*i)->threshold[n]) {
					(*i)->is_critical[n] = true;
				}
				for (list<node*>::iterator j = (*i)->inputs.begin(); j != (*i)->inputs.end(); ++j) {
					queue.push_back(*j);
				}
			}
		}

		int delay_below = 0;
		int leakage_below = 0;
		int switching_below = 0;
		int all_below = 0;
		for (int i = 0; i < num_test_circuits; ++i) {
			if (critical_delay[i] <= threshold_delay) {
				++delay_below;
			}
			if (total_leakage[i] <= threshold_leakage) {
				++leakage_below;
			}
			if (total_switching[i] <= threshold_switching) {
				++switching_below;
			}
			if (critical_delay[i] <= threshold_delay &&
				total_leakage[i] <= threshold_leakage &&
				total_switching[i] <= threshold_switching) {
				++all_below;
			}
		}
		
		/*cout << delay_below << " "
			 << leakage_below << " "
			 << switching_below << " "
			 << all_below << "\n";*/
		if (times == 0) {
			reps_test_init_delay_below += delay_below;
			reps_test_init_leakage_below += leakage_below;
			reps_test_init_switching_below += switching_below;
			reps_test_init_all_below += all_below;
		} else {
			reps_test_delay_below += delay_below;
			reps_test_leakage_below += leakage_below;
			reps_test_switching_below += switching_below;
			reps_test_all_below += all_below;
		}
	}

	/*for (int i = 0; i < gates.size(); ++i) {
		cout << W_opt[i] << " ";
	} cout << "\n";*/

	for (list<node*>::iterator i = inputs.begin(); i != inputs.end(); ++i) {
		delete (*i);
	} inputs.clear();
	for (list<node*>::iterator i = gates.begin(); i != gates.end(); ++i) {
		delete (*i);
	} gates.clear();
	for (list<node*>::iterator i = outputs.begin(); i != outputs.end(); ++i) {
		delete (*i);
	} outputs.clear();
	node_map.clear();

	inf.close();
	}
	
	cout << reps_learn_init_delay_below / (double) num_reps << " "
		 << reps_learn_init_leakage_below / (double) num_reps << " "
		 << reps_learn_init_switching_below / (double) num_reps << " "
		 << reps_learn_init_all_below / (double) num_reps << "\n";

	cout << reps_learn_delay_below / (double) num_reps << " "
		 << reps_learn_leakage_below / (double) num_reps << " "
		 << reps_learn_switching_below / (double) num_reps << " "
		 << reps_learn_all_below / (double) num_reps << "\n";

	cout << reps_test_init_delay_below / (double) num_reps << " "
		 << reps_test_init_leakage_below / (double) num_reps << " "
		 << reps_test_init_switching_below / (double) num_reps << " "
		 << reps_test_init_all_below / (double) num_reps << "\n";

	cout << reps_test_delay_below / (double) num_reps << " "
		 << reps_test_leakage_below / (double) num_reps << " "
		 << reps_test_switching_below / (double) num_reps << " "
		 << reps_test_all_below / (double) num_reps << "\n";

	cout << reps_times / (double) num_reps << "\n";
}
