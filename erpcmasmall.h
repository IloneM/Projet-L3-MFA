#ifndef ERP_CMA_SMALL_H
#define ERP_CMA_SMALL_H

#include "rpcmasmall.h"

namespace libcmaes
{

template <class TBigStrat,class TCovarianceUpdate,class TGenoPheno>
class ERPCMASmall : public RPCMASmall<TBigStrat, TCovarianceUpdate, TGenoPheno> {
public:


    ERPCMASmall(CMAParameters<TGenoPheno> sparams,
				TBigStrat* bdimstrat, const double weight) : 
			RPCMASmall<TBigStrat, TCovarianceUpdate, TGenoPheno>(sparams, bdimstrat), _weight(weight) {}

	inline double weight() const {
		return _weight;
	}

	void set_weight(const double weight) {
		_weight = weight;
	}

void merge(std::vector<Candidate>::iterator low, std::vector<Candidate>::iterator mid, std::vector<Candidate>::iterator high) {
	std::vector<Candidate>::iterator i   = low;
	std::vector<Candidate>::iterator j   = mid;

	std::vector<Candidate> sorted(std::distance(low, high));
	std::vector<Candidate>::iterator x = sorted.begin();

	while(i < mid && j < high) {
		if (*j < *i) {
		// Use move semantics. So we don't copy expensive T object around.
			(*x) = std::move(*j);
			++j;
			++x;
		} else {
		// Put the ++ inline to show difference in readability.
		// Prefer the above to this.
			(*x++) = std::move(*i++);
		}
									
	}
	// Don't use a manual loop when you can use an already optimized algorithm
	x = std::move(i, mid,  x);
	x = std::move(j, high, x);

	// Now move the sorted values back.
	std::move(std::begin(sorted), std::end(sorted), low);
}

template<typename I>
// requires std::RandomAccessIterator<I>           // Proposed C++17 syntax for concepts.
//       && std::LessThanComparable<typename I::value_type>>
void sort(I low, I high) {
	size size = std::distance(low, high);
	if(size <= 1) {
		return;
	}
	I    mid(low)
	std::advance(mid, size/2);
	sort(low, mid);
	sort(mid, high);
	merge(low, mid, high);
}

	int build_projection_fitness(std::vector<Candidate>& candidates) {
/*
		int fit = candidates.size();
		for(int i=0; i< candidates.size(); i++) {
			for(int j=i; j< candidates.size(); j++) {
				
			}
		}
		*/
	}

	double update_weight() {

//		std::vector<Candidate> cust_rank_candidates = CMAStrategy<TCovarianceUpdate, TGenoPheno>::_solutions._candidates;
	/*
		for(int i=0; i< CMAStrategy<TCovarianceUpdate, TGenoPheno>::_candidates.cols(); i++)
				CMAStrategy<TCovarianceUpdate, TGenoPheno>::_candidates
	*/
		return 0;
	}

protected:
	double _weight;
};

}

#endif

