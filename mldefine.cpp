#include "mldefine.h"
#include "mydef.h"
#include "machineLearn.h"
#include "myrandom.h"
#include "bitCard.h"
#include "handGenerator.h"
#include "montecarlo.h"
#include "cardSelect.h"
#include <vector>
#include <cstring>
#include <boost/numeric/ublas/vector.hpp>
#include <cmath>
#include <iostream>

using namespace boost::numeric;

uvector MLdefine::zero(){
	uvector v(size());
	v.clear();
	return v;
}

double MLdefine::pi_theta_bunsi(const state& s,const action &a,const uvector &theta){
	uvector v = phi(s,a);
	return exp(ublas::inner_prod(v,theta));
}

double MLdefine::pi_theta_bunbo(const state& s,const uvector &theta){
	std::vector<Hand> hs = getAllValidHands(s.info,s.myCards());
	double bunbo=0.0;
	for(int i=0;i<hs.size();i++){
		uvector v = phi(s,hs[i]);
		bunbo+=exp(ublas::inner_prod(v,theta));
	}
	return bunbo;
}

double MLdefine::pi_theta(const state& s,action &a,const uvector &theta){//使わない
	return pi_theta_bunsi(s,a,theta)/pi_theta_bunbo(s,theta);
}

uvector MLdefine::psi(const state &s,action &a,const uvector &theta){
	uvector p = phi(s,a);
	std::vector<Hand> hs = getAllValidHands(s.info,s.myCards());
	double bunbo = pi_theta_bunbo(s,theta);
	uvector minus=zero();

	for(int i=0;i<hs.size();i++){
		minus+=(pi_theta_bunsi(s,hs[i],theta)/bunbo)*phi(s,hs[i]);
	}
	return p-minus;
}

Hand MLdefine::AI(const state &s,const uvector &theta){
	if(!s.info.onset && !checkAllValidHands(s.info,s.myCards())){
		return Hand();
	}
	//double bunbo = pi_theta_bunbo(s,theta);
	std::vector<Hand> hs = getAllValidHands(s.info,s.myCards());
	
	const int tehuda = bitCount(s.myCards());

	double ratios[hs.size()];
	double sum=0.0;

	for(int i=0;i<hs.size();i++){
		if(tehuda==hs[i].qty)return hs[i];
		double r = pi_theta_bunsi(s,hs[i],theta);
		ratios[i]=r;
		sum+=r;
	}


	double r = randDouble(0.0,sum);
	for(int i=0;i<hs.size();i++){
		r-=ratios[i];
		if(r<=0.0){
			return hs[i];
		}
	}
	return hs.back();
}

uvector MLdefine::delta_theta(state s,const uvector &theta){
	double t_scores[5]={0.0};
	v_teacher(s,t_scores);
	mlTEXT txt;
	txt.s = s;
	memcpy(txt.answer,t_scores,sizeof t_scores);
	return delta_theta(txt,theta);
}

uvector MLdefine::delta_theta(mlTEXT text,const uvector &theta){
	double t_scores[5]={0.0};
	memcpy(t_scores,text.answer,sizeof t_scores);
	state s = text.s;

	uvector g[5];
	for(int i=0;i<5;i++){
		g[i]=zero();
	}
	double vn[5]={0.0};

	const int N = 100;
	const int goalnum=bitCount(s.info.goal);

	for(int i=0;i<N;i++){
		fieldInfo info = s.info;
		int prevrank = info.rank[info.mypos];
		int pos = s.cinfo.firstPlayer;
		Cards hands[5];
		memcpy(hands,s.hands,sizeof hands);

		int T[5]={0};
		uvector right[5];
		for(int i=0;i<5;i++){
			right[i]=zero();
		}
		double z[5]={0.0};
		int goal = info.goal;
		while(bitCount(info.goal)!=5){
			state t;
			info.mypos = pos;
			t.info = info;
			memcpy(t.hands,hands,sizeof hands);

			t.cinfo=s.cinfo;
			Hand hand = AI(t,theta);
			{
				T[pos]++;
				right[pos] += psi(t,hand,theta);
			}

			hands[pos]=diffHand(hands[pos],hand);
			pos = info.simulate(hand,pos);
			if(goal!=info.goal){
				int gd = goal^info.goal;
				int t=0;
				for(t=0;t<5;t++){
					if(gd==(1<<t))break;
				}
				z[t]=rank2score(bitCount(goal),goalnum,prevrank);
				goal = info.goal;
			}
		}
		for(int i=0;i<5;i++){
			vn[i]+=z[i];
			g[i]+=z[i]/(N*T[i])*right[i];
		}
	}
	
	for(int i=0;i<5;i++){
		cout << "v_a= "<<t_scores[i] << " v= "<<(vn[i]/N)<< " delta= " << t_scores[i]-(vn[i]/N) << endl;
	}
	uvector dtheta=zero();
	for(int i=0;i<5;i++){
		dtheta+=(t_scores[i]-(vn[i]/N))*g[i];
	}
	return dtheta;
}



