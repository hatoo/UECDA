#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "bitCard.h"
#include "mydef.h"
#include "handGenerator.h"

using namespace std;

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
	vector<Hand> res;
	res.reserve(128);
	const bool hasJoker = myCards&JOKER;
	const Cards mask = MASK(min_ord,rev);
	const int numMemo[][16]={
		{15,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}, //個数 ,内容
		{4,1,2,4,8},
		{6,3,5,9,6,10,12},
		{4,14,13,11,7},
		{1,15}
	};

	if(myCards&mask)
		for(int i=min_ord;i>=0&&i<13;rev?i--:i++){
			Cards tmp = myCards >> (i*4);
			if((tmp&0xF)==0)continue;
			if(!rev&&tmp==0LL)break;
			for(int k=0;k<numMemo[num][0];k++){
				const int fourbit = numMemo[num][k+1];
				const int count = bitCount(fourbit);
				if((((tmp&fourbit)==fourbit)||(hasJoker&&(count>1)
								&&bitCount((tmp&fourbit)^fourbit)==1))
					&&(suit==0||suit==fourbit)){
					Hand h;
					h.hands = (tmp&fourbit)<<(4*i);
					h.qty=count;
					h.seq=false;
					h.ord = i;
					h.suit=fourbit;
					res.push_back(h);

					if(strict&&(count>1)&&hasJoker&&((tmp&fourbit)==fourbit)){
						vector<Hand> ij = insertJoker(h);
						res.insert(res.end(),ij.begin(),ij.end());
					}
				}
			}
		}
	if(hasJoker&&(num==0||num==1)){
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

vector<Hand> getGroup_old(Cards myCards,int num,int min_ord,
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
	const Cards mask = MASK(min_ord,rev);
	if(myCards&mask)
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
	if(!info.onset){
		//pass 最後に入れる
		Hand h;
		h.hands=0LL;
		h.qty=0;
		res.push_back(h);
	}
	return res;
}
/*
void removeHands(vector<Hand>& hands,Crads submit,Cards myCards);
*/
vector<Hand> generateAllHands(Cards myCards,bool strict){
	auto res = getGroup(myCards,0,0,false,0,strict);
	auto seq =   getSeq(myCards,0,0,false,0,strict);
	res.insert(res.end(),seq.begin(),seq.end());
	return res;
}
/*
//void generateGroup(bitValidHandsArray *vha, int64 myCards);
*/
//パス以外の行動を取れるか
bool checkAllValidHands(const fieldInfo& info, Cards myCards){
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

vector<Hand> minHandPair(Cards c){
	vector<Hand> res;
	while(c){
		auto hands = generateAllHands(c);
		int index=0;
		for(int i=0;i<hands.size();i++){
			if(hands[index].qty<hands[i].qty)index=i;
		}
		res.push_back(hands[index]);
		c = diffHand(c,hands[index]);
	}
	return res;
}

int minHandPairNum_Group(Cards c){
	int res= 0;
	for(int i=0;i<13;i++){
		if(c&(0xFLL << (4*i)))res++;
	}
	return res;

}

int minHandPairNum(Cards c){//不完全な実装
	const Cards seqs[] = {0x111,0x222,0x444,0x888};
	int simple = minHandPairNum_Group(c);
	int seq = 0;
	
	for(int i=0;i<11;i++){
		for(int k=0;k<4;k++){
			Cards tmp = seqs[k] << (4*i);
			if((c&tmp)==tmp){
				c^=tmp;
				seq++;
			}
		}
	}
	return min(simple,minHandPairNum_Group(c)+seq);
}

inline bool checkSeq(Cards myCards){
	Cards suits[]={
		0x1111111111111uLL,
		0x2222222222222uLL,
		0x4444444444444uLL,
		0x8888888888888uLL,
	};
	for(int i=0;i<4;i++){
		if(bitCount(myCards&suits[i])>=3)return true;
	}
	return false;
}

vector<Hand> getOnsetHands(Cards myCards){
	const bool hasjoker = myCards&JOKER;
	std::vector<Hand> res;
	if(hasjoker || checkSeq(myCards))res=getSeq(myCards);
	res.reserve(res.size()+14);

	for(int i=0;i<13;i++){
		Cards c = myCards&(0xFLL<<(4*i));
		if((myCards>>(4*i)) ==0)break;
		if(c!=0LL){
			const int count = bitCount(c);
			const int suit  = c>>(4*i);
			Hand h;
			h.hands = c;
			h.qty = count;
			h.ord = i;
			h.seq = false;
			h.suit = suit;
			res.push_back(h);

			if(hasjoker&&count<4){
				for(int i=0;i<4;i++){
					if((suit&(1<<i))==0){
						Hand h;
						h.hands = c;
						h.qty=count+1;
						h.ord=i;
						h.seq=false;
						h.suit = suit|(1<<i);
						res.push_back(h);
						break;
					}
				}
			}
		}
	}
	if(myCards&JOKER){
		Hand h;
		h.hands = 0LL;
		h.qty=1;
		h.ord=13;
		h.seq=false;
		h.suit=0;
		res.push_back(h);
	}
	return res;
}

bool isBecomeOnset(fieldInfo info,const Hand &h,Cards oppCards){
	const int handnum = info.maxOppHandNum();
	info.simulate(h,info.mypos);
	if(info.onset)return true;
	if(handnum<h.qty)return true;
	return !checkAllValidHands(info,oppCards);
}
