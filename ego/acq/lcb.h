#pragma once

#include <ego/base/factory.h>

#include "acq.h"

namespace NEgo {

	class TAcqLCB : public IAcq {
	public:
		TAcqLCB(size_t dimSize)
            : IAcq(dimSize)
        {
        	Parameters = {5.0};
        }

        TAcqLCB::Result UserCalc(const TVectorD& x) const override final;
        
        size_t GetParametersSize() const override final;
	};


	REGISTER_ACQ(TAcqLCB);

} // namespace NEgo