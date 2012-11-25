#include "mydef.h"
#include "machineLearn.h"
#include "myrandom.h"
#include "bitCard.h"
#include "handGenerator.h"
#include "montecarlo.h"
#include <vector>
#include <cstring>
#include <fstream>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <cmath>

#pragma once
using namespace boost::numeric;
/*
class MLvector:public boost::numeric::ublas::vector<double>{
	virtual MLvector phi(const state& s,const action& a);
	double pi_theta_bunbo(const state& s);
	double pi_theta_bunsi(const state& s,const action& a);
	double pi_theta(const state& s,const action& a);
	MLvector 
public:
	virtual Hand AI(const state &s);//thetaは自分
	//プログラムからはこいつが呼ばれる
	virtual Hand AI(const fieldInfo &info,Cards myCards,Cards oppCards
			,const changeInfo &cinfo);
};*/
template<class V>
uvector zero(){
	uvector v(V::size);
	v.clear();
	return v;
}
/*
void savevec(const char* path,uvector a){
	std::vector<double> v;
	for(int i=0;i<a.size();i++){
		v.push_back(a[i]);
	}
	ofstream file(path);
	boost::archive::text_oarchive oa(file);
	oa << v;
}

uvector loadvec(const char* path,const int size){
	std::vector<double> data;
	std::ifstream file(path);
	if(!file.is_open()){
		uvector v(size);
		v.clear();
		savevec(path,v);
		return loadvec(path,size);
	}

	boost::archive::text_iarchive ia(file);
	ia >> data;

	uvector ret(size);
	for(int i=0;i<size;i++){
		ret[i]=data[i];
	}
	return ret;
}
*/

/*
 * class Vはconst int V::size...ベクトルのサイズ
 * V::phi(s,a)を定義していなければならない
 */
template<class V>
double pi_theta_bunsi(const state& s,const action &a,const uvector &theta){
	uvector v = V::phi(s,a);
	return exp(ublas::inner_prod(v,theta));
}

template<class V>
double pi_theta_bunbo(const state& s,const uvector &theta){
	std::vector<Hand> hs = getAllValidHands(s.info,s.myCards());
	double bunbo=0.0;
	for(int i=0;i<hs.size();i++){
		uvector v = V::phi(s,hs[i]);
		bunbo+=exp(ublas::inner_prod(v,theta));
	}
	return bunbo;
}

template<class V>
double pi_theta(const state& s,action &a,const uvector &theta){//使わない
	return pi_theta_bunsi<V>(s,a,theta)/pi_theta_bunbo<V>(s,theta);
}

template<class V>
uvector psi(const state &s,action &a,const uvector &theta){
	uvector p = V::phi(s,a);
	std::vector<Hand> hs = getAllValidHands(s.info,s.myCards());
	double bunbo = pi_theta_bunbo<V>(s,theta);
	uvector minus=zero<V>();

	for(int i=0;i<hs.size();i++){
		minus+=(pi_theta_bunsi<V>(s,hs[i],theta)/bunbo)*V::phi(s,hs[i]);
	}
	return p-minus;
}

template<class V>
Hand AI(const state &s,const uvector &theta){
	double bunbo = pi_theta_bunbo<V>(s,theta);
	std::vector<Hand> hs = getAllValidHands(s.info,s.myCards());
	
	const int tehuda = bitCount(s.myCards());
	for(int i=0;i<hs.size();i++){
		if(tehuda==hs[i].qty)return hs[i];
	}

	double r = randDouble(0.0,bunbo);
	for(int i=0;i<hs.size();i++){
		double ratio = pi_theta_bunsi<V>(s,hs[i],theta);
		r-=ratio;
		if(r<=0.0){
			return hs[i];
		}
	}
	//ここにはこないはず
	return hs.back();
}

template<class V>
uvector delta_theta(state s,uvector theta){
	double t_scores[5]={0.0};
	v_teacher(s,t_scores);
	mlTEXT txt;
	txt.s = s;
	memcpy(txt.answer,t_scores,sizeof t_scores);
	return delta_theta(txt,theta);
}

template<class V>
uvector delta_theta(mlTEXT text,uvector theta){
	double t_scores[5]={0.0};
	memcpy(t_scores,text.answer,sizeof t_scores);
	state s = text.s;

	uvector g[5];
	for(int i=0;i<5;i++){
		g[i]=zero<V>();//uvector(vector_size);
		//g[i].clear();
	}
	double vn[5]={0.0};

	const int N = 100;
	const int goalnum=bitCount(s.info.goal);

	for(int i=0;i<N;i++){
		fieldInfo info = s.info;
		int pos = s.cinfo.firstPlayer;
		Cards hands[5];
		memcpy(hands,s.hands,sizeof hands);

		int T[5]={0};
		uvector right[5];
		for(int i=0;i<5;i++){
			right[i]=zero<V>();//uvector(vector_size);
			//right[i].clear();
		}
		double z[5]={0.0};
		int goal = info.goal;
		while(bitCount(info.goal)!=5){
			state t;
			info.mypos = pos;
			t.info = info;
			memcpy(t.hands,hands,sizeof hands);

			t.cinfo=s.cinfo;
			Hand hand = AI<V>(t,theta);
			{
				T[pos]++;
				right[pos] += psi<V>(t,hand,theta);
			}

			hands[pos]=diffHand(hands[pos],hand);
			pos = info.simulate(hand,pos);
			if(goal!=info.goal){
				int gd = goal^info.goal;
				int t=0;
				for(t=0;t<5;t++){
					if(gd==(1<<t))break;
				}
				z[t]=rank2score(bitCount(goal),goalnum);
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
	uvector dtheta=zero<V>();//(vector_size);
	//dtheta.clear();
	for(int i=0;i<5;i++){
		dtheta+=(t_scores[i]-(vn[i]/N))*g[i];
	}
	return dtheta;
}
