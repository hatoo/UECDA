#include <set>
#include "mydef.h"

//if qty==0 then not win
void selectHand(ProtocolCards p,fieldInfo& info,Cards myCards,Cards oppCards);
bool isWIN(fieldInfo f,Cards myCards,Cards oppCards,int maxHandNum);
Hand winHand(fieldInfo info,Cards myCards,Cards oppCards,int maxHandNum,
		std::set<Cards> failed);
