#pragma once

#include <ego/base/factory.h>
#include <ego/model/model.h>

#include <vector>

using namespace std;
using namespace NEgo;



class TInfWrap;
class TModelWrap;


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

void SetDebugLogLevel();


class TCovWrap {
	friend class TModelWrap;
	friend class TInfWrap;
public:
	TCovWrap(const char* covName, size_t dim_size, vector<double> params);
	static void ListEntities();

	TMatWrap CalculateKernel(const TMatWrap &left, const TMatWrap &right) const;
	TMatWrap CalculateKernel(const TMatWrap &m) const;

private:

	SPtr<ICov> Cov;
};

class TMeanWrap {
	friend class TModelWrap;
	friend class TInfWrap;
public:
	TMeanWrap(const char* meanName, size_t dim_size, vector<double> params);
	static void ListEntities();

private:

	SPtr<IMean> Mean;
};


class TLikWrap {
	friend class TModelWrap;
	friend class TInfWrap;
public:
	TLikWrap(const char* likName, size_t dim_size, vector<double> params);
	static void ListEntities();

private:

	SPtr<ILik> Lik;
};


class TInfWrap {
	friend class TModelWrap;
public:
	TInfWrap(const char* infName);
	static void ListEntities();

private:
	TString InfName;
};


class TModelWrap {
public:
	TModelWrap(TInfWrap* wrap, TMeanWrap* mean, TCovWrap* cov, TLikWrap* lik);

	void SetData(const TMatWrap &x, const TMatWrap &y);
private:
	TModel model;
};


