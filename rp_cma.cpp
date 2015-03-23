#include "rp_cma.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

//
// Transform code so that vector effectively be threated as matrix and end with others stuffs
//

void RP_CMA::setRandProjections() {
	m_rand_projections = Eigen::Matrix<Eigen::MatrixXd, 1, Eigen::Dynamic>(m_projs_numbers);//row vector in fact
	for(unsigned int i=0; i< m_projs_numbers; i++) {
		m_rand_projections[i] = RP_CMA::generate_single_random_projection(m_d, m_D);
	}
}

/* 
 * The probability of a vector to be lineary dependent of the i precedently generated vectors is almost 0
 * This justify the fact that we can just randomly generate vectors and make them orthogonals to the others,
 * iteriting some times more in the (very few!) cases where the randomly generated vector almost belongs to the already generated plan
 * (i.e. the norm of the projection in orthogonal space is smaller than PRECISION)
 */
Eigen::MatrixXd RP_CMA::generate_single_random_projection(unsigned int d, unsigned int D) {
	Eigen::MatrixXd result(d,D);

	for(unsigned int i=0; i< d; i++) {
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

void RP_CMA::m_init_seed() {
	srand((unsigned int) time(0));
}

RP_CMA::RP_CMA(unsigned int D, unsigned int d, std::function<double(Eigen::RowVectorXd)> f) : m_D(D), m_d(d), m_projs_numbers(3), m_samples_size(1), bigDistribution(Eigen::Matrix<double,1,Eigen::Dynamic>::Zero(D), Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic>::Identity(D,D)), smallDistribution(Eigen::Matrix<double,1,Eigen::Dynamic>::Zero(d), Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Identity(d,d)), m_K(5)
{
	m_f = f;

	m_init_seed();

	m_projs_weight = Eigen::RowVectorXd::Constant(1, m_projs_numbers, 1/m_projs_numbers);

	setRandProjections();
/*
 * L'execution de l'alogrithme ci-dessous nous apprend que les vecteurs ne sont pas tout a fait orthogonaux (sans soutes du aux erreurs d'arrondis).
 * Nous avons en effet (empiriquement) 1e-16 >= <v1,v2> >= 1e-17 
 * Todo: trouver une borne pour l'erreurs d'approximation (si possible) et essayer de le resoudre 
 */
/*
	for(int i=0; i< m_rand_projections.innerSize(); i++) {
		std::cout << m_rand_projections[i] << "\n||v1||: " << m_rand_projections[i].row(0).norm() << "\n||v2||: " << m_rand_projections[i].row(1).norm()
				  << "\n<v1,v2>:" << m_rand_projections[i].row(0).dot(m_rand_projections[i].row(1)) << "\n\n";
	}
*/
}

int RP_CMA::run() {

	Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> testBig = bigDistribution.samples(m_K,1.0);

	Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> testSmall = smallDistribution.samples(m_K,1.0);

	std::cout << testBig << '\n' << testSmall;

	return 0;
}

