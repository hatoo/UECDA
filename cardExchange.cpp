#include "cardExchange.h"
#include "bitCard.h"
#include "handGenerator.h"

void checkCards(ProtocolCards select_cards, ProtocolCards my_cards, int select_qty){
	Cards myCards = setBit(my_cards);
	auto hands = generateAllHands(myCards);
	vector<int> lessFive;
	for(int i=1/* スペードの３は絶対に渡さない */;lessFive.size()<=5;i++){
		if(i!=)
	}
}
