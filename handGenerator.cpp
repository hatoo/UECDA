#include <vector>
#include <iostream>
#include <cmath>
#include "bitCard.h"
#include "mydef.h"
#include "handGenerator.h"

using namespace std;
//Cards == int64

vector<Hand> insertJoker(Hand h){
	vector<Hand> res;
	for(int i=0;i<53;i++){
		const Cards c = 1LL<<i;
		if(h.hands&c){
			Hand t=h;
			t.hands^=c;
			res.push_back(t);
		}
	}
	return res;
}

vector<Hand> getGroup(Cards myCards,int num,int min_ord,
		bool rev,unsigned char suit,bool strict){
	vector<Hand> res;
	bool joker = myCards&JOKER;
	for(int i=min_ord;i>=0&&i<13;rev?i--:i++){
		Cards tmp = myCards>>(i*4);
		for(int64 k=1;k<16;k++){
			const int count = bitCount(k);
			//手札でマークkの役がつくれるか。ジョーカーがある場合も考える
			if((suit==0||suit==k)
				&&(num==0||count==num)
				&&((tmp&k)==k||(joker&&(count>1)&&bitCount(tmp&k)==(count-1)))){
				Hand h;
				h.hands = (tmp&k)<<(4*i);
				h.qty=count;
				h.seq=false;
				h.ord = i;
				h.suit=k;
				res.push_back(h);
				if(strict&&(count>1)&&joker&&((tmp&k)==k)){
					vector<Hand> ij = insertJoker(h);
					res.insert(res.end(),ij.begin(),ij.end());
				}
			}
		}
	}
	//joker単体
	if(joker&&(num==0||num==1)){
		Hand h;
		h.hands = 0LL;
		h.qty = 1;
		h.seq = false;
		h.ord = 13;
		h.suit=0;
		res.push_back(h);
	}
	return res;
}

vector<Hand> getSeq(Cards myCards,int num,int min_ord,
		bool rev,unsigned char suit,bool strict){
	vector<Hand> res;
	bool joker = myCards&JOKER;
	for(int s=0;s<4;s++){
		if(suit!=0 && !(suit==(1LL<<s)))continue;
		for(int i=min_ord;i>=0&&i<13;rev?i--:i++){
			Cards use_joker=0LL;
			Cards hands = 0LL;
			for(int k=i;k>=0&&k<13;rev?k--:k++){
				if((myCards&(1LL<<(k*4+s)))==0){
					if(joker&&(use_joker==0LL)){
						use_joker=1LL<<(k*4+s);
					}else{
						break;
					}
				}
				hands|=(1LL<<(k*4+s));
				int n = abs(k-i)+1;
				if(bitCount(use_joker)>1){
					cerr << "use_joker=" << use_joker << endl;
				}
				if(n>=3&&(n==num||(num==0))){
					Hand h;
					h.hands = hands^use_joker;
					h.seq=true;
					h.qty=n;
					h.ord=rev?k:i;
					h.suit=1<<s;
					res.push_back(h);
					if(strict&&joker&&(use_joker==0LL)){
						vector<Hand> ij = insertJoker(h);
						res.insert(res.end(),ij.begin(),ij.end());
					}
				}
			}
		}
	}
	return res;
}

vector<Hand> getAllValidHands(const fieldInfo& info,Cards myCards,bool strict){
	vector<Hand> res;
	if(info.onset){
		res=getGroup(myCards,0,0,false,0,strict);
		vector<Hand> t=getSeq(myCards,0,0,false,0,strict);
		res.insert(res.end(),t.begin(),t.end());
	}else{
		if(!info.seq){
		res=getGroup(myCards,info.qty,info.ord+(info.rev?-1:1)
				,info.rev,info.lock?info.suit:0,strict);
		}
		if(info.seq){
		vector<Hand> t=getSeq(myCards,info.qty,
				info.ord+(info.rev?-1:info.qty)
				,info.rev,info.lock?info.suit:0,strict);
		res.insert(res.end(),t.begin(),t.end());}
		//pass
		Hand h;
		h.hands=0LL;
		h.qty=0;
		res.push_back(h);
	}
	if(!info.onset&&(myCards&SP3)&&info.qty==1&&(info.rev?info.ord==-1:info.ord==13)){
		//sp3
		Hand h;
		h.hands=SP3;
		h.qty=1;
		h.seq=false;
		h.ord=0;
		h.suit=SP3;
		res.push_back(h);
	}
	return res;
}
/*
void removeHands(vector<Hand>& hands,Crads submit,Cards myCards);

vector<Hand> generateAllHands(int64 myCards);

//void generateGroup(bitValidHandsArray *vha, int64 myCards);

//パス以外の行動を取れるか
bool checkAllValidHands(const fieldInfo& info, int64 myCards);
*/
