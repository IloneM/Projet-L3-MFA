#ifndef RP_CMA_SMALL_H
#define RP_CMA_SMALL_H

#include <cmaes.h>
#include "hyperparameters.h"

namespace libcmaes {

FitFunc fnull = [](const double *x, const int N) {return 0;};

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>> class RPCMABig;

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
class RPCMASmall : public CMAStrategy<TCovarianceUpdate,TGenoPheno>
{
protected:

	RPCMABig<TCovarianceUpdate,TGenoPheno>* _bdimstrat;

	dMat _randProjection;

    /**
     * \brief Setup parameters for small dimension strategy
     * @param parameters forwarded by big dimension strategy
     */
	CMAParameters<TGenoPheno>& setupParameters(CMAParameters<TGenoPheno>& params) const {
		params.set_uh(false);
		params.initialize_parameters();
		return params;
	}

public:

      /**
       * \brief dummy constructor
       */

      /**
       * \brief constructor. We assume bdimstrat to be fully setup in way to use its properties
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
	   *
	   * \warn _bdimstrat and _randProjection must be set before calling parent constructor
	   *
       */
      RPCMASmall(CMAParameters<TGenoPheno> sparams, RPCMABig<TCovarianceUpdate,TGenoPheno>* bdimstrat) : 
		CMAStrategy<TCovarianceUpdate,TGenoPheno>(fnull, setupParameters(sparams)),
		_bdimstrat(bdimstrat),
		_randProjection(grp(sparams.dim(), bdimstrat->get_parameters().dim())) {
		//is licit since others values depending of x0 evoluates as x0 does (no one at wrtiting moment)
		CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.set_x0(_randProjection * bdimstrat->get_parameters().get_x0min(), _randProjection * bdimstrat->get_parameters().get_x0max());
	  }

      /**
       * \brief constructor for starting from an existing solution.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       * @param cmasols solution object to start from
       */

	~RPCMASmall() {}
/*
  void eval(const dMat &candidates, const dMat &phenocandidates=dMat(0,0))
  {
#ifdef HAVE_DEBUG
    std::chrono::time_point<std::chrono::system_clock> tstart = std::chrono::system_clock::now();
#endif
    // one candidate per row.
#pragma omp parallel for if (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._mt_feval)
    for (int r=0;r<candidates.cols();r++)
      {
	CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._candidates.at(r).set_x(candidates.col(r));
	CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._candidates.at(r).set_id(r);
	CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._candidates.at(r).set_fvalue(_bdimstrat->get_solutions()._candidates.at(r).get_fvalue());
	
	//std::cerr << "candidate x: " << _solutions._candidates.at(r)._x.transpose() << std::endl;
      }
    
    // if an elitist is active, reinject initial solution as needed.
    if (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_niter > 0 && (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._elitist || CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._initial_elitist || (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_initial_elitist && CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._initial_elitist_on_restart)))
      {
	// get reference values.
	double ref_fvalue = std::numeric_limits<double>::max();
	Candidate ref_candidate;
	
	if (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._initial_elitist_on_restart || CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._initial_elitist)
	  {
	    ref_fvalue = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._initial_candidate.get_fvalue();
	    ref_candidate = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._initial_candidate;
	  }
	else if (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._elitist)
	  {
	    ref_fvalue = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._best_seen_candidate.get_fvalue();
	    ref_candidate = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._best_seen_candidate;
	  }

	// reinject intial solution if half or more points have value above that of the initial point candidate.
	int count = 0;
	for (int r=0;r<candidates.cols();r++)
	  if (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._candidates.at(r).get_fvalue() < ref_fvalue)
	    ++count;
	if (count/2.0 < candidates.cols()/2)
	  {
#ifdef HAVE_DEBUG
	    std::cout << "reinjecting solution=" << ref_fvalue << std::endl;
#endif
	    CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._candidates.at(1) = ref_candidate;
	  }
      }
    
#ifdef HAVE_DEBUG
    std::chrono::time_point<std::chrono::system_clock> tstop = std::chrono::system_clock::now();
    CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._elapsed_eval = std::chrono::duration_cast<std::chrono::milliseconds>(tstop-tstart).count();
#endif
  }
*/	
    /**
     * \brief Build the fvalue copying it from RPCMABig candidate
	 * @param the column of the candidate to be evaluated
	 * @param the candidates stored as column of the matrix
	 * @return the computed function value
     */
	inline double build_fvalue(const int &col, const dMat& candidates)
      {
      return _bdimstrat->get_solutions()._candidates.at(col).get_fvalue();
      }

      /**
       * \brief Generate a Random Projection from big to small space
       * @param d is the small dimension
	   * @param D is the big dimension
	   * TODO: try to make random as in enmvn.h
       */
	static dMat grp(int d, int D) {
		dMat result(d,D);

		for(int i=0; i< d; i++) {
			Eigen::RowVectorXd randVector = Eigen::RowVectorXd::Random(D);

			Eigen::RowVectorXd newBasisCandidate = randVector;

			for(int j=0; j< i; j++) {
				newBasisCandidate -= result.row(j) * randVector.dot(result.row(j));//we don't need to divide by result.row(j).norm() cause all vectors are normalized
			}

			if(newBasisCandidate.norm() > HyperParameters::eps) {
				result.row(i) = newBasisCandidate / newBasisCandidate.norm();
			} else {
				i--;
			}
		}
		return result;
	}

	inline dMat randProjection() {
		return _randProjection;
	}

      /**
       * \brief Compute a value proportionnal to the probability of X projection, according to the small gaussian distribution
       * @param X the point to evaluate. Note X belongs to big space.
       */
	inline double probabilityFitness(dVec X) {
		return (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions.csqinv() * (_randProjection * X - CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions.xmean())).squaredNorm();
	}

      /**
       * \brief Select the best candidate from the set given in parameters
       * @param The set of candidates generated by RPCMABig (candidates are the matrix columns).
       */
	dVec bestCandidate(dMat candidates) {
		if(candidates.rows() < 1) {//should not occur in normal process
			//TODO: throw exception or compute error
			return dVec();//empty vector
		}
		dVec max(candidates.rows());
		double maxValue = probabilityFitness(candidates.col(0));

		double candidateMark;

		for(int i=1; i< candidates.cols(); i++) {
			candidateMark = probabilityFitness(candidates.col(i));
			if(candidateMark > maxValue) {
				max = candidates.col(i);
				maxValue = candidateMark;
			}
		}

		return max;
	}
};

}

#endif

