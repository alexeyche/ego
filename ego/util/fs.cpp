#include "fs.h"

#include <fstream>

namespace NEgo {

	bool FileExists(const TString& name) {
	    std::ifstream f(name.c_str());
	    if (f.good()) {
	        f.close();
	        return true;
	    } else {
	        f.close();
	        return false;
	    }
	}

} // namespace NEgo