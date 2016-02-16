#pragma once

#include "config.h"

#include <ego/model/model.h>

#include <ego/protos/strategy.pb.h>

namespace NEgo {

	class TStrategy : public ISerial<NEgoProto::TStrategyState> {
	public:
        TStrategy() {}

		TStrategy(const TStrategyConfig& config, SPtr<TModel> model);

        void OptimizeHypers();

        void Optimize(TOptimCallback cb);

        void OptimizeStep(TOptimCallback cb);

        void SerialProcess(TSerializer& serial) override;

        void SetModel(SPtr<TModel> model);

        TVectorD GetNextPoint();

	private:
        ui32 IterationNumber;

        TMatrixD InitSamples;

		TStrategyConfig Config;

		SPtr<TModel> Model;
	};

} // namespace NEgo