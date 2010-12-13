#include <cmath>
#include <cstdlib>
#include "Math.h"
using namespace std;

double 
calcKd(double ktp, double kfit, double L, double n, double u, double phi) {
	return (ktp*kfit*L*L)/(2*n*u*phi);
}

double
calcPhi(double T) {
	return (BOLTZMANN_K*T/ELECTRON_CHARGE_Q);
}

double
calcIC(double dibl, double vdd, double vt, double n, double phi) {
	double temp = ((1+dibl)*vdd)-vt;
	temp = temp/(2*n*phi);
	temp = exp(temp);
	temp += 1;
	temp = log(temp);
	return temp*temp;
}

double
markovicDelay(double kd, double vdd, double ic, double y1, double w1, double w2) {
	return kd*vdd/ic*(y1*w1+w2)/w1;
}

double
markovicIS_no_W(double n, double u, double cox, double L, double phi) {
	return 2*n*u*cox*phi*phi/L;
}

double 
markovicLeakageCurrent(double IS_noW, double W, double dibl, double vdd, double vt, double n, double phi) {
	double temp = (dibl*vdd-vt)/(n*phi);
	return IS_noW*W*exp(temp);
}

double 
wangDeltaV_th(double b, double alpha, double t){
	return b*pow(alpha, 6)*pow(t, 6);
}

