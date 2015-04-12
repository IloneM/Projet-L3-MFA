#ifndef RP_CMA_PARAMATERS_H
#define RP_CMA_PARAMATERS_H

//#include <Eigen/Eigen>
#include <cmaparameters.h>

/*
 * Class wich includes some rp_cma specific parameters
 */

using namespace libcmaes;

template <class TGenoPheno=GenoPheno<NoBoundStrategy> >
class RPCMAParameters : public CMAParameters<TGenoPheno>
{
	public:
		RPCMAParameters() : CMAParameters() {}
      
      /**
       * \brief Constructor.
       * @param dim problem dimensions
       * @param x0 initial search point
       * @param sigma initial distribution step size (positive, otherwise automatically set)
       * @param lambda number of offsprings sampled at each step
       * @param seed initial random seed, useful for reproducing results (if unspecified, automatically generated from current time)
       * @param gp genotype / phenotype object
       * @param sep whether to use sep-CMA-ES, using diagonal covariance matrix (modifies covariance default learning rate)
       */
		RPCMAParameters(
			const int &dim,
			const int &sdim,
		    const double *x0,
		    const double &sigma,
		    const int &lambda=-1,
		    const uint64_t &seed=0,
		    const TGenoPheno &gp=GenoPheno<NoBoundStrategy>()) : CMAParameters(dim,x0,sigma,lambda,seed,gp), _sdim(sdim) {}
      
      /**
       * \brief Constructor.
       * @param x0 initial search point as vector of problem dimension
       * @param sigma initial distribution step size (positive, otherwise automatically set)
       * @param lambda number of offsprings sampled at each step
       * @param seed initial random seed, useful for reproducing results (if unspecified, automatically generated from current time)
       * @param gp genotype / phenotype object
       * @param sep whether to use sep-CMA-ES, using diagonal covariance matrix (modifies covariance default learning rate)
       */
		RPCMAParameters(
			const std::vector<double> &x0,
		    const double &sigma,
		    const int &lambda=-1,
		    const uint64_t &seed=0,
		    const TGenoPheno &gp=GenoPheno<NoBoundStrategy>()) : CMAParameters(dim,x0,sigma,lambda,seed,gp), _sdim(sdim) {}
      
      /**
       * \brief Constructor.
       * @param x0 initial search point as vector of problem dimension
       * @param sigma vector of initial distribution step sizes (positive, otherwise automatically set)
       * @param lambda number of offsprings sampled at each step
       * @param seed initial random seed, useful for reproducing results (if unspecified, automatically generated from current time)
       * @param gp genotype / phenotype object
       * @param sep whether to use sep-CMA-ES, using diagonal covariance matrix (modifies covariance default learning rate)
       */
      CMAParameters(const std::vector<double> &x0,
		    const std::vector<double> &sigma,
		    const int &lambda=-1,
		    const std::vector<double> &lbounds=std::vector<double>(),
		    const std::vector<double> &ubounds=std::vector<double>(),
		    const uint64_t &seed=0);
      
      ~CMAParameters();

	protected:
		int _sdim;//the small dimension (d<<D)
};

#endif
