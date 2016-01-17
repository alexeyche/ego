#include "inf.h"

#include <ego/util/log/log.h>

namespace NEgo {

	size_t IInf::GetParametersSize() const {
		return
            Mean->GetParametersSize() +
            Cov->GetParametersSize() +
            Lik->GetParametersSize();
	}
    
    void IInf::SetParameters(const TVector<double>& parameters) {
    	size_t hypSizeIdx = 0;
        Mean->SetParameters(TVector<double>(
        	parameters.begin() + hypSizeIdx, parameters.begin() + hypSizeIdx + Mean->GetParametersSize()
        ));
        hypSizeIdx += Mean->GetParametersSize();

		Cov->SetParameters(TVector<double>(
        	parameters.begin() + hypSizeIdx, parameters.begin() + hypSizeIdx + Cov->GetParametersSize()
        ));
        hypSizeIdx += Cov->GetParametersSize();

        Lik->SetParameters(TVector<double>(
        	parameters.begin() + hypSizeIdx, parameters.begin() + hypSizeIdx + Lik->GetParametersSize()
        ));
        hypSizeIdx += Lik->GetParametersSize();
    }

    TVector<double> IInf::GetParameters() const {
    	auto meanHyps = Mean->GetParameters();
    	auto covHyps = Cov->GetParameters();
    	auto likHyps = Lik->GetParameters();
    	
    	TVector<double> params;
    	params.insert(params.end(), meanHyps.begin(), meanHyps.end());
    	params.insert(params.end(), covHyps.begin(), covHyps.end());
    	params.insert(params.end(), likHyps.begin(), likHyps.end());
    	return params;
    }

} // namespace NEgo
