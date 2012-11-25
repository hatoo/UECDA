/* Author    : Fumiya Suto      */
// modified hatoo
#include <cstring>
//#include <smmintrin.h>
#include "bitCard.h"
#include "mydef.h"

/*
inline int bitCount(int64 cards){
	int64 t;
	int res = 0;
	for(t=cards;t;t&=(t-1)) res++;
	unsigned long low = cards;
	unsigned long high= cards>>32;
	return _mm_popcnt_u32(low)+_mm_popcnt_u32(high);
}
*/

Cards setBit(ProtocolCards cards){
	int64 result = 0;
	
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

int maxStrength(Cards c,bool rev=false){
	if(c&JOKER)return 13;
	if(rev)return 12 - minStrength(c,false);
	for(int i=12;i>=0;i--){
		if(c&(0xF<<(4*i)))return i;
	}
	return 0;
}

int minStrength(Cards c,bool rev=false){
	if(c&JOKER)return 13;
	if(rev)return 12 - maxStrength(c,false);
	for(int i=0;i<=12;i++){
		if(c&(0xf<<(4*i)))return i;
	}
	return 0;
}

Cards diffHand(Cards c,const Hand &h){
	c^=(c&h.hands);
	if(h.jokerUsed())c^=(c&JOKER);
	return c;
}

void clearCards(int cards[8][15]){  
  /*
    ?????œn???ꂽ?J?[?h?e?[?u??cards?̃J?[?h?????̕??????S??0?ɂ??A?J?[?h???ꖇ?ֳ?????Ԃɂ????B
  */
  int s,t;
  
  for(s=0;s<5;s++){
    for(t=0;t<15;t++){
      cards[s][t]=0;
    }   
  }
}
