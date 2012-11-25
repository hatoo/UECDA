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
	MLdefine2012 mld2012;
	MLdefine10 mld10;
	MLdefine5 mld5;
	MLdefine *def=0;
	string notrev;
	string rev;
	string output;
	int ch;
	while((ch=getopt(argc, argv,"t:n:r:o:"))!= -1){
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
			case 'n':
				notrev=optarg;
				break;
			case 'r':
				rev=optarg;
				break;
			case 'o':
				output=optarg;
				break;
		}
	}

	if(def==0 || notrev=="" || rev=="" || output=="")return 0;
	cout << "merge " << notrev << " and " << rev << endl;
	uvector nr=load(notrev.c_str(),def->size());
	uvector r=load(rev.c_str(),def->size());

	save(output.c_str(),def->merge(nr,r));
	return 0;
}
