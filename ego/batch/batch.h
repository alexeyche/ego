#pragma once

#include <ego/base/base.h>
#include <ego/base/la.h>
#include <ego/strategy/config.h>

namespace NEgo {

	class TStrategyConfig;
    class TModel;
    
    class IBatchPolicy {
    public:
        IBatchPolicy(SPtr<TModel> model, const TStrategyConfig& config);

        virtual ~IBatchPolicy() {
        }

        virtual void InitNewBatch() = 0;

        virtual TVectorD GetNextElementInBatch() = 0;

     protected:
        SPtr<TModel> Model;
        TStrategyConfig Config;
    };


} // namespace NEgo
