#pragma once

#include <ego/base/entities.h>
#include <ego/base/la.h>
#include <ego/opt/opt.h>
#include <ego/protos/config.pb.h>

#include <ego/util/log/log.h>
#include <ego/util/optional.h>

#include <ego/distr/distr.h>

#include <ctime>

namespace NEgo {

    struct TModelConfig {
        TModelConfig()
            : Seed(std::time(0))
            , AcqOptMethod("GN_DIRECT")
            , HypOptMethod("CG")
            , MaxEval(20)
            , HypOptMaxEval(20)
            , HypOptFreq(1)
        {
            
        }

        TModelConfig(const NEgoProto::TModelConfig &config) {
            ENSURE(config.has_input() && config.has_output(), "Need input and output data");
            ENSURE(config.has_cov(), "Need specification of covariance in model config");
            ENSURE(config.has_mean(), "Need specification of mean in model config");
            ENSURE(config.has_lik(), "Need specification of likelihood in model config");
            ENSURE(config.has_inf(), "Need specification of inference method in config");
            ENSURE(config.has_acq(), "Need specification of acquisition function in config");

            Input = config.input();
            Output = config.output();

            Cov = config.cov();
            Mean = config.mean();
            Lik = config.lik();
            Inf = config.inf();
            Acq = config.acq();
            AcqOptMethod = config.acqopt();
            HypOptMethod = config.opt();

            if(config.has_seed()) {
                Seed = config.seed();
            } else {
                Seed = std::time(0);
            }
        }

        TString Input;
        TString Output;

        TString Cov;
        TString Mean;
        TString Lik;
        TString Inf;
        TString Acq;

        TString AcqOptMethod;
        TString HypOptMethod;
        ui32 HypOptMaxEval;
        ui32 HypOptFreq;
        ui32 Seed;

        ui32 MaxEval;
    };

    class IAcq;

    using TOptimCallback = std::function<double(const TVectorD&)>;

    class TModel {
    public:
        static constexpr double ParametersDefault = 1.0;

        TModel(TModelConfig config);
        TModel();

        TInfValue GetNegativeLogLik(const TVectorD& v);
        TInfValue GetNegativeLogLik() const;

        size_t GetHyperParametersSize() const;

        TVectorD GetHyperParameters() const;

        void SetHyperParameters(const TVectorD &v);

        size_t GetDimSize() const;

        TDistrVec GetPrediction(const TMatrixD &Xnew);

        SPtr<IDistr> GetPointPrediction(const TVectorD& Xnew);

        void SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq);

        void SetData(const TMatrixD &x, const TVectorD &y);

        TPair<TMatrixD, TVectorD> GetData() const;

        void SetConfig(TModelConfig config);

        void Optimize(TOptimCallback cb);

        const double& GetMinimum() const;
        
        void SetMinimum(double v);

    private:
        TMatrixD X;
        TVectorD Y;

        SPtr<IMean> Mean;
        SPtr<ICov> Cov;
        SPtr<ILik> Lik;
        SPtr<IInf> Inf;
        SPtr<IAcq> Acq;
        
        TModelConfig Config;

        TOptional<TPosterior> Posterior;

        double MinF;
    };

} // namespace NEgo


