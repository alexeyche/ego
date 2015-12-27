#pragma once

#include <ego/cov/cov.h>

#include <vector>

using namespace std;
using namespace NEgo;



class TMatWrap {
public:
	TMatWrap(double *v, size_t n_rows, size_t n_cols);

	TMatWrap();
	
	TMatWrap(const TMatrixD &m);

	TMatWrap(const TMatWrap &m);
	
	~TMatWrap();

	double GetValue(size_t i, size_t j);

	TMatWrap& operator=(const TMatWrap &m);

	size_t GetNRows();
	
	size_t GetNCols();
	
	TMatrixD ToMatrix() const;	

	static TMatWrap FromMatrix(TMatrixD m);
private:
	double *V;
	size_t NRows;
	size_t NCols;
};





class TCovWrap {
public:
	TCovWrap(const char* covName, size_t dim_size, vector<double> params);
	static void ListEntities();
	
	TMatWrap CalculateKernel(const TMatWrap &left, const TMatWrap &right) const;
	TMatWrap CalculateKernel(const TMatWrap &m) const;

private:

	SPtr<ICov> Cov;
};
