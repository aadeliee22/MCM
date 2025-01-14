#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>
#include <time.h>
using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<double> dis(0, 1);

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
void neighbor(vector < vector <double> >& na, int size)
{
	int sizes = size * size;
	for (int i = 0; i < size * size; i++) {
		na[i][0] = (i + size * (size - 1)) % sizes;
		na[i][1] = (i + size) % sizes;
		na[i][2] = (i - 1 + size) % size + (i / size) * size;
		na[i][3] = (i + 1) % size + (i / size) * size;
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
void Cluster_1step(vector<double>& v, int size, double padd, vector < vector <double> >& na)
{
	int i = size * size * dis(gen);
	vector<int> stack(1, i);
	double oldspin = v[i];
	double newspin = -v[i];
	v[i] = newspin;
	int sp = 0;
	while (1) {
		if (v[na[i][0]] == oldspin && dis(gen) < padd) { stack.push_back(na[i][0]); v[na[i][0]] = newspin; }
		if (v[na[i][1]] == oldspin && dis(gen) < padd) { stack.push_back(na[i][1]); v[na[i][1]] = newspin; }
		if (v[na[i][2]] == oldspin && dis(gen) < padd) { stack.push_back(na[i][2]); v[na[i][2]] = newspin; }
		if (v[na[i][3]] == oldspin && dis(gen) < padd) { stack.push_back(na[i][3]); v[na[i][3]] = newspin; }
		sp++;
		if (sp >= stack.size()) break;
		i = stack.at(sp);
	}

}
void MC_1cycle(int size, double T, vector < vector <double> >& na, vector<double>& magnet, vector<double>& magsus)
{
	int step1 = 2000, step2 = 10000;
	int scale=1;
	// double slope = (double(size)*size/3.0)/2.7;
	// if (T>2.3) {
	// 	scale = slope * (T - 2.3);
	// 	if (scale == 0) scale = 1;
	// }
	// int trash_step = scale*(int(size/16)+2);
	// if (T > 2.0 && T <= 2.5) trash_step = trash_step*2;

	double padd = 1 - exp(-2 / T);
	vector<double> array(size * size, 0);
	initialize(array, size);

	for (int k = 0; k < step1*scale; k++) { Cluster_1step(array, size, padd, na); }

	double M = 0, Mag = 0, Mag2 = 0;
	for (int k = 0; k < step2; k++) {
		Cluster_1step(array, size, padd, na);
		M = Magnet(array, size);
		Mag = Mag + M; Mag2 = Mag2 + pow(M, 2);
		magnet.at(k) = M;
		magsus.at(k) = pow(size,2)*(Mag2 / (k+1) - pow(Mag / (k+1), 2))/T;
	}

}
int main()
{
	int size;
	cout << "Size: "; cin >> size;
	double temperature = 2.269;

	clock_t start = clock();

	ofstream File;
	File.open("at_cluster.txt");
	cout << "(center2) File open: " << size << endl;
	File << "trial magnet magsus size: " << size << endl;

	vector < vector <double> > near(size * size, vector<double>(4, 0));
	vector<double> magnet(10000, 0); vector<double> magsus(10000,0);
	neighbor(near, size);
	for (int h = 0; h < 10; h++) {
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<double> dis(0, 1);
		MC_1cycle(size, temperature, near, magnet, magsus);
		for (int i=0;i<10000;i++){
			File << h << " " << magnet.at(i) << " " << magsus.at(i) << endl;
		}
		cout << h+1 << " trial end" << endl;
	}
	File.close();


	cout << endl << "total time: " << (double(clock()) - double(start)) / CLOCKS_PER_SEC << " sec" << endl;
	return 0;
}
