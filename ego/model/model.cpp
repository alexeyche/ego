#include "model.h"

#include <ego/base/factory.h>


namespace NEgo {

    TModel::TModel(const TModelConfig& config, ui32 D) {
        InitWithConfig(config, D);
    }

    TModel::TModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y) {
        InitWithConfig(config, x.n_cols);
        SetData(x, y);
    }

    TModel::TModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) {
        SetModel(mean, cov, lik, inf, acq);
    }

    TModel::TModel(const TModel& model) {
        InitWithConfig(model.Config, model.GetDimSize());
        SetParameters(model.GetParameters());
        SetData(model.X, model.Y);
    }

    SPtr<IModel> TModel::Copy() const {
        return MakeShared(new TModel(*this));
    }

    // Setters

    void TModel::SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) {
        Mean = mean;
        Cov = cov;
        Lik = lik;
        Inf = inf;
        Acq = acq;
        Acq->SetModel(*this);
    }

    SPtr<ILik> TModel::GetLikelihood() const {
        return Lik;
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
                    return MakePair(
                        FmuDeriv % likRes.FirstArgPartialDeriv(indexRow).first
                      , Fs2Deriv % likRes.SecondArgPartialDeriv(indexRow).second
                    );
                }
            );
    }

    SPtr<IAcq> TModel::GetCriterion() const {
        return Acq;
    }

    // Helpers

    TInfResult TModel::GetNegativeLogLik() const {
        return Inf->Calc(X, Y);
    }

    
    void TModel::Update() {
        L_DEBUG << "Updating posterior";
        Posterior.emplace(Inf->Calc(X, Y).Posterior());
    }

} // namespace NEgo
