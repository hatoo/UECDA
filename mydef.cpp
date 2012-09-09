#include <cmath>
#include "bitCard.h"
#include "mydef.h"

bool Hand::jokerUsed(){
	return bitCount(hands)!=qty;
}

void fieldInfo::set(int my_num,ProtocolCards& p){
	onset = p[5][4] > 0;
	rev   = p[5][6] > 0;
	lock  = p[5][7] > 0;
	
	if(onset){
		qty=0;
		ord=0;
		lock=false;
		seq=false;
		pass=0;
	}
	for(int i=0;i<5;i++){
		seat[p[6][i+10]]=i;
		if(p[6][i+10] == my_num)mypos=i;
	}

	for(int i=0;i<5;i++){
		lest[seat[i]]=p[6][i];
		rank[seat[i]]=p[6][i+5];
	}
}

void fieldInfo::set_ba(ProtocolCards& cards){
	set(seat[mypos],cards);
	//カードのある位置を探す
	int s=0,n=0;
	while(n<15&&cards[s][n]==0){
		s++;
		if(s==4){
			n++;
			s=0;
		}
	}
	onset = cards[5][4] > 0;

	bool isSeq=n<14&&cards[s][n+1];
	ord = n-1;
	if(!onset&&isSeq){
		int q=0;
		for(;n<14&&cards[s][n];q++,n++);
		seq=true;
		suit=1LL<<s;
		qty=q;
	}
	if(!onset&&!isSeq){
		suit=0;
		qty=0;
		seq=false;
		for(int i=s;i<4;i++){
			if(cards[i][n]){
				suit|=(1<<i);
				qty++;
			}
		}
		if(qty==1&&cards[s][n]==2)ord=rev?-1:13;
	}
	//縛りや革命は::setで十分
		// パス・あがり情報、プレイヤーの残りカード枚数を更新
	int cur = cards[5][3];
	if(cards[6][cur] == lest[seat[cur]]){
		pass |= (1 << seat[cur]);
	} else {
		lest[seat[cur]] = cards[6][cur];
		if(lest[seat[cur]] == 0)
			goal |= (1 << seat[cur]);
	}
}

bool fieldInfo::SingleJoker(){
	return qty==1&&(rev?ord==-1:ord==13);
}

void fieldInfo::submit(const Hand &h){
	if(h.qty==0)return;//pass
	lock = !onset&&suit==h.suit;
	onset=false;
	qty=h.qty;
	suit=h.suit;
	ord=h.ord;
	seq=h.seq;
	if((h.seq&&h.qty>=5)||(!h.seq&&h.qty>=4)){
		rev=!rev;
	}
}

void fieldInfo::reset(){
	onset=true;
	suit=0;
	qty=0;
	seq=false;
	ord=0;
	lock=0;
	pass=0;
}

int fieldInfo::simulate(const Hand &h,int pos){
	const int ALL = 0x1F;
	submit(h);
	if(h.qty==0){
		pass|=(1<<pos);
	}
	lest[pos]-=h.qty;
	if(lest[pos]==0){
		goal|=(1<<pos);
	}
	if(pass==ALL){
		reset();
		pass=goal;
		if(goal==ALL){
			return pos;//とりあえずposを返す
		}else{
			int i=pos;
			while(goal&(1<<i)){
				i++;
				i%=5;
			}
			return i;
		}
	}else{
		int i=pos;
		while(pass&(1<<i)){
			i++;
			i%=5;
		}
		return i;
	}
}


/*
void fieldInfo::set_ba(ProtocolCards &cards){
	fieldInfo *info=this;
	int i,j,count=0, joker=0;
	i=j=0;
  
	//カードのある位置を探す
	while(j<15&&cards[i][j]==0){
		i++;
		if(i==4){
			j++;
			i=0;
		}
	}
	if(cards[i][j] == 2) joker = 1;
	//階段が否か
	if(j<14){
		info->seq = (cards[i][j+1] > 0);
	}
	//枚数を数える また強さを調べる
	if(info->seq == 0){
		//枚数組
		info->suit = 0;
		for(;i<5;i++){
			if(cards[i][j]>0){
				count++;
				info->suit |= (1<<i);
			}
		}
		if(j==0||j==14||(count==1&&joker==1)){
			if(info->rev == 0) info->ord = 14;
			else               info->ord = 0;
		}else{
			info->ord = j;
		}
	}else{
		//階段
		while(j+count<15 && cards[i][j+count]>0){
			count++;
		}
		// 役の中で一番弱いカードの強さを役の強さとする
		info->ord = j;
		info->suit = (1<<i);
	}
	info->qty = count;
	if(info->qty > 0) info->onset = 0;
	info->ord--;

	// パス・あがり情報、プレイヤーの残りカード枚数を更新
	int cur = cards[5][3];
	if(cards[6][cur] == info->lest[info->seat[cur]]){
		info->pass |= (1 << info->seat[cur]);
	} else {
		info->lest[info->seat[cur]] = cards[6][cur];
		if(info->lest[info->seat[cur]] == 0)
			info->goal |= (1 << info->seat[cur]);
	}
}*/
