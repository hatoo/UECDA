#include "mydef.h"
#include <functional>

#pragma once

//プレイアウトをしたときの自分の順位を返す大富豪-大貧民<-> 0-4
int playout(fieldInfo info,Hand h,Cards myCards,Cards oppCards
		,const changeInfo& cinfo);

Hand fastWeakAI(const fieldInfo &info,Cards myCards,Cards oppCards);

void DevideCards(fieldInfo info,Cards oppCards,Cards out[5]
		,const changeInfo& cinfo);

Hand montecalroSearch(fieldInfo info,Cards myCards,Cards oppCards
		,const changeInfo& cinfo);

Hand montecalroSearch_mt(fieldInfo info,Cards myCards,Cards oppCards
		,const changeInfo &cinfo);

Hand montecalroSearch_Functional(fieldInfo info,Cards myCards,Cards oppCards
		,const changeInfo &cinfo
		,const std::function<Hand(const fieldInfo&,Cards myCards,Cards oppCards,const changeInfo &cinfo)> &AI
		,const int playoutnum);

Hand DoubleMontecalroSearch(fieldInfo info,Cards myCards,Cards oppCards
		,const changeInfo &cinfo);

double rank2score(int rank,int goalnum,int prevrank);
