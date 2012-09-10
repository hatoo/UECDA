#include <iostream>
#include "mydef.h"
#include "handGenerator.h"
#include <vector>
#include "bitCard.h"
#include "myrandom.h"
#include "montecarlo.h"

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
	initRand();
	Cards allcard = (1LL<<40)-1;

	fieldInfo info;
	info.onset=false;
	info.qty=1;info.suit=2;info.ord=4;info.seq=false;
	info.rev=0;info.lock=false;info.mypos=0;
	info.lest[1]=10;
	info.lest[2]=10;
	info.lest[3]=10;
	info.lest[4]=10;
	Cards out[5];//={0};
	DevideCards(info,allcard,out);
	/*
	vector<Hand> r = getAllValidHands(info,allcard,true);
	for(int i=0;i<r.size();i++){
		show(r[i]);
	}
	cout << "size= " << r.size() << endl;
	*/
	//cout << checkAllValidHands(info,allcard)<<endl;
	for(int i=1;i<5;i++){
		showCards(out[i]);
	}
	cout << sizeof(Cards) << endl;
	return 0;
}
