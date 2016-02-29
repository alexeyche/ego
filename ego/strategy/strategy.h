#pragma once

#include "config.h"

#include <ego/model/model.h>
#include <ego/util/serial.h>

#include <ego/protos/strategy.pb.h>

#include <mutex>

namespace NEgo {

    struct TPoint {
        TPoint(TString id, const TVectorD& x)
            : Id(id)
            , X(x)
        {
        }


        TString Id;
        TVectorD X;
    };

	class TStrategy : public ISerial<NEgoProto::TStrategyState> {
	public:

        TStrategy() {}

		TStrategy(const TStrategyConfig& config, SPtr<TModel> model);

        TStrategy(const TStrategy& strategy);

        TStrategy& operator=(const TStrategy& strategy);

        void OptimizeHypers(const TOptConfig& optConfig);

        void SerialProcess(TSerializer& serial) override;

        void SetModel(SPtr<TModel> model);

        SPtr<TModel> GetModel() const;

        void AddPoint(const TPoint& p, double target);

        TPoint GetNextPoint();

        const TStrategyConfig& GetConfig() const;
	private:
        ui32 BatchNumber;

        ui32 StartIterationNum;
        TMutex AddPointMut;

        ui32 EndIterationNum;
        TMutex NextPointMut;

        TMatrixD InitSamples;

		TStrategyConfig Config;

		SPtr<TModel> Model;
        SPtr<IBatchPolicy> BatchPolicy;
	};

} // namespace NEgo