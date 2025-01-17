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
	for (int i = 0; i < size * size; i++) {v[i] = dis(gen) < 0.5 ? 1 : -1;}
}
void neighbor(vector < vector <double> >& na, int size)
{
	int sizes = size * size;
	int na_2, na_3;
	for (int i = 0; i < size * size; i++) {
		na_2 = (i - 1 + size) % size + (i / size) * size;
		na_3 = (i + 1) % size + (i / size) * size;
		na[i][0] = (i + size * (size - 1)) % sizes;
		na[i][1] = (i + size) % sizes;
		na[i][2] = na_2;
		na[i][3] = na_3;
		na[i][4] = (na_2 + size * (size - 1)) % sizes;
		na[i][5] = (na_3 + size * (size - 1)) % sizes;
		na[i][6] = (na_2 + size) % sizes;
		na[i][7] = (na_3 + size) % sizes;
		na[i][8] = (i + size * (size - 2)) % sizes;
		na[i][9] = (i + 2 * size) % sizes;
		na[i][10] = (i - 2 + size) % size + (i / size) * size;
		na[i][11] = (i + 2) % size + (i / size) * size;
	}
}
double Magnet(vector<double> v, int size)
{
	double m = 0;
	for (int i = 0; i < size*size; i++) { m = m + v.at(i);}
	m = abs(m) / (size*size); //absolute value of average spin
	return m;
}
double originalEnergy(vector<double> v, int size, vector < vector <double> > na, double K)
{
	double e = 0;
	for (int i = 0; i < size*size; i++) {
		e = e - v[i] * (v[na[i][1]] + v[na[i][3]] + K * v[na[i][1]] * v[na[i][3]] * v[na[i][7]]);
	}
	return e;
}
double nnnEne(vector<double> v, vector < vector <double> > na, int ith)
{
	double nnn = 0;
	for (vector<int>::size_type i=0; i<v.size(); i++){
		nnn = nnn - v[i] * (v[na[i][4*ith-3]] + v[na[i][4*ith-1]]);
	}
	return nnn;
}
double delU1(vector<double> v, int i, vector < vector <double> > na)
{
	double E = 2 * v[i] * (v[na[i][0]] + v[na[i][1]] + v[na[i][2]] + v[na[i][3]]);
	return E;
}
double delU2(vector<double> v, int i, vector < vector <double> > na)
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
void MC_1step(vector<double>& v, int size, double* expE, vector < vector <double> > na, double K)
{
	double Ediff;
	double Ediff1;
	double Ediff2;
	gen.seed(rd);
	for (int i = 0; i < size; i=i+2) {
		for (int j = 0; j < size; j=j+2) {
			Ediff1 = delU1(v, size * i + j, na);
			Ediff2 = delU2(v, size * i + j, na);
			Ediff = Ediff1 + Ediff2 * K;
			if (Ediff <= 0) v[size * i + j] = -v[size * i + j];
			else {
				if (dis(gen) <= exp_delU(Ediff1, expE)*pow(exp_delU(Ediff2, expE), K)) 
				{v[size * i + j] = -v[size * i + j];}
			}
		}
	}
	for (int i = 0; i < size; i=i+2) {
		for (int j = 1; j < size; j=j+2) {
			Ediff1 = delU1(v, size * i + j, na);
			Ediff2 = delU2(v, size * i + j, na);
			Ediff = Ediff1 + Ediff2 * K;
			if (Ediff <= 0) v[size * i + j] = -v[size * i + j];
			else {
				if (dis(gen) <= exp_delU(Ediff1, expE)*pow(exp_delU(Ediff2, expE), K)) 
				{v[size * i + j] = -v[size * i + j];}
			}
		}
	}
	for (int i = 1; i < size; i=i+2) {
		for (int j = 0; j < size; j=j+2) {
			Ediff1 = delU1(v, size * i + j, na);
			Ediff2 = delU2(v, size * i + j, na);
			Ediff = Ediff1 + Ediff2 * K;
			if (Ediff <= 0) v[size * i + j] = -v[size * i + j];
			else {
				if (dis(gen) <= exp_delU(Ediff1, expE)*pow(exp_delU(Ediff2, expE), K)) 
				{v[size * i + j] = -v[size * i + j];}
			}
		}
	}
	for (int i = 1; i < size; i=i+2) {
		for (int j = 1; j < size; j=j+2) {
			Ediff1 = delU1(v, size * i + j, na);
			Ediff2 = delU2(v, size * i + j, na);
			Ediff = Ediff1 + Ediff2 * K;
			if (Ediff <= 0) v[size * i + j] = -v[size * i + j];
			else {
				if (dis(gen) <= exp_delU(Ediff1, expE)*pow(exp_delU(Ediff2, expE), K)) 
				{v[size * i + j] = -v[size * i + j];}
			}
		}
	}

}
void met_cycle(int size, double T, int step2, vector < vector <double> > na, double K, 
vector<double>& magnet)
{
	int step1 = 2500;

	double expE[4] = { exp(-8 / T), exp(-4 / T), exp(4 / T), exp(8 / T) };
	vector<double> array(size * size, 0);
	initialize(array, size);

	for (int k = 0; k < step1; k++) { MC_1step(array, size, &expE[0], na, K); }
	for (int k = 0; k < step2; k++) {
		MC_1step(array, size, &expE[0], na, K);
		magnet.at(k) = Magnet(array, size);
	}
}

int main()
{
	random_device rd;
	gen.seed(rd);
	double K = 0.2; double temp = 2.493; 
	int step2 = 50000;
	int size = 96;

	vector < vector <double> > near(size * size, vector<double>(12, 0));
	vector<double> magnet(step2, 0); 
	neighbor(near, size);

	clock_t start = clock();

	ofstream Fileout;
	Fileout.open("at_met.txt");
	cout << "met open: " << size << endl;
	Fileout << "trial magnet size: " << size << endl;
	for (int h = 0; h < 10; h++){
		gen.seed(rd);
		met_cycle(size, temp, step2, near, K, magnet);
		for (int i = 0; i < step2; i++){
			Fileout << h << " " << magnet.at(i) << " " << endl;
		}
		cout << h << " end" << endl;
	}
	
	Fileout.close();

	cout << "total time: " << (double(clock()) - double(start)) / CLOCKS_PER_SEC << " sec" << endl;
	return 0;
}
