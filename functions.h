#include <iostream>
#include <cmath>
#include <functional>
#include <random>

std::normal_distribution<double> norm(0.0,1.0);
std::cauchy_distribution<double> cauch(0.0,1.0);
std::mt19937 gen;

dMat orthogonalBasis(const int N)
{
  static dMat b = dMat::Zero(N,N);
  static bool initialized = false;

  if (initialized)
    return b;
  
  std::random_device rd;
  std::normal_distribution<double> norm(0.0,1.0);
  std::mt19937 gen(rd()); //TODO: seed ?
  initialized = true;
  double sp = 0.0;
  
  for (int i=0;i<N;i++)
    {
      /* sample from Gaussian. */
      for (int j=0;j<N;j++)
	b(i,j) = norm(gen);
      /* substract projection of previous vectors */
      for (int j=i-1;j>=0;--j)
	{
	  sp = 0.0;
	  for (int k=0;k<N;++k)
	    sp += b(i,k)*b(j,k); // scalar product.
	  for (int k=0;k<N;k++)
	    b(i,k) -= sp * b(j,k);
	}
      sp = 0.0;
      for (int k=0;k<N;++k)
	sp += b(i,k)*b(i,k);
      for (int k=0;k<N;++k)
	b(i,k) /= sqrt(sp);
    }
  return b;
};

typedef std::function<double (const double*, const int &n)> FitFunc;

// classical test functions for single-objective optimization problems.
FitFunc ackleys = [](const double *x, const int N)
{
  return -20.0*exp(-0.2*sqrt(0.5*(x[0]*x[0]+x[1]*x[1]))) - exp(0.5*(cos(2.0*M_PI*x[0]) + cos(2.0*M_PI*x[1]))) + 20.0 + exp(1.0);
};

FitFunc fsphere = [](const double *x, const int N)
{
  double val = 0.0;
  for (int i=0;i<N;i++)
    val += x[i]*x[i];
  return val;
};
/*
GradFunc grad_fsphere = [](const double *x, const int N)
{
  dVec grad(N);
  for (int i=0;i<N;i++)
    grad(i) = 2.0*x[i];
  return grad;
};
*/
FitFunc cigtab = [](const double *x, const int N)
{
  int i;
  double sum = 1e4*x[0]*x[0] + 1e-4*x[1]*x[1];
  for(i = 2; i < N; ++i)
    sum += x[i]*x[i];
  return sum;
};

FitFunc rosenbrock = [](const double *x, const int N)
{
  double val = 0.0;
  for (int i=0;i<N-1;i++)
    {
      val += 100.0*pow((x[i+1]-x[i]*x[i]),2) + pow((x[i]-1.0),2);
    }
  return val;
};
/*
GradFunc grad_rosenbrock = [](const double *x, const int N)
{
  dVec grad = dVec::Zero(N);
  for (int i=0;i<N-1;i++)
    {
      grad(i) = -400.0*x[i]*(x[i+1]-x[i]*x[i])-2.0*(1.0-x[i]);
      grad(i+1) += 200.0*(x[i+1]-x[i]*x[i]);
    }
  return grad;
};
*/
FitFunc beale = [](const double *x, const int N)
{
  return pow(1.5-x[0]+x[0]*x[1],2) + pow(2.25 - x[0] + x[0]*x[1]*x[1],2) + pow(2.625-x[0]+x[0]*pow(x[1],3),2);
};

FitFunc goldstein_price = [](const double *x, const int N)
{
  return (1.0 + pow(x[0] + x[1] + 1.0,2)*(19.0-14.0*x[0]+3*x[0]*x[0]-14.0*x[1]+6*x[0]*x[1]+3*x[1]*x[1]))*(30.0+pow(2.0*x[0]-3.0*x[1],2)*(18.0-32.0*x[0]+12.0*x[0]*x[0]+48.0*x[1]-36.0*x[0]*x[1]+27.0*x[1]*x[1]));
};

FitFunc booth = [](const double *x, const int N)
{
  return pow(x[0]+2.0*x[1]-7,2) + pow(2*x[0]+x[1]-5,2);
};

FitFunc bukin = [](const double *x, const int N)
{
  return 100.0 * sqrt(fabs(x[1]-0.01*x[0]*x[0])) + 0.01*fabs(x[0]+10.0);
};

FitFunc matyas = [](const double *x, const int N)
{
  return 0.26*(x[0]*x[0]+x[1]*x[1])-0.48*x[0]*x[1];
};

FitFunc levi = [](const double *x, const int N)
{
  return pow(sin(3*M_PI*x[0]),2) + pow(x[0]-1,2) * (1.0+pow(sin(3*M_PI*x[1]),2)) + pow(x[1]-1,2)*(1.0+pow(sin(2.0*M_PI*x[1]),2));
};

FitFunc camel = [](const double *x, const int N)
{
  return 2.0*x[0]*x[0] - 1.05*pow(x[0],4) + pow(x[0],6)/6.0 + x[0]*x[1] + x[1]*x[1];
};

FitFunc easom = [](const double *x, const int N)
{
  return -cos(x[0])*cos(x[1])*exp(-(pow((x[0]-M_PI),2)+pow((x[1]-M_PI),2)));
};

FitFunc crossintray = [](const double *x, const int N)
{
  return -0.0001*pow(fabs(sin(x[0])*sin(x[1])*exp(fabs(100.0-sqrt(x[0]*x[0]+x[1]*x[1])/M_PI)))+1.0,0.1);
};

