#pragma once

#include <ego/base/factory.h>

#include <random>

#include "acq.h"

namespace NEgo {
	
	
	class TAcqRandom : public IAcq {
	public:
		TAcqRandom() 
			: Distr(0.0, 1.0)
			, Generator(1) 
		{
		}

        TDistrRet EvaluateCriteria(const TVectorD& x) override final;

     	void UpdateCriteria() override final;

        void SetHyperParameters(const TVectorD &params) override final;

        size_t GetHyperParametersSize() const override final;

        TVectorD GetHyperParameters() const override final;

    private:
    	std::uniform_real_distribution<double> Distr;
    	std::mt19937 Generator;
	};


	REGISTER_ACQ(TAcqRandom);

} // namespace NEgo