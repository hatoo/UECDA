#include "mydef.h"
#include "machineLearn.h"
#include <vector>

#pragma once

class MLdefine{
public:
	virtual int size()=0;//{return 5;}
	virtual uvector phi(const state &s,const action &a)=0;
	virtual Hand optimizedAI(const fieldInfo &,Cards myCards,Cards oppCards
			,double *theta)=0;
	uvector zero();
	double pi_theta_bunsi(const state& s,const action &a,const uvector &theta);
	double pi_theta_bunbo(const state& s,const uvector &theta);
	double pi_theta(const state& s,action &a,const uvector &theta);
	uvector psi(const state &s,action &a,const uvector &theta);
	Hand AI(const state &s,const uvector &theta);
	uvector delta_theta(state s,const uvector &theta);
	uvector delta_theta(mlTEXT text,const uvector &theta);
	uvector delta_theta(const uvector &theta,bool rev = false);
	void montecarlo_teacher(const state &s,double *theta
			,double scores[5],const int simulate_num);
	double error(mlTEXT text,uvector theta,const int N=100);
	mlTEXT makeTEXT(double *theta,bool rev = false);
	virtual void show(const uvector &theta);
	virtual uvector merge(uvector notrev,uvector rev){return notrev;}
};
