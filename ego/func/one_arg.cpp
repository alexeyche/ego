#include "one_arg.h"

namespace NEgo {

	// template <>
	// double PartialDerivativeDefault(const double& dArg, ui32 index) {
	// 	ENSURE(index == 0, "Trying to calculate partial derivative of double function at index " << index);
	// 	return dArg;
	// }

	// template <>
	// TPair<TVectorD, TVectorD> PartialDerivativeDefault(const TPair<TVectorD, TVectorD>& dArg, ui32 index) {
	// 	return MakePair(PartialDerivativeDefault<TVectorD>(dArg.first, index), PartialDerivativeDefault<TVectorD>(dArg.second, index));
	// }

	// template <>
	// ui32 GetReturnValueSize(const double& v) {
	// 	return 1;
	// }

	// template <>
	// ui32 GetReturnValueSize(const TPair<TVectorD, TVectorD>& v) {
	// 	return v.first.size();
	// }


} // namespace NEgo