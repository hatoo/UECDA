#include "myrandom.h"
#include <random>
#include <chrono>
//#include "mt19937ar.h"

using namespace std;

std::mt19937 engine;

void initRand(){
	engine = mt19937(std::chrono::system_clock::now().time_since_epoch().count());
	//init_genrand(std::chrono::system_clock::now().time_since_epoch().count());
}

int randInt(int low,int high){
		std::uniform_int_distribution<int> d(low,high) ;
		//int c = low+genrand_int31()%(1+high-low);
		int res = d(engine);
		return res;
}

double randDouble(double low,double high){
	std::uniform_real_distribution<double> d(low,high);
	double res = d(engine);
	//double c = low+genrand_real1()*(high-low);
	return res;
}
