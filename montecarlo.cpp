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
#include <cmath>
#include "mydebug.h"
#include "cardSelect.h"
#include <set>
#include <random>
#include <array>
#include "machineLearn.h"
#include <functional>
#include "fastWeakAI.h"
#include "ml3.h"
//#include <thread>
//#include <future>
//#include <boost/thread.hpp>
//#include <boost/ref.hpp>

using namespace std;
//using namespace boost;

Hand SimpleAI(const fieldInfo &info,Cards myCards){
	vector<Hand> hs = getAllValidHands(info,myCards);
	return hs[randInt(0,hs.size()-1)];
}

Hand fastWeakAI(const fieldInfo &info,Cards myCards,Cards oppCards){
	vector<Hand> hands = getAllValidHands(info,myCards);
	//double r =randDouble(0.0,1.0);
	if(hands.size()==1)return hands[0];
	Hand pass;pass.qty=0;pass.hands=0LL;
	//if(r<=0.5)return pass;
	int min_str=100;
	for(int i=0;i<13;i++){
		if((myCards>>(4*i))& 0xF){
			min_str = min(min_str,Strength(i,info.rev));
		}
	}

	auto score_follow=[&](Hand &a){
		if(a.qty==0)return 0;
		int res=0;
		if(a.hands&Eight)    res+=0xF<<(4*6);//tekitou
		if(a.suit==info.suit)res+=0xF<<(4*5);//
		res += (0xF-Strength(a.ord,info.rev));
		return res;
	};

	auto score_lead=[&](Hand &a){
		int res=0;
		res +=a.qty<<(4*6);
		//if(a.hands&Eight)res+=0xF;
		int str = Strength(a.ord,info.rev);
		res += str==min_str?0:(0xF-str);//最弱のカードは出したくない
		return res;
	};


	auto cmp_follow = [&](Hand &a,Hand &b){
		return score_follow(a) < score_follow(b);
	};

	auto cmp_lead = [&](Hand &a,Hand &b){
		return score_lead(a) < score_lead(b);
	};
	
	//auto f = info.onset?score_lead:score_follow;
	/*int total=0;
	vector<int> scores;
	for(int i=0;i<hands.size();i++){
		int s = info.onset?score_lead(hands[i])
			:score_follow(hands[i]);

		scores.push_back(s);
		total+=s;
	}
	int rand = randInt(0,total);
	for(int i=0;i<hands.size();i++){
		rand-=scores[i];
		if(rand<=0)return hands[i];
	}
	cerr << "fastweakai" << endl;
	return pass;
	*/
	
	if(info.onset){
		
		return *max_element(hands.begin(),hands.end(),cmp_lead);
	}else{
		return *max_element(hands.begin(),hands.end(),cmp_follow);
	}

}


int playout(fieldInfo info,Hand h,Cards myCards,Cards oppCards
		,const changeInfo &cinfo){
	int pos = info.mypos;
	Hand hand = h;
	Cards hands[5]={0};
	DevideCards(info,oppCards,hands,cinfo);
	
	hands[info.mypos]=myCards;
	for(int i=0;i<5;i++){
		if(info.lest[i]==0)info.goal|=(1<<i);
	}

	const int mypos = info.mypos;

	oppCards |= myCards;
	while(!(info.goal&(1<<mypos)) && ((info.goal|(1<<mypos))!=0x1F)){
		hands[pos]=diffHand(hands[pos],hand);
		oppCards = diffHand(oppCards,hand);
		pos = info.simulate(hand,pos);
		info.mypos = pos;
		
		//if(hands[pos]){
			//if(pos==mypos && bitCount(hands[pos])<=5){
				//hand = fastWeakAI2(info,hands[pos],oppCards^hands[pos],hands);
			//}else{
				hand = mlAI3(info,hands[pos],oppCards^hands[pos]);//,cinfo);
			//	hand = fwAI1(info,hands[pos],oppCards^hands[pos]);
			//}
		//}
	}
	int count = bitCount(info.goal);
	return info.goal&(1<<mypos)?count-1:count;
}



