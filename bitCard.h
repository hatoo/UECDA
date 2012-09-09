#include "mydef.h"

const Cards JOKER = (1LL<<52);
const Cards SP3  = (1LL);

int bitCount(Cards cards);

Cards setBit(ProtocolCards cards);

void bitToArray(ProtocolCards cards, Cards bitCards);
void setSubmitCard(ProtocolCards submit, const Hand& h);

int maxStrength(Cards c,bool rev);
int minStrength(Cards c,bool rev);

Cards diffHand(Cards c,Hand &h);
