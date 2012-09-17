#include "mydef.h"

#pragma once

//プレイアウトをしたときの自分の順位を返す大富豪-大貧民<-> 0-4
int playout(fieldInfo info,Hand h,Cards myCards,Cards oppCards);

Hand fastWeakAI(const fieldInfo &info,Cards myCards,Cards oppCards);

void DevideCards(fieldInfo info,Cards oppCards,Cards out[5]);

Hand montecalroSearch(fieldInfo info,Cards myCards,Cards oppCards);
