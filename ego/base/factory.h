#pragma once

#include <ego/base/base.h>
#include <ego/base/errors.h>
#include <ego/util/string.h>

#include <map>
#include <vector>

namespace NEgo {

	class ICov;
	class IMean;
	class ILik;
	class IInf;

	class TFactory {
		template <typename BASE, template <typename> class FUN>
		using TCreateMap = std::map<TString, FUN<BASE>>;

		// Simple Ctor
		template <typename BASE>
		using TSimpleEntityCtor = SPtr<BASE> (*)(size_t);

    	template<typename BASE, typename INST> static SPtr<BASE> CreateCbSimple(size_t dim_size) { return std::move(SPtr<BASE>(new INST(dim_size))); }

		// Inference entities Ctor
		
		template <typename BASE>
		using TInfCtor = SPtr<BASE> (*)(SPtr<IMean>, SPtr<ICov>, SPtr<ILik>);

		template<typename BASE, typename INST> static SPtr<BASE> CreateCbInf(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik) { return std::move(SPtr<BASE>(new INST(mean, cov, lik))); }
	
	public:
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

	    template <typename R, typename T, typename ... Params>
	    SPtr<R> CreateEntity(TString name, const T &map, Params ... params) {
			auto cbPtr = map.find(name);
			ENSURE(cbPtr != map.end(), "Can't find  entity with name " << name);
			return cbPtr->second(std::forward<Params>(params) ... );
	    }


	    SPtr<ICov> CreateCov(TString name, size_t dim_size);

	    SPtr<IMean> CreateMean(TString name, size_t dim_size);

	    SPtr<ILik> CreateLik(TString name, size_t dim_size);

	    SPtr<IInf> CreateInf(TString name, SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik);

	    static TFactory& Instance();

	    void PrintEntities();

	    std::vector<TString> GetCovNames() const;

	private:
		TCreateMap<ICov, TSimpleEntityCtor> CovMap;
		TCreateMap<IMean, TSimpleEntityCtor> MeanMap;
		TCreateMap<ILik, TSimpleEntityCtor> LikMap;
		TCreateMap<IInf, TInfCtor> InfMap;
	};

	#define REGISTRATOR_CLASS(Name, SuffixToReplace, ReplaceBy) \
		template <class TObj> \
		struct T##Name##Registrator { \
			T##Name##Registrator(TString name) { \
				ReplaceStr(name, SuffixToReplace, ReplaceBy); \
				TFactory::Instance().Register##Name<TObj>(name); \
			} \
		};\

	REGISTRATOR_CLASS(Cov, "TCov", "c");
	REGISTRATOR_CLASS(Mean, "TMean", "m");
	REGISTRATOR_CLASS(Lik, "TLik", "l");
	REGISTRATOR_CLASS(Inf, "TInf", "i");

	#define REGISTER_COV(CovType) static TCovRegistrator<CovType> GENERATE_UNIQUE_ID(TCovRegistrator)(#CovType);
	#define REGISTER_MEAN(MeanType) static TMeanRegistrator<MeanType> GENERATE_UNIQUE_ID(TMeanRegistrator)(#MeanType);
	#define REGISTER_LIK(LikType) static TLikRegistrator<LikType> GENERATE_UNIQUE_ID(TLikRegistrator)(#LikType);
	#define REGISTER_INF(InfType) static TInfRegistrator<InfType> GENERATE_UNIQUE_ID(TInfRegistrator)(#InfType);

} // namespace NEgo

#define Factory NEgo::TFactory::Instance()