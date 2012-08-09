#include <vector>
#include "mydef.h"
#include "handGenerator.h"
#include "bitCard.h"
#include "myrandom.h"

using namespace std;

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

	setSubmitCard(p,hs[randInt(0,hs.size()-1)]);
}
