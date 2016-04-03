#include "square_dist.h"

#include <ego/util/log/log.h>

namespace NEgo {

	TSquareDistFunctor::TSquareDistFunctor(size_t dimSize)
		: TParent(dimSize)
	{
	}

	TSquareDistFunctor::Result TSquareDistFunctor::UserCalc(const TMatrixD &left, const TMatrixD &right) const {
		TVectorD left2sum = NLa::RowSum(left % left);
        TVectorD right2sum = NLa::RowSum(right % right);
        TMatrixD leftSums = NLa::RepMat(left2sum, 1, right.n_rows);
        TMatrixD rightSums = NLa::RepMat(NLa::Trans(right2sum), left.n_rows, 1);

        TMatrixD res2 = - 2.0 * left * NLa::Trans(right) + leftSums + rightSums;
        
        NLa::ForEach(res2, [](double &v) { if (v<0.0) v = 0.0; });
        TMatrixD r = NLa::Sqrt(res2);

        return TSquareDistFunctor::Result()
			.SetValue(
				[=]() -> TMatrixD {
					return r;
				}
			)
			.SetFirstArgDeriv(
				[=]() -> TMatrixD {
                    TMatrixD dR(left.n_rows, right.n_rows);
                    for(size_t ri=0; ri < left.n_rows; ++ri) {
                        for(size_t rj=0; rj < right.n_rows; ++rj) {
                        	double sum = NLa::Sum(left.row(ri) - right.row(rj));
                        	if(std::abs(sum) < std::numeric_limits<double>::epsilon()) {
                        		dR(ri, rj) = 0.0;
                        	} else {
                        		dR(ri, rj) = sum/r(ri, rj);
                        	}

                        }
                    }
                    return dR;
				}
			)
			.SetSecondArgDeriv(
				[=]() -> TMatrixD {
                    TMatrixD dR(left.n_rows, right.n_rows);
                    for(size_t ri=0; ri < left.n_rows; ++ri) {
                        for(size_t rj=0; rj < right.n_rows; ++rj) {
                        	double sum = - NLa::Sum(left.row(ri) - right.row(rj));
                            if(std::abs(sum) < std::numeric_limits<double>::epsilon()) {
                        		dR(ri, rj) = 0.0;
                        	} else {
                        		dR(ri, rj) = sum/r(ri, rj);
                        	}
                        }
                    }
                    return dR;
				}
			)
            .SetFirstArgPartialDeriv(
                [=](ui32 indexRow, ui32 indexCol) -> TMatrixD {
                    throw TErrNotImplemented() << "Not implemented";
                }
            )
            .SetSecondArgPartialDeriv(
                [=](ui32 indexRow, ui32 indexCol) -> TMatrixD {
                    TMatrixD dR = NLa::Zeros(left.n_rows, right.n_rows);
                    for(size_t ri=0; ri < left.n_rows; ++ri) {
                        double sum = - (left(ri, indexCol) - right(indexRow, indexCol));
                        if(std::abs(sum) < std::numeric_limits<double>::epsilon()) {
                            dR(ri, indexRow) = 0.0;
                        } else {
                            dR(ri, indexRow) = sum/r(ri, indexRow);
                        }
                    }
                    return dR;
                }
            );
	}


} // namespace NEgo