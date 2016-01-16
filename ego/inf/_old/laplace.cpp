#include "laplace.h"

#include <ego/util/log/log.h>

#include <ego/opt/brent_min.h>

#include <math.h>

namespace NEgo {

	TInfLaplace::TInfLaplace(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik)
        : IInf(mean, cov, lik)
    {
    }

    TPsiTup TInfLaplace::Psi(const TVectorD& alpha, const TVectorD& mean, const TMatrixD& K, SPtr<ILik> lik, const TVectorD& Y) {
        // std::cout << "Psi input: "; alpha.t().print(std::cout);
        TVectorD f = K * alpha + mean;
        auto likRes = lik->CalculateLogLikelihood(Y, f);
        TVectorD dlp = likRes.GetDerivative();
        TVectorD lp = likRes.GetValue();
        TVectorD W = - likRes.GetSecondDerivative();
        
        // std::cout << "Psi return:\n\tpsi: " << NLa::AsScalar(NLa::Trans(alpha) * ((f-mean)/2.0) - NLa::Sum(lp)) << "\n";
        // std::cout << "\tdpsi: "; TVectorD(K * (alpha - dlp)).t().print( std::cout); std::cout << "\n";
        // std::cout << "\tf: "; f.t().print( std::cout);  std::cout << "\n";
        // std::cout << "\talpha: "; alpha.t().print( std::cout); std::cout << "\n";
        // std::cout << "\tdlp: "; dlp.t().print( std::cout); std::cout << "\n";
        // std::cout << "\tW: "; W.t().print( std::cout); std::cout << "\n";
        return { 
            NLa::AsScalar(NLa::Trans(alpha) * ((f-mean)/2.0) - NLa::Sum(lp))
          , K * (alpha - dlp)
          , f
          , alpha
          , dlp
          , W
        };
    }

    void TInfLaplace::IterReweightedLeastSquares(
        TVectorD& alpha
      , const TVectorD &mean
      , const TMatrixD &K
      , SPtr<ILik> lik
      , const TVectorD &Y
      , IterReweightedLeastSquaresConfig config) 
    {
        ui32 sMinLine = 0;
        ui32 sMaxLine = 2;
        ui32 nMaxLine = 10;
        double thrLine = 1e-04;
        
        TVectorD f = K * alpha + mean;
        auto likRes = lik->CalculateLogLikelihood(Y, f);
        TVectorD lp = likRes.GetValue();
        TVectorD dlp = likRes.GetDerivative();
        TVectorD d2lp = likRes.GetSecondDerivative();
        TVectorD W = -d2lp;

        ui32 n = K.n_rows;

        double psi_new = Psi(alpha, mean, K, lik, Y).get<NPsiTup::psi>();
        
        double psi_old = std::numeric_limits<double>::max();
        ui32 it = 0;
        while ( ((psi_old - psi_new) > config.Tol) && (it < config.MaxEval) ) {
            // std::cout << "alpha begin irls:";
            // alpha.t().print(std::cout);
            
            psi_old = psi_new; it++;
            
            NLa::ForEach(W, [](double &val) { 
                if (val < std::numeric_limits<double>::min()) val = 0.0; 
            });
            
            TVectorD sW = NLa::Sqrt(W);
            TMatrixD L = NLa::Chol(NLa::Eye(n) + sW * NLa::Trans(sW) % K);
            TVectorD b = W % (f-mean) + dlp;
            TVectorD dalpha = b - sW % NLa::CholSolve(L, sW % (K * b)) - alpha;
            
            // std::cout << "alpha: ";
            // alpha.t().print(std::cout); std::cout << "\n";
            // std::cout << "dalpha: ";
            // dalpha.t().print(std::cout); std::cout << "\n";
                    
            auto minRet = NOpt::BrentMin<TPsiTup, NPsiTup::psi>(
                sMinLine, sMaxLine, nMaxLine, thrLine, 
                [&](double s) -> TPsiTup {
                    return Psi(alpha + s*dalpha, mean, K, lik, Y);
                }
            );
            
            psi_new = minRet.Fmin.get<NPsiTup::psi>();
            alpha = minRet.Fmin.get<NPsiTup::alpha>();
            f = minRet.Fmin.get<NPsiTup::f>();
            dlp = minRet.Fmin.get<NPsiTup::dlp>();
            W = minRet.Fmin.get<NPsiTup::W>();
            // std::cout << "psi_new: " << psi_new << "\n";
        }
        // std::cout << "alpha: "; alpha.t().print(std::cout); std::cout << "\n";
    }

