#include "model.h"

#include <ego/base/factory.h>


namespace NEgo {

    const double TModel::ParametersDefault = 0.0; // log(1.0)

    TModel::TModel() 
        : TParent(0)
        , MinF(std::numeric_limits<double>::max()) 
    {
        MetaEntity = true;
    }

    TModel::TModel(TModelConfig config)
        : TParent(0)
        , Config(config)
        , MinF(std::numeric_limits<double>::max())
    {
        MetaEntity = true;
        TMatrixD inputData = NLa::ReadCsv(Config.Input);
        X = NLa::HeadCols(inputData, inputData.n_cols-1);
        Y = NLa::TailCols(inputData, 1);
        
        size_t D = X.n_cols;

        Mean = Factory.CreateMean(Config.Mean, D);
        Cov = Factory.CreateCov(Config.Cov, D);
        Lik = Factory.CreateLik(Config.Lik, D);
        Inf = Factory.CreateInf(Config.Inf, Mean, Cov, Lik);
        Acq = Factory.CreateAcq(Config.Acq, D);
        Acq->SetModel(*this);

        TVector<double> v(GetParametersSize(), TModel::ParametersDefault);
        SetParameters(v);

        SetData(X, Y);
    }

    TModel::TModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq, TMatrixD x, TVectorD y) 
        : TParent(0)
        , MinF(std::numeric_limits<double>::max()) 
    {
        MetaEntity = true;
        SetModel(mean, cov, lik, inf, acq);
        
        TVector<double> v(GetParametersSize(), TModel::ParametersDefault);
        SetParameters(v);
        
        SetData(x, y);
    }

    // Setters

    void TModel::SetData(const TMatrixD &x, const TVectorD &y) {
        X = x;
        Y = y;
        MinF = NLa::Min(Y);
        DimSize = X.n_cols;

        L_DEBUG << "Got input values with size [" << X.n_rows << "x" << X.n_cols << "] and " << " target values with size [" << Y.n_rows << "x" << Y.n_cols << "] with minimum target " << MinF;

        Posterior.emplace(Inf->Calc(X, Y).Posterior());
        NOpt::OptimizeModelLogLik(*this, NOpt::MethodFromString(Config.HypOptMethod), NOpt::TOptimizeConfig(Config.HypOptMaxEval));
    }

    void TModel::SetConfig(TModelConfig config) {
        Config = config;
    }

    const double& TModel::GetMinimum() const {
        return MinF;
    }
    
    void TModel::SetMinimum(double v) {
        MinF = v;
    }
    
    void TModel::SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) {
        Mean = mean;
        Cov = cov;
        Lik = lik;
        Inf = inf;
        Acq = acq;
        Acq->SetModel(*this);
    }

    TPair<TMatrixD, TVectorD> TModel::GetData() const {
        return MakePair(X, Y);
    }

    // Functor methods

    size_t TModel::GetParametersSize() const {
        return Inf->GetParametersSize();
    }

    TVector<double> TModel::GetParameters() const {
        return Inf->GetParameters();
    }

    void TModel::SetParameters(const TVector<double> &v) {
        Inf->SetParameters(v);
    }

    TModel::Result TModel::UserCalc(const TMatrixD& Xnew) const {
        ENSURE(X.n_rows>0, "Data is not set");
        ENSURE(Posterior, "Posterior must be calculated before calling prediction methods");
        ENSURE(Posterior->L.n_rows == X.n_rows, "Posterior must be recalculated for new data");

        
        auto crossCovRes = Cov->CrossCovariance(Xnew);
        auto covRes = Cov->Calc(X, Xnew);
        auto meanRes = Mean->Calc(Xnew);

        TMatrixD Ks = covRes.Value();
        TVectorD ms = meanRes.Value();
        TVectorD kss = NLa::Diag(crossCovRes.Value());
        
        bool isCholesky = false;
        if(NLa::Sum(NLa::TriangLow(Posterior->L, true)) < std::numeric_limits<double>::epsilon()) {
            isCholesky = true;
        }

        TVectorD Fmu = ms + NLa::Trans(Ks) * Posterior->Alpha;
        TVectorD Fs2;
        TMatrixD Kinv;
        if(isCholesky) {
            Kinv = NLa::CholSolve(Posterior->L, NLa::Eye(Posterior->L.n_rows));
            
            TMatrixD V = NLa::Solve(NLa::Trans(Posterior->L), NLa::RepMat(Posterior->DiagW, 1, Xnew.n_rows) % Ks);
            Fs2 = kss - NLa::Trans(NLa::ColSum(V % V));
            // Fs2 = kss - NLa::Trans(NLa::ColSum(NLa::Trans(Ks) * Kinv * Ks));
        } else {
            TMatrixD LKs = Posterior->L * Ks;
            Fs2 = kss + NLa::Trans(NLa::ColSum(Ks % LKs));
            NLa::ForEach(Fs2, [](double &v) { if(v < 0.0) v = 0; });
        }

        return TModel::Result()
            .SetValue(
                [=]() -> TPair<TVectorD, TVectorD> {
                    return MakePair(Fmu, Fs2); 
                }
            );
            // .SetArgDeriv(
            //     [=]() -> TPair<TVectorD, TVectorD> {
            //         return MakePair(
            //             meanRes.ArgDeriv() + NLa::Trans(covRes.SecondArgDeriv()) * Posterior->Alpha
            //           , NLa::Diag(crossCovRes.SecondArgDeriv()) - NLa::Trans(NLa::ColSum(2.0 * NLa::Trans(Ks) * Kinv * covRes.SecondArgDeriv()))
            //         );
            //     }
            // );
    }

    // Helpers

    TInfResult TModel::GetNegativeLogLik(const TVector<double>& v) {
        SetParameters(v);
        return Inf->Calc(X, Y);
    }

    TInfResult TModel::GetNegativeLogLik() const {
        return Inf->Calc(X, Y);
    }


    TDistrVec TModel::GetPrediction(const TMatrixD &Xnew) {
        TVectorD Fmu, Fs2;
        Tie(Fmu, Fs2) = Calc(Xnew).Value();
        auto predDistrParams = Lik->CalculatePredictiveDistribution(Fmu, Fs2);
        return Lik->GetPredictiveDistributions(predDistrParams, Config.Seed);
    }

    SPtr<IDistr> TModel::GetPointPrediction(const TVectorD& Xnew) {
        TDistrVec v = GetPrediction(NLa::Trans(Xnew));
        ENSURE(v.size() == 1, "UB");
        return v[0];
    }
    
    void TModel::Optimize(TOptimCallback cb) {
        for(size_t iterNum=0; iterNum < Config.MaxEval; ++iterNum) {
            L_DEBUG << "Iteration number " << iterNum << ", best " << GetMinimum();
            L_DEBUG << "Optimizing acquisition function ...";

            TVectorD x;
            double crit;
            Tie(x, crit) = NOpt::OptimizeAcquisitionFunction(Acq, NOpt::MethodFromString(Config.AcqOptMethod));
            
            L_DEBUG << "Found criteria value: " << crit;
            double res = cb(x);

            L_DEBUG << "Got result: " << res;

            if(res < GetMinimum()) {
                L_DEBUG << "Got new minimum (" << res << "<" << GetMinimum() << ")";
                SetMinimum(res);
            }
            
            L_DEBUG << "Updating posterior";
            
            X = NLa::RowBind(X, NLa::Trans(x));
            Y = NLa::RowBind(Y, NLa::VectorFromConstant(1, res));

            Posterior.emplace(Inf->Calc(X, Y).Posterior());
            if((iterNum+1) % Config.HypOptFreq == 0) {
                NOpt::OptimizeModelLogLik(
                    *this, 
                    NOpt::MethodFromString(Config.HypOptMethod), 
                    NOpt::TOptimizeConfig(Config.HypOptMaxEval)
                );
            }
        }
    }

} // namespace NEgo
