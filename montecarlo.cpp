#include "montecarlo.h"
#include "mydef.h"
#include "bitCard.h"
#include "myrandom.h"
#include "handGenerator.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <utility>

using namespace std;

Hand fastWeakAI(const fieldInfo &info,Cards myCards){
	vector<Hand> hands = getAllValidHands(info,myCards);
	return hands[randInt(0,hands.size()-1)];
}

int playout(fieldInfo info,Hand h,Cards myCards,Cards oppCards){
	int pos = info.mypos;
	Hand hand = h;
	return 0;
}

void DevideCards(fieldInfo info,Cards oppCards,Cards out[5]){
	memset(out,0,sizeof out);
	info.lest[info.mypos]=0;
	vector<pair<int,int> > left;//pos,lest
	for(int i=0;i<5;i++){
		if(!(info.goal&(1<<i))){
			left.push_back(make_pair(i,info.lest[i]));
		}
	}
	for(int i=0;i<53;i++){
		if(oppCards&(1LL << i)){
			if(left.empty()){
				cerr << "Devide Cards error!!!"<< endl;
				return;
			}
			int index = randInt(0,left.size());
			int seat = left[index].first;
			left[index].second--;
			if(left[index].second==0)left.erase(left.begin()+index);
			out[seat] |= (1LL << i);
		}
	}
	if(!left.empty()){
		cerr << "Devide Cards Error!!!2" << endl;
	}
}
