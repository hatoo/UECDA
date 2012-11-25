#include "mldefine2012.h"
#include "mydef.h"
#include "bitCard.h"
#include "handGenerator.h"
#include "myrandom.h"
#include "machineLearn.h"

#include <vector>


int getIndex(const Hand &h,bool rev){
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


uvector MLdefine2012::phi(const state &s,const action &a){
	uvector v=zero();
	Cards newHand = diffHand(s.myCards(),a);
	std::vector<Hand> hs = getAIHand(newHand);
	for(int i=0;i<hs.size();i++){
		int a=min(3,bitCount(s.myCards())/3);
		v[512*a+getIndex(hs[i],s.info.rev)]=1.0;
	}
	return v;
}

/*
inline double calcScore(Cards _myCards,const Hand& h,bool rev,
			double *theta,int a){
	double res=0.0;
	if(h.rev())rev=!rev;
	Cards myCards = diffHand(_myCards,h);

	auto p=[=](bool seq,int ord,int qty){
		return theta[512*a+(rev?256:0)+(seq?56:0)+(4*ord)+(seq?qty-3:qty-1)];
	};

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
					res+=p(true,ord,count);
				}
				count=0;
				ord=-1;
				seq=0uLL;
			}
		}
		if(count>=3){
			res+=p(true,ord,count);
		}
	}

	for(int i=0;i<13;i++){
		Cards c = myCards&(0xFLL<<(4*i));
		if((myCards>>(4*i))==0)break;
		if(c!=0LL){
			res+=p(false,i,bitCount(c));
		}
	}
	if(myCards&JOKER){
		res+=p(false,13,1);
	}
	return exp(res);
}
*/

Hand MLdefine2012::optimizedAI(const fieldInfo & info,Cards myCards,Cards oppCards
			,double *theta){
	if(!info.onset && !checkAllValidHands(info,myCards)){
		return Hand();
	}
	std::vector<Hand> hs = 
	info.onset?getOnsetHands(myCards):getAllValidHands(info,myCards);
	if(hs.size()==1)return hs[0];
	const int tehuda = bitCount(myCards);

	double sum=0.0;
	double ratios[hs.size()];
	const int loopnum=hs.size();

	int a=min(3,tehuda/3);
	for(int i=0;i<loopnum;i++){
		if(tehuda==hs[i].qty)return hs[i];
		const double ratio = calcScore(myCards,hs[i],info.rev,theta,a);
		sum+=ratio;
		ratios[i]=ratio;
	}

	double r = randDouble(0.0,sum);
	for(int i=0;i<hs.size();i++){
		r-=ratios[i];
		if(r<=0.0)return hs[i];
	}
	return hs.back();
}

