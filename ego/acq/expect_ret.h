#pragma once

#include <ego/base/factory.h>

#include "acq.h"

namespace NEgo {
	
	
	class TAcqExpectRet : public IAcq {
	public:
		TAcqExpectRet()
		{
		}

        TDistrRet EvaluateCriteria(const TVectorD& x) override final;

     	void UpdateCriteria() override final;

        void SetHyperParameters(const TVectorD &params) override final;

        size_t GetHyperParametersSize() const override final;

        TVectorD GetHyperParameters() const override final;

	};


	REGISTER_ACQ(TAcqExpectRet);

} // namespace NEgo