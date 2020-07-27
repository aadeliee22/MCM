#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <trng/yarn2.hpp>
#include <trng/uniform01_dist.hpp>
using namespace std;

random_device rd;
trng::yarn2 gen;
trng::uniform01_dist<> dis;

void initialize(vector<double>& v, int size) //initial -random- state
{
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if ((i + j) % 2 == 0) v[size * i + j] = 1;
			else v[size * i + j] = -1;
		}
	}
	/*for (int i = 0; i < size * size; i++) {
		v[i] = dis(gen) < 0.5 ? 1 : -1;
	}*/
}
void color(vector<double>& v, int size) //graphing state
{
	for (int i = 0; i < size * size; i++) {
		if (i % size == 0) cout << endl;
		if (v[i] == 1) cout << "* ";
		if (v[i] == -1) cout << ". ";
	}
}
void neighbor(vector < vector <double> >& na, int size)
{
	int sizes = size * size;
	for (int i = 0; i < size * size; i++) {
		int na_2 = (i - 1 + size) % size + (i / size) * size;
		int na_3 = (i + 1) % size + (i / size) * size;
		na[i][0] = (i + size * (size - 1)) % sizes;
		na[i][1] = (i + size) % sizes;
		na[i][2] = na_2;
		na[i][3] = na_3;
		na[i][4] = (na_2 + size * (size - 1)) % sizes;
		na[i][5] = (na_3 + size * (size - 1)) % sizes;
		na[i][6] = (na_2 + size) % sizes;
		na[i][7] = (na_3 + size) % sizes;
	}
}
double Magnet(vector<double>& v, int size)
{
	double m = 0;
	for (vector<int>::size_type i = 0; i < v.size(); i++) {
		m = m + v.at(i);
	}
	m = abs(m) / (v.size()); //absolute value of average spin
	return m;
}
double Energy(vector<double>& v, int size, vector < vector <double> >& na, double K)
{
	double e = 0;
	for (int i = 0; i < size * size; i++) {
		e = e - v[i] * (v[na[i][1]] + v[na[i][3]] + K * v[na[i][1]] * v[na[i][3]] * v[na[i][7]]);
	}
	return e;
}
double delU1(vector<double>& v, int size, int i, vector < vector <double> >& na)
{
	double E = 2 * v[i] * (v[na[i][0]] + v[na[i][1]] + v[na[i][2]] + v[na[i][3]]);
	return E;
}
double delU2(vector<double>& v, int size, int i, vector < vector <double> >& na)
{
	double E = 2 * v[i] * ((v[na[i][4]]*v[na[i][2]] + v[na[i][5]]*v[na[i][3]])*v[na[i][0]] 
							+ (v[na[i][6]]*v[na[i][2]] + v[na[i][7]]*v[na[i][3]])*v[na[i][1]] );
	return E;
}
double exp_delU(double E, double* expE)
{
	double result1;
	if (E == 8) result1 = *(expE);
	else if (E == 4) result1 = *(expE + 1);
	else if (E == -4) result1 = *(expE + 2);
	else if (E == -8) result1 = *(expE + 3);
	else result1 = 1;
	return result1;
}
void MC_1step(vector<double>& v, int size, double* expE, vector < vector <double> >& na, double K)
{
	double Ediff;
	double Ediff1;
	double Ediff2;
	gen.seed(rd);
	for (int i = 0; i < size; i=i+2) {
		for (int j = 0; j < size; j=j+2) {
			Ediff1 = delU1(v, size, size * i + j, na);
			Ediff2 = delU2(v, size, size * i + j, na);
			Ediff = Ediff1 + Ediff2 * K;
			if (Ediff <= 0) v[size * i + j] = -v[size * i + j];
			else {
				if (dis(gen) <= exp_delU(Ediff1, expE)*pow(exp_delU(Ediff2, expE), K)) v[size * i + j] = -v[size * i + j];
			}
		}
	}
	for (int i = 0; i < size; i=i+2) {
		for (int j = 1; j < size; j=j+2) {
			Ediff1 = delU1(v, size, size * i + j, na);
			Ediff2 = delU2(v, size, size * i + j, na);
			Ediff = Ediff1 + Ediff2 * K;
			if (Ediff <= 0) v[size * i + j] = -v[size * i + j];
			else {
				if (dis(gen) <= exp_delU(Ediff1, expE)*pow(exp_delU(Ediff2, expE), K)) v[size * i + j] = -v[size * i + j];
			}
		}
	}
	for (int i = 1; i < size; i=i+2) {
		for (int j = 0; j < size; j=j+2) {
			Ediff1 = delU1(v, size, size * i + j, na);
			Ediff2 = delU2(v, size, size * i + j, na);
			Ediff = Ediff1 + Ediff2 * K;
			if (Ediff <= 0) v[size * i + j] = -v[size * i + j];
			else {
				if (dis(gen) <= exp_delU(Ediff1, expE)*pow(exp_delU(Ediff2, expE), K)) v[size * i + j] = -v[size * i + j];
			}
		}
	}
	for (int i = 1; i < size; i=i+2) {
		for (int j = 1; j < size; j=j+2) {
			Ediff1 = delU1(v, size, size * i + j, na);
			Ediff2 = delU2(v, size, size * i + j, na);
			Ediff = Ediff1 + Ediff2 * K;
			if (Ediff <= 0) v[size * i + j] = -v[size * i + j];
			else {
				if (dis(gen) <= exp_delU(Ediff1, expE)*pow(exp_delU(Ediff2, expE), K)) v[size * i + j] = -v[size * i + j];
			}
		}
	}

}
void MC_1cycle(int size, double T, double& mag, double& mag_sus, double& mag2, double& mag4, vector < vector <double> >& na, double K)
{
	int step1 = 2500, step2 = 10000;
	int trash_step =  size+5;
	if (T > 2.1 && T < 2.4) trash_step = trash_step * 2;

	double expE[4] = { exp(-8 / T), exp(-4 / T), exp(4 / T), exp(8 / T) };
	vector<double> array(size * size, 0);
	initialize(array, size);

	for (int k = 0; k < step1; k++) { MC_1step(array, size, &expE[0], na, K); }

	vector<double> magnet(step2, 0);
	//vector<double> energy(step2, 0);
	for (int k = 0; k < step2; k++) {
		for (int h = 0; h < trash_step; h++) {
			MC_1step(array, size, &expE[0], na, K);
		}
		magnet.at(k) = Magnet(array, size);
		//energy.at(k) = Energy(array, size, na);
	}

	double Mag = 0, Mag2 = 0, Mag4 = 0;
	for (vector<int>::size_type i = 0; i < magnet.size(); i++) {
		Mag = Mag + magnet.at(i);
		Mag2 = Mag2 + pow(magnet.at(i), 2);
		//Mag4 = Mag4 + pow(magnet.at(i), 4);
		/*Ene = Ene + energy.at(i);
		Ene2 = Ene2 + pow(energy.at(i), 2);*/
	}

	mag = Mag / step2;
	//ene = Ene / step2;
	mag_sus = pow(size, 2) * (Mag2 / step2 - pow(Mag / step2, 2)) / T;
	//mag2 = Mag2 / step2;
	//mag4 = Mag4 / step2;
	//sp_heat = (Ene2 / step2 - pow(Ene / step2, 2)) / pow(size * T, 2);
	//return Mag, Ene, mag_sus, sp_heat;
}
void MC_1cycle_graphing(int size, double T, vector < vector <double> >& na, double K)
{
	int step1 = 1000, step2 = 5000;
	double expE[4] = { exp(-8 / T), exp(-4 / T), exp(4 / T), exp(8 / T) };
	vector<double> array(size * size, 0);

	initialize(array, size);
	cout << "Initial state: " << endl;
	color(array, size);
	cout << endl;

	for (int k = 0; k < step1; k++) { MC_1step(array, size, &expE[0], na, K); }

	vector<double> magnet(step2, 0);
	vector<double> energy(step2, 0);
	for (int k = 0; k < step2; k++){
		MC_1step(array, size, &expE[0], na, K);
		magnet.at(k) = Magnet(array, size);
		energy.at(k) = Energy(array, size, na, K);
	}

	double Mag = 0, Mag2 = 0, Ene = 0, Ene2 = 0;
	for (vector<int>::size_type i = 0; i < magnet.size(); i++) {
		Mag = Mag + magnet.at(i);
		Mag2 = Mag2 + pow(magnet.at(i), 2);
		Ene = Ene + energy.at(i);
		Ene2 = Ene2 + pow(energy.at(i), 2);
	}

	cout << "Final state with temperature " << T <<" :"<< endl;
	color(array, size);
	cout << endl;
	cout << "Magnetization: " << Mag/step2 << endl;
	cout << "Energy (H): " << Ene/step2 << endl;
	cout << "Magnetic susceptibility: " << pow(size, 2) * (Mag2 / step2 - pow(Mag / step2, 2)) / T << endl;
	cout << "Specific heat: " << (Ene2 / step2 - pow(Ene / step2, 2)) / pow(size * T, 2) << endl;
}

