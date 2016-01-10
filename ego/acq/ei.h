#pragma once

#include <ego/base/factory.h>

#include "acq.h"

namespace NEgo {
	
	
	class TAcqEI : public IAcq {
	public:
		TAcqEI() : Exp(1)
		{
		}

        TDistrRet EvaluateCriteria(const TVectorD& x) override final;

     	void UpdateCriteria() override final;

        void SetHyperParameters(const TVectorD &params) override final;

        size_t GetHyperParametersSize() const override final;

        TVectorD GetHyperParameters() const override final;

    private:
    	ui32 Exp;
	};


	REGISTER_ACQ(TAcqEI);

} // namespace NEgo