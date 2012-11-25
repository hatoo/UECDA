#include "machineLearn.h"
#include "mldefine.h"
#include "mldefine2012.h"
#include "mldefine10.h"
#include "mldefine5.h"
#include "myrandom.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>

using namespace std;

int main(int argc,char *argv[]){
	enum{Learn,GenTEXT,MontecarloLearn,Error} mode=GenTEXT;
	string output;
	string input;
	string initvec;
	double alpha = 10;
	MLdefine2012 mld2012;
	MLdefine10 mld10;
	MLdefine5  mld5;
	MLdefine *def=&mld2012;
	bool rev=false;
	int n=2500;

	int ch;
	while((ch=getopt(argc, argv,"o:l:gn:v:e:a:mt:r:"))!= -1){
		switch(ch){
		case 't':
			if(strcmp("mld2012",optarg)==0){
				cout << optarg << endl;
				def = &mld2012;
			}else
			if(strcmp("mld10",optarg)==0){
				cout << optarg << endl;
				def = &mld10;
				cout << mld10.size() << endl;
			}else
			if(strcmp("mld5",optarg)==0){
				cout << optarg << endl;
				def = &mld5;
				cout << def->size() << endl;
			}else{
				return 0;
			}
			break;
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
		case 'm':
			mode = MontecarloLearn;
			break;
		case 'r':
			if(string("true")==optarg){
				cout << "rev = true" << endl;
				rev = true;
			}else if(string("false")==optarg){
				cout << "rev = false" << endl;
				rev = false;
			}else{
				return 0;
			}
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
			uvector v = def->zero();
			if(initvec!=""){ v = load(initvec.c_str(),def->size());}

			for(int i=0;i<min((int)n,(int)tb.size());i++){
				cout << "i= " << i << endl;
				cout << "rev=" << (tb[i].s.info.rev?"true":"false") <<endl;
				v+=alpha* def->delta_theta(tb[i],v);//delta_theta<V>(tb[i],v);
				def->show(v);
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
			uvector v=def->zero();
			if(initvec!="")v=load(initvec.c_str(),def->size());
			double theta[def->size()];
			for(int i=0;i<def->size();i++)theta[i]=v[i];

			if(output=="")output="a.txt";
			vector<mlTEXT> textbook;
			for(int i=0;i<n;i++){
				std::cout << "i= " << i << std::endl;
				textbook.push_back(def->makeTEXT(theta,rev));
				if(i%10==9){
					saveTextBook(output.c_str(),textbook);
					std::cout << "saved" << std::endl;
				}
			}
			saveTextBook(output.c_str(),textbook);
			}
			break;
		case MontecarloLearn:
			{
				uvector v=def->zero();
				if(initvec!="")v=load(initvec.c_str(),def->size());
				for(int i=0;i<n;i++){
					cout <<  i << " times learned" << endl;
					v+=alpha* def->delta_theta(v,rev);
					def->show(v);
					if(i%10==9){
						save(output.c_str(),v);
						cout << "saved" << endl;
					}
				}
				save(output.c_str(),v);
			}
			break;
		case Error:
			{
				uvector v = def->zero();
				if(initvec!="")v=load(initvec.c_str(),def->size());
				std::vector<mlTEXT> tb = loadTextBook(input.c_str());
				double deltaN=0.0;
				const int N = min((int)n,(int)tb.size());
				for(int i=0;i<N;i++){
					deltaN+=def->error(tb[i],v);
					cout << i << endl;
				}
				cout << "average_error = " << deltaN/N << endl;
			break;
		}
	}

	return 0;
}