int main()
{
	random_device rd;
	gen.seed(rd);
	double K=0.2;
	int size;
	cout << "What size?: ";	cin >> size;
	double temp;
	cout << "What temp?: ";	cin >> temp;
	double Mag = 0, mag_sus = 0, Mag2 = 0, Mag4 = 0;

	clock_t start = clock();

	vector < vector <double> > near(size * size, vector<double>(8, 0));
	neighbor(near, size);
	MC_1cycle_graphing(size, temp, near, K);

	// ofstream File;
	// File.open("met_center_80.txt");
	// cout << "(met_center) File open: " << size << endl;
	// File << "size temperature m m^2 m^4 mag_sus" << endl;
	// // for (int k = 1; k < 75; k++) { // 0.02~1.48
	// // 	for (int h = 0; h < 5; h++) {
	// // 		MC_1cycle(size, 0.02 * k, Mag, mag_sus, Mag2, Mag4, near);
	// // 		File << size << " " << 0.02 * k << " " << Mag << " " << Mag2 << " " << Mag4 << " " << mag_sus << " " << endl;
	// // 	}
	// // 	cout << 0.02 * k << "end" << endl;
	// // }
	// // for (int k = 150; k < 200; k++) { // 1.50~1.99
	// // 	for (int h = 0; h < 5; h++) {
	// // 		MC_1cycle(size, 0.01 * k, Mag, mag_sus, Mag2, Mag4, near);
	// // 		File << size << " " << 0.01 * k << " " << Mag << " " << Mag2 << " " << Mag4 << " " << mag_sus << " " << endl;
	// // 	}
	// // 	cout << 0.01 * k << "end" << endl;
	// // }
	// for (int k = 2200; k < 2500; k++) { // 2.000~2.499
	// 	for (int h = 0; h < 5; h++) {
	// 		MC_1cycle(size, 0.001 * k, Mag, mag_sus, Mag2, Mag4, near);
	// 		File << size << " " << 0.001 * k << " " << Mag << " "  << mag_sus << " " << endl;
	// 	}
	// 	cout << 0.001 * k << " end" << endl;
	// }
	// // for (int k = 250; k < 300; k++) { // 2.50~2.99
	// // 	for (int h = 0; h < 5; h++) {
	// // 		MC_1cycle(size, 0.01 * k, Mag, mag_sus, Mag2, Mag4, near);
	// // 		File << size << " " << 0.01 * k << " " << Mag << " " << Mag2 << " " << Mag4 << " " << mag_sus << " " << endl;
	// // 	}
	// // 	cout << 0.01 * k << "end" << endl;
	// // }
	// // for (int k = 150; k < 226; k++) { // 3.00~4.5
	// // 	for (int h = 0; h < 5; h++) {
	// // 		MC_1cycle(size, 0.02 * k, Mag, mag_sus, Mag2, Mag4, near);
	// // 		File << size << " " << 0.02 * k << " " << Mag << " " << Mag2 << " " << Mag4 << " " << mag_sus << " " << endl;
	// // 	}
	// // 	cout << 0.02 * k << "end" << endl;
	// // }
	// File.close();

	cout << endl << "total time: " << (double(clock()) - double(start)) / CLOCKS_PER_SEC << " sec" << endl;
	return 0;
}
