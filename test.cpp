#include "mlvector2010.h"
#include "mlAlgorithm.h"

int main(){
	typedef mlvector2010 V;

	V v = loadvec<V>("test.out");
	v[0]=99;
	savevec<V>("test.out",v);

}
