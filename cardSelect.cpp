#include <vector>
#include <iostream>
#include "mydef.h"
#include "handGenerator.h"
#include "bitCard.h"
#include "myrandom.h"
#include "mydebug.h"

using namespace std;

void removeHands(vector<Hand> &hands,Hand){
}

Hand isWIN(fieldInfo f,Cards myCards,Cards oppCards){
	
}

void selectHand(ProtocolCards p,fieldInfo& info,Cards myCards){
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
	setSubmitCard(p,hs[randInt(0,hs.size()-1)]);
}
