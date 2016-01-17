#pragma once

#include <ego/base/factory.h>

#include "acq.h"

namespace NEgo {
	
	
	class TAcqEI : public IAcq {
	public:
		TAcqEI(size_t dimSize)
            : IAcq(dimSize)
            , Exp(1)
        {
        }
		
        TAcqEI::Result UserCalc(const TVectorD& x) const override final;

    private:
    	ui32 Exp;
	};


	REGISTER_ACQ(TAcqEI);

} // namespace NEgo