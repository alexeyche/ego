#pragma once

#include <ego/cov/cov.h>

#include <vector>

using namespace std;
using namespace NEgo;



class TMatWrap {
public:
	TMatWrap(double *v, size_t n_rows, size_t n_cols)
		: V(nullptr)
		, NRows(n_rows)
		, NCols(n_cols)
	{
		V = new double[NRows*NCols];
		memcpy(V, v, NRows*NCols*sizeof(double));
	}

	TMatWrap() 
		: V(nullptr)
		, NRows(0)
		, NCols(0) 
		, Owner(false)
	{
	}
	
	TMatWrap(const TMatrixD m) {
		FromMatrix(m);
	}

	TMatWrap(const TMatWrap &m) {
		*this = m;		
	}
	double GetValue(size_t i, size_t j) {
		return V[i*NCols + j];
	}

	TMatWrap& operator=(const TMatWrap &m) {
	 	if(this != &m) {
	 		NRows = m.NRows;
	 		NCols = m.NCols;
		 	
	 		Owner = true;
	 		if(V) {
	 			delete V;
	 		}
			V = new double[m.NRows*m.NCols];
			memcpy(V, m.V, m.NRows*m.NCols*sizeof(double));
	 	}
	 	return *this;
	 }

	~TMatWrap() {
		delete V;
	}
	
	size_t GetNRows() {
		return NRows;
	}
	
	size_t GetNCols() {
		return NCols;
	}
	
	TMatrixD ToMatrix() const;	

	static TMatWrap FromMatrix(TMatrixD m);
	
private:
	double *V;
	size_t NRows;
	size_t NCols;
	bool Owner;
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
