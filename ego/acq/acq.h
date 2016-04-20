#pragma once

#include <ego/func/one_arg.h>
#include <ego/base/la.h>
#include <ego/util/optional.h>

namespace NEgo {

	class IModel;


    class IAcq : public TOneArgFunctor<double, TVectorD> {
    public:
        using TParent = TOneArgFunctor<double, TVectorD>;

        IAcq(size_t dimSize)
            : TParent(dimSize)
        {
        }

        virtual ~IAcq() {
        }
        virtual void Update() {}

        virtual void EnhanceGlobalSearch() {}
        
        void SetModel(IModel& model);

     protected:
        TOptional<IModel&> Model;
    };


} // namespace NEgo
