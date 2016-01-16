#include "link_funcs.h"

namespace NEgo {
	namespace NLinkFuncs {

		TLinkFunTup Exp(const TMatrixD &f) {
			return {
				f,
				NLa::Ones(f.n_rows, f.n_cols),
				NLa::Zeros(f.n_rows, f.n_cols),
				NLa::Zeros(f.n_rows, f.n_cols)
			};
		}

	} // namespace NLinkFuncs
} // namespace NEgo