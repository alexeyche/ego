#include "ego_wrapper.h"


#include <ego/base/factory.h>
#include <ego/util/log/log.h>

// TMatWrap implementation

TMatWrap::TMatWrap(double *v, size_t n_rows, size_t n_cols)
	: V(nullptr)
	, NRows(n_rows)
	, NCols(n_cols)
{
	V = new double[NRows*NCols];
	memcpy(V, v, NRows*NCols*sizeof(double));
}

TMatWrap::TMatWrap() 
	: V(nullptr)
	, NRows(0)
	, NCols(0) 
{
}

TMatWrap::TMatWrap(const TMatrixD &m) {
	FromMatrix(m);
}

TMatWrap::TMatWrap(const TMatWrap &m) {
	*this = m;		
}

TMatWrap::~TMatWrap() {
	delete V;
}

double TMatWrap::GetValue(size_t i, size_t j) {
	return V[i*NCols + j];
}

TMatWrap& TMatWrap::operator=(const TMatWrap &m) {
 	if(this != &m) {
 		NRows = m.NRows;
 		NCols = m.NCols;
	 	
 		if(V) {
 			delete V;
 		}
		V = new double[m.NRows*m.NCols];
		memcpy(V, m.V, m.NRows*m.NCols*sizeof(double));
 	}
 	return *this;
 }


size_t TMatWrap::GetNRows() {
	return NRows;
}

size_t TMatWrap::GetNCols() {
	return NCols;
}

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

// TCovWrap implementation

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
	return TMatWrap::FromMatrix(Cov->CalculateKernel(left.ToMatrix(), right.ToMatrix()).GetValue());
}

TMatWrap TCovWrap::CalculateKernel(const TMatWrap &m) const {
	return CalculateKernel(m, m);
}
