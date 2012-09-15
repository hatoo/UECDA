#include <vector>
#include "mydef.h"

using namespace std;
//Cards == int64
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
bool checkAllValidHands(fieldInfo& info, Cards myCards);

vector<Hand> minHandPair(Cards c);
/*最短ターンで上がる時の役の集合
 *今のところ枚数の多い役をとっていくだけ*/
int minHandPairNum(Cards c);//未実装
