#ifndef RP_CMA_HP_H
#define RP_CMA_HP_H

#define SMALL_DIM 30
#define PRECISION 10e-6
#define M_K 15
#define M_M 50
#define SPC 0.75
#define SLC M_M/10

class HyperParameters {
public:
	static int _d; //projection dimension (d << D where D is the problem dimension)
	constexpr static double eps = PRECISION;
	static int _K; //interference of projection (\in [[1,100]])
	static int _M; //number of projections (to set)
	static double _spc; //sparse coefficient (\in ]0;1[)
	static int _slc; //slanting coefficient (\in [[1;_M]])

	inline static int d() {
		return _d;
	}

	inline static int K() {
		return _K;
	}

	inline static int M() {
		return _M;
	}

	inline static double spc() {
		return _spc;
	}

	inline static int slc() {
		return _slc;
	}
};

int HyperParameters::_d = SMALL_DIM;
int HyperParameters::_K = M_K;
int HyperParameters::_M = M_M;//to set
double HyperParameters::_spc = SPC;//to set
int HyperParameters::_slc = SLC;//to set

#endif

