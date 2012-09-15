#include <vector>
#include "mydef.h"

using namespace std;
//Cards == int64
vector<Hand> getAllValidHands(const fieldInfo& info,Cards myCards,bool strict=false);
/*strict==falseの場合 ジョーカーなしでその役を構成できる場合は
 * ジョーカーをあえて使ってその役を作ることをやめる
*/
vector<Hand> getGroup(Cards myCards,int num=0,int min_ord=0,
		bool rev = false,unsigned char suit=0,bool strict=false);
vector<Hand> getSeq(Cards myCards,int num=0,int min_ord=0,
		bool rev = false,unsigned char suit=0,bool strict=false);
void removeHands(vector<Hand>& hands,Cards submit,Cards myCards);
vector<Hand> generateAllHands(int64 myCards,bool strict=false);
//パス以外の行動を取れるか
bool checkAllValidHands(fieldInfo& info, Cards myCards);
