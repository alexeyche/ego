#include "brent_min.h"

namespace NEgo {
	namespace NOpt {

		double sign(double v) {
			if(v>std::numeric_limits<double>::min()) {
				return 1.0;
			}
			if(v<-std::numeric_limits<double>::min()) {
				return -1.0;
			}
			return 0.0;
		}



	} // namespace NOpt
} // namespace NEgo
