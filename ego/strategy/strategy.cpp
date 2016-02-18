#include "strategy.h"

#include <ego/util/protobuf.h>

#include <ego/util/sobol.h>

namespace NEgo {

    TStrategy::TStrategy(const TStrategyConfig& config, SPtr<TModel> model)
        : Config(config)
        , Model(model)
        , IterationNumber(0)
        , BatchNumber(0)
    {
        InitSamples = GenerateSobolGrid(Config.InitSamplesNum, Model->GetDimSize());
    }

    void TStrategy::SerialProcess(TSerializer& serial) {
        NEgoProto::TStrategyConfig protoConfig = Config.ProtoConfig;

        serial(protoConfig, NEgoProto::TStrategyState::kStrategyConfigFieldNumber);
        serial(IterationNumber, NEgoProto::TStrategyState::kIterationNumberFieldNumber);
        serial(BatchNumber, NEgoProto::TStrategyState::kBatchNumberFieldNumber);
        serial(InitSamples, NEgoProto::TStrategyState::kInitSamplesFieldNumber);

        if (serial.IsInput()) {
            Config = TStrategyConfig(protoConfig);
        }
    }

    void TStrategy::SetModel(SPtr<TModel> model) {
        Model = model;
    }

    void TStrategy::OptimizeHypers() {
        ENSURE(Model, "Model is not set while optimizing hyperparameters");

        NOpt::OptimizeModelLogLik(*Model, Config.HyperOpt);
        Model->Update();
    }

    void TStrategy::Optimize(TOptimCallback cb) {
        ENSURE(Model, "Model is not set while optimizing function");

        for(size_t iterNum=0; iterNum < Config.IterationsNum; ++iterNum) {
            L_DEBUG << "Iteration number " << iterNum << ", best " << Model->GetMinimumY();

            OptimizeStep(cb);

            if((iterNum+1) % Config.HyperOptFreq == 0) {
                OptimizeHypers();
            }
        }
    }

    void TStrategy::AddPoint(const TPoint& p, double target) {
        L_DEBUG << "Got point with id " << p.Id;
        Model->AddPoint(p.X, target);
    }


    TPoint TStrategy::GetNextPoint() {
        if (IterationNumber < InitSamples.n_rows) {
            L_DEBUG << "Going to return random next point";

            return TPoint(
                NStr::TStringBuilder() << IterationNumber << "-init",
                InitSamples.row(IterationNumber++)
            );
        }
        L_DEBUG << "Going to generate optimal next point";

        L_DEBUG << "Optimizing acquisition function ...";

        TVectorD x;
        double crit;
        Tie(x, crit) = NOpt::OptimizeAcquisitionFunction(Model->GetAcq(), Config.AcqOpt);
        L_DEBUG << "Found criteria value: " << crit;

        return TPoint(NStr::TStringBuilder() << IterationNumber << "-" << BatchNumber, x);
    }


    void TStrategy::OptimizeStep(TOptimCallback cb) {
        L_DEBUG << "Optimizing acquisition function ...";

        TVectorD x;
        double crit;
        Tie(x, crit) = NOpt::OptimizeAcquisitionFunction(Model->GetAcq(), Config.AcqOpt);
        L_DEBUG << "Found criteria value: " << crit;
        double res = cb(x);

        L_DEBUG << "Got result: " << res;

        Model->AddPoint(x, res);
        Model->Update();
    }

} // namespace NEgo