#pragma once

#include <functional>

namespace NEgo {

    template <typename V, typename dV>
    class TValue {
    private:
        using TValueCb = std::function<V()>;
        using TDerivativeCb = std::function<dV()>;

    public:
        TValue(TValueCb valueCb)
            : ValueCb(valueCb)
            , DerivativeCb([] { return dV(); })
        {
        }

        TValue(TValueCb valueCb, TDerivativeCb derivativeCb)
            : ValueCb(valueCb)
            , DerivativeCb(derivativeCb)
        {
        }

        V GetValue() const {
            return ValueCb();
        }
        dV GetDerivative() const {
            return DerivativeCb();
        }
    private:
        TValueCb ValueCb;
        TDerivativeCb DerivativeCb;
    };

} // namespace NEgo