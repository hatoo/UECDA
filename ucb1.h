#include <cmath>

#pragma once

const int INF_SCORE = 1000;//0除算が発生するときはとりあえずこれを返す

template<class T>
class ucb1Record{
		double s;
		double s2;
		int n;
		public:
		void pushScore(double score);
		double x() const;
		double var2() const;
		T tag;
		ucb1Record(T t);
		ucb1Record(){}
		double ucb1_tuned(int nf) const;
		double ucb1(int nf) const;
};

template<class T>
ucb1Record<T>::ucb1Record(T t){
	tag=t;
	s=s2=0.0;
	n=0;
}

template<class T>
void ucb1Record<T>::pushScore(double score){
	s+=score;
	s2+=score*score;
	n++;
}

template<class T>
double ucb1Record<T>::x() const{
	if(n==0)return INF_SCORE;
	return s/n;
}

template<class T>
double ucb1Record<T>::var2() const{
	if(n==0)return INF_SCORE;
	return s2/n-(s*s)/(n*n);
}

template<class T>
double ucb1Record<T>::ucb1_tuned(int nf) const{
	if(n==0){return INF_SCORE;}
	double c1 = min(0.25,var2()+sqrt(2.0*log(nf)/n));
	double x1 = x() + sqrt(c1*log((double)nf)/n);
	return x1;
}

template<class T>
double ucb1Record<T>::ucb1(int nf) const{
	if(n==0){return INF_SCORE;}
	
	return x()+sqrt(2.0*log((double)nf)/n);
}
