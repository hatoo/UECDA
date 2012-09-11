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
	for(int i=0;i<hands.size();i++){
		if(hands[i].qty>hands[t].qty)t=i;
	}
	return hands[randInt(0,hands.size()-1)];
}

int playout(fieldInfo info,Hand h,Cards myCards,Cards oppCards){
	int pos = info.mypos;
	Hand hand = h;
	Cards hands[5]={0};
	//
	int total=0;
	for(int i=0;i<5;i++){
		if(i!=info.mypos){
			total+=info.lest[i];
		}
	}
	if(total!=bitCount(oppCards)){
		cerr << "playout error" << endl;
		return 0;
	}
	//cerr << "here" << endl;
	//
	DevideCards(info,oppCards,hands);
	int t2=0;
	for(int i=0;i<5;i++){
		t2+=bitCount(hands[i]);
	}
	if(t2!=total || info.lest[info.mypos]!=bitCount(myCards)){cerr<<"playout error2" << endl;}
	//cerr << "here2" << endl;
	hands[info.mypos]=myCards;
	/*for(int i=0;i<5;i++){
		if(info.lest[i]==0)info.goal|=(1<<i);
	}*/
	int rid = randInt(1,10000);
	while(!(info.goal&(1<<info.mypos)) && ((info.goal|(1<<info.mypos))!=0x1F)){
		Cards cpy = hands[pos];
		hands[pos]=diffHand(hands[pos],hand);
		//cerr <<"id=" << rid << " pos= " << pos << " lest= " << info.lest[pos] << " handlest= " << bitCount(hands[pos]) << " goal= " << info.goal << " pass= " << info.pass<< endl;
		int t=pos;
		pos = info.simulate(hand,pos);
		if(hands[pos]==0){
			cerr << "err pos= " <<pos << " lest= " << info.lest[pos] << " goal= " << info.goal << " pass= " << info.pass << endl;
			cerr << "t= " << t << " lest= " << info.lest[t] << " " << hands[t]<< endl;
		}
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

	for(int i=0;i<vhs.size();i++){
		records.push_back(URecord(i));
	}

	for(int i=0;i<3000;i++){
		auto ucb1_tuned = [=](URecord &a,URecord &b){
			return a.ucb1_tuned(i)<b.ucb1_tuned(i);
		};

		auto maxelem = max_element(records.begin(),records.end(),ucb1_tuned);
		int rank = playout(info,vhs[maxelem->tag],myCards,oppCards);
		double score[] = {1,0.75,0.5,0.25,0};
		maxelem->pushScore(score[rank]);
	}
	auto cmp = [](URecord &a,URecord &b){
		return a.x()<b.x();
	};
	auto maxelem = max_element(records.begin(),records.end(),cmp);

	return vhs[maxelem->tag];
}
