#include "gauss.h"

#include <ego/base/la.h>

#include <cmath>

namespace NEgo {

	double TDistrGauss::StandardPdf(double x) const {
		return NLa::NormPdf(x);
	}
	
	double TDistrGauss::StandardCdf(double x) const {
		return NLa::NormCdf(x);
	}
	
    double TDistrGauss::Sample() {
    	return Distr(Generator);
    }

} // namespace NEgo