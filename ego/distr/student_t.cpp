#include "student_t.h"

		
namespace NEgo {

	double TDistrStudentT::StandardPdf(double x) const {
	}
	
	double TDistrStudentT::StandardCdf(double x) const {
	}
	
    double TDistrStudentT::Sample() {
    	return Distr(Generator);
    }

} // namespace NEgo