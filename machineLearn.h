/* 参考文献 http://uecda.nishino-lab.jp/sympo/images/Suto.pdf */

#include "mydef.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/serialization/serialization.hpp>

#include <vector>

#pragma once

#define default_filepath "save.dat"

struct state{
	Cards myCards() const;
	fieldInfo info;
	changeInfo cinfo;
	Cards hands[5];
private:
	friend class boost::serialization::access;
	template<class Archive>
	  void serialize( Archive& ar, unsigned int ver )
	  {
		ar & info & cinfo & hands;
	  }
};

typedef Hand action;


const int vector_size = 512;

typedef boost::numeric::ublas::vector<double> uvector;

//typedef std::pair<state,double[5] > mlTEXT;
struct mlTEXT{
	state s;
	double answer[5];
private:
	friend class boost::serialization::access;
	template<class Archive>
	  void serialize( Archive& ar, unsigned int ver )
	  {
		ar & s & answer;
	  }
};



uvector load(const char* path,const int size);
void save(const char* path,uvector v);

std::vector<mlTEXT> loadTextBook(const char* path);
void saveTextBook(const char* path,std::vector<mlTEXT> v);

uvector phi(const state &s,action &a);
double pi_theta(const state& s,action &a,const uvector &theta);
uvector psi(const state &s,action &a,const uvector &theta);
Hand AI(const state &s,const uvector &theta);//use random

void initmlAI(const char *fname=default_filepath);
Hand mlAI(const fieldInfo &info,Cards myCards);

double v(state s);
void v_teacher(state s,double scores[5]);

double g(state s);

state createState(bool rev=false);

uvector delta_theta(state s,uvector theta);
uvector delta_theta(mlTEXT text,uvector theta);

mlTEXT makeTEXT(bool rev=false);
double errors(mlTEXT text,uvector theta);
//std::vector<Hand> getAIHand(Cards myCards);
//int getIndex(Hand &h,bool rev);
std::vector<Hand> getAIHand(Cards myCards);
