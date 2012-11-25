#include "mydef.h"
#include <smmintrin.h>

#pragma once

const Cards JOKER = (1LL<<52);
const Cards SP3  = (1LL);
const Cards DIA3 = (1LL<<2);
const Cards Eight = (0xFLL<<(4*5));

inline int countBit_NOSSE(unsigned long x) {
  x = x - (( x >> 1 ) & 0x55555555);
  x = (x & 0x33333333) + (( x >> 2) & 0x33333333);
  x = ( x + ( x >> 4 )) & 0x0F0F0F0F;
  x = x + ( x >> 8 );
  x = x + ( x >> 16 );
  return x & 0x0000003F;
}


inline int bitCount(Cards cards){
	unsigned long low = cards;
	unsigned long high= cards>>32;

#ifdef NOSSE
	return countBit_NOSSE(low)+countBit_NOSSE(high);
#else
	return _mm_popcnt_u32(low)+_mm_popcnt_u32(high);
#endif
}

inline int Strength(int ord,bool rev){
	return rev?12-ord:ord;
}

Cards setBit(ProtocolCards cards);

void bitToArray(ProtocolCards cards, Cards bitCards);
void setSubmitCard(ProtocolCards submit, const Hand& h);

int maxStrength(Cards c,bool rev);
int minStrength(Cards c,bool rev);

Cards diffHand(Cards c,const Hand &h);

void clearCards(int cards[8][15]);
