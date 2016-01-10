#include "gamma.h"

		
namespace NEgo {

	double TDistrGamma::StandardPdf(double x) const {
	}
	
	double TDistrGamma::StandardCdf(double x) const {
	}
	
    double TDistrGamma::Sample() {
    	return Distr(Generator);
    }


} // namespace NEgo