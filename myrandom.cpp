#include "myrandom.h"
#include <random>
#include <chrono>

using namespace std;

std::mt19937 engine;

void initRand(){
	engine= mt19937(std::chrono::system_clock::now().time_since_epoch().count());
}

int randInt(int low,int high){
	std::uniform_int_distribution<int> d(low,high) ;
	return d(engine);
}
