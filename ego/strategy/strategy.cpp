#include "strategy.h"
#include "strategy_funcs.h"


#include <ego/util/protobuf.h>
#include <ego/util/sobol.h>

#include <future>

namespace NEgo {

    TStrategy::TStrategy(const TStrategyConfig& config, SPtr<TModel> model)
        : Config(config)
        , Model(model)
        , StartIterationNum(0)
        , EndIterationNum(0)
        , BatchNumber(0)
    {
        InitSamples = GenerateSobolGrid(Config.InitSampleSize, Model->GetDimSize());

        BatchPolicy = Factory.CreateBatchPolicy(Config.BatchPolicy, Model, Config);
    }

    TStrategy::TStrategy(const TStrategy& strategy) {
        (*this) = strategy;
    }

    TStrategy& TStrategy::operator=(const TStrategy& strategy) {
        if (this != &strategy) {
            BatchNumber = strategy.BatchNumber;
            StartIterationNum = strategy.StartIterationNum;
            EndIterationNum = strategy.EndIterationNum;
            InitSamples = strategy.InitSamples;
            Config = strategy.Config;
            Model = strategy.Model;
            BatchPolicy = Factory.CreateBatchPolicy(Config.BatchPolicy, Model, Config);
        }
        return *this;
    }

    const TStrategyConfig& TStrategy::GetConfig() const {
        return Config;
    }

    void TStrategy::SerialProcess(TSerializer& serial) {
        NEgoProto::TStrategyConfig protoConfig = Config.ProtoConfig;

        serial(protoConfig, NEgoProto::TStrategyState::kStrategyConfigFieldNumber);
        serial(StartIterationNum, NEgoProto::TStrategyState::kStartIterationNumFieldNumber);
        serial(EndIterationNum, NEgoProto::TStrategyState::kEndIterationNumFieldNumber);
        serial(BatchNumber, NEgoProto::TStrategyState::kBatchNumberFieldNumber);
        serial(InitSamples, NEgoProto::TStrategyState::kInitSamplesFieldNumber);

        if (serial.IsInput()) {
            Config = TStrategyConfig(protoConfig);
            BatchPolicy = Factory.CreateBatchPolicy(Config.BatchPolicy, Model, Config);
        }
    }

    void TStrategy::SetModel(SPtr<TModel> model) {
        Model = model;
    }
    
    SPtr<TModel> TStrategy::GetModel() const {
        return Model;
    }
    
    void TStrategy::OptimizeHypers(const TOptConfig& optConfig) {
        ENSURE(Model, "Model is not set while optimizing hyperparameters");

        // TMatrixD starts = GenerateSobolGrid(Config.HyperOpt.MinimizersNum, Model->GetParametersSize(), Config.HyperLowerBound, Config.HyperUpperBound);

        // TVector<std::future<TPair<TVector<double>, double>>> results;
        // for (size_t minNum=0; minNum < Config.HyperOpt.MinimizersNum; ++minNum) {
        //     results.push_back(std::async(
        //         std::launch::async,
        //         [&]() {
        //             TVectorD start = NLa::Trans(starts.row(minNum));
        //             TModel modelOpt(*Model);
        //             try {
        //                 return NOpt::OptimizeModelLogLik(modelOpt, NLa::VecToStd(start), Config.HyperOpt);
        //             } catch (const TEgoAlgebraError& err) {
        //                 L_DEBUG << "Got algebra error, ignoring";
        //                 return MakePair(TVector<double>(), std::numeric_limits<double>::max());
        //             }
        //         }
        //     ));
        // }
        // double bestNegLogLik = std::numeric_limits<double>::max();
        // TVector<double> bestParams;
        // for (auto& f: results) {
        //     auto r = f.get();
        //     L_DEBUG << "Got result from starting at " << NLa::VecToStr(r.first) << " -> " << r.second;
        //     if (r.second < bestNegLogLik) {
        //         bestNegLogLik = r.second;
        //         bestParams = r.first;
        //     }
        // }
        // ENSURE(bestParams.size() > 0, "Best optimization result is not selected");
        // L_DEBUG << "Found best optimization result at " << NLa::VecToStr(bestParams) << " -> " << bestNegLogLik;
        // Model->SetParameters(bestParams);
        // Model->Update();
        NOpt::OptimizeModelLogLik(*Model, Model->GetParameters(), optConfig);
        Model->Update();
    }

    
    void TStrategy::AddPoint(const TPoint& p, double target) {
        TGuard lock(AddPointMut);

        L_DEBUG << "Got point with id " << p.Id;
        Model->AddPoint(p.X, target);

        ++EndIterationNum;
        L_DEBUG << "Closed " << EndIterationNum << " iteration ";

        if ((EndIterationNum > 0) && (EndIterationNum > InitSamples.n_rows)) {
            if (EndIterationNum % Config.HyperOptFreq == 0) {
                L_DEBUG << "Updating model hyperparameters";
                OptimizeHypers(Config.HyperOpt);
            } else {
                L_DEBUG << "Updating model";
                Model->Update();
            }
        }
    }


    TPoint TStrategy::GetNextPoint() {
        TGuard lock(NextPointMut);

        if (StartIterationNum < InitSamples.n_rows) {
            L_DEBUG << "Going to return random next point";

            return TPoint(
                NStr::TStringBuilder() << StartIterationNum << "-init",
                InitSamples.row(StartIterationNum++)
            );
        } else 
        if (StartIterationNum == InitSamples.n_rows) {
            L_DEBUG << "Updating model hyperparameters with init samples";
            OptimizeHypers(Config.HyperOpt);
        }
        
        if ((StartIterationNum - InitSamples.n_rows) % Config.BatchSize == 0) {
            if (StartIterationNum != EndIterationNum) {
                L_DEBUG << "There are some calculation still goind on (" << StartIterationNum - EndIterationNum  << ")";
                throw TEgoNotAvailable() << "Ego is not available, waiting for " << StartIterationNum - EndIterationNum << " iterations to finish";
            }
            BatchPolicy->InitNewBatch();
            L_DEBUG << "Creating a new batch";
        }

        return TPoint(
            NStr::TStringBuilder() << StartIterationNum++ << "-" << BatchNumber, 
            BatchPolicy->GetNextElementInBatch()
        );
    }


} // namespace NEgo