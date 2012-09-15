#include "mydef.h"
#include <smmintrin.h>

const Cards JOKER = (1LL<<52);
const Cards SP3  = (1LL);

inline int bitCount(Cards cards){
	unsigned long low = cards;
	unsigned long high= cards>>32;
	return _mm_popcnt_u32(low)+_mm_popcnt_u32(high);
}

inline int Strength(int ord,bool rev){
	return rev?12-ord:ord;
}

Cards setBit(ProtocolCards cards);

void bitToArray(ProtocolCards cards, Cards bitCards);
void setSubmitCard(ProtocolCards submit, const Hand& h);

int maxStrength(Cards c,bool rev);
int minStrength(Cards c,bool rev);

Cards diffHand(Cards c,Hand &h);
