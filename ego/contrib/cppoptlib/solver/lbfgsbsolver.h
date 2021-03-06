// CppNumericalSolver
#include <iostream>
#include <list>

#include <ego/base/errors.h>
#include <ego/base/la.h>

using namespace NEgo;

#include "isolver.h"
#include "../linesearch/morethuente.h"
#ifndef LBFGSBSOLVER_H_
#define LBFGSBSOLVER_H_
namespace cppoptlib {
template<typename Dtype>
class LbfgsbSolver : public ISolver<Dtype, 1> {
  // last updates
  std::list<Vector<Dtype>> xHistory;
  // workspace matrices
  Matrix<Dtype> W, M;
  // ref to problem statement
  Problem<Dtype> *objFunc_;
  Vector<Dtype> lboundTemplate;
  Vector<Dtype> uboundTemplate;
  Dtype theta;
  int DIM;
  /**
   * @brief sort pairs (k,v) according v ascending
   * @details [long description]
   *
   * @param v [description]
   * @return [description]
   */
  std::vector<int> sort_indexes(const std::vector< std::pair<int, Dtype> > &v) {
    std::vector<int> idx(v.size());
    for (size_t i = 0; i != idx.size(); ++i)
      idx[i] = v[i].first;
    sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {
      return v[i1].second < v[i2].second;
    });
    return idx;
  }
  /**
   * @brief Algorithm CP: Computation of the generalized Cauchy point
   * @details PAGE 8
   *
   * @param c [description]
   */
  void GetGeneralizedCauchyPoint(Vector<Dtype> &x, Vector<Dtype> &g, Vector<Dtype> &x_cauchy,
  Vector<Dtype> &c) {
    const int DIM = x.n_rows;
    // Given x,l,u,g, and B = \theta I-WMW
    // {all t_i} = { (idx,value), ... }
    // TODO: use "std::set" ?
    std::vector<std::pair<int, Dtype> > SetOfT;
    // the feasible set is implicitly given by "SetOfT - {t_i==0}"
    Vector<Dtype> d = -g;
    // n operations
    for (int j = 0; j < DIM; j++) {
      if (g(j) == 0) {
        SetOfT.push_back(std::make_pair(j, std::numeric_limits<Dtype>::max()));
      } else {
        Dtype tmp = 0;
        if (g(j) < 0) {
          tmp = (x(j) - uboundTemplate(j)) / g(j);
        } else {
          tmp = (x(j) - lboundTemplate(j)) / g(j);
        }
        SetOfT.push_back(std::make_pair(j, tmp));
      }
    }
    // sortedindices [1,0,2] means the minimal element is on the 1-st entry
    std::vector<int> sortedIndices = sort_indexes(SetOfT);
    x_cauchy = x;
    // Initialize
    // p :=     W^Dtype*p
    Vector<Dtype> p = (W.t() * d);                     // (2mn operations)
    // c :=     0
    c = Zero<Dtype>(M.n_rows, 1);
    // f' :=    g^Dtype*d = -d^Td
    Dtype f_prime = -arma::dot(d, d);                         // (n operations)
    // f'' :=   \theta*d^Dtype*d-d^Dtype*W*M*W^Dtype*d = -\theta*f' - p^Dtype*M*p
    Dtype f_doubleprime = (Dtype)(-1.0 * theta) * f_prime - arma::dot(p, M * p); // (O(m^2) operations)
    // \delta t_min :=  -f'/f''
    Dtype dt_min;
    if (std::fabs(f_doubleprime) < std::numeric_limits<double>::epsilon()) {
      dt_min = 0.0;
    } else {
      dt_min = -f_prime / f_doubleprime;
    }
     
    
    // t_old :=     0
    Dtype t_old = 0;
    // b :=     argmin {t_i , t_i >0}
    int i = 0;
    for (int j = 0; j < DIM; j++) {
      i = j;
      if (SetOfT[sortedIndices[j]].second > 0)
        break;
    }
    int b = sortedIndices[i];
    // see below
    // t                    :=  min{t_i : i in F}
    Dtype t = SetOfT[b].second;
    // \delta Dtype             :=  t - 0
    Dtype dt = t ;
    // examination of subsequent segments
    while ((dt_min >= dt) && (i < DIM)) {
      if (d(b) > 0)
        x_cauchy(b) = uboundTemplate(b);
      else if (d(b) < 0)
        x_cauchy(b) = lboundTemplate(b);
      // z_b = x_p^{cp} - x_b
      Dtype zb = x_cauchy(b) - x(b);
      // c   :=  c +\delta t*p
      c += dt * p;
      // cache
      Vector<Dtype> wbt = W.row(b).t();
      f_prime += dt * f_doubleprime + g(b) * g(b) + theta * g(b) * zb - AsScalar<Dtype>(g(b) *
      wbt.t() * (M * c));
      f_doubleprime += (Dtype) - 1.0 * theta * g(b) * g(b)
                       - (Dtype) 2.0 * (g(b) * (arma::dot(wbt, M * p)))
                       - AsScalar<Dtype>(g(b) * g(b) * wbt.t() * (M * wbt));
      
      p += g(b) * wbt;
      d(b) = 0;
      if (std::fabs(f_doubleprime) < std::numeric_limits<double>::epsilon()) {
        dt_min = 0.0; 
      } else {
        dt_min = -f_prime / f_doubleprime;  
      }
      
      t_old = t;
      ++i;
      if (i < DIM) {
        b = sortedIndices[i];
        t = SetOfT[b].second;
        dt = t - t_old;
      }
    }
    dt_min = std::max(dt_min, (Dtype)0.0);
    t_old += dt_min;
    #pragma omp parallel for
    for (int ii = i; ii < x_cauchy.n_rows; ii++) {
      x_cauchy(sortedIndices[ii]) = x(sortedIndices[ii]) + t_old * d(sortedIndices[ii]);
    }
    c += dt_min * p;
  }
  /**
   * @brief find alpha* = max {a : a <= 1 and  l_i-xc_i <= a*d_i <= u_i-xc_i}
   * @details [long description]
   *
   * @param FreeVariables [description]
   * @return [description]
   */
  Dtype findAlpha(Vector<Dtype> &x_cp, Vector<Dtype> &du, std::vector<int> &FreeVariables) {
    Dtype alphastar = 1;
    const unsigned int n = FreeVariables.size();
    for (unsigned int i = 0; i < n; i++) {
      if (du(i) > 0) {
        alphastar = std::min(alphastar, (uboundTemplate(FreeVariables[i]) - x_cp(FreeVariables[i])) / du(i));
      } else {
        alphastar = std::min(alphastar, (lboundTemplate(FreeVariables[i]) - x_cp(FreeVariables[i])) / du(i));
      }
    }
    return alphastar;
  }
  /**
   * @brief solving unbounded probelm
   * @details [long description]
   *
   * @param SubspaceMin [description]
   */
  void SubspaceMinimization(Vector<Dtype> &x_cauchy, Vector<Dtype> &x, Vector<Dtype> &c, Vector<Dtype> &g,
  Vector<Dtype> &SubspaceMin) {
    Dtype theta_inverse = 1 / theta;
    std::vector<int> FreeVariablesIndex;
    for (int i = 0; i < x_cauchy.n_rows; i++) {
      if ((x_cauchy(i) != uboundTemplate(i)) && (x_cauchy(i) != lboundTemplate(i))) {
        FreeVariablesIndex.push_back(i);
      }
    }
    const int FreeVarCount = FreeVariablesIndex.size();
    Matrix<Dtype> WZ = Zero<Dtype>(W.n_cols, FreeVarCount);
    for (int i = 0; i < FreeVarCount; i++)
      WZ.col(i) = W.row(FreeVariablesIndex[i]).t();
    Vector<Dtype> rr = (g + theta * (x_cauchy - x) - W * (M * c));
    // r=r(FreeVariables);
    Vector<Dtype> r = Zero<Dtype>(FreeVarCount, 1);
    for (int i = 0; i < FreeVarCount; i++)
      r.row(i) = rr.row(FreeVariablesIndex[i]);
    // STEP 2: "v = w^T*Z*r" and STEP 3: "v = M*v"
    Vector<Dtype> v = M * (WZ * r);
    // STEP 4: N = 1/theta*W^T*Z*(W^T*Z)^T
    Matrix<Dtype> N = theta_inverse * WZ * WZ.t();
    // N = I - MN
    N = Identity<Dtype>(N.n_rows, N.n_rows) - M * N;
    // STEP: 5
    // v = N^{-1}*v
    // v = N.lu().solve(v);
    v = N.i() * v;
    // STEP: 6
    // HERE IS A MISTAKE IN THE ORIGINAL PAPER!
    Vector<Dtype> du = -theta_inverse * r - theta_inverse * theta_inverse * WZ.t() * v;
    // STEP: 7
    Dtype alpha_star = findAlpha(x_cauchy, du, FreeVariablesIndex);
    // STEP: 8
    Vector<Dtype> dStar = alpha_star * du;
    SubspaceMin = x_cauchy;
    for (int i = 0; i < FreeVarCount; i++) {
      SubspaceMin(FreeVariablesIndex[i]) = SubspaceMin(FreeVariablesIndex[i]) + dStar(i);
    }
  }
 public:
  
