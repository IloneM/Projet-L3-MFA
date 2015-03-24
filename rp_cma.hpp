#ifndef RP_CMA_H
#define RP_CMA_H

#include <Eigen/Eigen>
#include <functional>
#include <eigenmvn.h>

class RP_CMA
{

protected:

//general space properties

/*
 * attributes 
 */

unsigned int m_D;//size of the big space

unsigned int m_d;//size of the "small space" (i.e. d<<D)

unsigned int m_K;//number of samples

std::function<double(Eigen::RowVectorXd)> m_f;//function to be optimized

Eigen::EigenMultivariateNormal<double> smallDistribution;

Eigen::EigenMultivariateNormal<double> bigDistribution;

//projections settings

/*
 * attributes 
 */

unsigned int m_projs_numbers;//the number of random projections generated

Eigen::Matrix<Eigen::MatrixXd,1,Eigen::Dynamic> m_rand_projections;//row vector in fact

Eigen::RowVectorXd m_projs_weight;//weight of each projection for optimization purpose

/*
 * methods
 */
public:

void setRandProjections();//no parameter taken, cause randProjections is precisly random!

static Eigen::MatrixXd generate_single_random_projection(unsigned int d, unsigned int D);//where D is the big dimension ans d the little one

int run();

//samples settings

/*
 * attributes 
 */

unsigned int m_samples_size;//the number of samples generated

/*
 * methods
 */

//class internal stuffs

//computational constants

#define PRECISION 1e-8

/*
 * methods
 */

protected:

void m_init_seed();

public:

RP_CMA(unsigned int D, unsigned int d, std::function<double(Eigen::RowVectorXd)> f);
//cycle();

//protected generate

};

#endif
