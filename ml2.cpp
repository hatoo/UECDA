#include "mydef.h"
#include "machineLearn.h"
#include "mlAlgorithm.h"
#include "mlvector2010.h"
#include "mlvector2012.h"

typedef mlvector2012 VECTOR;

double vec[VECTOR::size];

void initmlAI2(const char *fname){
	uvector v = load(fname,VECTOR::size);
	for(int i=0;i<VECTOR::size;i++){
		vec[i]=v[i];
	}
}
Hand mlAI2(const fieldInfo &info,Cards myCards){
	/*state s;
	s.info=info;
	s.hands[info.mypos]=myCards;
	return AI<VECTOR>(s,vec);*/
	return VECTOR::AI(info,myCards,vec);
}