    TLogDetTup TInfLaplace::LogDet(const TMatrixD& K, const TVectorD &w) {
        size_t m = K.n_rows;
        size_t n = K.n_cols;
        ENSURE(m == n, "Input matrix must be quadratic");
        TMatrixD A = NLa::Eye(n) + K % NLa::RepMat(NLa::Trans(w), n, 1);
        auto luRes = NLa::Lu(A);
        TVectorD u = NLa::Diag(luRes.get<NLa::NLuTup::U>());
        int signU = static_cast<int>(NLa::Prod(NLa::Sign(u)));
        
        int detP = 1;
        TVectorD p = luRes.get<NLa::NLuTup::P>() * NLa::Linspace(1.0, n, n);
        for(size_t i=0; i<n; ++i) {
            if(std::abs(static_cast<double>(i+1) - p(i)) > std::numeric_limits<double>::epsilon()) {
                detP = -detP;
                auto idxRes = NLa::Find(NLa::Abs(p - (i+1.0)) < std::numeric_limits<double>::epsilon());
                if(idxRes.size() != 1) {
                    std::cerr << "UB in LogDet: " << idxRes << "\n";
                }
                ENSURE(idxRes.size() == 1, "UB in LogDet: " << idxRes);
                ui32 j = idxRes(0);
                auto tmp = p(i);
                p(i) = p(j);
                p(j) = tmp;
            }
        }
        TLogDetTup res;
        if(signU != detP) {
            res.get<NLogDetTup::ld>() = std::numeric_limits<double>::infinity();
        } else {
            res.get<NLogDetTup::ld>() = NLa::Sum(NLa::Log(NLa::Abs(u)));
        }
        res.get<NLogDetTup::inv>() = NLa::Solve(
            luRes.get<NLa::NLuTup::U>(), 
            NLa::Solve(luRes.get<NLa::NLuTup::L>(), luRes.get<NLa::NLuTup::P>())
        );
        
        res.get<NLogDetTup::mwInv>() = -NLa::RepMat(w, 1, n) % res.get<NLogDetTup::inv>();
        return res;
    }

