#include "factory.h"

#include <ego/base/entities.h>
#include <ego/util/log/log.h>
#include <ego/util/parse_parenthesis.h>

#include <ego/model/model.h>
#include <ego/solver/config.h>

namespace NEgo {

	TFactory& TFactory::Instance() {
		static TFactory _inst;
		return _inst;
	}

    SPtr<IModel> TFactory::CreateModel(TString name, const TModelConfig& config, size_t dimSize) {
        return CreateEntity<IModel>(name, ModelMap, config, dimSize);
    }

    SPtr<ICov> TFactory::CreateCov(TString name, size_t dimSize) {
    	return CreateEntity<ICov>(name, CovMap, dimSize);
    }

    SPtr<IMean> TFactory::CreateMean(TString name, size_t dimSize) {
        return ParseParenthesis<SPtr<IMean>>(
            name,
            [&](TString childName) -> SPtr<IMean> {
                L_DEBUG << "Creating mean function \"" << childName << "\"";
                return Factory.CreateSimpleMean(childName, dimSize);
            },
            [&](TString parentName, TVector<SPtr<IMean>> childs) -> SPtr<IMean> {
                L_DEBUG << "Creating composite mean function \"" << parentName << "\", with childs size " << childs.size();
                return Factory.CreateCompMean(parentName, childs);
            }
        );
    }

    SPtr<IMean> TFactory::CreateSimpleMean(TString name, size_t dimSize) {
    	ENSURE(CompMeanMap.find(name) == CompMeanMap.end(), "Dealing with composite mean function " << name << " as with regular function");
    	return CreateEntity<IMean>(name, MeanMap, dimSize);
    }

    SPtr<IMean> TFactory::CreateCompMean(TString name, TVector<SPtr<IMean>> means) {
    	ENSURE(MeanMap.find(name) == MeanMap.end(), "Dealing with mean function " << name << " as with composite");
    	return CreateEntity<IMean>(name, CompMeanMap, means);
    }

	SPtr<ILik> TFactory::CreateLik(TString name, size_t dimSize) {
		return CreateEntity<ILik>(name, LikMap, dimSize);
	}

    SPtr<IInf> TFactory::CreateInf(TString name, SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik) {
    	return CreateEntity<IInf>(name, InfMap, mean, cov, lik);
    }
    
    SPtr<IAcq> TFactory::CreateAcq(TString name, size_t dimSize) {
        return CreateEntity<IAcq>(name, AcqMap, dimSize);
    }

    SPtr<IBatchPolicy> TFactory::CreateBatchPolicy(TString name, SPtr<IModel> model, const TSolverConfig& config) {
        return CreateEntity<IBatchPolicy>(name, BatchPolicyMap, model, config);
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
        PRINT_MAP(AcqMap, "Acquisition functions: ");
        PRINT_MAP(BatchPolicyMap, "Batch policies: ");
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
    
    TVector<TString> TFactory::GetAcqNames() const {
        return GetNames(AcqMap);
    }
    
    TVector<TString> TFactory::GetBatchPolicyNames() const {
        return GetNames(BatchPolicyMap);
    }

    TVector<TString> TFactory::GetModelNames() const {
        return GetNames(ModelMap);
    }

    bool TFactory::CheckInfName(const TString &s) const {
        return InfMap.find(s) != InfMap.end();
    }
} // namespace NEgo
