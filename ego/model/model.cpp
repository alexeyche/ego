#include "model.h"

#include <ego/base/factory.h>

#include <ego/util/sobol.h>

#include <future>

namespace NEgo {

    TModel::TModel(const TModelConfig& config, ui32 D) 
        : MinF(MakePair(std::numeric_limits<double>::max(), 0))
    {
        InitWithConfig(config, D);
        StartParams = GetParameters();
        Sobol.Init(StartParams.size());
    }

    TModel::TModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y) 
        : MinF(MakePair(std::numeric_limits<double>::max(), 0))
    {
        InitWithConfig(config, x.n_cols);
        StartParams = GetParameters();
        Sobol.Init(StartParams.size());
        SetData(x, y);
    }

    TModel::TModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq)
        : MinF(MakePair(std::numeric_limits<double>::max(), 0))
    {
        SetModel(mean, cov, lik, inf, acq);
        StartParams = GetParameters();
        Sobol.Init(StartParams.size());
    }

    TModel::TModel(const TModel& model)
        : MinF(MakePair(std::numeric_limits<double>::max(), 0))
    {
        InitWithConfig(model.Config, model.GetDimSize());
        SetParameters(model.GetParameters());
        StartParams = GetParameters();
        Sobol.Init(StartParams.size());
        SetData(model.X, model.Y);
        Sobol = model.Sobol;
    }

    SPtr<IModel> TModel::Copy() const {
        return MakeShared(new TModel(*this));
    }

    void TModel::SetData(const TMatrixD &x, const TVectorD &y) {
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

    void TModel::SetMinimum(double v, ui32 idx) {
        MinF = MakePair(v, idx);
    }

    const double& TModel::GetMinimumY() const {
        return MinF.first;
    }

    TVectorD TModel::GetMinimumX() const {
        ENSURE(X.n_rows > 0, "Failed to find minimum for empty model");
        return X.row(MinF.second);
    }

    SPtr<ICov> TModel::GetCovariance() const {
        return Cov;
    }

    void TModel::SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) {
        Mean = mean;
        Cov = cov;
        Lik = lik;
        Inf = inf;
        Acq = acq;
        Acq->SetModel(*this);
    }

    // Functor methods

    size_t TModel::GetParametersSize() const {
        return Inf->GetParametersSize();
    }

    TVector<double> TModel::GetParameters() const {
        return Inf->GetParameters();
    }

    void TModel::SetParameters(const TVector<double>& v) {
        Inf->SetParameters(v);
    }

    SPtr<IAcq> TModel::GetAcqusitionFunction() const {
        return Acq;
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

        // NLa::DebugSave(Ks, "Ks");
        // NLa::DebugSave(ms, "ms");
        // NLa::DebugSave(kss, "kss");
        // NLa::DebugSave(Posterior->Alpha, "Alpha");
        // NLa::DebugSave(Posterior->DiagW, "DiagW");
        // NLa::DebugSave(Posterior->Linv, "Linv");
        // NLa::DebugSave(Posterior->L, "L");
        // NLa::DebugSave(Fmu, "Fmu");
        // NLa::DebugSave(V, "V");
        // NLa::DebugSave(Fs2, "Fs2");

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
            )
            .SetArgPartialDeriv(
                [=](ui32 indexRow, ui32 indexCol) -> TPair<TVectorD, TVectorD> {
                    TMatrixD KsDeriv = covRes.SecondArgPartialDeriv(indexRow, indexCol);
                    TVectorD FmuDeriv = meanRes.ArgPartialDeriv(indexRow, indexCol) + NLa::Trans(KsDeriv) * Posterior->Alpha;
                    TVectorD Fs2Deriv;
                    if(Posterior->IsCholesky) {
                        TMatrixD Vderiv = NLa::RepMat(Posterior->DiagW, 1, Xnew.n_rows) % (Posterior->Linv * KsDeriv);
                        Fs2Deriv = NLa::Diag(crossCovRes.SecondArgPartialDeriv(indexRow, indexCol)) - NLa::Trans(NLa::ColSum(2.0 * V % Vderiv));
                    } else {
                        TMatrixD Vderiv = Posterior->L * KsDeriv;
                        Fs2Deriv = NLa::Diag(crossCovRes.SecondArgPartialDeriv(indexRow, indexCol)) + NLa::Trans(NLa::ColSum(V % KsDeriv + Vderiv % Ks));
                    }
                    // NLa::DebugSave(KsDeriv, "KsDeriv");
                    // NLa::DebugSave(FmuDeriv, "FmuDeriv");
                    // NLa::DebugSave(FmuDeriv, "FsDeriv");
                    // NLa::DebugSave(likRes.FirstArgPartialDeriv(indexRow).first, "LikArgDerivMu");
                    // NLa::DebugSave(likRes.SecondArgPartialDeriv(indexRow).second, "LikArgDerivFs");
                    return MakePair(
                        FmuDeriv % likRes.FirstArgPartialDeriv(indexRow).first
                      , Fs2Deriv % likRes.SecondArgPartialDeriv(indexRow).second
                    );
                }
            );
    }

    TDistrVec TModel::GetPrediction(const TMatrixD &Xnew) {
        auto calcRes = Calc(Xnew).Value();
        return Lik->GetPredictiveDistributions(calcRes.first, calcRes.second, Config.Seed);
    }

    SPtr<IDistr> TModel::GetPointPrediction(const TVectorD& Xnew) {
        TDistrVec v = GetPrediction(NLa::Trans(Xnew));
        ENSURE(v.size() == 1, "UB");
        return v[0];
    }

    SPtr<IDistr> TModel::GetPointPredictionWithDerivative(const TVectorD& Xnew) {
        TMatrixD XnewM = NLa::Trans(Xnew);
        auto calcRes = Calc(XnewM);

        auto preds = calcRes.Value();
        ENSURE((preds.first.size() == 1) && (preds.second.size() == 1), "UB");
        ENSURE_ERR(preds.second(0) >= 0, TErrAlgebraError() << "Got negative variance, something wrong in system. X: " << NLa::VecToStr(Xnew) << ", Mean: " << preds.first(0) << ", Var: " << preds.second(0));

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

        auto d = Lik->GetDistributionsWithDerivative(mean, sd, meanDeriv, sdDeriv, Config.Seed);
        return d;
    }


    IAcq::Result TModel::CalcCriterion(const TVectorD& x) const {
        return Acq->Calc(x);
    }

    // Helpers

    TInfResult TModel::GetNegativeLogLik() const {
        return Inf->Calc(X, Y);
    }

    
    void TModel::Update() {
        // L_DEBUG << "Updating posterior with X " << X.n_rows << ":" << X.n_cols << ", Y " << Y.size();
        Posterior.emplace(Inf->Calc(X, Y).Posterior());
    }

    void TModel::SerialProcess(TProtoSerial& serial) {
        TVector<double> params;
        if (serial.IsOutput()) {
            params = GetParameters();
            Config.AcqParameters = Acq->GetParameters();
        }
        TMatrixD x(X);
        TVectorD y(Y);

        serial(Config, NEgoProto::TModelState::kModelConfigFieldNumber);
        serial(x, NEgoProto::TModelState::kXFieldNumber);
        serial(y, NEgoProto::TModelState::kYFieldNumber);
        serial(params, NEgoProto::TModelState::kParametersFieldNumber);

        if (serial.IsInput()) {
            InitWithConfig(Config, x.n_cols);
            SetParameters(params);
            SetData(x, y);
        }
    }

    void TModel::AddPoint(const TVectorD& x, double y) {
        if(y < GetMinimumY()) {
            L_DEBUG << "Got new minimum (" << y << " < " << GetMinimumY() << ")";
            SetMinimum(y, X.n_rows);
        }
        X = NLa::RowBind(X, NLa::Trans(x));
        Y = NLa::RowBind(Y, NLa::VectorFromConstant(1, y));
        // L_DEBUG << "Updating criterion";
        Acq->Update();
    }


    TMatrixD TModel::GetX() const {
        return X;
    }
    
    TVectorD TModel::GetY() const {
        return Y;
    }

    ui32 TModel::GetSize() const {
        return X.n_rows;
    }

    ui32 TModel::GetDimSize() const {
        return X.n_cols;
    }

    void TModel::OptimizeHypers(const TOptConfig& config) {
        L_DEBUG << "Going to optimize model parameters with " << config.Method;
        TVector<TVector<double>> starts;

        TVectorD startParams = NLa::StdToVec(StartParams);
        TVectorD lowBound = startParams - 0.5*startParams;
        TVectorD upBound = startParams + 0.5*startParams;
            
        // TMatrixD grid = GenerateSobolGrid(10, GetParametersSize());
        TMatrixD grid = Sobol.Sample(100);
        for (ui32 samp=0; samp < 100; ++samp) {
            starts.push_back(
                NLa::VecToStd(
                    lowBound + (upBound - lowBound) % NLa::Trans(grid.row(samp)) 
                )
            );
        }
        
        double bestOpt = std::numeric_limits<double>::max();
        TVector<double> bestParams;
        
        for (size_t iter=0; iter < 100; iter +=10) {
            TVector<TPair<std::future<TPair<TVector<double>, double>>, TVector<double>>> results;
            for (size_t minNum=0; minNum < 10; ++minNum) {
                TVector<double> start = starts[iter+minNum];

                results.push_back(std::make_pair(std::async(
                    std::launch::async,
                    [=]() {
                        try {
                            return NOpt::OptimizeModelLogLik(*this, start, config);
                        } catch (const TErrAlgebraError& err) {
                            L_DEBUG << "Got algebra error, ignoring";
                            return MakePair(TVector<double>(), std::numeric_limits<double>::max());
                        }
                    }
                ), start));
            }
            for (auto& f: results) {
                auto r = f.first.get();
                L_DEBUG << "Got result from starting at " << NLa::VecToStr(f.second) << " -> " << r.second << " at " << NLa::VecToStr(r.first);
                if (r.second < bestOpt) {
                    bestOpt = r.second;
                    bestParams = r.first;
                }
            }
        }
        ENSURE(bestParams.size() > 0, "Best optimization result is not selected");
        L_DEBUG << "Found best optimization result at " << NLa::VecToStr(bestParams) << " -> " << bestOpt;
        
        SetParameters(bestParams);
        Update();
    }


} // namespace NEgo
