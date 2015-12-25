#include "factory.h"

#include <ego/base/entities.h>


namespace NEgo {

	TFactory& TFactory::Instance() {
		static TFactory _inst;
		return _inst;
	}


    UPtr<ICov> TFactory::CreateCov(TString name, size_t dim_size) {
    	return CreateEntity<ICov>(name, dim_size, CovMap);
    }
    UPtr<IMean> TFactory::CreateMean(TString name, size_t dim_size) {
    	return CreateEntity<IMean>(name, dim_size, MeanMap);
    }


	void TFactory::PrintEntities() {
		std::cout << "Covariances: \n";
		for(const auto& c: CovMap) {
			std::cout << "\t" << c.first << "\n";
		}

		std::cout << "Mean functions: \n";
		for(const auto& m: MeanMap) {
			std::cout << "\t" << m.first << "\n";
		}


	}

} // namespace NEgo
