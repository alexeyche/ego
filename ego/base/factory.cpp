#include "factory.h"

#include <ego/base/entities.h>
#include <ego/util/log/log.h>
#include <ego/util/parse_parenthesis.h>


namespace NEgo {

	TFactory& TFactory::Instance() {
		static TFactory _inst;
		return _inst;
	}


    SPtr<ICov> TFactory::CreateCov(TString name, size_t dim_size) {
    	return CreateEntity<ICov>(name, CovMap, dim_size);
    }

    SPtr<IMean> TFactory::CreateMean(TString name, size_t dim_size) {
        return ParseParenthesis<SPtr<IMean>>(
            name,
            [&](TString childName) -> SPtr<IMean> {
                L_DEBUG << "Creating mean function \"" << childName << "\"";
                return Factory.CreateSimpleMean(childName, dim_size);
            },
            [&](TString parentName, TVector<SPtr<IMean>> childs) -> SPtr<IMean> {
                L_DEBUG << "Creating composite mean function \"" << parentName << "\", with childs size " << childs.size();
                return Factory.CreateCompMean(parentName, childs);
            }
        );
    }

    SPtr<IMean> TFactory::CreateSimpleMean(TString name, size_t dim_size) {
    	ENSURE(CompMeanMap.find(name) == CompMeanMap.end(), "Dealing with composite mean function " << name << " as with regular function");
    	return CreateEntity<IMean>(name, MeanMap, dim_size);
    }

    SPtr<IMean> TFactory::CreateCompMean(TString name, TVector<SPtr<IMean>> means) {
    	ENSURE(MeanMap.find(name) == MeanMap.end(), "Dealing with mean function " << name << " as with composite");
    	return CreateEntity<IMean>(name, CompMeanMap, means);
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

		PRINT_MAP(CompMeanMap, "Mean functions (composite): ");

		PRINT_MAP(LikMap, "Likelihood functions: ");
		PRINT_MAP(InfMap, "Inference methods: ");
	}


	TVector<TString> TFactory::GetCovNames() const {
    	return GetNames(CovMap);
    }

    TVector<TString> TFactory::GetMeanNames() const {
        return GetNames(MeanMap);
    }

    TVector<TString> TFactory::GetLikNames() const {
        return GetNames(LikMap);
    }

    TVector<TString> TFactory::GetInfNames() const {
        return GetNames(InfMap);
    }

} // namespace NEgo
