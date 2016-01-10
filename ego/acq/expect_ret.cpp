#include "expect_ret.h"

#include <ego/model/model.h>

namespace NEgo {

    TDistrRet TAcqExpectRet::EvaluateCriteria(const TVectorD& x) {
    	ENSURE(Model, "Model is not set");
        SPtr<IDistr> d = Model->GetPointPrediction(x);
        return TDistrRet(
            [=]() {
                return d->GetMean();
            },
            [=]() {
                return TVectorD();
            }
        );
    }

    void TAcqExpectRet::UpdateCriteria() {

    }


	void TAcqExpectRet::SetHyperParameters(const TVectorD &params) {
	}

    size_t TAcqExpectRet::GetHyperParametersSize() const {
		return 0;    	
    }

    TVectorD TAcqExpectRet::GetHyperParameters() const {
    	return TVectorD();
    }


} // namespace NEgo