uvector MLdefine::delta_theta(const uvector &theta,bool rev){
	double array[theta.size()];
	for(int i=0;i<theta.size();i++)array[i]=theta[i];
	mlTEXT txt;
	txt.s = createState(rev);
	montecarlo_teacher(txt.s,array,txt.answer,10);
	return delta_theta(txt,theta);
}

void MLdefine::montecarlo_teacher(const state &s,double *theta
			,double scores[5],const int simulate_num=10){

	auto MyAI=[theta,this](fieldInfo info,Cards myCards,Cards oppCards
			,const changeInfo &cinfo){

		int maxHandNum=info.maxOppHandNum();
		Hand h = winHand(info,myCards,oppCards,maxHandNum);
		if(h.qty!=0){
			return h;
		}
		if(!checkAllValidHands(info,myCards)){
			return Hand();
		}
		return montecalroSearch_Functional(info,myCards,oppCards,cinfo
				,[theta,this](const fieldInfo& info,Cards myCards,Cards oppCards,const changeInfo &)
				{return this->optimizedAI(info,myCards,oppCards,theta);},3000);
	};

	for(int i=0;i<5;i++)scores[i]=0.0;
	
	//const int simulate_num = 10;
	const int goalnum=bitCount(s.info.goal);

	for(int i=0;i<simulate_num;i++){
		fieldInfo info = s.info;
		int prevrank = info.rank[info.mypos];
		int pos = s.cinfo.firstPlayer;
		Cards hands[5];
		memcpy(hands,s.hands,sizeof hands);

		Cards oppCards=0;
		for(int i=0;i<5;i++)oppCards|=hands[i];

		int goal = info.goal;
		while(bitCount(info.goal)<4){
			info.mypos = pos;
			Hand hand = MyAI(info,hands[pos],oppCards^hands[pos],s.cinfo);//selectHand_(info,hands[pos],oppCards^hands[pos],s.cinfo);
			
			hands[pos]=diffHand(hands[pos],hand);
			oppCards = diffHand(oppCards,hand);
			pos = info.simulate(hand,pos);
			if(goal!=info.goal){
				int gd = goal^info.goal;
				int t=0;
				for(t=0;t<5;t++){
					if(gd==(1<<t))break;
				}
				scores[t]+=rank2score(bitCount(goal),goalnum,prevrank)/simulate_num;
				goal = info.goal;
			}
		}

		std::cout << "sensei's score : ";
		for(int i=0;i<5;i++){
			std::cout << scores[i] << " ";
		}
		std::cout << std::endl;
	}
}

double MLdefine::error(mlTEXT text,uvector theta,const int N){
	double t_scores[5]={0.0};
	memcpy(t_scores,text.answer,sizeof t_scores);
	state s = text.s;

	double vn[5]={0.0};

	const int goalnum=bitCount(s.info.goal);

	for(int i=0;i<N;i++){
		fieldInfo info = s.info;
		int prevrank = info.rank[info.mypos];
		int pos = s.cinfo.firstPlayer;
		Cards hands[5];
		memcpy(hands,s.hands,sizeof hands);

		double z[5]={0.0};
		int goal = info.goal;
		while(bitCount(info.goal)!=5){
			state t;
			info.mypos = pos;
			t.info = info;
			memcpy(t.hands,hands,sizeof hands);

			t.cinfo=s.cinfo;
			Hand hand = AI(t,theta);

			hands[pos]=diffHand(hands[pos],hand);
			pos = info.simulate(hand,pos);
			if(goal!=info.goal){
				int gd = goal^info.goal;
				int t=0;
				for(t=0;t<5;t++){
					if(gd==(1<<t))break;
				}
				z[t]=rank2score(bitCount(goal),goalnum,prevrank);
				goal = info.goal;
			}
		}
		for(int i=0;i<5;i++){
			vn[i]+=z[i];
		}
	}
	
	double delta=0.0;
	for(int i=0;i<5;i++){
		double v = vn[i]/N;
		delta+=(t_scores[i]-v)*(t_scores[i]-v);
	}

	return delta;

}

mlTEXT MLdefine::makeTEXT(double *theta,bool rev){
	mlTEXT text;
	text.s=createState(rev);
	montecarlo_teacher(text.s,theta,text.answer,10);
	return text;
}

void MLdefine::show(const uvector &theta){}
