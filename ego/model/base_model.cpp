#include "base_model.h"


namespace NEgo {


    IModel::IModel()
        : TParent(0)
    {
        MetaEntity = true;
    }



    void IModel::SetConfig(const TModelConfig& config) {
        Config = config;
    }

    const TModelConfig& IModel::GetConfig() const {
        return Config;
    }

    

    TInfResult IModel::GetNegativeLogLik(const TVector<double>& v) {
        SetParameters(v);
        return GetNegativeLogLik();
    }

    void IModel::InitWithConfig(const TModelConfig& config, ui32 D) {
        SetData(TMatrixD(0, D), TVectorD());
        Config = config;
        auto mean = Factory.CreateMean(Config.Mean, D);
        auto cov = Factory.CreateCov(Config.Cov, D);
        auto lik = Factory.CreateLik(Config.Lik, D);
        auto inf = Factory.CreateInf(Config.Inf, mean, cov, lik);
        auto acq = Factory.CreateAcq(Config.Acq, D);
        if (Config.AcqParameters.size()>0) {
            acq->SetParameters(Config.AcqParameters);    
        }
        SetModel(mean, cov, lik, inf, acq);
    }

    bool IModel::Empty() const {
        return GetSize() == 0;
    }

    TPair<TMatrixD, TVectorD> IModel::GetData() const {
        return MakePair(GetX(), GetY());
    }

    

} // namespace NEgo
