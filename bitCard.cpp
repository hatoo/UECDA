/* bitCard.c : ビット表現したカード集合に対する操作を定義 */ 
/* Author    : Fumiya Suto                                */
#include <cstring>
#include "bitCard.h"
#include "mydef.h"

// 与えられたビット配列中の1ビットの数を返す
int bitCount(int64 cards){
	int64 t;
	int res = 0;
	for(t=cards;t;t&=(t-1)) res++;
	return res;
}

// 配列表現されているカード集合をビット表現に変換する
Cards setBit(ProtocolCards cards){
	int64 result = 0;
	int64 jokerBit = (1LL << 52);
	
	for(int i=0;i<=4;i++)
		for(int k=0;k<=14;k++){
			if(cards[i][k]==1){result|=1LL<<(4*(k-1)+i);}
			if(cards[i][k]==2){result|=JOKER;}
		}

	return result;
}
/*
void bitToArray(int cards[8][15], int64 bitCards){
	int i, j;
	for(i=0;i<5;i++){
		for(j=0;j<15;j++){
			cards[i][j] = 0;
		}
	}
	for(i=0;i<4;i++){
		for(j=0;j<13;j++){
			if((bitCards>>(i*13+j))&1 == 1) cards[i][j+1] = 1;
		}
	}
	if((bitCards>>52)==1) cards[4][1] = 2;
}*/

// hが表す手を配列submit上に格納する
void setSubmitCard(int submit[8][15], const Hand& h){
	memset(submit,sizeof submit,0);
	if(h.qty==0)return;//pass
	if(h.qty==1&&h.ord==13){//joker only
		submit[0][1]=2;
		return;
	}
	if(h.seq){
		int s=0;
		while(h.suit!=(1<<s))s++;
		for(int o=h.ord;o<h.ord+h.qty;o++){
			if(h.hands & (1LL<<(o*4+s))){
				submit[s][o+1]=1;
			}else{
				submit[s][o+1]=2;
			}
		}
	}else{
		for(int s=0;s<4;s++){
			if(h.suit&(1LL<<s)){
				if(h.hands&(1LL<<(h.ord*4+s))){
					submit[s][h.ord+1]=1;
				}else{
					submit[s][h.ord+1]=2;
				}
			}
		}
	}
}

