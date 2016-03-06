#pragma once

#include <ego/base/base.h>
#include <ego/base/errors.h>
#include <ego/util/string.h>

#include <ego/mean/comp_mean.h>

#include <map>
#include <vector>

namespace NEgo {

	class ICov;
	class ILik;
	class IInf;
	class IAcq;
	class IBatchPolicy;
	class IModel;
	class TSolverConfig;
	class TModelConfig;
	
	class TFactory {
		template <typename BASE, template <typename> class FUN>
		using TCreateMap = std::map<TString, FUN<BASE>>;

		// Model Ctor
		template <typename BASE>
		using TModelCtor = SPtr<BASE> (*)(const TModelConfig&, size_t);

		template<typename BASE, typename INST> static SPtr<BASE> CreateCbModel(const TModelConfig& config, size_t D) { return std::move(SPtr<BASE>(new INST(config, D))); }

		// Simple Ctor
		template <typename BASE>
		using TSimpleEntityCtor = SPtr<BASE> (*)(size_t);

    	template<typename BASE, typename INST> static SPtr<BASE> CreateCbSimple(size_t dimSize) { return std::move(SPtr<BASE>(new INST(dimSize))); }

		// Inference entities Ctor

		template <typename BASE>
		using TInfCtor = SPtr<BASE> (*)(SPtr<IMean>, SPtr<ICov>, SPtr<ILik>);

		template<typename BASE, typename INST> static SPtr<BASE> CreateCbInf(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik) { return std::move(SPtr<BASE>(new INST(mean, cov, lik))); }

		// Composite entities ctor

		template <typename BASE>
		using TCompCtor = SPtr<BASE> (*)(TVector<SPtr<typename BASE::TElem>>);

		template<typename BASE, typename INST> static SPtr<BASE> CreateCbComp(TVector<SPtr<typename BASE::TElem>> elems) { return std::move(SPtr<BASE>(new INST(elems))); }

		// High order ctors

		template <typename BASE>
		using TBatchPolicyCtor = SPtr<BASE> (*)(SPtr<IModel>, const TSolverConfig&);

		template<typename BASE, typename INST> static SPtr<BASE> CreateCbBatchPolicy(SPtr<IModel> model, const TSolverConfig& config) { return std::move(SPtr<BASE>(new INST(model, config))); }

	public:
		template <typename T>
	    void RegisterModel(TString type) {
	    	ModelMap[type] = &CreateCbModel<IModel, T>;
	    }

	    template <typename T>
	    void RegisterCov(TString type) {
	    	CovMap[type] = &CreateCbSimple<ICov, T>;
	    }

		template <typename T>
	    void RegisterMean(TString type) {
	    	MeanMap[type] = &CreateCbSimple<IMean, T>;
	    }

		template <typename T>
	    void RegisterLik(TString type) {
	    	LikMap[type] = &CreateCbSimple<ILik, T>;
	    }

		template <typename T>
	    void RegisterInf(TString type) {
	    	InfMap[type] = &CreateCbInf<IInf, T>;
	    }

		template <typename T>
	    void RegisterCompMean(TString type) {
	    	CompMeanMap[type] = &CreateCbComp<ICompMean, T>;
	    }

		template <typename T>
	    void RegisterAcq(TString type) {
	    	AcqMap[type] = &CreateCbSimple<IAcq, T>;
	    }

		template <typename T>
	    void RegisterBatchPolicy(TString type) {
	    	BatchPolicyMap[type] = &CreateCbBatchPolicy<IBatchPolicy, T>;
	    }


	    template <typename R, typename T, typename ... Params>
	    SPtr<R> CreateEntity(TString name, const T &map, Params ... params) {
			auto cbPtr = map.find(name);
			ENSURE(cbPtr != map.end(), "Can't find entity with name " << name);
			return cbPtr->second(params ... );
	    }

	    SPtr<IModel> CreateModel(TString name, const TModelConfig& config, size_t dimSize);

	    SPtr<ICov> CreateCov(TString name, size_t dimSize);

	    SPtr<IMean> CreateMean(TString name, size_t dimSize);

	    SPtr<IMean> CreateSimpleMean(TString name, size_t dimSize);

