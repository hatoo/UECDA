#include "mldefine.h"
#include "mydef.h"
#include "bitCard.h"
#include "handGenerator.h"

#include <cmath>

#pragma once

class MLdefine2012:public MLdefine{
public:
	MLdefine2012(){}
	int size(){return 4*512;}
	uvector phi(const state &s,const action &a);
	Hand optimizedAI(const fieldInfo &,Cards myCards,Cards oppCards
			,double *theta);
};

int getIndex(const Hand &h,bool rev);

inline double calcScore(Cards _myCards,const Hand& h,bool rev,
			double *theta,int a){
	double res=0.0;
	if(h.rev())rev=!rev;
	Cards myCards = diffHand(_myCards,h);

	const auto p=[=](bool seq,int ord,int qty){
		return theta[512*a+(rev?256:0)+(seq?56:0)+(4*ord)+(seq?qty-3:qty-1)];
	};

	for(int i=0;i<4;i++){
		const Cards suit = 1uLL<<i;
		Cards seq = 0uLL;
		int count = 0;
		int ord=-1;
		//bool joker_used = !(myCards&JOKER);
		for(int k=0;k<13;k++){
			if(myCards&(suit<<(4*k))){
				if(ord== -1)ord=k;
				seq|=(suit<<(4*k));
				count++;
			}else{
				if(count>=3){
					res+=p(true,ord,count);
				}
				count=0;
				ord=-1;
				seq=0uLL;
			}
		}
		if(count>=3){
			res+=p(true,ord,count);
		}
	}

	for(int i=0;i<13;i++){
		Cards c = myCards&(0xFLL<<(4*i));
		if((myCards>>(4*i))==0)break;
		if(c!=0LL){
			res+=p(false,i,bitCount(c));
		}
	}
	if(myCards&JOKER){
		res+=p(false,13,1);
	}
	return exp(res);
}
