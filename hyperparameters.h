#ifndef RP_CMA_HP_H
#define RP_CMA_HP_H

#define SMALL_DIM 40
#define PRECISION 10e-6
#define K 100

class HyperParameters {
public:
	static int _d; //= SMALL_DIM;
	constexpr static double eps = PRECISION;
	static int _k; //= K;

	inline static int d() {
		return _d;
	}

	inline static int k() {
		return _k;
	}
};

int HyperParameters::_d = 0;
int HyperParameters::_k = 0;

#endif

