#include "machineLearn.h"
#include "mydef.h"
#include "handGenerator.h"
#include "myrandom.h"
#include "montecarlo.h"
#include "cardSelect.h"
#include "bitCard.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>

#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/serialization/serialization.hpp>

#include <fstream>
#include <boost/archive/text_oarchive.hpp> // テキスト形式アーカイブに書き込み
#include <boost/archive/text_iarchive.hpp> // テキスト形式アーカイブから読み込み

#include <iostream>
#include "mydebug.h"

#include <unistd.h>
#include <string>

using namespace std;

int main(int argc,char *argv[]){
	enum{Learn,GenTEXT,Error} mode=GenTEXT;
	string output;
	string input;
	string initvec;
	int n=2500;

	int ch;
	while((ch=getopt(argc, argv,"o:l:gn:v:e:"))!= -1){
	switch (ch){
		case 'o':
			if(optarg){
				cout << optarg << endl;
				output=optarg;
			}
			break;
		case 'l':
			mode=Learn;
			input = optarg;
			break;
		case 'g':
			mode=GenTEXT;
			break;
		case 'n':
			n=atoi(optarg);
			break;
		case 'v':
			initvec=optarg;
			break;
		case 'e':
			input=optarg;
			mode = Error;
			break;
		default:
			return 0;
	}
	}
	initRand();

	switch(mode){
		case Learn:
			{
			std::vector<mlTEXT> tb = loadTextBook(input.c_str());
			if(output=="")output="a.dat";
			uvector v = load(initvec==""?output.c_str():initvec.c_str());

			for(int i=0;i<min((int)n,(int)tb.size());i++){
				cout << "i= " << i << endl;
				v+=10.0*delta_theta(tb[i],v);

				for(int q=0;q<14;q++){
					cout << q << " : ";
					for(int k=0;k<4;k++){
						cout << exp(v[4*q+k]) << " ";
					}
					cout << endl;
				}
				if(i%10==9){
					save(output.c_str(),v);
					cout << "saved" << endl;
				}
			}
			save(output.c_str(),v);
			}
			break;
		case GenTEXT:
			{
			if(output=="")output="a.txt";
			vector<mlTEXT> textbook;
			for(int i=0;i<n;i++){
				std::cout << "i= " << i << std::endl;
				textbook.push_back(makeTEXT());
				if(i%10==9){
					saveTextBook(output.c_str(),textbook);
					std::cout << "saved" << std::endl;
				}
			}
			saveTextBook(output.c_str(),textbook);
			}
			break;
		case Error:
			{
				uvector v(vector_size);v.clear();
				if(initvec!="")v=load(initvec.c_str());
				std::vector<mlTEXT> tb = loadTextBook(input.c_str());
				double deltaN=0.0;
				const int N = min((int)n,(int)tb.size());
				for(int i=0;i<N;i++){
					deltaN+=errors(tb[i],v);
					cout << i << endl;
				}
				cout << "average_error = " << deltaN/N << endl;
			}
			break;
	}

	return 0;
}
