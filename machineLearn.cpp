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
#include <boost/serialization/vector.hpp>

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <iostream>
#include "mydebug.h"
#include <cstring>

#include <boost/foreach.hpp>

//#include <boost/thread.hpp>
#include <boost/bind.hpp>

//#include "mlAlgorithm.h"
//
#undef NDEBUG
#include <cassert>
#define NDEBUG


using namespace boost::numeric;

/*
struct Serialize_data{
	double array[vector_size];

private:
	friend class boost::serialization::access;
	template<class Archive>
	  void serialize( Archive& ar, unsigned int ver )
	  {
		ar & array;
	  }
};*/
/*
uvector load(const char* path){
	uvector v(vector_size);
	v.clear();
	Serialize_data data;

	std::ifstream file(path);
	if(!file.is_open()){save(path,v);return load(path);}
	boost::archive::text_iarchive ia(file);
	ia >> data;
	for(int i=0;i<vector_size;i++){
		v[i] = data.array[i];
	}
	return v;
}

void save(const char* path,uvector v){
	Serialize_data data;

	for(int i=0;i<vector_size;i++){
		data.array[i]=v[i];
	}

	ofstream file(path);
	boost::archive::text_oarchive oa(file);
	oa << data;
}*/

void save(const char* path,uvector a){
	std::vector<double> v;
	for(int i=0;i<a.size();i++){
		v.push_back(a[i]);
	}
	ofstream file(path);
	boost::archive::text_oarchive oa(file);
	oa << v;
}

uvector load(const char* path,const int size = 512){
	std::vector<double> data;
	std::ifstream file(path);
	if(!file.is_open()){
		uvector v(size);
		v.clear();
		save(path,v);
		return load(path,size);
	}

	boost::archive::text_iarchive ia(file);
	ia >> data;

	assert(data.size()==size);

	uvector ret(size);
	for(int i=0;i<size;i++){
		ret[i]=data[i];
	}
	return ret;
}

std::vector<mlTEXT> loadTextBook(const char* path){
	std::vector<mlTEXT> text;
	std::ifstream file(path);

	boost::archive::text_iarchive ia(file);
	ia >> text;

	return text;
}

void saveTextBook(const char* path,std::vector<mlTEXT> v){
	ofstream file(path);
	boost::archive::text_oarchive oa(file);
	oa << v;
}

Cards state::myCards() const{
	return hands[info.mypos];
}



inline int getIndex(const Hand &h,bool rev){
	/* メモリ配置
	 * 通常時用:0~
	 * 革命時用:256~
	 * 256の内訳
	 * グループ:(枚数)1~4 * ord  = 4*14 = 56
	 * 階段    :(枚数)3~6 * ord  = 4*13 = 52
	 * 他は使わない
	 */
	if(h.rev())rev=!rev;
	int res=0;
	int ord = h.qty==1&&h.jokerUsed()?13:h.ord;//革命時はジョーカ単体のordが-1になるので
	//if(ord==13)cout<<"aa" << endl;
	if(rev)res+=256;
	if(h.qty==0){
		//res += 56+52;
	}else
	if(h.seq){
		res += 56+4*ord+min(6,(int)h.qty)-3;
	}else{
		res += 4*ord+min(4,(int)h.qty)-1;
	}
	return res;
}

std::vector<Hand> getAIHand(Cards myCards){
	std::vector<Hand> res;
	res.reserve(24);

	for(int i=0;i<4;i++){
		const Cards suit = 1uLL<<i;
		Cards seq = 0uLL;
		int count = 0;
		int ord=-1;
		//bool joker_used = !(myCards&JOKER);
		for(int k=0;k<13;k++){
			if(myCards&(suit<<(4*k))){
				if(ord== -1)ord=k;
				seq|=(suit<<(4*k));
				count++;
			}else{
				if(count>=3){
					Hand h;
					h.seq=true;
					h.ord=ord;
					h.hands=seq;
					h.qty=count;
					h.suit=suit;
					res.push_back(h);
				}
				count=0;
				ord=-1;
				seq=0uLL;
			}
		}
		if(count>=3){
			Hand h;
			h.seq=true;
			h.ord=ord;
			h.hands=seq;
			h.qty=count;
			h.suit=suit;
			res.push_back(h);
		}
	}

	for(int i=0;i<13;i++){
		Cards c = myCards&(0xFLL<<(4*i));
		if((myCards>>(4*i)) ==0)break;
		if(c!=0LL){
			Hand h;
			h.hands = c;
			h.qty = bitCount(c);
			h.ord = i;
			h.seq = false;
			h.suit = c>>(4*i);
			res.push_back(h);
		}
	}
	if(myCards&JOKER){
		Hand h;
		h.hands = 0LL;
		h.qty=1;
		h.ord=13;
		h.seq=false;
		h.suit=0;
		res.push_back(h);
	}
	return res;
}


