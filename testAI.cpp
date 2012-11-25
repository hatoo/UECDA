#include "mydef.h"
#include "handGenerator.h"
#include "myrandom.h"
#include "bitCard.h"

#include <cmath>
#include <vector>

using namespace std;

const double param[166] = {
	// for Normal
	// 5-Sequence
	-0.165103445, -1.367353957, -1.084887944, -0.103446973,  0.005614329, // 3-7
	-0.177613166,  0.020646977, -0.204253239,  0.018329075, // 8-J
	// 4-Sequence
	-1.250221599, -1.305045504, -1.456065252, -0.889654668, -2.987781810, // 3-7
	-1.388516263, -1.176554458, -0.794348341, -2.055029278, -1.890634784, // 8-Q
	// 3-Sequence
	-3.704179332, -4.281589257, -4.719447787, -4.495022219, // 3-6
	-5.089943875, -6.087235981, -2.976696921, -4.422503509, // 7-T
	-4.221070745, -4.222361704, -4.002477132, // J-K
	// Solo, Group(2-4pair)
	-3.902118723, -5.356407798, -4.459366682, -1.673269738, // 3
	-3.813512676, -5.646004771, -4.959193566, -0.598674962, // 4
	-3.707419103, -5.827590172, -5.076551563, -0.455296345, // 5
	-2.914792373, -5.043195734, -4.436616845, -0.790830702, // 6
	-2.659857260, -4.498763022, -3.548758188, -0.164144807, // 7
	-6.162883027, -7.620724057, -6.552047173, -0.409127296, // 8
	-3.377120658, -4.432217918, -3.604395246, -0.447012953, // 9
	-2.931679825, -3.827014698, -2.405417588, -0.186615449, // T
	-2.977701799, -3.088013233, -1.833889336, -0.034788011, // J
	-3.180260758, -2.933643112, -1.445919210,  0.025592376, // Q
	-3.039855900, -2.900483696, -1.851938754,  0.005744616, // K
	-2.396018281, -2.156369375, -1.014889482,  0.035431113, // A
	-2.386842708, -2.704110364, -2.010499011, -0.169877329, // 2
	 0.557952043, // JOKER
	// for Revolution
	// 5-Sequence
	 0.015941512, -0.076186286, -1.869357120, -0.280747683, -0.003119470, // 3-7
	-0.892415555,  0.001491174, -0.634278907, -1.064733776, // 8-J
	// 4-Sequence
	-1.496744051, -0.825525999, -2.733283880, -0.894995405, -1.898804633, // 3-7
	-1.538144313, -1.025547067, -0.837688942, -1.185448678, -3.504837011, // 8-Q
	// 3-Sequence
	-4.213282878, -3.897294649, -4.012261980, -4.517719657, // 3-6
	-5.015807231, -5.507484021, -4.051525440, -4.608462404, // 7-T
	-4.057425508, -4.326998459, -5.129364509, // J-K
	// Solo, Group(2-4pair)
	-2.415693628, -2.782669751, -1.843946193, -0.004313174, // 3
	-2.704387616, -2.350158654, -1.209685749,  0.019062053, // 4
	-3.187466989, -2.919736669, -1.705203490, -0.060453580, // 5
	-3.442788490, -3.467241349, -2.027801109, -0.012216187, // 6
	-3.321312748, -3.495115522, -2.244767429, -0.129789376, // 7
	-5.744447193, -5.757793499, -5.355164182, -2.659961621, // 8
	-3.147674227, -3.078045473, -2.287018461, -0.368540465, // 9
	-2.965633813, -4.850912354, -3.522098268,  0.016110389, // T
	-3.047409384, -4.952092305, -4.031908245, -0.387839210, // J
	-3.378235820, -5.678286406, -4.574724270, -0.436443491, // Q
	-3.720091654, -6.173770806, -5.468976055, -0.706931919, // K
	-4.073372779, -6.424423870, -5.079275011, -0.898643720, // A
	-4.051587944, -5.937719397, -5.274388701, -2.298538090, // 2
	 0.848130302, // JOKER
};

inline int index(Hand &h,bool rev){
	const int seq5Size =  9;  // 5枚階段  3〜Jの9通り
	const int seq4Size = 10;  // 4枚階段  3〜Qの10通り
	const int seq3Size = 11;  // 3枚階段  3〜Kの11通り
	const int groupSize = 52; // 1〜4枚組 3〜2の13通り × (1〜4)枚 = 52通り
	const int jokerSize = 1;  // jokerは1枚
	int seq5Index  = 0;
	int seq4Index  = seq5Index+seq5Size;   // = 0+9  = 9;
	int seq3Index  = seq4Index+seq4Size;   // = 9+10 = 19;
	int groupIndex = seq3Index+seq3Size;   // = 19+11 = 30;
	int jokerIndex = groupIndex+groupSize; // = 30+52 = 82;

	int seqindex[]={seq5Index,seq4Index,seq3Index};

	int res = rev?83:0;
	if(h.qty==1&&h.jokerUsed()){
		res+=jokerIndex;
	}else
		if(h.seq){
			int start = seqindex[5-min(5,(int)h.qty)];
			res += start+h.ord;
		}else{
			res+=groupIndex;
			res+=h.ord*4+(h.qty-1);
		}
	return res;
}

vector<Hand> getAIHand(Cards myCards){
	vector<Hand> res;
	auto hs = getSeq(myCards);
	for(int i=5;i>=3;i--){
		for(int k=0;k<hs.size();k++){
			if(hs[k].qty==i){
				bool b=true;
				for(int q=0;q<res.size();q++){
					if((res[q].hands&hs[k].hands)==hs[k].hands){
						b=false;
						break;
					}
				}
				if(b)res.push_back(hs[k]);
			}
		}
	}
	for(int i=0;i<13;i++){
		Cards c = myCards&(0xFLL<<(4*i));
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
		h.hands = 0;
		h.qty=1;
		h.ord=13;
		h.seq=false;
		h.suit=0;
		res.push_back(h);
	}
	return res;
}

double calcValue(const fieldInfo &info,Cards myCards){
	vector<Hand> hs = getAIHand(myCards);
	
	double res=0.0;
	for(int i=0;i<hs.size();i++){
		if(hs[i].qty==0)continue;
		res+=param[index(hs[i],info.rev)];
	}
	return res;
}

Hand testAI(fieldInfo info,Cards myCards,Cards oppCards){
	vector<Hand> hs = getAllValidHands(info,myCards);

	double sum=0.0;
	vector<double> ratiomap;
	for(auto it=hs.begin();it!=hs.end();it++){
		double ratio = exp(calcValue(info,myCards^it->hands));
		sum += ratio;
		ratiomap.push_back(ratio);
	}
	double r = randDouble(0,sum);
	for(int i=0;i<ratiomap.size();i++){
		r -= ratiomap[i];
		if(r<=0.0)return hs[i];
	}
	return hs.back();
}
