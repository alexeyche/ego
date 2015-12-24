#include "factory.h"

#include <ego/cov/matern.h>


namespace NEgo {

	TFactory& TFactory::Instance() {
		static TFactory _inst;
		return _inst;	
	}
	

	UPtr<ICov> TFactory::CreateCov(TString name, size_t dim_size) {
		auto cbPtr = CovMap.find(name);
		ENSURE(cbPtr != CovMap.end(), "Can't find  entity with name " << name);
		return CovMap[name](dim_size);
	}


} // namespace NEgo
	