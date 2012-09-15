#include "montecarlo.h"
#include "mydef.h"
#include "bitCard.h"
#include "myrandom.h"
#include "handGenerator.h"
#include "ucb1.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <utility>
#include <algorithm>
#include "mydebug.h"

using namespace std;

Hand fastWeakAI(const fieldInfo &info,Cards myCards){
	vector<Hand> hands = getAllValidHands(info,myCards);
	int t=0;
	if(hands.size()==1)return hands[0];
	Hand pass;pass.qty=0;pass.hands=0LL;
	const int turn = minHandPairNum(myCards);
	for(int i=0;i<hands.size();i++){
		Cards d = diffHand(myCards,hands[i]);
		int newturn = minHandPairNum(d);
		//if(hands[i].hands&&(0xFLL<<(4*5)))newturn--;
		if(newturn<turn)return hands[i];
	}
	return pass;
	/*
	if(info.onset){
		for(int i=0;i<hands.size();i++){
			if(hands[i].qty>hands[t].qty 
					|| (hands[i].qty==hands[t].qty&&
					Strength(hands[i].ord,info.rev)<Strength(hands[t].ord,info.rev)))t=i;
		}
	}else{
		for(int i=0;i<hands.size();i++){
			if(hands[i].qty!=0 && hands[i].ord<hands[t].ord
					|| (hands[i].qty==hands[t].qty&&
					Strength(hands[i].ord,info.rev)<Strength(hands[t].ord,info.rev)))t=i;
		}
	}*/
	return hands[/*t];/*/randInt(0,hands.size()-2)];
}

int playout(fieldInfo info,Hand h,Cards myCards,Cards oppCards){
	int pos = info.mypos;
	Hand hand = h;
	Cards hands[5]={0};
	DevideCards(info,oppCards,hands);
	
	hands[info.mypos]=myCards;
	for(int i=0;i<5;i++){
		if(info.lest[i]==0)info.goal|=(1<<i);
	}

	while(!(info.goal&(1<<info.mypos)) && ((info.goal|(1<<info.mypos))!=0x1F)){
		//Cards cpy = hands[pos];
		hands[pos]=diffHand(hands[pos],hand);
		pos = info.simulate(hand,pos);
		if(hands[pos])
		hand = fastWeakAI(info,hands[pos]);
	}
	int count = bitCount(info.goal);
	return info.goal&(1<<info.mypos)?count-1:count;
}

void DevideCards(fieldInfo info,Cards oppCards,Cards out[5]){
	memset(out,0,sizeof(Cards)*5);
	//info.lest[info.mypos]=0;
	vector<pair<int,int> > left;//pos,lest
	for(int i=0;i<5;i++){
		if(i!=info.mypos && info.lest[i]!=0){
			left.push_back(make_pair(i,info.lest[i]));
		}
	}
	//cerr << "here3" << endl;
	for(int i=0;i<53;i++){
		const Cards card = ((Cards)1LL) << i;
		if(oppCards&card){
			if(left.empty()){
				cerr << "Devide Cards error!!!"<< endl;
				return;
			}
			int index = randInt(0,left.size()-1);
			int seat = left[index].first;
			left[index].second--;
			if(left[index].second==0)left.erase(left.begin()+index);
			out[seat] |= card;
		}
	}
	if(!left.empty()){
		cerr << "Devide Cards Error!!!2" << endl;
	}
}

Hand montecalroSearch(fieldInfo info,Cards myCards,Cards oppCards){
	typedef ucb1Record<int> URecord;

	vector<Hand> vhs = getAllValidHands(info,myCards);
	vector<URecord> records;
	const double goalnum = bitCount(info.goal);

	for(int i=0;i<vhs.size();i++){
		records.push_back(URecord(i));
	}

	for(int i=0;i<3000;i++){
		auto ucb1_tuned = [=](URecord &a,URecord &b){
			return a.ucb1_tuned(i)<b.ucb1_tuned(i);
		};

		auto maxelem = max_element(records.begin(),records.end(),ucb1_tuned);
		double rank = playout(info,vhs[maxelem->tag],myCards,oppCards);
		if(goalnum==4)cerr << "104" << endl;
		//double score[] = {1,0.75,0.5,0.25,0};
		double score = 1.0-(rank-goalnum)/(4.0-goalnum);
		maxelem->pushScore(score);
	}
	auto cmp = [](URecord &a,URecord &b){
		return a.x()<b.x();
	};
	auto maxelem = max_element(records.begin(),records.end(),cmp);

	return vhs[maxelem->tag];
}
