#include "mldefine.h"
#include "mydef.h"
#include "bitCard.h"
#include "handGenerator.h"

#include <cmath>

#pragma once

class MLdefine5:public MLdefine{
public:
	MLdefine5(){}
	int size(){return 5*11*512;}
	uvector phi(const state &s,const action &a);
	Hand optimizedAI(const fieldInfo &,Cards myCards,Cards oppCards
			,double *theta);
	void show(const uvector &theta);
	uvector merge(uvector notrev,uvector rev);
};

