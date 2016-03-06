#pragma once

#include <ego/base/base.h>
#include <ego/base/la.h>
#include <ego/solver/config.h>

namespace NEgo {

	class IModel;
    
    class IBatchPolicy {
    public:
        IBatchPolicy(SPtr<IModel> model, const TSolverConfig& config);

        virtual ~IBatchPolicy() {
        }

        virtual void InitNewBatch() = 0;

        virtual TVectorD GetNextElementInBatch() = 0;

     protected:
        SPtr<IModel> Model;
        TSolverConfig Config;
    };


} // namespace NEgo
