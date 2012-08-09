#include <vector>
#include "mydef.h"

using namespace std;
//Cards == int64
vector<Hand> getAllValidHands(const fieldInfo& info,Cards myCards);

vector<Hand> getGroup(Cards myCards,int num=0,int min_ord=0,
		bool rev = false,unsigned char suit=0);

vector<Hand> getSeq(Cards myCards,int num=0,int min_ord=0,
		bool rev=false,unsigned char suit=0);

void removeHands(vector<Hand>& hands,Cards submit,Cards myCards);

vector<Hand> generateAllHands(int64 myCards);

//void generateGroup(bitValidHandsArray *vha, int64 myCards);

//パス以外の行動を取れるか
bool checkAllValidHands(const fieldInfo& info, int64 myCards);
