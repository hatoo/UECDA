#include "mydef.h"

#pragma once

//プレイアウトをしたときの自分の順位を返す大富豪-大貧民<-> 0-4
int playout(fieldInfo info,Hand h,Cards myCards,Cards oppCards);

Hand fastWeakAI(fieldInfo &info,Cards myCards);

void DevideCards(fieldInfo info,Cards oppCards,Cards out[5]);
