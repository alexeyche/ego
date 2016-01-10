#include "student_t.h"

#include <ego/base/base.h>
#include <ego/base/errors.h>

#include <ego/distr/student_t.h>

#include <ego/util/log/log.h>

namespace NEgo {

	TLikStudentT::TLikStudentT(size_t dim_size)
        : ILik(dim_size)
    {
    }

    double TLikStudentT::MinValueOfNu = 1.0;

    TLogLikValue TLikStudentT::CalculateLogLikelihood(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const {
        ENSURE(GetHyperParametersSize() == 2, "Hyperparameters must be set");
        double nu = exp(GetHyperParameters()(0)) + MinValueOfNu;
        double sn2 = exp(2.0*GetHyperParameters()(1));

        double lZ = NLa::LogGamma(nu/2.0 + 0.5) - NLa::LogGamma(nu/2.0) - NLa::Log(nu * M_PI * sn2)/2.0;
                
        auto logPFun = [&](const TVectorD& Yf, const TVectorD& meanf) -> TVectorD {
            TVectorD Ymean = Yf-meanf;
            return lZ - (nu + 1.0) * NLa::Log( 1 + (Ymean % Ymean)/(nu * sn2) )/2.0;
        };


        TVectorD logP;
        if( (variance.size()>0) && (NLa::Norm(variance) > std::numeric_limits<double>::epsilon()) ) { // variance is not zero
            auto gauh = NLa::Gauher();
            const TVectorD &gauher = gauh.get<NLa::NGauherTup::quad>();

            TVectorD sig = NLa::Sqrt(variance);

            TMatrixD lZMat(Y.size(), gauher.size());
            TMatrixD meanMat = sig * NLa::Trans(gauher) + NLa::RepMat(mean, 1, lZMat.n_cols);

            for(size_t coli=0; coli<lZMat.n_cols; ++coli) {
                lZMat.col(coli) = logPFun(Y, meanMat.col(coli));
            }
            logP = NLa::LogExpAx(lZMat, gauh.get<NLa::NGauherTup::weights>());
        } else {
            logP = logPFun(Y, mean);
        }

        TVectorD r = Y - mean;
        TVectorD r2 = r % r;
        TVectorD a = r2 + nu*sn2;
        TVectorD a2 = a % a;
        TVectorD a3 = a2 % a;
                
        return TLogLikValue(
            [=]() -> TVectorD {
                return logP;
            },
            [=]() -> TVectorD {
                return (nu+1.0)*r/a;
            },
            [=]() -> TVectorD {
                return (nu+1.0)*(r2-nu*sn2)/a2;
            },
            [=]() -> TVectorD {
                return (nu+1.0) * 2.0 * (r % (r2-3.0*nu*sn2)) / a3;
            },
            [=]() -> TMatrixD {
                TMatrixD lp_dhyp(Y.size(), 2);
                lp_dhyp.col(0) = (1.0 - MinValueOfNu/nu) * (
                    nu * (NLa::Psi(nu/2.0+0.5) - NLa::Psi(nu/2.0))/2.0 - 0.5 - 
                    nu * NLa::Log(1.0 + r2/(nu*sn2))/2.0 + 
                    (nu/2.0+0.5) * r2 /(nu*sn2 + r2)
                );
                lp_dhyp.col(1) = (nu+1.0)*r2/a - 1.0;
                return lp_dhyp;
            },
            [=]() -> TMatrixD {
                TMatrixD dlp_dhyp(Y.size(), 2);
                dlp_dhyp.col(0) = (1.0 - MinValueOfNu/nu) * (
                    nu * r % ( a - sn2*(nu+1.0) )/a2
                );
                dlp_dhyp.col(1) = - (nu + 1.0) * 2.0 * nu * sn2 * r/a2;
                return dlp_dhyp;
            },
            [=]() -> TMatrixD {
                TMatrixD d2lp_dhyp(Y.size(), 2);
                d2lp_dhyp.col(0) = (1.0 - MinValueOfNu/nu) * (
                    nu * ( r2 % (r2-3.0 * sn2 * (1.0+nu)) + nu * sn2*sn2)/a3
                );
                d2lp_dhyp.col(1) = (nu+1.0) * 2.0 * nu * sn2 * (a-4.0*r2)/a3; 
                return d2lp_dhyp;
            }
        );
    }


    TPair<TVectorD, TVectorD> TLikStudentT::GetMarginalMeanAndVariance(const TVectorD &mean, const TVectorD &variance) const {
        double nu = exp(GetHyperParameters()(0)) + MinValueOfNu;
        double sn2 = exp(2.0*GetHyperParameters()(1));

        if(nu<=2.0) {
            return MakePair(mean, NLa::Inf(mean.size()));
        } else {
            return MakePair(mean, variance + nu*sn2/(nu-2.0));
        }
    }


    void TLikStudentT::SetHyperParameters(const TVectorD &params) {
    	ENSURE(params.size() == 2, "StudentT likelihood expecting two parameters");
        Params = params;
    }
    
    const TVectorD& TLikStudentT::GetHyperParameters() const {
    	return Params;
    }

    size_t TLikStudentT::GetHyperParametersSize() const {
        return 2;
    }
        
    SPtr<IDistr> TLikStudentT::GetDistribution(double mean, double sd, ui32 seed) {
        return SPtr<IDistr>(new TDistrStudentT(mean, sd, seed));
    }

} // namespace NEgo