	    SPtr<IMean> CreateCompMean(TString name, TVector<SPtr<IMean>> means);

	    SPtr<ILik> CreateLik(TString name, size_t dimSize);

	    SPtr<IInf> CreateInf(TString name, SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik);

	    SPtr<IAcq> CreateAcq(TString name, size_t dimSize);

	    SPtr<IBatchPolicy> CreateBatchPolicy(TString name, SPtr<IModel> model, const TSolverConfig& config);

	    static TFactory& Instance();

	    void PrintEntities();

	    template <typename T>
		TVector<TString> GetNames(const T &map) const {
	    	TVector<TString> names;
	    	for(const auto &m: map) {
	    		names.push_back(m.first);
	    	}
	    	return names;
		}

	    TVector<TString> GetCovNames() const;

	    TVector<TString> GetMeanNames() const;

	    TVector<TString> GetLikNames() const;

	    TVector<TString> GetInfNames() const;

	    TVector<TString> GetAcqNames() const;

	    TVector<TString> GetBatchPolicyNames() const;

	    TVector<TString> GetModelNames() const;

	    bool CheckInfName(const TString &s) const;
	private:
		TCreateMap<ICov, TSimpleEntityCtor> CovMap;

		TCreateMap<IMean, TSimpleEntityCtor> MeanMap;
		TCreateMap<ICompMean, TCompCtor> CompMeanMap;

		TCreateMap<ILik, TSimpleEntityCtor> LikMap;
		TCreateMap<IInf, TInfCtor> InfMap;

		TCreateMap<IAcq, TSimpleEntityCtor> AcqMap;

		TCreateMap<IBatchPolicy, TBatchPolicyCtor> BatchPolicyMap;

		TCreateMap<IModel, TModelCtor> ModelMap;
	};

	#define REGISTRATOR_CLASS(Name, SuffixToReplace, ReplaceBy) \
		template <class TObj> \
		struct T##Name##Registrator { \
			T##Name##Registrator(TString name) { \
				NStr::Replace(name, SuffixToReplace, ReplaceBy); \
				TFactory::Instance().Register##Name<TObj>(name); \
			} \
		};\

	REGISTRATOR_CLASS(Cov, "TCov", "c");
	REGISTRATOR_CLASS(Mean, "TMean", "m");
	REGISTRATOR_CLASS(CompMean, "TMean", "m");
	REGISTRATOR_CLASS(Lik, "TLik", "l");
	REGISTRATOR_CLASS(Inf, "TInf", "i");
	REGISTRATOR_CLASS(Acq, "TAcq", "a");
	REGISTRATOR_CLASS(BatchPolicy, "TBatchPolicy", "bp");
	REGISTRATOR_CLASS(Model, "T", "");

	#define REGISTER_COV(CovType) static TCovRegistrator<CovType> JOIN(TCovRegistrator, CovType)(#CovType);
	#define REGISTER_MEAN(MeanType) static TMeanRegistrator<MeanType> JOIN(TMeanRegistrator, MeanType)(#MeanType);
	#define REGISTER_LIK(LikType) static TLikRegistrator<LikType> JOIN(TLikRegistrator, LikType)(#LikType);
	#define REGISTER_INF(InfType) static TInfRegistrator<InfType> JOIN(TInfRegistrator, InfType)(#InfType);
	#define REGISTER_COMP_MEAN(CompMeanType) static TCompMeanRegistrator<CompMeanType> JOIN(TCompMeanRegistrator, CompMeanType)(#CompMeanType);
	#define REGISTER_ACQ(AcqType) static TAcqRegistrator<AcqType> JOIN(TAcqRegistrator, AcqType)(#AcqType);
	#define REGISTER_BATCH_POLICY(BatchPolicyType) static TBatchPolicyRegistrator<BatchPolicyType> JOIN(TBatchPolicyRegistrator, BatchPolicyType)(#BatchPolicyType);
	#define REGISTER_MODEL(ModelType) static TModelRegistrator<ModelType> JOIN(TModelRegistrator, ModelType)(#ModelType);
	
} // namespace NEgo

#define Factory NEgo::TFactory::Instance()