#include "sum.h"

#include <ego/base/errors.h>
#include <ego/util/log/log.h>

namespace NEgo {

    TMeanSum::TMeanSum(TVector<SPtr<IMean>> means)
        : ICompMean(means)
    {
    }

    TMeanRet TMeanSum::CalculateMean(const TMatrixD &m) {
        ENSURE(m.n_cols ==  DimSize, "Col size of input matrix are not satisfy to mean function params: " << DimSize);
        
        TVector<TMeanRet> meanValues;
        for(const auto& mean: Means) {
            meanValues.push_back(mean->CalculateMean(m));
        }

        return TMeanRet(
            [=]() -> TVectorD { 
                TVectorD accum = NLa::Zeros(m.n_rows);
                for(const auto& v: meanValues) {
                    accum += v.GetValue();
                }
                return accum; 
            }, 
            [=]() -> TMatrixD {
                TMatrixD ans;
                for(const auto& v: meanValues) {
                    ans = NLa::ColBind(ans, v.GetDerivative());
                }
                return ans;
            }
        );
    }

    void TMeanSum::SetHyperParameters(const TVectorD &params) {
        size_t nhyp = GetHyperParametersSize();
        if(nhyp != params.size()) {
            throw TEgoException() << "Mean composite function has " << Means.size() << " functions and need " << nhyp << " params to work\n"; 
        }
        size_t pIdx = 0;
        for(const auto& m: Means) {
            size_t childNHyp = m->GetHyperParametersSize();
            m->SetHyperParameters(NLa::SubVec(params, pIdx, pIdx+childNHyp));
            pIdx += childNHyp;
        }
    }
    
    size_t TMeanSum::GetHyperParametersSize() const {
        size_t hypNum = 0;
        for(const auto& m: Means) {
            hypNum += m->GetHyperParametersSize();
        }
        return hypNum;
    }

    TVectorD TMeanSum::GetHyperParameters() const {
        TVectorD params;
        for(const auto& m: Means) {
            params = NLa::RowBind(params, m->GetHyperParameters());
        }
        return params;
    }

} //namespace NEgo