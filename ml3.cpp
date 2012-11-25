#include "mydef.h"
#include "ml3.h"
#include <vector>
#include "machineLearn.h"
#include "mldefine.h"
#include "mldefine2012.h"

std::vector<double> vec3;

MLdefine *def;// = MLdefine2012();

void setMLdefine(MLdefine *mdf){
	def=mdf;
}

void initmlAI3(const char *fname){
	uvector v = load(fname,def->size());
	vec3.clear();
	for(int i=0;i<v.size();i++){
		vec3.push_back(v[i]);
	}
}
Hand mlAI3(const fieldInfo &info,Cards myCards,Cards oppCards){
	return def->optimizedAI(info,myCards,oppCards,&vec3[0]);
}
