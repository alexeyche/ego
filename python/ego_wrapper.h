#pragma once

#include <ego/base/factory.h>
#include <ego/model/model.h>
#include <ego/solver/solver.h>
#include <ego/opt/opt.h>

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

class TAcqWrap {
	friend class TModelWrap;
	friend class TInfWrap;
public:
	TAcqWrap(const char* acqName, size_t dim_size, vector<double> params);

	static void ListEntities();

	TPair<TMatWrap, TMatWrap> EvaluateCriteria(const TMatWrap& m) const;
	void SetParameters(vector<double> params);

private:
	SPtr<IAcq> Acq;
};


class TInfWrap {
	friend class TModelWrap;
public:
	TInfWrap(const char* infName);
	static void ListEntities();

private:
	TString InfName;
};

class TDistrWrap {
public:
	TDistrWrap() {}

	TDistrWrap(SPtr<IDistr> distr)
		: Distr(distr)
	{
	}

	double GetMean() const;
	double GetSd() const;
private:
	SPtr<IDistr> Distr;
};

typedef double (*FOptimCallback)(const std::vector<double> &, void *);

class TSolverWrap;

class TModelWrap {
	friend TSolverWrap;
public:
	TModelWrap(TMeanWrap* mean, TCovWrap* cov, TLikWrap* lik, TInfWrap* wrap, TAcqWrap* acq);

	void SetData(const TMatWrap &x, const TMatWrap &y);

	void SetConfig(TModelConfig config);

	SPtr<IModel> GetModel();

	TVector<double> GetParameters() const;

	TPair<TMatWrap, TMatWrap> GetData() const;

	TVector<TDistrWrap> GetPrediction(const TMatWrap &x);

	void Optimize(FOptimCallback cb, void *userData);

	void Update();

	void OptimizeHypers(const TOptConfig& config);
private:
	SPtr<IModel> Model;
};

TVariable FillVariableWithType(const TVariable& var, TString type);

class TSolverWrap {
public:
	TSolverWrap(TModelWrap* model, const TSolverSpec& solverSpec);

	TSolverWrap(const TString& solverFile);

	TVector<double> GetNextPoint();

private:
	TSolver Solver;
};
