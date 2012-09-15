#pragma once

void initRand();

int randInt(int low,int high);
/*{
	std::uniform_int_distribution<int> d(low,high) ;
	return d(engine);
}*/
