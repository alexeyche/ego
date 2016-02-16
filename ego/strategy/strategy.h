#pragma once

#include "config.h"

#include <ego/model/model.h>

#include <ego/protos/strategy.pb.h>

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

        void OptimizeHypers();

        void Optimize(TOptimCallback cb);

        void OptimizeStep(TOptimCallback cb);

        void SerialProcess(TSerializer& serial) override;

        void SetModel(SPtr<TModel> model);

        void AddPoint(const TPoint& p, double target);

        TPoint GetNextPoint();

	private:
        ui32 BatchNumber;
        ui32 IterationNumber;

        TMatrixD InitSamples;

		TStrategyConfig Config;

		SPtr<TModel> Model;
	};

} // namespace NEgo