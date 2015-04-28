#ifndef RP_CMA_HP_H
#define RP_CMA_HP_H

#define SMALL_DIM 50
#define PRECISION 10e-6
#define K 40

class HyperParameters {
public:
	const static int _d = SMALL_DIM;
	constexpr static double eps = PRECISION;
	const static int _k = K;

	inline static int d() {
		return _d;
	}

	inline static int k() {
		return _k;
	}
};

#endif