uvector phi(const state &s,action &a){
	uvector v(vector_size);
	v.clear();
	Cards newHand = diffHand(s.myCards(),a);
	std::vector<Hand> hs = getAIHand(newHand);
	for(int i=0;i<hs.size();i++){
		v[getIndex(hs[i],s.info.rev)]=1.0;
	}
	
	return v;
}

double pi_theta_bunsi(const state& s,action &a,const uvector &theta){
	uvector v = phi(s,a);
	double bunsi = exp(ublas::inner_prod/*内積*/(v,theta));
	return bunsi;
}

double pi_theta_bunbo(const state& s,const uvector &theta){
	std::vector<Hand> hs = getAllValidHands(s.info,s.myCards());
	double bunbo=0.0;
	for(int i=0;i<hs.size();i++){
		uvector v = phi(s,hs[i]);
		bunbo+=exp(ublas::inner_prod(v,theta));
	}
	return bunbo;
}

double pi_theta(const state& s,action &a,const uvector &theta){//使わない
	return pi_theta_bunsi(s,a,theta)/pi_theta_bunbo(s,theta);
}

uvector psi(const state &s,action &a,const uvector &theta){
	uvector p = phi(s,a);
	std::vector<Hand> hs = getAllValidHands(s.info,s.myCards());
	double bunbo = pi_theta_bunbo(s,theta);
	uvector minus(vector_size);
	minus.clear();
	for(int i=0;i<hs.size();i++){
		minus+=(pi_theta_bunsi(s,hs[i],theta)/bunbo)*phi(s,hs[i]);
	}
	return p - minus;
}

Hand AI(const state &s,const uvector &theta){
	double bunbo = pi_theta_bunbo(s,theta);
	std::vector<Hand> hs = getAllValidHands(s.info,s.myCards());
	
	const int tehuda = bitCount(s.myCards());
	for(int i=0;i<hs.size();i++){
		if(tehuda==hs[i].qty)return hs[i];
	}

	double r = randDouble(0.0,bunbo);
	for(int i=0;i<hs.size();i++){
		double ratio = pi_theta_bunsi(s,hs[i],theta);
		r-=ratio;
		if(r<=0.0){
			return hs[i];
		}
	}
	//ここにはこないはず
	std::cout << "aaaaa" << std::endl;
	return hs.back();
}

