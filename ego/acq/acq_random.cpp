#include "acq_random.h"

#include <ego/model/model.h>

namespace NEgo {

    TDistrRet TAcqRandom::EvaluateCriteria(const TVectorD& x) {
    	ENSURE(Model, "Model is not set");
        return TDistrRet(
            [=]() {
                return Distr(Generator);
            },
            [=]() {
                return TVectorD();
            }
        );
    }

    void TAcqRandom::UpdateCriteria() {

    }


	void TAcqRandom::SetHyperParameters(const TVectorD &params) {
	}

    size_t TAcqRandom::GetHyperParametersSize() const {
		return 0;    	
    }

    TVectorD TAcqRandom::GetHyperParameters() const {
    	return TVectorD();
    }


} // namespace NEgo
