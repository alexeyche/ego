#pragma once

#include <ego/util/tagged_tuple.h>

#include <ego/base/la.h>

namespace NEgo {

	namespace NLinkFunTup {
		struct lp {};
		struct dlp {};
		struct d2lp {};
		struct d3lp {};
	} // namespace NLinkFunTup

	using TLinkFunTup = TTagTuple<
		NLinkFunTup::lp, TMatrixD,
		NLinkFunTup::dlp, TMatrixD,
		NLinkFunTup::d2lp, TMatrixD,
		NLinkFunTup::d3lp, TMatrixD
	>;
	using TLinkFunSpec = TLinkFunTup(*)(const TMatrixD&);

	namespace NLinkFuncs {
		
		TLinkFunTup Exp(const TMatrixD &f);

	} // namespace NLinkFuncs
}