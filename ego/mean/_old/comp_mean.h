#pragma once

#include <ego/base/base.h>

#include "mean.h"

namespace NEgo {

    class ICompMean : public IMean {
    public:
        using TElem = IMean;

        ICompMean(TVector<SPtr<IMean>> means)
            : IMean(0)
            , Means(means)
        {
            ENSURE(Means.size()>0, "Got composite mean function with zero sized vector of functions");
            DimSize = Means[0]->GetDimSize();
        }

    protected:

        TVector<SPtr<IMean>> Means;
    };

} // namespace NEgo