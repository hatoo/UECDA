#include <iostream>
#include "mydef.h"
#include "handGenerator.h"
#include <vector>
#include "bitCard.h"
#include "mydebug.h"

using namespace std;

void dumpCards(Cards c){
	for(int ord=0;ord<13;ord++){
		const int cnum[] = {3,4,5,6,7,8,9,10,11,12,13,1,2};
		const char suit[] = "SHDC";
		for(int s=0;s<4;s++){
			if(c>>(4*ord+s)&1){
				cerr << cnum[ord] << suit[s] << " ";
			}
		}
	}
	if(c&JOKER)cerr << "JOKER";
	cerr << endl;
}

void dumpHand(Hand h){
	cerr << "cards= ";
	dumpCards(h.hands);
	cerr << "qty= " << (int)h.qty << endl;
	cerr << "seq= " << h.seq << endl;
	cerr << "ord= " << (int)h.ord << endl;
	cerr << "suit=" << (int)h.suit<< endl;
	cerr << endl;
}

void dumpFinfo(fieldInfo info){
	cerr << "onset= " << info.onset << endl;
	cerr << "seq= " << info.seq << endl;
	cerr << "qty= " << (int)info.qty << endl;
	cerr << "ord= " << (int)info.ord << endl;
	cerr << "suit=" << (int)info.suit<< endl;
	cerr << "lock=" << info.lock << endl;
	cerr << "rev= " << info.rev << endl;
	cerr << endl;

}
