#include <vector>
#include <set>
#include <iostream>
#include <cmath>
#include "mydef.h"
#include "handGenerator.h"
#include "bitCard.h"
#include "myrandom.h"
#include "mydebug.h"

using namespace std;

int count=0;
int mcount =0;

void removeHands(vector<Hand> &hands,Hand){
}


void putHand(fieldInfo &f,const Hand &h){
	f.lock = !f.onset&&f.suit==h.suit;
	f.onset=false;
	f.qty=h.qty;
	f.suit=h.suit;
	f.ord=h.ord;
	f.seq=h.seq;
}

bool isWIN(fieldInfo info,Cards myCards,Cards oppCards,int maxHandNum,set<Cards>& failed){
	if(failed.find(myCards)!=failed.end())return false;
	vector<Hand> vhs = getAllValidHands(info,myCards);
	for(int i=0;i<vhs.size();i++){
		fieldInfo t=info;
		putHand(t,vhs[i]);
		if(vhs[i].hands&(0xf<<5) //8切り
				|| vhs[i].qty>maxHandNum
				|| (maxStrength(oppCards,info.rev)<=minStrength(myCards,info.rev))
				|| !checkAllValidHands(t,oppCards)){
			Cards next = myCards^vhs[i].hands;
			if(vhs[i].jokerUsed())next^=JOKER;
			if(next==0)return true;
			if(failed.find(next)!=failed.end())return false;
			t.onset=true;t.suit=0;t.qty=0;
			bool b = isWIN(t,next,oppCards,maxHandNum,failed);
			if(b)return true;
		}
	}
	failed.insert(myCards);
	return false;
}

Hand winHand(fieldInfo info,Cards myCards,Cards oppCards,int maxHandNum){
	set<Cards> failed;
	vector<Hand> vhs = getAllValidHands(info,myCards);
	for(int i=0;i<vhs.size();i++){
		fieldInfo t=info;
		t.submit(vhs[i]);
		if(vhs[i].hands&(0xf<<5) || vhs[i].qty>maxHandNum || !checkAllValidHands(t,oppCards)){
			Cards next = myCards^vhs[i].hands;
			if(vhs[i].jokerUsed())next^=JOKER;
			if(next==0)return vhs[i];
			//t.onset=true;t.suit=0;t.qty=0;
			t.reset();
			bool b = isWIN(t,next,oppCards,maxHandNum,failed);
			if(b)return vhs[i];
		}
	}
	Hand h;
	h.qty=0;
	if(count<failed.size()){
		//cerr << "size=" << failed.size() << endl;
		//count=failed.size();
	}
	return h;
}

void selectHand(ProtocolCards p,fieldInfo& info,Cards myCards,Cards oppCards){
	vector<Hand> hs=getAllValidHands(info,myCards);
	/*int idx=0;
	for(int i=0;i<hs.size();i++){
		if(info.onset&&hs[i].qty>hs[idx].qty)idx=i;
		if(!info.onset&&hs[i].qty!=0&&
				   (!info.rev&&hs[i].ord<hs[idx].ord)
				|| (info.rev&&hs[i].ord>hs[idx].ord)){
					idx=i;
				}
	}*/
	bool b = checkAllValidHands(info,myCards);
	if(!info.onset&&b!=(hs.size()>1)){
		cerr << "checkAllValidHands error" << endl;
		cerr << "b= " << b << endl;
		cerr << "size= " << hs.size() << endl;
		dumpCards(myCards);
		dumpFinfo(info);
	}

	int maxHandNum=0;
	for(int i=0;i<5;i++){
		if(i!=info.mypos){
			maxHandNum=max(maxHandNum,info.lest[i]);
		}
	}
	Hand h = winHand(info,myCards,oppCards,maxHandNum);
	if(count > mcount){
		/*
		cerr << "count= " << count << endl;
		dumpCards(myCards);
		dumpCards(oppCards);
		mcount=count;*/
	}
	if(h.qty>100){
		cerr << "isWin is true" << endl;
		dumpFinfo(info);
		dumpCards(myCards);
		dumpCards(oppCards);
		cerr << "mhn= " << maxHandNum << endl;
		cerr << "hand= " << endl;
		dumpHand(h);
		cerr << endl;
	}
	setSubmitCard(p,h.qty!=0?h:hs[randInt(0,hs.size()-1)]);
}