    TInfValue TInfLaplace::CalculateNegativeLogLik(const TMatrixD &X, const TVectorD &Y) {
        ENSURE(X.n_rows == Y.n_rows, "Need X and Y with the same number of rows");

        size_t n = X.n_rows;
    	size_t D = X.n_cols;

        auto covV = Cov->CrossCovariance(X);
        auto meanV = Mean->CalculateMean(X);

        auto K = covV.Value();
    	auto m = meanV.GetValue();

        TVectorD alpha = NLa::Zeros(n);

        IterReweightedLeastSquares(alpha, m, K, Lik, Y);
        
        TVectorD f = K * alpha + m;
        // std::cout << "alpha: "; alpha.t().print(std::cout);
        // std::cout << "f: "; f.t().print(std::cout);
        auto likRes = Lik->CalculateLogLikelihood(Y, f);

        TVectorD lp = likRes.GetValue();
        TVectorD W = -likRes.GetSecondDerivative();
        
        // std::cout << "W: "; W.t().print(std::cout); std::cout << "\n";
        
        
        bool isWNeg = NLa::Any(W<0.0);
        
        TVectorD diagW = NLa::Sqrt(NLa::Abs(W)) % NLa::Sign(W);

        TMatrixD pL;
        double nlZ;
        TLogDetTup logDetRes;
        TMatrixD invLogDetRes;
        if(isWNeg) {
            logDetRes = LogDet(K, W);
            invLogDetRes = logDetRes.get<NLogDetTup::inv>();
            nlZ = NLa::AsScalar(NLa::Trans(alpha) * ((f-m)/2.0) - NLa::Sum(lp) + logDetRes.get<NLogDetTup::ld>()/2.0);
            pL = logDetRes.get<NLogDetTup::mwInv>();
        } else {
            pL = NLa::Chol(NLa::Eye(n) + diagW * NLa::Trans(diagW) % K);
            nlZ = NLa::AsScalar(NLa::Trans(alpha) * ((f-m)/2.0) + NLa::Sum(NLa::Log(NLa::Diag(pL)) - lp));
        }
        // std::cout << "nlZ: " << nlZ << ", isWNeg: " << isWNeg << "\n";

        return TInfValue(
            [=]() {
                return nlZ;
            },
            [=]() {
                TMatrixD Z;
                TVectorD g;

                if(isWNeg) {
                    Z = - pL;
                    g = NLa::RowSum(invLogDetRes % K)/2.0;
                } else {
                    Z = NLa::RepMat(diagW, 1, n) % NLa::CholSolve(pL, NLa::DiagMat(diagW));
                    TMatrixD C = NLa::Solve(NLa::Trans(pL), NLa::RepMat(diagW, 1, n) % K);
                    g = (NLa::Diag(K) - NLa::Trans(NLa::ColSum(C % C)))/2.0;
                }
                TVectorD dfhat = g % likRes.GetThirdDerivative();
                
                TVectorD dNLogLik(Mean->GetHyperParametersSize() + Cov->GetParametersSize() + Lik->GetHyperParametersSize());
                size_t hypIdx=0;

                TMatrixD meanD = meanV.GetDerivative();
                for(size_t meanHypIdx=0; meanHypIdx < Mean->GetHyperParametersSize(); ++meanHypIdx, ++hypIdx) {
                    auto dm = meanD.col(meanHypIdx);
                    dNLogLik(hypIdx) = NLa::AsScalar(
                        - NLa::Trans(alpha) * dm - NLa::Trans(dfhat) * (dm - K * (Z * dm))
                    );
                }

                TVector<TMatrixD> covD = covV.ParamDeriv();
                TVectorD dlp = likRes.GetDerivative();
                for(const auto& dK: covD) {
                    TVectorD b = dK * dlp;
                    dNLogLik(hypIdx) =  NLa::AsScalar(
                        NLa::Sum(Z % dK)/2.0 - NLa::Trans(alpha) * dK * alpha/2.0 - NLa::Trans(dfhat)*(b-K*(Z*b))
                    );
                    ++hypIdx;
                }
                
                TMatrixD dHyp = likRes.GetHyperDerivative();
                TMatrixD dFirstHyp = likRes.GetHyperFirstDerivative();
                TMatrixD dSecondHyp = likRes.GetHyperSecondDerivative();
                for(size_t likHypIdx=0; likHypIdx < Lik->GetHyperParametersSize(); ++likHypIdx, ++hypIdx) {
                    TVectorD b = K * dFirstHyp.col(likHypIdx);
                    dNLogLik(hypIdx) = NLa::AsScalar(
                        - NLa::Trans(g) * dSecondHyp.col(likHypIdx) - NLa::Sum(dHyp.col(likHypIdx)) - NLa::Trans(dfhat)*(b-K*(Z*b))
                    );
                }
                return dNLogLik;
            },
            [=]() {
                return TPosterior(pL, alpha, diagW);
            }
        );
    }
    void TInfLaplace::UpdatePosterior(const TMatrixD &X, const TVectorD &Y, TPosterior& post) {
        throw TEgoException() << "Not implemented";
    }

} // namespace NEgo
