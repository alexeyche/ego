#include "cg.h"

#include "impl.h"

#include <ego/util/log/log.h>

namespace NEgo {
	namespace NOpt {

		TCgMinimizeConfig::TCgMinimizeConfig() {
		}

		TCgMinimizeConfig::TCgMinimizeConfig(TOptimizeConfig config)
			: MaxEval(config.MaxEval)
		{
		}

		TPair<TVectorD, double> CgMinimize(const TVectorD &X, TCallback f, TCgMinimizeConfig config) {
			double f0;
			TVectorD df0;

			Tie(f0, df0) = f(X);

			TVectorD Z = X;
			double fX = f0;
			ui32 i = 0;
			L_DEBUG<< "Line search " << i << ", function value " << fX;
			i++;

			TVectorD s = -df0;
			double d0 = NLa::AsScalar(- NLa::Trans(s) * s);
			double x3 = config.FirstReduction/(1-d0);

			TVector<double> fProgress;

			bool lineSearchFailed = false;
			double f2 = f0;
			double x2 = 0.0;
			double d2 = d0;
			double d3;
			TVectorD df3 = df0;
			double f3 = NLa::AsScalar(- NLa::Trans(df3) * s);

			while (i < config.MaxEval) {
				// L_DEBUG<< "Eval " << i;
				TVectorD BestX = Z;
				double BestF = f0;
				TVectorD BestDeriv = df0;

				TVectorD X3;

				size_t m = config.MaxLineSearchEval;
				// L_DEBUG<< "Iteration number " << i << ", f2 = " << f2 << ", d2 = " << d2 << ", BestX = " << BestX(0);
				while (true) {
					x2 = 0.0; f2 = f0; d2 = d0; f3 = f0; df3 = df0;
					bool lineSearchSuccess = false;
					while (!lineSearchSuccess && (m>0)) {
						try {
							--m; i++;

							X3 = Z+x3*s;
							// L_DEBUG<< "Trying to eval with " << NLa::VecToStr(X3);
							Tie(f3, df3) = f(X3);
							// L_DEBUG<< "Eval with " << NLa::VecToStr(X3);
							// L_DEBUG<< "\tgot " << f3 << " " << NLa::VecToStr(df3);
							if(std::isnan(f3) || NLa::IsNan(df3) || std::isinf(f3)) {
								// L_DEBUG<< "Oopsie, got error while eval";
								throw TEgoException() << "Got nans";
							}
							L_DEBUG<< "Line search " << i << ", function value " << f3;

							lineSearchSuccess = true;
							// L_DEBUG<< "Line search good with function value " << f3;
						} catch(const TEgoException &e) {
							// L_DEBUG<< "Got error while evaluating function: " << e.what() << "; lets bisect and try again";
							x3 = (x2+x3)/2.0;
						}
					}
					if (f3 < BestF) {
						// L_DEBUG<< "Got goodie " << f3;
						BestX = X3;
						BestF = f3;
						BestDeriv = df3;
					}
					d3 = NLa::AsScalar(NLa::Trans(df3) * s); // new slope
					// L_DEBUG<< "new slope " << d3;
					if ( (d3 > config.Sig*d0) || (f3 > (f0 + x3*config.Rho*d0)) || (m == 0) ) {
						// L_DEBUG<< "We done extrapolating with d3 " << d3 << " and f3 " << f3;
						break;
					}
					double x1 = x2;
					double f1 = f2;
					double d1 = d2;
					// L_DEBUG<< "x1 = " << x1 << ", f1 = " << f1 << ", d1 = " << d1;
					x2 = x3; f2 = f3; d2 = d3;
					// L_DEBUG<< "x2 = " << x2 << ", f2 = " << f2 << ", d2 = " << d2;
					double A = 6.0*(f1-f2)+3.0*(d2+d1)*(x2-x1); // cubic extrapolation
					double B = 3.0*(f2-f1)-(2.0*d1+d2)*(x2-x1);
					x3 = x1-d1*((x2-x1)*(x2-x1))/(B+sqrt(B*B-A*d1*(x2-x1)));
					// L_DEBUG<< "A = " << A << ", B = " << B << ", x3 = " << x3 << ", Aleft = " << 6.0*(f1-f2) << ", Aright = " << 3.0*(d2+d1)*(x2-x1);
					// L_DEBUG<< "x3 formula, sqrt of: " << B*B-A*d1*(x2-x1);

					if (std::isnan(x3) || std::isinf(x3) || (x3 < 0)) {
						x3 = x2 * config.ExtrapolateNums;
						// L_DEBUG<< "First if " << x3;
					} else
					if (x3 > x2 * config.ExtrapolateNums) {
						x3 = x2 * config.ExtrapolateNums;
						// L_DEBUG<< "Second if " << x3;
					} else {
						double x3tmp = x2 + config.InterruptWithin*(x2 - x1);
						if(x3 < x3tmp) {
							x3 = x3tmp;
							// L_DEBUG<< "Third if " << x3;
						} else {
							// L_DEBUG<< "No if " << x3;
						}
	 				}

				}
				double x4 = x3, f4 = f3, d4 = d3;
				while (((fabs(d3) > -config.Sig*d0) || (f3 > f0 +x3 * config.Rho * d0)) && (m > 0)) {
					// L_DEBUG<< "Second loop, d3 = " << d3 << ", f3 = " << f3 << f3 << ", m = " << m;
					if ((d3 > 0.0) || (f3 > f0+x3*config.Rho*d0)) {
						x4 = x3; f4 = f3; d4 = d3;
					} else {
						x2 = x3; f2 = f3; d2 = d3;
					}
					if (f4 > f0) {
						x3 = x2-(0.5*d2*(x4-x2)*(x4-x2))/(f4-f2-d2*(x4-x2));
					} else {
						double A = 6.0*(f2-f4)/(x4-x2)+3.0*(d4+d2);
						double B = 3.0*(f4-f2)-(2.0*d2+d4)*(x4-x2);
						x3 = x2+(sqrt(B*B-A*d2*(x4-x2)*(x4-x2))-B)/A;
					}
					if (std::isnan(x3) || std::isinf(x3)) {
						x3 = (x2+x4)/2.0;
					}
					x3 = std::max(std::min(x3, x4-config.InterruptWithin*(x4-x2)), x2+config.InterruptWithin*(x4-x2));
					X3 = Z+x3*s;
					Tie(f3, df3) = f(X3);

					if (f3 < BestF) {
						BestX = X3;
						BestF = f3;
						BestDeriv = df3;
					}
					m--; i++;
					d3 = NLa::AsScalar(NLa::Trans(df3) * s);
				}
				if ((fabs(d3) < -config.Sig*d0) && (f3 < f0 + x3*config.Rho*d0)) { // line search succeed
					Z = Z + x3*s;
					f0 = f3;
					fProgress.push_back(f0);
					L_DEBUG<< "Line search " << i << ", function value " << f0;
					s = NLa::AsScalar((NLa::Trans(df3)*df3 - NLa::Trans(df0)*df3)/(NLa::Trans(df0)*df0))*s - df3;
					df0 = df3;
					d3 = d0;
					d0 = NLa::AsScalar(NLa::Trans(df0) * s);
					if (d0 > 0.0) {
						s = -df0;
						d0 = NLa::AsScalar(- NLa::Trans(s) * s);
					}
					x3 = x3 * std::min(config.MaxSlopeRatio, d3/(d0 - std::numeric_limits<double>::min()));
					lineSearchFailed = false;
				} else {
					Z = BestX;
					f0 = BestF;
					df0 = BestDeriv;
					if ((lineSearchFailed) || (i > config.MaxEval)) {
						break;
					}
					s = - df0;
					d0 = NLa::AsScalar(- NLa::Trans(s) * s);
					x3 = 1.0/(1.0 - d0);
					lineSearchFailed = true;
				}
			}
			return TPair<TVectorD, double>(Z, f0);
		}



	} // namespace NOpt
} // namespace NEgo