void DevideCards(fieldInfo info,Cards oppCards,Cards out[5]
		,const changeInfo &cinfo){
	memset(out,0,sizeof(Cards)*5);
	info.lest[info.mypos]=0;

	auto putCard=[&](int pos,Cards c){
		out[pos]|=(oppCards&c);
		info.lest[pos]-=bitCount(oppCards&c);
		oppCards ^= (oppCards&c);
	};
	//cinfoを元にカードを配る
	//あまり強さに貢献してるとは思えない
	putCard(cinfo.oppPos,cinfo.hasCards);
	if(cinfo.firstPlayer!= -1){
		putCard(cinfo.firstPlayer,DIA3);
	}
	//JOKERを平民以上のプレイヤーに配る
	if(oppCards&JOKER){
		vector<int> ps;
		for(int i=0;i<5;i++){
			if(info.rank[i]<=2&&info.lest[i]>0){
				ps.push_back(i);
			}
		}
		if(!ps.empty()){
			putCard(ps[randInt(0,ps.size()-1)],JOKER);
		}
	}
	//残りを配る
	vector<int> yama;
	yama.reserve(bitCount(oppCards));
	for(int i=0;i<53;i++){
		if((oppCards>>i)&1)
			yama.push_back(i);
	}
	random_shuffle(yama.begin(),yama.end(),[](int high){return randInt(0,high-1);});

	{
		auto it=yama.begin();
		for(int i=0;i<5;i++){
			for(int k=0;k<info.lest[i];k++){
				out[i]|= 1uLL<<(*it);
				it++;
			}
		}
	}


	/*
	vector<pair<int,int> > left;//pos,lest
	for(int i=0;i<5;i++){
		if(i!=info.mypos && info.lest[i]!=0){
			left.push_back(make_pair(i,info.lest[i]));
		}
	}
	for(int i=0;oppCards>>i;i++){
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
	}*/
}

Hand montecalroSearch(fieldInfo info,Cards myCards,Cards oppCards
		,const changeInfo &cinfo){
	int prevrank = info.rank[info.mypos];
	typedef ucb1Record<int> URecord;

	vector<Hand> vhs = getAllValidHands(info,myCards);
	/*vector*/URecord records[vhs.size()];
	//records.reserve(vhs.size());
	const double goalnum = bitCount(info.goal);

	for(int i=0;i<vhs.size();i++){
		//records.push_back(URecord(i));
		records[i]=URecord(i);
	}

	for(int i=0;i<SEARCH_NUM;i++){
		const auto ucb1_tuned = [=](const URecord &a,const URecord &b){
			return a.ucb1_tuned(i)<b.ucb1_tuned(i);
		};

		auto maxelem = max_element(records/*.begin()*/,records+vhs.size()/*.end()*/,ucb1_tuned);
		double rank = playout(info,vhs[maxelem->tag],myCards,oppCards,cinfo);
		//double score[] = {1,0.75,0.5,0.25,0};
		double score = rank2score(rank,goalnum,prevrank);//1.0-(rank-goalnum)/(4.0-goalnum);
		maxelem->pushScore(score);
	}
	const auto cmp = [](const URecord &a,const URecord &b){
		return a.x()<b.x();
	};
	const auto maxelem = max_element(records/*.begin()*/,records+vhs.size()/*.end()*/,cmp);

	return vhs[maxelem->tag];
}

double sig_n(double n,double x){
	return 1/(1+exp(-n*x));
}

double rank2score(int rank,int goalnum,int prevrank){
	double t = 1.0-(rank-goalnum)/(4.0-goalnum);
	double w = (5.0-rank);
	return /*t*t;/*/sig_n(1,2.0*(2.0-rank));//t*t;//prevrank>=3?t*t:t;//1.0-(rank-goalnum)/(4.0-goalnum);
}

