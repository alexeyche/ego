#include "base_model.h"


namespace NEgo {


    IModel::IModel()
        : TParent(0)
        , MinF(MakePair(std::numeric_limits<double>::max(), 0))
    {
        MetaEntity = true;
    }


    void IModel::SetData(const TMatrixD &x, const TVectorD &y) {
        X = x;
        Y = y;
        if (X.size()>0) {
            MinF = NLa::MinIdx(Y);
            DimSize = X.n_cols;
            L_DEBUG << "Got input values with size [" << X.n_rows << "x" << X.n_cols << "] and " << " target values with size [" << Y.n_rows << "x" << Y.n_cols << "] with minimum target " << MinF.first;
            Update();
        } else {
            L_DEBUG << "Got empty input values";
        }
    }

	TPair<TRefWrap<const TMatrixD>, TRefWrap<const TVectorD>> IModel::GetData() const {
        return MakePair(std::cref(X), std::cref(Y));
    }

    void IModel::SetConfig(const TModelConfig& config) {
        Config = config;
    }

    const double& IModel::GetMinimumY() const {
        return MinF.first;
    }

	TVectorD IModel::GetMinimumX() const {	
        ENSURE(X.n_rows > 0, "Failed to find minimum for empty model");
        return X.row(MinF.second);
    }

    void IModel::SetMinimum(double v, ui32 idx) {
        MinF = MakePair(v, idx);
    }

    ui32 IModel::GetDimSize() const {
        return X.n_cols;
    }

    bool IModel::Empty() const {
        return X.n_rows == 0;
    }

    TDistrVec IModel::GetPrediction(const TMatrixD &Xnew) {
        auto calcRes = Calc(Xnew).Value();
        return GetLikelihood()->GetPredictiveDistributions(calcRes.first, calcRes.second, Config.Seed);
    }

    SPtr<IDistr> IModel::GetPointPrediction(const TVectorD& Xnew) {
        TDistrVec v = GetPrediction(NLa::Trans(Xnew));
        ENSURE(v.size() == 1, "UB");
        return v[0];
    }

    SPtr<IDistr> IModel::GetPointPredictionWithDerivative(const TVectorD& Xnew) {
        TMatrixD XnewM = NLa::Trans(Xnew);
        auto calcRes = Calc(XnewM);

        auto preds = calcRes.Value();
        ENSURE((preds.first.size() == 1) && (preds.second.size() == 1), "UB");
        ENSURE_ERR(preds.second(0) >= 0, TEgoAlgebraError() << "Got negative variance, something wrong in system. X: " << NLa::VecToStr(Xnew) << ", Mean: " << preds.first(0) << ", Var: " << preds.second(0));

        double mean = preds.first(0);
        double sd = sqrt(preds.second(0));

        TVectorD meanDeriv(Xnew.size());
        TVectorD sdDeriv(Xnew.size());
        for (size_t index=0; index < Xnew.size(); ++index) {
            auto deriv = calcRes.ArgPartialDeriv(0, index);
            ENSURE((deriv.first.size() == 1) && (deriv.second.size() == 1), "UB");

            meanDeriv(index) = deriv.first(0);
            sdDeriv(index) = 0.5 * deriv.second(0) / sd;
        }

        auto d = GetLikelihood()->GetDistributionsWithDerivative(mean, sd, meanDeriv, sdDeriv, Config.Seed);
        return d;
    }


    TInfResult IModel::GetNegativeLogLik(const TVector<double>& v) {
        SetParameters(v);
        return GetNegativeLogLik();
    }

    void IModel::InitWithConfig(const TModelConfig& config, ui32 D) {
        X = TMatrixD(0, D);
        Config = config;
        auto mean = Factory.CreateMean(Config.Mean, D);
        auto cov = Factory.CreateCov(Config.Cov, D);
        auto lik = Factory.CreateLik(Config.Lik, D);
        auto inf = Factory.CreateInf(Config.Inf, mean, cov, lik);
        auto acq = Factory.CreateAcq(Config.Acq, D);
        SetModel(mean, cov, lik, inf, acq);
    }

    void IModel::SerialProcess(TSerializer& serial) {
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


} // namespace NEgo
