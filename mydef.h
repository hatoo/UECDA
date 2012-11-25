//#include <vector>
#include <boost/serialization/serialization.hpp>
using namespace std;

#pragma once

#define NDEBUG

#define SEARCH_NUM 3000

//カードの集合 公式プログラムとは違いスペード3,ハート3,ダイヤ3,クローバー3,スペード4.....の順番になっている

typedef unsigned long long int int64;
typedef int64 Cards;

//プロトコルで使うやつ
typedef int ProtocolCards[8][15];


// 手の情報を保持する構造体
//const Cards NO_JOKER=0;
struct Hand{
	Cards hands;//ジョーカー以外のカードの集合
	unsigned char qty;   // カード枚数
	bool seq;   // 階段かどうか
	unsigned char ord;   // 強さ(通常時で一番弱い)
	unsigned char suit;  // マーク
	bool jokerUsed() const;
	bool rev() const;
	Hand(){
		//pass
		hands=0uLL;
		qty=0;
		seq=false;
		ord=0;
		suit=0;
	};
};

//typedef vector<Hand> Hands;

struct fieldInfo{
	bool onset; // 開始状態かどうか、場にカードが出ているか
	int qty;   // 場に出ているカードの枚数
	int suit;  // 場に出ているカードのマーク
	int ord;   // 場に出ているカードの強さ(役の中で一番弱い(通常時)カードの強さ)
	bool seq;   // 場に出ているカードが階段か
	bool lock;  // しばりが発生しているか
	bool rev;   // 革命が発生しているか
	int pass;  // 現在パスしているプレイヤーの集合
	int goal;  // 既に上がったプレイヤーの集合

	int mypos;   // 自プレイヤーの席
	int seat[5]; // seat[i] : プレイヤーiが座っている席
	int lest[5]; // lest[i] : 席iに座っているプレイヤーの残りカード枚数
	int rank[5]; // rank[i] : 席iに座っているプレイヤーの階級 0-4->大富豪-大貧民
	void set(int my_num,ProtocolCards& p);
	void set_ba(ProtocolCards& cards);
	bool SingleJoker() const;
	void submit(const Hand &h);
	int simulate(const Hand &h,int pos);//次にカードをだす権利のある人を返す
	void reset();//revは保持する
	int maxOppHandNum() const;
private:
	friend class boost::serialization::access;
	template<class Archive>
	  void serialize( Archive& ar, unsigned int ver )
	  {
		ar & onset & qty & suit & ord & seq & lock & rev & pass & goal &
			mypos & seat & lest & rank;
	  }
};

struct changeInfo{
	int oppPos;      // 交換相手の席
	int64 hasCards;  // 交換相手が絶対に持っているカード
	int64 notCards;  // 交換相手が持ち得ないカード
	int firstPlayer; // 最初に行動したプレイヤー(ダイヤの3を所持)
	void set(Cards before,Cards after,fieldInfo info);
private:
	friend class boost::serialization::access;
	template<class Archive>
	  void serialize( Archive& ar, unsigned int ver )
	  {
		ar & oppPos & hasCards & notCards & firstPlayer;
	  }
};