  void 
  X(const Vector<Dtype>& v) {
    for (size_t vi=0; vi < v.size(); ++vi) {
      ENSURE(v(vi) <= uboundTemplate(vi), v << " is out of upper bound (" << v(vi) << " > " << uboundTemplate(vi));
      ENSURE(v(vi) >= lboundTemplate(vi), v << " is out of lower bound (" << v(vi) << " < " << lboundTemplate(vi));
    }
  }

  void minimize(Problem<Dtype> &objFunc, Vector<Dtype> & x0) {
    objFunc_ = &objFunc;
    DIM = x0.n_rows;
    if (objFunc.hasLowerBound()) {
      lboundTemplate = objFunc_->lowerBound();
    }else {
      lboundTemplate = -Ones<Dtype>(DIM)* std::numeric_limits<Dtype>::infinity();
    }
    if (objFunc.hasUpperBound()) {
      uboundTemplate = objFunc_->upperBound();
    }else {
      uboundTemplate = Ones<Dtype>(DIM)* std::numeric_limits<Dtype>::infinity();
    }
    theta = 1.0;
    W = Zero<Dtype>(DIM, 0);
    M = Zero<Dtype>(0, 0);
    xHistory.push_back(x0);
    Matrix<Dtype> yHistory = Zero<Dtype>(DIM, 0);
    Matrix<Dtype> sHistory = Zero<Dtype>(DIM, 0);
    Vector<Dtype> x = x0, g = x0;
    size_t k = 0;
    Dtype f = objFunc.value(x);
    objFunc.gradient(x, g);
    // conv. crit.
    auto noConvergence =
      [&](Vector<Dtype> & x, Vector<Dtype> & g)->bool {
        return (arma::norm(ElWiseMin<Dtype>(ElWiseMax<Dtype>(Vector<Dtype>(x - g), lboundTemplate), uboundTemplate) - x, "inf") >= 1e-4);
      };
    while (noConvergence(x, g) && (k < this->settings_.maxIter)) {
      Dtype f_old = f;
      Vector<Dtype> x_old = x;
      Vector<Dtype> g_old = g;
      // STEP 2: compute the cauchy point
      Vector<Dtype> CauchyPoint = Zero<Dtype>(DIM, 1);
      Vector<Dtype> c = Zero<Dtype>(DIM, 1);
      GetGeneralizedCauchyPoint(x, g, CauchyPoint, c);
      // STEP 3: compute a search direction d_k by the primal method for the sub-problem
      Vector<Dtype> SubspaceMin;
      SubspaceMinimization(CauchyPoint, x, c, g, SubspaceMin);
      // STEP 4: perform linesearch and STEP 5: compute gradient
      Dtype alpha_init = 1.0;
      Vector<Dtype> searchDir = ElWiseMin<Dtype>(ElWiseMax<Dtype>(SubspaceMin-x, lboundTemplate), uboundTemplate);
      const Dtype rate = MoreThuente<Dtype, decltype(objFunc), 1>::linesearch(x, searchDir,  objFunc, alpha_init);
      // update current guess and function information
      x = x - rate*(x-SubspaceMin);
      x = ElWiseMin<Dtype>(ElWiseMax<Dtype>(x, lboundTemplate), uboundTemplate);
      f = objFunc.value(x);
      objFunc.gradient(x, g);
      xHistory.push_back(x);
      // prepare for next iteration
      Vector<Dtype> newY = g - g_old;
      Vector<Dtype> newS = x - x_old;
      // STEP 6:
      Dtype test = arma::dot(newS, newY);
      test = (test < 0) ? -1.0 * test : test;
      if (test > 1e-7 * arma::norm(newY, 2.0)) {
        if (k < this->settings_.m) {
          yHistory.resize(DIM, k + 1);
          sHistory.resize(DIM, k + 1);
        } else {
          yHistory.head_cols(this->settings_.m - 1) = yHistory.tail_cols(this->settings_.m - 1).eval();
          sHistory.head_cols(this->settings_.m - 1) = sHistory.tail_cols(this->settings_.m - 1).eval();
        }
        yHistory.col(yHistory.n_cols-1) = newY;
        sHistory.col(yHistory.n_cols-1) = newS;
        // STEP 7:
        theta = AsScalar<Dtype>((newY.t() * newY) / (newY.t() * newS));
        // W = Zero<Dtype>(yHistory.n_rows, yHistory.n_cols + sHistory.n_cols);
        W = arma::join_rows(yHistory, (theta * sHistory));
        Matrix<Dtype> A = sHistory.t() * yHistory;
        Matrix<Dtype> L = arma::trimatl(A);
        L.diag() = Zero<Dtype>(L.n_rows);
        Matrix<Dtype> MM(A.n_rows + L.n_rows, A.n_rows + L.n_cols);
        Matrix<Dtype> D(A.n_rows, A.n_cols, arma::fill::zeros);
        D.diag() = -1 * A.diag();
        MM.submat(0,0, D.n_rows-1, D.n_cols-1) = D;
        Matrix<Dtype> Lt = L.t();
        MM.submat(0, D.n_cols, Lt.n_rows-1, MM.n_cols-1) = Lt;
        MM.submat(D.n_rows, 0, MM.n_rows-1, L.n_cols-1) = L;
        MM.submat(D.n_rows, D.n_cols, MM.n_rows-1, MM.n_cols-1) = ((sHistory.t() * sHistory) * theta);
        
        Matrix<Dtype> U;
        Vector<Dtype> s;
        Matrix<Dtype> V;
        arma::svd(U, s, V, MM);
        M = V * arma::diagmat(1/s) * U.t();
      }
      Vector<Dtype> ttt = Zero<Dtype>(1, 1);
      ttt(0) = f_old - f;
      if (arma::norm(ttt) < 1e-8) {
        // successive function values too similar
        break;
      }
      k++;
    }
    x0 = x;
  }
};
}
/* namespace cppoptlib */
#endif /* LBFGSBSOLVER_H_ */
