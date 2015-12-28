#pragma once

// #include <dlib/optimization.h>
#include <nlopt.hpp>

#include <ego/base/la.h>

namespace NEgo {
	class TModel;

	namespace NOpt {
		// typedef dlib::matrix<double,0,1> opt_column_vector;

		// double rosen (const opt_column_vector& m)
		// /*
		//     This function computes what is known as Rosenbrock's function.  It is
		//     a function of two input variables and has a global minimum at (1,1).
		//     So when we use this function to test out the optimization algorithms
		//     we will see that the minimum found is indeed at the point (1,1).
		// */
		// {
		//     const double x = m(0);
		//     const double y = m(1);

		//     // compute Rosenbrock's function and return the result
		//     return 100.0*pow(y - x*x,2) + pow(1 - x,2);
		// }

		// // This is a helper function used while optimizing the rosen() function.
		// const opt_column_vector rosen_derivative (const opt_column_vector& m) {
		//     const double x = m(0);
		//     const double y = m(1);

		//     // make us a column vector of length 2
		//     opt_column_vector res(2);

		//     // now compute the gradient vector
		//     res(0) = -400*x*(y-x*x) - 2*(1-x); // derivative of rosen() with respect to x
		//     res(1) = 200*(y-x*x);              // derivative of rosen() with respect to y
		//     return res;
		// }

		// std::function<double(const opt_column_vector&)> ModelCallback(std::function<double(const TModel&, const TVectorD&)> cb, const TModel &self);
		// std::function<opt_column_vector(const opt_column_vector&)> ModelCallbackDeriv(std::function<TVectorD(const TModel&, const TVectorD&)> cb, const TModel &self);

		// opt_column_vector ConvertToOptLib(const TVectorD &v);
		// TVectorD ConvertFromOptLib(const opt_column_vector &optVec);

		// TVectorD Minimize(
		// 	const TModel &self
		//   , std::function<double(const TModel&, const TVectorD&)> cb
		//   , std::function<TVectorD(const TModel&, const TVectorD&)> cbDeriv
		//   , const TVectorD &startPoint
		// );

		double NLoptMinimizer(const std::vector<double> &x, std::vector<double> &grad, void* f_data);


		TVectorD Minimize(const TModel &self, TVectorD init);

	} // namespace NOpt
} // namespace NEgo