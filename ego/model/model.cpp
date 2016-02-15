#include "model.h"

#include <ego/base/factory.h>


namespace NEgo {

    const double TModel::ParametersDefault = 0.0; // log(1.0)

    TModel::TModel()
        : TParent(0)
    {
        MetaEntity = true;
    }

    TModel::TModel(const TModelConfig& config, ui32 D)
        : TParent(0)
        , Config(config)
        , MinF(MakePair(std::numeric_limits<double>::max(), 0))
    {
        MetaEntity = true;

        InitWithConfig(Config, D);

        X = TMatrixD(0, D);

        TVector<double> v(GetParametersSize(), TModel::ParametersDefault);
        SetParameters(v);
    }

    TModel::TModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y)
        : TParent(0)
        , Config(config)
        , MinF(MakePair(std::numeric_limits<double>::max(), 0))
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
        , MinF(MakePair(std::numeric_limits<double>::max(), 0))
    {
        MetaEntity = true;
        SetModel(mean, cov, lik, inf, acq);
    }

    TModel::TModel(const TModel& model)
        : TParent(0)
    {
        MetaEntity = true;

        InitWithConfig(model.Config, model.GetDimSize());
        SetParameters(model.GetParameters());
        SetData(model.X, model.Y);
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
        if (X.size()>0) {
            MinF = NLa::MinIdx(Y);
            DimSize = X.n_cols;
            L_DEBUG << "Got input values with size [" << X.n_rows << "x" << X.n_cols << "] and " << " target values with size [" << Y.n_rows << "x" << Y.n_cols << "] with minimum target " << MinF.first;
            Posterior.emplace(Inf->Calc(X, Y).Posterior());
        } else {
            L_DEBUG << "Got empty input values";
        }
    }

    void TModel::SetConfig(const TModelConfig& config) {
        Config = config;
    }

    const double& TModel::GetMinimumY() const {
        return MinF.first;
    }

    TVectorD TModel::GetMinimumX() const {
        ENSURE(X.n_rows > 0, "Failed to find minimum for empty model");
        return X.row(MinF.second);
    }

    void TModel::SetMinimum(double v, ui32 idx) {
        MinF = MakePair(v, idx);
    }

    void TModel::SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) {
        Mean = mean;
        Cov = cov;
        Lik = lik;
        Inf = inf;
        Acq = acq;
        Acq->SetModel(*this);
    }

    TPair<TRefWrap<const TMatrixD>, TRefWrap<const TVectorD>> TModel::GetData() const {
        return MakePair(std::cref(X), std::cref(Y));
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
            L_DEBUG << "Non cholesky matrix";
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

    SPtr<IAcq> TModel::GetAcq() const {
        return Acq;
    }

    void TModel::AddPoint(const TVectorD& x, double y) {
        if(y < GetMinimumY()) {
            L_DEBUG << "Got new minimum (" << y << " < " << GetMinimumY() << ")";
            SetMinimum(y, X.n_rows);
        }
        X = NLa::RowBind(X, NLa::Trans(x));
        Y = NLa::RowBind(Y, NLa::VectorFromConstant(1, y));
    }

    void TModel::Update() {
        L_DEBUG << "Updating posterior";
        Posterior.emplace(Inf->Calc(X, Y).Posterior());
    }

    void TModel::SerialProcess(TSerializer& serial) {
        NEgoProto::TModelConfig protoConfig = Config.ProtoConfig;
        TVector<double> params;

        if (serial.IsOutput()) {
            params = GetParameters();
        }
        TMatrixD x(X);
        TVectorD y(Y);

        serial(protoConfig, NEgoProto::TModelState::kModelConfigFieldNumber);
        serial(x, NEgoProto::TModelState::kXFieldNumber);
        serial(y, NEgoProto::TModelState::kYFieldNumber);
        serial(params, NEgoProto::TModelState::kParametersFieldNumber);

        if (serial.IsInput()) {
            TModelConfig newConfig(protoConfig);
            InitWithConfig(newConfig, x.n_cols);
            SetParameters(params);
            SetData(x, y);
        }
    }

    bool TModel::Empty() const {
        return X.n_rows == 0;
    }

} // namespace NEgo
