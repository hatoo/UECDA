#include <set>
#include "mydef.h"

#pragma once

enum AImode{MLAI,FWAI};

void setMode(AImode m);

Hand selectHand_(fieldInfo info,Cards myCards,Cards oppCards,
		const changeInfo cinfo);

void selectHand(ProtocolCards p,fieldInfo& info,Cards myCards,Cards oppCards,const changeInfo& cinfo);
bool isWIN(fieldInfo f,Cards myCards,Cards oppCards,int maxHandNum,std::set<Cards>& failed);
Hand winHand(fieldInfo info,Cards myCards,Cards oppCards,int maxHandNum);

