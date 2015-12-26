#include "ego_wrapper.h"


#include <ego/base/factory.h>
#include <ego/util/log/log.h>


TMatrixD TMatWrap::ToMatrix() const {
	TMatrixD m(NRows, NCols);
	for(size_t i=0; i<NRows; ++i) {
		for(size_t j=0; j<NCols; ++j) {
			m(i, j) = V[i*NCols + j];
		}
	}
	return m;
}

TMatWrap TMatWrap::FromMatrix(TMatrixD m) {
	double *v = new double[m.n_cols*m.n_rows];
	for(size_t i=0; i<m.n_rows; ++i) {
		for(size_t j=0; j<m.n_cols; ++j) {
			v[i*m.n_cols + j] = m(i, j);			
		}
	}
	TMatWrap wr(v, m.n_rows, m.n_cols);
	delete v;
	return wr;
}



TCovWrap::TCovWrap(const char* covName, size_t dim_size, vector<double> params) {
    TLog::Instance().SetLogLevel(TLog::DEBUG_LEVEL);

	Cov = Factory.CreateCov(covName, dim_size);
	L_DEBUG << "Creating covariance function: " << covName;

	Cov->SetHyperParameters(params);
}

void TCovWrap::ListEntities() {
	for(const auto &e: Factory.GetCovNames()) {
		std::cout << e << "\n";
	}
}

TMatWrap TCovWrap::CalculateKernel(const TMatWrap &left, const TMatWrap &right) const {
	NLa::Print(left.ToMatrix());
	return TMatWrap::FromMatrix(Cov->CalculateKernel(left.ToMatrix(), right.ToMatrix()));
}

TMatWrap TCovWrap::CalculateKernel(const TMatWrap &m) const {
	return CalculateKernel(m, m);
}
