#include <vector>
#include "mydef.h"

//#include "bitCard.h"

#pragma once

using namespace std;

inline Cards MASK(int ord,bool rev){//ordよりも強いカードの集合
	return rev?(1LL<<(4+ord*4))-1:((1LL<<((13-ord)*4))-1)<<(ord*4);
}

vector<Hand> insertJoker(Hand h);
//Cards == int64

/*
 * 場が流れたときに3356と持っているとき、あえて3単体でだすよりも33と
 * だした方が多くの場合いいのでそういう手を列挙する
 */
vector<Hand> getOnsetHands(Cards myCard);

vector<Hand> getAllValidHands(const fieldInfo& info,Cards myCards,bool strict=false);
/*strict==falseの場合 ジョーカーなしでその役を構成できる場合は
 * ジョーカーをあえて使ってその役を作ることをやめる
 * 使っている箇所はない
*/
vector<Hand> getGroup(Cards myCards,int num=0,int min_ord=0,
		bool rev = false,unsigned char suit=0,bool strict=false);

vector<Hand> getSeq(Cards myCards,int num=0,int min_ord=0,
		bool rev = false,unsigned char suit=0,bool strict=false);
void removeHands(vector<Hand>& hands,Cards submit,Cards myCards);
vector<Hand> generateAllHands(int64 myCards,bool strict=false);//パスは含まない
//パス以外の行動を取れるか
bool checkAllValidHands(const fieldInfo& info, Cards myCards);

bool isBecomeOnset(fieldInfo info,const Hand &h,Cards oppCards);

vector<Hand> minHandPair(Cards c);
/*最短ターンで上がる時の役の集合
 *今のところ枚数の多い役をとっていくだけ*/
int minHandPairNum(Cards c);//未実装
