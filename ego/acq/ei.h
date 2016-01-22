#pragma once

#include <ego/base/factory.h>

#include "acq.h"

namespace NEgo {
	
	
	class TAcqEI : public IAcq {
	public:
		TAcqEI(size_t dimSize)
            : IAcq(dimSize)
        {
        }
		
        TAcqEI::Result UserCalc(const TVectorD& x) const override final;
	};


	REGISTER_ACQ(TAcqEI);

} // namespace NEgo