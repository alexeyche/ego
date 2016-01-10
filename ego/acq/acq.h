#pragma once

#include <ego/base/entity.h>
#include <ego/base/value.h>

#include <ego/util/optional.h>

namespace NEgo {
	
	class TModel;

    using TDistrRet = TValue<double, TVectorD>;


    class IAcq {
    public:
        
        virtual ~IAcq() {
        }

        virtual TDistrRet EvaluateCriteria(const TVectorD& x) = 0;

     	virtual void UpdateCriteria() = 0;
     
        virtual void SetHyperParameters(const TVectorD &params) = 0;        

        virtual size_t GetHyperParametersSize() const = 0;

        virtual TVectorD GetHyperParameters() const = 0;
        
        void SetModel(TModel& model);

        size_t GetDimSize() const;
        
     protected:
        TOptional<TModel&> Model;
    };


} // namespace NEgo
