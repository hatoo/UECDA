#include "mldefine2012.h"
#include "mldefine10.h"
#include "mydef.h"
#include "bitCard.h"
#include "handGenerator.h"
#include "myrandom.h"
#include "machineLearn.h"

#include <vector>
#include <iostream>

/*
int getIndex(const Hand &h,bool rev){
	/* メモリ配置
	 * 通常時用:0~
	 * 革命時用:256~
	 * 256の内訳
	 * グループ:(枚数)1~4 * ord  = 4*14 = 56
	 * 階段    :(枚数)3~6 * ord  = 4*13 = 52
	 * 他は使わない
	 *
	if(h.rev())rev=!rev;
	int res=0;
	int ord = h.qty==1&&h.jokerUsed()?13:h.ord;//革命時はジョーカ単体のordが-1になるので
		//if(ord==13)cout<<"aa" << endl;
	if(rev)res+=256;
	if(h.qty==0){
		//res += 56+52;
	}else
	if(h.seq){
		res += 56+4*ord+min(6,(int)h.qty)-3;
	}else{
		res += 4*ord+min(4,(int)h.qty)-1;
	}
	return res;
}*/


uvector MLdefine10::phi(const state &s,const action &a){
	uvector v=zero();
	Cards newHand = diffHand(s.myCards(),a);
	std::vector<Hand> hs = getAIHand(newHand);
	for(int i=0;i<hs.size();i++){
		int a=max(0,min(10,bitCount(s.myCards())-1));
		v[512*a+getIndex(hs[i],s.info.rev)]=1.0;
	}
	return v;
}

Hand MLdefine10::optimizedAI(const fieldInfo & info,Cards myCards,Cards oppCards
			,double *theta){
	if(!info.onset && !checkAllValidHands(info,myCards)){
		return Hand();
	}
	std::vector<Hand> hs = 
	info.onset?getOnsetHands(myCards):getAllValidHands(info,myCards);
	if(hs.size()==1)return hs[0];
	const int tehuda = bitCount(myCards);

	double sum=0.0;
	double ratios[hs.size()];
	const int loopnum=hs.size();

	int a=max(0,min(10,tehuda-1));
	for(int i=0;i<loopnum;i++){
		if(tehuda==hs[i].qty)return hs[i];
		const double ratio = calcScore(myCards,hs[i],info.rev,theta,a);
		sum+=ratio;
		ratios[i]=ratio;
	}

	double r = randDouble(0.0,sum);
	for(int i=0;i<hs.size();i++){
		r-=ratios[i];
		if(r<=0.0)return hs[i];
	}
	return hs.back();
}

void MLdefine10::show(const uvector &theta){
	for(int q=0;q<14;q++){
		std::cout << q << " : ";
		for(int k=0;k<4;k++){
			std::cout << exp(theta[512*9+4*q+k]) << " ";
		}
		std::cout << std::endl;
	}
}

uvector MLdefine10::merge(uvector notrev,uvector rev){
	for(int i=0;i<11;i++)
		for(int k=256;k<512;k++){
			notrev[512*i+k]=rev[512*i+k];
		}
	return notrev;
}