void v_teacher(state s,double scores[5]){//先生の形勢判断
	for(int i=0;i<5;i++)scores[i]=0.0;
	
	const int simulate_num = 10;
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
			Hand hand = selectHand_(info,hands[pos],oppCards^hands[pos],s.cinfo);
			
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

void DevideCardsALL(int start,Cards out[5]){
	std::vector<int> yama;
	for(int i=0;i<53;i++){
		yama.push_back(i);
	}
	std::random_shuffle(yama.begin(),yama.end()
			,[](int high){return randInt(0,high-1);});
	for(auto it=yama.begin();it!=yama.end();it++){
		out[start] |= (1LL) << *it;
		start++;
		start%=5;
	}
}

Cards check(Cards c,int qty){
	//仮実装
	Cards out = 0uLL;
	int k = 1;
	for(int i=0;i<qty;i++){
		while(!(c&(1uLL<<k)))k++;
		out |= 1uLL<<k;
		k++;
	}
	return out;
}

Cards topN(Cards c,int qty){
	Cards out = 0uLL;
	int k = 52;
	for(int i=0;i<qty;i++){
		while(!(c&(1uLL<<k)))k--;
		out |= 1uLL<<k;
		k--;
	}
	return out;
}

void exchange(Cards &upper,Cards &lower,int qty){
	Cards l = topN(lower,qty);
	lower ^= l;
	upper |= l;
	Cards h = check(upper,qty);
	upper ^= h;
	lower |= h;
}

state createState(bool rev){
	state s;
	Cards out[5] = {0};
	Cards rank[5] = {0,1,2,3,4};
	random_shuffle(rank,rank+5,[](int i){return randInt(0,i-1);});
	//面倒なのでcinfo関係は今のところ未実装
	changeInfo cinfo;cinfo.oppPos=0;cinfo.hasCards=0;cinfo.notCards=0;
	int start = randInt(0,4);
	int mypos = randInt(0,4);
	DevideCardsALL(start,out);
	//memcpy(s.hands,out,sizeof out);

	s.info.reset();
	s.info.rev=rev;
	s.info.goal=0;
	s.info.mypos = mypos;
	
	//exchange(out[rank[0]],out[rank[4]],2);
	//exchange(out[rank[1]],out[rank[3]],1);

	for(int i=0;i<5;i++){
		s.hands[i]=out[i];
		s.info.lest[i] = bitCount(out[i]);
		s.info.rank[rank[i]]=2;//i;
		if(out[i]&DIA3){cinfo.firstPlayer = i;}
	}
	s.cinfo = cinfo;
	
	int t=0;
	for(int i=0;i<5;i++){
		t+=bitCount(s.hands[i]);
	}
	cout << t << endl;

	return s;
}

uvector delta_theta(state s,uvector theta){
	
	
	double t_scores[5]={0.0};
	v_teacher(s,t_scores);
	mlTEXT txt;
	txt.s = s;
	memcpy(txt.answer,t_scores,sizeof t_scores);
	return delta_theta(txt,theta);
}

uvector delta_theta(mlTEXT text,uvector theta){
	double t_scores[5]={0.0};
	memcpy(t_scores,text.answer,sizeof t_scores);
	state s = text.s;

	uvector g[5];
	for(int i=0;i<5;i++){
		g[i]=uvector(vector_size);
		g[i].clear();
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
			right[i]=uvector(vector_size);
			right[i].clear();
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
	uvector dtheta(vector_size);
	dtheta.clear();
	for(int i=0;i<5;i++){
		dtheta+=(t_scores[i]-(vn[i]/N))*g[i];
	}
	return dtheta;
}

uvector loaded_theta;
double myarray[vector_size];

void initmlAI(const char *fname){
	loaded_theta=load(fname);
	for(int i=0;i<vector_size;i++){
		myarray[i]=loaded_theta[i];
	}
}

inline double calcScore_fast(Cards _myCards,const Hand &h,bool rev,/*const uvector &*/const double theta[vector_size]){
	double res=0.0;
	if(h.rev())rev=!rev;
	Cards myCards = diffHand(_myCards,h);

	/* メモリ配置
	 * 通常時用:0~
	 * 革命時用:256~
	 * 256の内訳
	 * グループ:(枚数)1~4 * ord  = 4*14 = 56
	 * 階段    :(枚数)3~6 * ord  = 4*13 = 52
	 * 他は使わない
	 */
	auto p=[=](bool seq,int ord,int qty){
		return theta[(rev?256:0)+(seq?56:0)+(4*ord)+(seq?qty-3:qty-1)];
	};

	for(int i=0;i<4;i++){
		const Cards suit = 1uLL<<i;
		Cards seq = 0uLL;
		int count = 0;
		int ord=-1;
		//bool joker_used = !(myCards&JOKER);
		for(int k=0;k<13;k++){
			if(myCards&(suit<<(4*k))){
				if(ord== -1)ord=k;
				seq|=(suit<<(4*k));
				count++;
			}else{
				if(count>=3){
					res+=p(true,ord,count);
				}
				count=0;
				ord=-1;
				seq=0uLL;
			}
		}
		if(count>=3){
			res+=p(true,ord,count);
		}
	}

	for(int i=0;i<13;i++){
		Cards c = myCards&(0xFLL<<(4*i));
		if((myCards>>(4*i))==0)break;
		if(c!=0LL){
			res+=p(false,i,bitCount(c));
		}
	}
	if(myCards&JOKER){
		res+=p(false,13,1);
	}

	return exp(res);
}

Hand mlAI(const fieldInfo &info,Cards myCards){
	if(!info.onset && !checkAllValidHands(info,myCards)){
		Hand h;h.qty=0;
		h.hands=0;
		return h;
	}
	//場に何もカードがないときはグループで出せる時にあえて単体で出す
	//理由がない（と思う）のでgetAIHandにして高速化している
	std::vector<Hand> hs = 
		info.onset?getAIHand(myCards):getAllValidHands(info,myCards);
	if(hs.size()==1)return hs[0];
	const int tehuda = bitCount(myCards);

	double sum=0.0;
	double ratios[hs.size()];
	const int loopnum=hs.size();

	//#pragma omp parallel num_threads(8)
	//#pragma omp parallel for reduction(+:sum)
	for(int i=0;i<loopnum;i++){
		if(tehuda==hs[i].qty)return hs[i];
		const double ratio = calcScore_fast(myCards,hs[i],info.rev,myarray);//loaded_theta);
		sum+=ratio;
		ratios[i]=ratio;
	}

	double r = randDouble(0.0,sum);
	for(int i=0;i<hs.size();i++){
		r-=ratios[i];
		if(r<=0.0)return hs[i];
	}
	return hs.back();
}


mlTEXT makeTEXT(bool rev){
	double scores[5]={0.0};
	state s=createState(rev);

	for(int k=0;k<5;k++){
		dumpCards(s.hands[k]);
	}
	v_teacher(s,scores);
	mlTEXT res;
	res.s=s;
	memcpy(res.answer,scores,sizeof scores);
	return res;
}

double errors(mlTEXT text,uvector theta){
	double t_scores[5]={0.0};
	memcpy(t_scores,text.answer,sizeof t_scores);
	state s = text.s;

	double vn[5]={0.0};

	const int N = 100;
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
