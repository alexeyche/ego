#include <iostream>
#include "../../include/cppoptlib/meta.h"
#include "../../include/cppoptlib/problem.h"
#include "../../include/cppoptlib/solver/lbfgsbsolver.h"

// to use CppNumericalSolvers just use the namespace "cppoptlib"
namespace cppoptlib {

// we will solve ||Xb-y|| s.t. b>=0
template<typename T>
class NonNegativeLeastSquares : public Problem<T> {
    const Matrix<T> X;
    const Vector<T> y;

  public:
    NonNegativeLeastSquares(const Matrix<T> &X_, const Vector<T> y_) : X(X_), y(y_) {}

    T value(const Vector<T> &beta) {
        return arma::dot(X*beta-y, X*beta-y);
    }

    void gradient(const Vector<T> &beta, Vector<T> &grad) {
        grad = X.t()*2*(X*beta-y);
    }
};

}
int main(int argc, char const *argv[]) {

    const size_t DIM = 4;
    const size_t NUM = 5;
    typedef double T;

    // create model X*b for arbitrary b
    cppoptlib::Matrix<T> X         = cppoptlib::Random<T>(NUM, DIM);
    cppoptlib::Vector<T> true_beta = cppoptlib::Random<T>(DIM);
    cppoptlib::Vector<T> y         = X*true_beta;

    // perform non-negative least squares
    cppoptlib::NonNegativeLeastSquares<T> f(X, y);
    f.setLowerBound(cppoptlib::Zero<T>(DIM));

    // create initial guess (make sure it's valid >= 0)
    cppoptlib::Vector<T> beta = cppoptlib::Random<T>(DIM);
    
    beta = arma::abs(beta); //beta(beta.array() < 0).select(-beta, beta);
    std::cout << "start with b =          " << beta.t() << std::endl;

    // init L-BFGS-B for box-constrained solving
    cppoptlib::LbfgsbSolver<double> solver;
    solver.minimize(f, beta);

    // display results
    std::cout << "model s.t. b >= 0  loss:" << f(beta) << std::endl;
    std::cout << "for b =                 " << beta.t() << std::endl;
    std::cout << "true model         loss:" << f(true_beta) << std::endl;
    std::cout << "for b =                 " << true_beta.t() << std::endl;

    return 0;
}
