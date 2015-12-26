#include "factory.h"

#include <ego/base/entities.h>


namespace NEgo {

	TFactory& TFactory::Instance() {
		static TFactory _inst;
		return _inst;
	}


    SPtr<ICov> TFactory::CreateCov(TString name, size_t dim_size) {
    	return CreateEntity<ICov>(name, CovMap, dim_size);
    }
    SPtr<IMean> TFactory::CreateMean(TString name, size_t dim_size) {
    	return CreateEntity<IMean>(name, MeanMap, dim_size);
    }

	SPtr<ILik> TFactory::CreateLik(TString name, size_t dim_size) {
		return CreateEntity<ILik>(name, LikMap, dim_size);
	}
	
    SPtr<IInf> TFactory::CreateInf(TString name, SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik) {
    	return CreateEntity<IInf>(name, InfMap, mean, cov, lik);	
    }



	void TFactory::PrintEntities() {
		#define PRINT_MAP(M, D) {  \
			std::cout << D << "\n"; \
			for(const auto& e: M) {  \
				std::cout << "\t" << e.first << "\n"; \
			} \
			std::cout << "\n"; \
		} 

		PRINT_MAP(CovMap, "Covariance kernels: ");
		PRINT_MAP(MeanMap, "Mean functions: ");
		PRINT_MAP(LikMap, "Likelihood functions: ");
		PRINT_MAP(InfMap, "Inference methods: ");
	}


	std::vector<TString> TFactory::GetCovNames() const {
    	std::vector<TString> names;
    	for(const auto &m: CovMap) {
    		names.push_back(m.first);
    	}
    	return names;
    }

} // namespace NEgo
