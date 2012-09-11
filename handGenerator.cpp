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

vector<Hand> getGroup_debug(Cards myCards,int num,int min_ord,
		bool rev,unsigned char suit,bool strict){
	const Cards allnum = (1LL << 52)-1;
	vector<Hand> res;
	res.reserve(128);
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


vector<Hand> getGroup(Cards myCards,int num,int min_ord,
		bool rev,unsigned char suit,bool strict){
	const Cards allnum = (1LL << 52)-1;
	vector<Hand> res;
	res.reserve(128);
	bool joker = myCards&JOKER;
	if(!rev?myCards&((allnum<<(min_ord*4))&(~JOKER)):myCards&((allnum>>((12-min_ord)*4))))//もともとmin_ord以上のカードを持っていない場合はやめて高速化
	for(int i=min_ord;i>=0&&i<13;rev?i--:i++){
		Cards tmp = myCards>>(i*4);
		if(!rev&&tmp==0LL)break;
		for(int64 k=1;k<16;k++){
			if((tmp&k)==0)continue;
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
	/*
	if(getGroup_debug(myCards,num,min_ord,rev,suit,strict).size()!=res.size()){
		cerr << "getGroup error" << endl;
	}*/
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
*/
//パス以外の行動を取れるか
bool checkAllValidHands(fieldInfo& info, Cards myCards){
	if(info.onset)return true;
	if(info.qty==1&&(myCards&JOKER))return true;
	//スペ3
	if(info.SingleJoker()){
		return myCards&SP3;
	}
	if(info.seq){
		Cards mask=0;
		for(int i=0;i<info.qty;i++){
			mask<<=4;
			mask+=(info.lock?info.suit:1);
		}
		if(info.rev){
			mask<<=((info.ord-info.qty)*4);
		}else{
			mask<<=((info.ord+info.qty)*4);
		}
		const Cards tmp=mask;
		for(int k=0;k<4;k++){
			mask=tmp<<k;
			for(int i=0;i<(info.rev?info.ord-info.qty+1:14-info.ord-2*info.qty);
					i++,info.rev?mask>>=4:mask<<=4){
				if(myCards&JOKER){
					if(bitCount((myCards&mask)^mask)<=1)return true;
				}else if((myCards&mask)==mask)return true;
			}
			if(info.lock)return false;
		}
	}else{
		for(int ord=info.rev?info.ord-1:info.ord+1;
				ord>=0&&ord<=12;
				info.rev?ord--:ord++){
			if(info.lock){
				Cards mask=info.suit;
				mask<<=(4*ord);
				if(myCards&JOKER){
					if(bitCount(myCards&mask)>=info.qty-1 )return true;
				}else if((myCards&mask)==mask)return true;
			}else{
				Cards mask=0xF;
				mask<<=(4*ord);
				if(myCards&JOKER){
					if(bitCount(myCards&mask)>=(info.qty-1))return true;
				}else if(bitCount(myCards&mask)>=info.qty)return true;
			}
		}
	}
	return false;
}

