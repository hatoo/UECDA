#include <vector>
#include <set>
#include <iostream>
#include <cmath>
#include "cardSelect.h"
#include "mydef.h"
#include "handGenerator.h"
#include "bitCard.h"
#include "myrandom.h"
#include "mydebug.h"
#include "montecarlo.h"
#include "machineLearn.h"
#include "fastWeakAI.h"

using namespace std;

AImode mode = MLAI;

void setMode(AImode m){
	mode=m;
}

bool isWIN(fieldInfo info,Cards myCards,Cards oppCards,int maxHandNum,set<Cards>& failed){
	if(failed.find(myCards)!=failed.end())return false;
	vector<Hand> vhs = getAllValidHands(info,myCards);
	for(int i=0;i<vhs.size();i++){
		fieldInfo t=info;
		t.submit(vhs[i]);
		if(vhs[i].hands&(0xf<<5) //8切り
				|| vhs[i].qty>maxHandNum
				// || (maxStrength(oppCards,info.rev)<=minStrength(myCards,info.rev))
				|| !checkAllValidHands(t,oppCards)){
			Cards next = myCards^vhs[i].hands;
			if(vhs[i].jokerUsed())next^=JOKER;
			if(next==0)return true;
			if(failed.find(next)!=failed.end())return false;
			//t.onset=true;t.suit=0;t.qty=0;
			t.reset();
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
	//if(count<failed.size()){
		//cerr << "size=" << failed.size() << endl;
		//count=failed.size();
	//}
	return h;
}

Hand selectHand_(fieldInfo info,Cards myCards,Cards oppCards,
		const changeInfo cinfo){
	int maxHandNum=info.maxOppHandNum();
	Hand h = winHand(info,myCards,oppCards,maxHandNum);
	Hand pass;pass.qty=0;
	//return (fastWeakAI(info,myCards,oppCards));
	//*
	return (h.qty!=0?h:
			(!checkAllValidHands(info,myCards)?
			 pass:montecalroSearch(info,myCards,oppCards,cinfo)));
}


void selectHand(ProtocolCards p,fieldInfo& info,Cards myCards,Cards oppCards
		,const changeInfo& cinfo){
	//vector<Hand> hs=getAllValidHands(info,myCards);
	/*int idx=0;
	for(int i=0;i<hs.size();i++){
		if(info.onset&&hs[i].qty>hs[idx].qty)idx=i;
		if(!info.onset&&hs[i].qty!=0&&
				   (!info.rev&&hs[i].ord<hs[idx].ord)
				|| (info.rev&&hs[i].ord>hs[idx].ord)){
					idx=i;
				}
	}
	bool b = checkAllValidHands(info,myCards);
	if(!info.onset&&b!=(hs.size()>1)){
		cerr << "checkAllValidHands error" << endl;
		cerr << "b= " << b << endl;
		cerr << "size= " << hs.size() << endl;
		dumpCards(myCards);
		dumpFinfo(info);
	}
*/
	int maxHandNum=info.maxOppHandNum();
		/*0;
	for(int i=0;i<5;i++){
		if(i!=info.mypos){
			maxHandNum=max(maxHandNum,info.lest[i]);
		}
	}*/
	Hand h = winHand(info,myCards,oppCards,maxHandNum);
	if(h.qty!=0){ 
		setSubmitCard(p,h);
		return;
	}
	if(!checkAllValidHands(info,myCards)){
		Hand pass;pass.qty=0;pass.hands=0uLL;
		setSubmitCard(p,pass);
		return;
	}

	/*
	if(count > mcount){
		cerr << "count= " << count << endl;
		dumpCards(myCards);
		dumpCards(oppCards);
		mcount=count;
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
	}*/
	//Cards out[5];
	//DevideCards(info,oppCards,out);
	switch(mode){
		case FWAI:
			setSubmitCard(p,
					montecalroSearch_Functional(info,myCards,oppCards,cinfo,
						[](const fieldInfo& info,Cards myCards,Cards oppCards,const changeInfo &cinfo){
						return fwAI1(info,myCards,oppCards);
						},3000));
			break;
		case MLAI:
			setSubmitCard(p,montecalroSearch(info,myCards,oppCards,cinfo));
			break;
	}
	/*
	int o = playout(info,hs.back(),myCards,oppCards);
	if(h.qty>0 && o!=bitCount(info.goal)){
		cerr << "qty= " << (int)h.qty << endl;
		cerr << "o= " << o << endl;
		cerr << "goal= " << bitCount(info.goal) << endl;
		cerr << endl;
	}*/
		//setSubmitCard(p,mlAI(info,myCards,oppCards,cinfo));
	//*
	/*setSubmitCard(p,montecalroSearch_Functional(info,myCards,oppCards
				,cinfo
				,[](const fieldInfo& info,Cards myCards,Cards oppCards,const changeInfo &cinfo){
				if(!checkAllValidHands(info,myCards)){
					Hand pass;pass.qty=0;pass.hands=0uLL;
					return pass;
				}
				vector<Hand> hs = info.onset?
					getOnsetHands(myCards):
					getAllValidHands(info,myCards);
				if(hs.size()==1)return hs[0];
				//if(randDouble(0.0,1.0)<=0.1)return hs.back();
				return hs[randInt(0,hs.size()-2)];
				},60000));*/
	//setSubmitCard(p,DoubleMontecalroSearch(info,myCards,oppCards,cinfo));
	// */
}
