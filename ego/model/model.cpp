#include "model.h"

#include <ego/base/factory.h>


namespace NEgo {

    const double TModel::ParametersDefault = 0.0; // log(1.0)

    TModel::TModel(const TModelConfig& config, ui32 D)
        : TParent(0)
        , Config(config)
        , MinF(std::numeric_limits<double>::max())
    {
        MetaEntity = true;

        InitWithConfig(Config, D);

        TVector<double> v(GetParametersSize(), TModel::ParametersDefault);
        SetParameters(v);
    }

    TModel::TModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y)
        : TParent(0)
        , Config(config)
        , MinF(std::numeric_limits<double>::max())
    {
        MetaEntity = true;

        size_t D = x.n_cols;
        InitWithConfig(Config, D);

        TVector<double> v(GetParametersSize(), TModel::ParametersDefault);
        SetParameters(v);
        SetData(x, y);
    }

    TModel::TModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq)
        : TParent(0)
        , MinF(std::numeric_limits<double>::max())
    {
        MetaEntity = true;
        SetModel(mean, cov, lik, inf, acq);

        TVector<double> v(GetParametersSize(), TModel::ParametersDefault);
        SetParameters(v);
    }

    TModel::TModel(const TModel& model)
        : TParent(0)
    {
        MetaEntity = true;

        InitWithConfig(model.Config, model.GetDimSize());
        SetParameters(model.GetParameters());
        if (model.X.size()>0) {
            SetData(model.X, model.Y);    
        }
    }

    void TModel::InitWithConfig(const TModelConfig& config, ui32 D) {
        Config = config;
        auto mean = Factory.CreateMean(Config.Mean, D);
        auto cov = Factory.CreateCov(Config.Cov, D);
        auto lik = Factory.CreateLik(Config.Lik, D);
        auto inf = Factory.CreateInf(Config.Inf, mean, cov, lik);
        auto acq = Factory.CreateAcq(Config.Acq, D);
        SetModel(mean, cov, lik, inf, acq);
    }
    // Setters

    void TModel::SetData(const TMatrixD &x, const TVectorD &y) {
        X = x;
        Y = y;
        MinF = NLa::Min(Y);
        DimSize = X.n_cols;

        L_DEBUG << "Got input values with size [" << X.n_rows << "x" << X.n_cols << "] and " << " target values with size [" << Y.n_rows << "x" << Y.n_cols << "] with minimum target " << MinF;

        Posterior.emplace(Inf->Calc(X, Y).Posterior());
    }

    void TModel::SetConfig(const TModelConfig& config) {
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

    ui32 TModel::GetDimSize() const {
        return X.n_cols;
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

        TVectorD Fmu = ms + NLa::Trans(Ks) * Posterior->Alpha;

        TVectorD Fs2;
        TMatrixD V;

        if(Posterior->IsCholesky) {
            V = NLa::RepMat(Posterior->DiagW, 1, Xnew.n_rows) % (Posterior->Linv * Ks);
            Fs2 = kss - NLa::Trans(NLa::ColSum(V % V));
        } else {
            V = Posterior->L * Ks;
            Fs2 = kss + NLa::Trans(NLa::ColSum(Ks % V));
            NLa::ForEach(Fs2, [](double& v) { if(v < 0.0) v = 0; });
        }

        auto likRes = Lik->GetMarginalMeanAndVariance(Fmu, Fs2);

        TPair<TVectorD, TVectorD> distr = likRes.Value();

        return TModel::Result()
            .SetValue(
                [=]() -> TPair<TVectorD, TVectorD> {
                    return distr;
                }
            )
            .SetArgDeriv(
                [=]() -> TPair<TVectorD, TVectorD> {
                    TMatrixD KsDeriv = covRes.SecondArgDeriv();
                    TVectorD FmuDeriv = meanRes.ArgDeriv() + NLa::Trans(KsDeriv) * Posterior->Alpha;
                    TVectorD Fs2Deriv;
                    if(Posterior->IsCholesky) {
                        TMatrixD Vderiv = NLa::RepMat(Posterior->DiagW, 1, Xnew.n_rows) % (Posterior->Linv * KsDeriv);
                        Fs2Deriv = NLa::Diag(crossCovRes.SecondArgDeriv()) - NLa::Trans(NLa::ColSum(2.0 * V % Vderiv));
                    } else {
                        TMatrixD Vderiv = Posterior->L * KsDeriv;
                        Fs2Deriv = NLa::Diag(crossCovRes.SecondArgDeriv()) + NLa::Trans(NLa::ColSum(V % KsDeriv + Vderiv % Ks));
                    }
                    return MakePair(
                        FmuDeriv % likRes.FirstArgDeriv().first
                      , Fs2Deriv % likRes.SecondArgDeriv().second
                    );
                }
            );
    }

    // Helpers

    TInfResult TModel::GetNegativeLogLik(const TVector<double>& v) {
        SetParameters(v);
        return Inf->Calc(X, Y);
    }

    TInfResult TModel::GetNegativeLogLik() const {
        return Inf->Calc(X, Y);
    }

    void TModel::OptimizeHyp() {
        NOpt::OptimizeModelLogLik(*this, Config.HyperOpt);
        Posterior.emplace(Inf->Calc(X, Y).Posterior());
    }

    TDistrVec TModel::GetPrediction(const TMatrixD &Xnew) {
        auto calcRes = Calc(Xnew).Value();
        return Lik->GetPredictiveDistributions(calcRes.first, calcRes.second, Config.Seed);
    }

    TDistrVec TModel::GetPredictionWithDerivative(const TMatrixD &Xnew) {
        auto calcRes = Calc(Xnew);
        auto preds = calcRes.Value();
        auto predsDeriv = calcRes.ArgDeriv();
        return Lik->GetPredictiveDistributionsWithDerivative(preds.first, preds.second, predsDeriv.first, predsDeriv.second, Config.Seed);
    }

    SPtr<IDistr> TModel::GetPointPrediction(const TVectorD& Xnew) {
        TDistrVec v = GetPrediction(NLa::Trans(Xnew));
        ENSURE(v.size() == 1, "UB");
        return v[0];
    }
    SPtr<IDistr> TModel::GetPointPredictionWithDerivative(const TVectorD& Xnew) {
        TDistrVec v = GetPredictionWithDerivative(NLa::Trans(Xnew));
        ENSURE(v.size() == 1, "UB");
        return v[0];
    }


    void TModel::Optimize(TOptimCallback cb) {
        for(size_t iterNum=0; iterNum < Config.IterationsNum; ++iterNum) {
            L_DEBUG << "Iteration number " << iterNum << ", best " << GetMinimum();

            OptimizeStep(cb);

            if((iterNum+1) % Config.HyperOptFreq == 0) {
                NOpt::OptimizeModelLogLik(
                    *this,
                    Config.HyperOpt
                );
            }
        }
    }

    void TModel::OptimizeStep(TOptimCallback cb) {
        L_DEBUG << "Optimizing acquisition function ...";

        TVectorD x;
        double crit;
        Tie(x, crit) = NOpt::OptimizeAcquisitionFunction(Acq, Config.AcqOpt);
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
    }

} // namespace NEgo
