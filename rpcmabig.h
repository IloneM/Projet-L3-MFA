#ifndef RP_CMA_BIG_H
#define RP_CMA_BIG_H

//#include "abstractrpcma.h"
#include <cmaes.h>
#include "rpcmasmall.h"

#define SMALL_DIM 75
#define PRECISION 10e-6

/*
 * Importants points procedures seems to be eval for N_D and ask for n_d.
 * A overall class must be implemented
 */

using namespace libcmaes;

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
class RPCMABig : public CMAStrategy<TCovarianceUpdate,TGenoPheno>//AbstractRPCMA<TCovarianceUpdate,TGenoPheno>
{
protected:

//int _sdim;

	RPCMASmall<TCovarianceUpdate,TGenoPheno>* sdimstrat;//small dimension strategy

	dMat randProjection;

public:

	static dMat grp(int d, int D) {//generate random projection
		dMat result(d,D);

		for(int i=0; i< d; i++) {
			Eigen::RowVectorXd randVector = Eigen::RowVectorXd::Random(D);

			Eigen::RowVectorXd newBasisCandidate = randVector;

			for(unsigned int j=0; j< i; j++) {
				newBasisCandidate -= result.row(j) * randVector.dot(result.row(j));//we don't need to divide by result.row(j).norm() cause all vectors are normalized
			}

			if(newBasisCandidate.norm() > PRECISION) {
				result.row(i) = newBasisCandidate / newBasisCandidate.norm();
			} else {
				i--;
			}
		}
		return result;
	}

    /**
     * \brief dummy constructor
     */
    RPCMABig() : CMAStrategy<TCovarianceUpdate,TGenoPheno>() {}//, _sdim(0) {}
  
    /**
     * \brief constructor.
     * @param func objective function to minimize
     * @param parameters stochastic search parameters
     */
    RPCMABig(FitFunc &func,
	  CMAParameters<TGenoPheno> &params) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, params),
		randProjection(grp(SMALL_DIM, params.dim()))//, _sdim(SMALL_DIM)
	{
		std::vector<double> fake_x0(SMALL_DIM,0);
		CMAParameters<TGenoPheno> sparams(fake_x0, params.get_sigma_init(), params.lambda(), params.get_seed(), params.get_gp());
		sparams.set_x0(randProjection * params.get_x0min(), randProjection * params.get_x0max());//set the true x0
		sparams.initialize_parameters();

		sdimstrat = new RPCMASmall<TCovarianceUpdate,TGenoPheno>(func, sparams);
	}

    /**
     * \brief constructor for starting from an existing solution.
     * @param func objective function to minimize
     * @param parameters stochastic search parameters
     * @param cmasols solution object to start from
     */
    RPCMABig(FitFunc &func,
	  CMAParameters<TGenoPheno> &params,
	  const CMASolutions &cmasols) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, params, cmasols) {}//, _sdim(SMALL_DIM) {}

	~RPCMABig() {
		delete sdimstrat;
	}

};

//template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
//class RPCMA : public RPCMABig<TCovarianceUpdate,TGenoPheno> {};

#endif

