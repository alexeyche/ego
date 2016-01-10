#pragma once

#include <ego/base/base.h>
#include <ego/util/log/log.h>

#include <cmath>
#include <limits>
#include <functional>

namespace NEgo {
	namespace NOpt {

		double sign(double v);

		template <typename Ret>
		struct TBrentMinRet {
			double Xmin;
			Ret Fmin;
			ui32 FuncCount = 0;
		};

		template <typename Ret, typename ValueName>
		TBrentMinRet<Ret> BrentMin(double xlow, double xupp, ui32 Nitmax, double tol, std::function<Ret(double)> f) {
			TBrentMinRet<Ret> ret;
			
			double fa = f(xlow).template get<ValueName>();
			double fb = f(xupp).template get<ValueName>();
			
			// std::cout  << "BrentMin, fa: " << fa << "\n";
			// std::cout  << "BrentMin, fb: " << fb << "\n";
			
			ret.FuncCount += 2;
			
			double seps = std::sqrt(std::numeric_limits<double>::min());

			double c = 0.5*(3.0 - std::sqrt(5.0));
			
			double a = xlow;
			double b = xupp;
			
			double v = a + c*(b-a);
			
			double w = v; 
			double xf = v;
			
			double d = 0.0;
			double e = 0.0;

			double x = xf;
			ret.Fmin = f(x);
			double fx = ret.Fmin.template get<ValueName>();

			// std::cout  << "BrentMin, fx: " << fx << "\n";
			
			ret.FuncCount += 1;
			
			double fv = fx;
			double fw = fx;
			double xm = 0.5 * (a+b);
			double tol1 = seps * std::abs(xf)+ tol/3.0;
			double tol2 = 2.0 * tol1;

			while (std::abs(xf-xm) > (tol2 - 0.5*(b-a))) {
				// std::cout << std::abs(xf-xm) << " > " << (tol2 - 0.5*(b-a)) << "\n";
				bool goldenSection = true;
				if (std::abs(e) > tol1) {
					goldenSection = false;

			        double r = (xf-w)*(fx-fv);
			        double q = (xf-v)*(fx-fw);
			        double p = (xf-v)*q-(xf-w)*r;
			        q = 2.0*(q-r);
			        if (q > 0.0) {
			        	p = -p;
			        }
			        q = std::abs(q);
			        r = e;  e = d;

			        // Is the parabola acceptable
			        if ( (std::abs(p)<std::abs(0.5*q*r)) && (p>q*(a-xf)) && (p<q*(b-xf)) ) {

			            // Yes, parabolic interpolation step
			            d = p/q;
			            x = xf+d;

			            // f must not be evaluated too close to ax or bx
			            if (((x-a) < tol2) || ((b-x) < tol2)) {
			                double si = sign(xm-xf);
			                if(std::abs(xm-xf) < std::numeric_limits<double>::epsilon()) {
			                	si += 1.0;
			                }
			                d = tol1*si;
			            }
			        } else {
			            // Not acceptable, must do a golden section step
			            goldenSection = true;
			        }
				}
				if (goldenSection) {
					if (xf >= xm) {
						e = a - xf;
					} else {
						e = b - xf;
					}
					d = c * e;
				}
				double si = sign(d);
				if (std::abs(d) < std::numeric_limits<double>::epsilon()) {
					si += 1.0;
				}
				x = xf + si * std::max(std::abs(d), tol1);
				ret.Fmin = f(x);
				double fu = ret.Fmin.template get<ValueName>();
				// std::cout  << "BrentMin, it: " << ret.FuncCount << ", fu: " << fu << "\n";
				ret.FuncCount += 1;
				if( std::abs(fu - 372.428) < 1e-02) {
					// std::cout <<"fcking here: " << fu << "\n";
				}
				if (fu <= fx) {
					// std::cout << fu << " <= " << fx << "\n";
					if (x >= xf) {
						a = xf;
					} else {
						b = xf;
					}
					v = w; fv = fw;
					w = xf; fw = fx;
					xf = x; fx = fu;
				} else {
					// std::cout << fu << " > " << fx << "\n";
					if (x < xf) {
						a = x;
					} else {
						b = x;
					}
					if ((fu <= fw) || (std::abs(w - xf) < std::numeric_limits<double>::epsilon())) {
						v = w; fv = fw;
						w = x; fw = fu;
 					} else
 					if ((fu <= fw) || (std::abs(v - xf) < std::numeric_limits<double>::epsilon()) || (std::abs(v - w) < std::numeric_limits<double>::epsilon())) {
 						v = x; fv = fu;
 					} 
				}
				xm = 0.5 * (a+b);
				tol1 = seps * std::abs(xf) + tol/3.0;
				tol2 = 2.0*tol1;
				if (ret.FuncCount >= Nitmax) {
					// std::cout  << "BrentMin got functions evaluations more than allowed (" << Nitmax << ")";
					break;
				}
			}
			// std::cout << std::abs(xf-xm) << " <= " << (tol2 - 0.5*(b-a)) << "\n";
				
			if ( (fa < fx) && (fa <= fb) ) {
				xf = xlow; fx = fa;
			} else 
			if (fb < fx) {
				xf = xupp; fx = fb;
			}
			ret.Xmin = xf;
			// std::cout << "BrentMin, saving " << fx << "\n";
			ret.Fmin.template get<ValueName>() = fx;
			return ret;
		}




	} // namespace NOpt
} // namespace NEgo

		