int playout_Functional(fieldInfo info,Hand h,Cards myCards,Cards oppCards
		,const changeInfo &cinfo
		,const function<Hand(const fieldInfo&,Cards myCards,Cards oppCards,const changeInfo &cinfo)> &AI){
	int pos = info.mypos;
	Hand hand = h;
	Cards hands[5]={0};
	DevideCards(info,oppCards,hands,cinfo);
	
	hands[info.mypos]=myCards;
	
	for(int i=0;i<5;i++){
		if(info.lest[i]==0)info.goal|=(1<<i);
	}

	const int mypos = info.mypos;

	oppCards |= myCards;
	while(!(info.goal&(1<<mypos)) && ((info.goal|(1<<mypos))!=0x1F)){
		hands[pos]=diffHand(hands[pos],hand);
		oppCards = diffHand(oppCards,hand);
		pos = info.simulate(hand,pos);
		info.mypos = pos;
		if(hands[pos])
		hand = AI(info,hands[pos],oppCards,cinfo);//,oppCards^hands[pos],cinfo);
	}
	int count = bitCount(info.goal);
	return info.goal&(1<<mypos)?count-1:count;
}

Hand montecalroSearch_Functional(fieldInfo info,Cards myCards,Cards oppCards
		,const changeInfo &cinfo
		,const function<Hand(const fieldInfo&,Cards myCards,Cards oppCards,const changeInfo &cinfo)> &AI
		,const int playoutnum=3000){
	int prevrank = info.rank[info.mypos];
	typedef ucb1Record<int> URecord;

	vector<Hand> vhs = getAllValidHands(info,myCards);
	/*vector*/URecord records[vhs.size()];
	//records.reserve(vhs.size());
	const double goalnum = bitCount(info.goal);

	for(int i=0;i<vhs.size();i++){
		//records.push_back(URecord(i));
		records[i]=URecord(i);
	}

	for(int i=0;i<playoutnum;i++){
		const auto ucb1_tuned = [=](const URecord &a,const URecord &b){
			return a.ucb1_tuned(i)<b.ucb1_tuned(i);
		};

		auto maxelem = max_element(records/*.begin()*/,records+vhs.size()/*.end()*/,ucb1_tuned);
		double rank = playout_Functional(info,vhs[maxelem->tag],myCards,oppCards,cinfo,AI);
		//double score[] = {1,0.75,0.5,0.25,0};
		double score = rank2score(rank,goalnum,prevrank);//1.0-(rank-goalnum)/(4.0-goalnum);
		maxelem->pushScore(score);
	}
	const auto cmp = [](const URecord &a,const URecord &b){
		return a.x()<b.x();
	};
	const auto maxelem = max_element(records/*.begin()*/,records+vhs.size()/*.end()*/,cmp);

	return vhs[maxelem->tag];
}

Hand DoubleMontecalroSearch(fieldInfo info,Cards myCards,Cards oppCards
		,const changeInfo &cinfo){

	return montecalroSearch_Functional(info,myCards,oppCards,cinfo,
			[](const fieldInfo& info,Cards myCards,Cards oppCards,const changeInfo &cinfo){
		//Hand h= winHand(info,myCards,oppCards,info.maxOppHandNum());
		//if(h.qty!=0)return h;
		if(!checkAllValidHands(info,myCards)){
			Hand pass;pass.qty=0;pass.hands=0uLL;
			return pass;
		}
		return montecalroSearch_Functional(info,myCards,oppCards,cinfo,
			[](const fieldInfo& info,Cards myCards,Cards oppCards,const changeInfo &cinfo)->Hand{
			//Hand h= winHand(info,myCards,oppCards,info.maxOppHandNum());
			//if(h.qty!=0)return h;
			if(!checkAllValidHands(info,myCards)){
				Hand pass;pass.qty=0;pass.hands=0uLL;
				return pass;
			}
			/*vector<Hand> hs = getAllValidHands(info,myCards);
			return hs[randInt(0,hs.size()-1)];*/
			vector<Hand> hs = info.onset?
					getOnsetHands(myCards):
					getAllValidHands(info,myCards);
				if(hs.size()==1)return hs[0];
				//if(randDouble(0.0,1.0)<=0.1)return hs.back();
				return hs[randInt(0,hs.size()-2)];
			},10);
	},100);
}
