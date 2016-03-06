#include "test_framework.h"

#include <ego/util/any.h>
#include <ego/util/sobol.h>

using namespace NEgo;

TEST(AnyTest) {
    {
        TAny any;
        any.SetValue<double>(25.6);
        double r = any.GetValue<double>();
        ENSURE(r == 25.6, "Any equal to " << r);
        TAny any2 = any;
        double r2 = any2.GetValue<double>();
        ENSURE(r2 == 25.6, "Any2 equal to " << r2);
    }
    {
        TAny any;
        any.SetValue<ui32>(10);
        ui32 r = any.GetValue<ui32>();
        ENSURE(r == 10, "Any equal to " << r);
        TAny any2 = any;
        ui32 r2 = any2.GetValue<ui32>();
        ENSURE(r2 == 10, "Any2 equal to " << r2);
    }
    {
        TAny any;
        any.SetValue<int>(-10);
        int r = any.GetValue<int>();
        ENSURE(r == -10, "Any equal to " << r);
        TAny any2 = any;
        int r2 = any2.GetValue<int>();
        ENSURE(r2 == -10, "Any2 equal to " << r2);
    }
    {
        TAny any;
        any.SetValue<TString>("TestString");
        TString r = any.GetValue<TString>();
        ENSURE(r == "TestString", "Any equal to \"" << r << "\"");
        TAny any2 = any;
        TString r2 = any2.GetValue<TString>();
        ENSURE(r2 == "TestString", "Any2 equal to \"" << r2 << "\"");
    }
}


TEST(SobolSequenceTest) {
    bool test_good = false;
    ui32 iter = 0;
    while ((!test_good) && iter < 5) {
        test_good = NSobolImpl::run_test(10, 10000, /*verbose = */ false);
        if (test_good) {
            L_DEBUG << "Try " << iter << ": sobol test passed";
        } else {
            L_DEBUG << "Try " << iter << ": sobol test failed";
        }
        ++iter;
    }

    ENSURE(test_good , "Sobol sequence failed in comparing with simple random 5 times");

    L_DEBUG << "Pass";
}