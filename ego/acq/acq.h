#pragma once

#include <ego/func/one_arg.h>
#include <ego/base/la.h>
#include <ego/util/optional.h>

namespace NEgo {

	class TModel;


    class IAcq : public TOneArgFunctor<double, TVectorD> {
    public:
        using TParent = TOneArgFunctor<double, TVectorD>;

        IAcq(size_t dimSize)
            : TParent(dimSize)
        {
        }

        virtual ~IAcq() {
        }

        void SetModel(TModel& model);

     protected:
        TOptional<TModel&> Model;
    };


} // namespace NEgo