FitFunc eggholder = [](const double *x, const int N)
{
  return -(x[1]+47)*sin(sqrt(fabs(x[1]+0.5*x[0]+47.0))) - x[0]*sin(sqrt(fabs(x[0]-(x[1] + 47.0))));
};

FitFunc holdertable = [](const double *x, const int N)
{
  return -fabs(sin(x[0])*cos(x[1])*exp(fabs(1.0-sqrt(x[0]*x[0]+x[1]*x[1])/M_PI)));
};

FitFunc mccormick = [](const double *x, const int N)
{
  return sin(x[0]+x[1])+pow(x[0]-x[1],2) - 1.5*x[0] + 2.5*x[1] + 1.0;
};

FitFunc schaffer2 = [](const double *x, const int N)
{
  return 0.5 + (pow(sin(x[0]*x[0]-x[1]*x[1]),2)-0.5) / pow(1.0+0.001*(x[0]*x[0]+x[1]*x[1]),2);
};

FitFunc schaffer4 = [](const double *x, const int N)
{
  return 0.5 + (cos(sin(fabs(x[0]*x[0]-x[1]*x[1])))-0.5) / pow(1.0+0.001*(x[0]*x[0]+x[1]*x[1]),2);
};

FitFunc styblinski_tang = [](const double *x, const int N)
{
  double val = 0.0;
  for (int i=0;i<N;i++)
    val += pow(x[i],4) - 16.0*x[i]*x[i] + 5.0*x[i];
  return 0.5*val;
};

FitFunc rastrigin = [](const double *x, const int N)
{
  static double A = 10.0;
  double val = A*N;
  for (int i=0;i<N;i++)
    val += x[i]*x[i] - A*cos(2*M_PI*x[i]);
  return val;
};

FitFunc elli = [](const double *x, const int N)
{
  if (N == 1)
    return x[0] * x[0];
  double val = 0.0;
  for (int i=0;i<N;i++)
    val += exp(log(1e3)*2.0*static_cast<double>(i)/static_cast<double>((N-1))) * x[i]*x[i];
  return val;
};
/*
GradFunc grad_elli = [](const double *x, const int N)
{
  dVec grad(N);
  if (N == 1)
    {
      grad(0) = 2.0*x[0];
      return grad;
    }
  for (int i=0;i<N;i++)
    grad(i) = exp(log(1e3)*2.0*static_cast<double>(i)/static_cast<double>((N-1)))*2.0*x[i];
  return grad;
};
*/
FitFunc tablet = [](const double *x, const int N)
{
  double val = 1e6*x[0]*x[0];
  for (int i=1;i<N;i++)
    val += x[i]*x[i];
  return val;
};
/*
GradFunc grad_tablet = [](const double *x, const int N)
{
  dVec grad(N);
  grad(0) = 1e6*2.0*x[0];
  for (int i=0;i<N;i++)
    grad(i) = 2.0*x[i];
  return grad;
};
*/
FitFunc cigar = [](const double *x, const int N)
{
  double val = 0.0;
  for (int i=1;i<N;i++)
    val += x[i]*x[i];
  val *= 1e6;
  val += x[0]*x[0];
  return val;
};

FitFunc ellirot = [](const double *x, const int N)
{
  if (N == 1)
    return x[0]*x[0];
  dMat b = orthogonalBasis(N);
  double val = 0.0;
  for (int i=0;i<N;i++)
    {
      double y = 0.0;
      for (int k=0;k<N;k++)
	y += b(i,k)*x[k];
      val += exp(log(1e3)*2.0*static_cast<double>(i)/(N-1)) * y*y;
    }
  return val;
};

FitFunc diffpow = [](const double *x, const int N)
{
  if (N == 1)
    return x[0]*x[0];
  double val = 0.0;
  for (int i=0;i<N;i++)
    val += pow(fabs(x[i]),2.0+10.0*static_cast<double>(i)/(N-1));
  return val;
};

FitFunc diffpowrot = [](const double *x, const int N)
{
  if (N == 1)
    return x[0]*x[0];
  dMat b = orthogonalBasis(N);
  double val = 0.0;
  for (int i=0;i<N;i++)
    {
      double y = 0.0;
      for (int k=0;k<N;k++)
	y += b(i,k)*x[k];
      val += pow(fabs(y),2.0+10.0*static_cast<double>(i)/(N-1));
    }
  return val;
};

FitFunc hardcos = [](const double *x, const int N)
{
  double sum = 0.0;
  for (int i=0;i<N;i++)
    sum += x[i]*x[i];
  sum*=(cos(sum)+2.0);
  return sum;
};

// uncertainty handling testing functions
FitFunc fsphere_uhc = [](const double *x, const int N)
{
  double val = 0.0;
  for (int i=0;i<N;i++)
    val += x[i]*x[i];
  val += cauch(gen); // noise
  return val;
};

FitFunc elli_uh = [](const double *x, const int N)
{
  if (N == 1)
    return x[0] * x[0];
  double val = 0.0;
  for (int i=0;i<N;i++)
    val += pow(10,6*static_cast<double>(i)/static_cast<double>((N-1))) * x[i]*x[i];
  val += norm(gen); // noise
  return val;
};

FitFunc elli_uhc = [](const double *x, const int N)
{
  if (N == 1)
    return x[0] * x[0];
  double val = 0.0;
  for (int i=0;i<N;i++)
    val += pow(10,6*static_cast<double>(i)/static_cast<double>((N-1))) * x[i]*x[i];
  val += cauch(gen); // noise
  return val;
};

