//#include "mlvector2010.h"
#include "mlvector2012.h"
#include "mlAlgorithm.h"
#include "ml3.h"
#include "mldefine2012.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <unistd.h>

using namespace std;

int main(int argc,char *argv[]){
	enum{Learn,GenTEXT,Error} mode=GenTEXT;
	string output;
	string input;
	string initvec;
	double alpha = 10;

	typedef mlvector2012 V;

	int n=2500;

	int ch;
	while((ch=getopt(argc, argv,"o:l:gn:v:e:a:"))!= -1){
		switch(ch){
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
		case 'a':
			alpha=atof(optarg);
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
			uvector v = load(initvec==""?output.c_str():initvec.c_str()
					,V::size);

			for(int i=0;i<min((int)n,(int)tb.size());i++){
				cout << "i= " << i << endl;
				v+=alpha*delta_theta<V>(tb[i],v);

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
			MLdefine2012 def;
			setMLdefine(&def);
			initmlAI3(initvec.c_str());
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
			/*{
				V v=V::zero();//(vector_size);v.clear();
				if(initvec!="")v=loadvec<V>(initvec.c_str());
				std::vector<mlTEXT> tb = loadTextBook(input.c_str());
				double deltaN=0.0;
				const int N = min((int)n,(int)tb.size());
				for(int i=0;i<N;i++){
					deltaN+=errors(tb[i],v);
					cout << i << endl;
				}
				cout << "average_error = " << deltaN/N << endl;
			}*/
			break;
	}
	return 0;
}
