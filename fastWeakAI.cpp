#include "fastWeakAI.h"
#include "mydef.h"
#include "handGenerator.h"
#include "bitCard.h"

#include <boost/foreach.hpp>
#include <vector>
#include <algorithm>

using namespace std;

int halfOrd(Cards myCards){
	const int num = bitCount(myCards);
	
	int ret=0;
	for(int i=0,count=0;count<num/2&&i<53;i++){
		if((1uLL<<i)&myCards){
			count++;
			ret=i;
		}
	}
	return ret/4;
}

double average_Strength(Cards allCards,bool rev){
	return Strength(8,rev);
	double ret=0;
	int count=0;
	for(int i=0;i<53;i++){
		if(allCards&(1uLL<<i)){
			count++;
			ret+=Strength(i/4,rev);
		}
	}
	return count!=0?ret/count:0;
}

Hand fwAI1(const fieldInfo &info,Cards myCards,Cards oppCards){
	auto str=[&](const Hand &a,const Hand &b){
			return Strength(a.ord,info.rev)<Strength(b.ord,info.rev);
	};

	auto onset_cmp=[&](const Hand &a,const Hand &b){
			return Strength(a.ord,info.rev)-a.qty
				<Strength(b.ord,info.rev)-b.qty;
	};


	const int num = bitCount(myCards);

	if(info.onset){
		vector<Hand> hs = getOnsetHands(myCards);
		BOOST_FOREACH(Hand &h,hs){
			if(h.qty==num)return h;
		}
		return *min_element(hs.begin(),hs.end(),onset_cmp);
	}else{
		vector<Hand> hs = getAllValidHands(info,myCards);
		if(hs.size()==1)return hs[0];
		hs.pop_back();//パスを除外
		vector<Hand> becomeOnset;
		
		BOOST_FOREACH(Hand h,hs){
			if(h.qty==num)return h;
			if(isBecomeOnset(info,h,oppCards))
				becomeOnset.push_back(h);
		}
		if(!becomeOnset.empty()){
			return *min_element(becomeOnset.begin(),becomeOnset.end(),str);
		}

		//const int halford = halfOrd(myCards);
		const double astr = average_Strength(oppCards,info.rev);
		Hand m = *min_element(hs.begin(),hs.end(),str);
		if(Strength(m.ord,info.rev)<=astr){
			return m;
		}
		return Hand();
	}
}
