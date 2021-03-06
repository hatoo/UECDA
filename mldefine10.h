#include "mldefine.h"
#include "mydef.h"
#include "bitCard.h"
#include "handGenerator.h"

#include <cmath>

#pragma once

class MLdefine10:public MLdefine{
public:
	MLdefine10(){}
	int size(){return 11*512;}
	uvector phi(const state &s,const action &a);
	Hand optimizedAI(const fieldInfo &,Cards myCards,Cards oppCards
			,double *theta);
	void show(const uvector &theta);
	uvector merge(uvector notrev,uvector rev);
};

