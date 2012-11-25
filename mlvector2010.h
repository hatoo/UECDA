/* http://uecda.nishino-lab.jp/sympo/images/Suto.pdf
 * を元に実装した特徴ベクトルの定義 */
#include "mydef.h"
#include "machineLearn.h"
#include "myrandom.h"
#include "bitCard.h"
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include <cmath>

#pragma once
using namespace boost::numeric;

inline std::vector<Hand> getAIHand(Cards myCards){
	std::vector<Hand> res;
	res.reserve(24);

	for(int i=0;i<4;i++){
		const Cards suit = 1uLL<<i;
		Cards seq = 0uLL;
		int count = 0;
		int ord=-1;
		//bool joker_used = !(myCards&JOKER);
		for(int k=0;k<13;k++){
			if(myCards&(suit<<(4*k))){
				if(ord== -1)ord=k;
				seq|=(suit<<(4*k));
				count++;
			}else{
				if(count>=3){
					Hand h;
					h.seq=true;
					h.ord=ord;
					h.hands=seq;
					h.qty=count;
					h.suit=suit;
					res.push_back(h);
				}
				count=0;
				ord=-1;
				seq=0uLL;
			}
		}
		if(count>=3){
			Hand h;
			h.seq=true;
			h.ord=ord;
			h.hands=seq;
			h.qty=count;
			h.suit=suit;
			res.push_back(h);
		}
	}

	for(int i=0;i<13;i++){
		Cards c = myCards&(0xFLL<<(4*i));
		if((myCards>>(4*i)) ==0)break;
		if(c!=0LL){
			Hand h;
			h.hands = c;
			h.qty = bitCount(c);
			h.ord = i;
			h.seq = false;
			h.suit = c>>(4*i);
			res.push_back(h);
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


class mlvector2010{
	static int getIndex(const Hand &h,bool rev){
	/* メモリ配置
	 * 通常時用:0~
	 * 革命時用:256~
	 * 256の内訳
	 * グループ:(枚数)1~4 * ord  = 4*14 = 56
	 * 階段    :(枚数)3~6 * ord  = 4*13 = 52
	 * 他は使わない
	 */
		if(h.rev())rev=!rev;
		int res=0;
		int ord = h.qty==1&&h.jokerUsed()?13:h.ord;//革命時はジョーカ単体のordが-1になるので
		//if(ord==13)cout<<"aa" << endl;
		if(rev)res+=256;
		if(h.qty==0){
			//res += 56+52;
		}else
		if(h.seq){
			res += 56+4*ord+min(6,(int)h.qty)-3;
		}else{
			res += 4*ord+min(4,(int)h.qty)-1;
		}
		return res;
	}
public:
	static const int size = 512;
	static uvector phi(const state& s,const action& a){
		uvector v(size);
		v.clear();
		Cards newHand = diffHand(s.myCards(),a);
		std::vector<Hand> hs = getAIHand(newHand);
		for(int i=0;i<hs.size();i++){
			v[getIndex(hs[i],s.info.rev)]=1.0;
		}
		return v;
	}
};
