#include <iostream>
#include "mydef.h"
#include "handGenerator.h"
#include <vector>
#include "bitCard.h"

using namespace std;

void showCards(Cards c){
	for(int ord=0;ord<13;ord++){
		const int cnum[] = {3,4,5,6,7,8,9,10,11,12,13,1,2};
		const char suit[] = "SHDC";
		for(int s=0;s<4;s++){
			if(c>>(4*ord+s)&1){
				cout << cnum[ord] << suit[s] << " ";
			}
		}
	}
	if(c&JOKER)cout << "JOKER";
	cout << endl;
}

void show(Hand h){
	cout << "cards= ";
	showCards(h.hands);
	cout << "qty= " << (int)h.qty << endl;
	cout << "seq= " << h.seq << endl;
	cout << "ord= " << (int)h.ord << endl;
	cout << "suit=" << (int)h.suit<< endl;
	cout << endl;
}

int main(){
	Cards allcard =(1LL<<53)-1;
	fieldInfo info;
	info.onset=true;
	info.qty=2;info.suit=6;info.ord=2;info.seq=false;
	info.rev=1;info.lock=true;
	vector<Hand> r = getAllValidHands(info,allcard,true);
	for(int i=0;i<r.size();i++){
		show(r[i]);
	}
	cout << "size= " << r.size() << endl;

	return 0;
}
