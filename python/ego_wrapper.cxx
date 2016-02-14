#include "ego_wrapper.h"

#include <ego/util/pretty_print.h>

#include <ego/base/entities.h>

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

TMatWrap::TMatWrap(const TMatrixD &m)
	: V(nullptr)
	, NRows(0)
	, NCols(0)
{
	*this = FromMatrix(m);
}

TMatWrap::TMatWrap(const TMatWrap &m)
	: V(nullptr)
	, NRows(0)
	, NCols(0)
{
	*this = m;
}

TMatWrap::~TMatWrap() {
	if(V) {
		delete V;
	}
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

void SetDebugLogLevel() {
    TLog::Instance().SetLogLevel(TLog::DEBUG_LEVEL);
}


TCovWrap::TCovWrap(const char* covName, size_t dim_size, vector<double> params) {
	Cov = Factory.CreateCov(covName, dim_size);
	L_DEBUG << "Creating covariance function: " << covName;
	if(params.size()>0) {
		L_DEBUG << "Setting hyperparameters " << params;
		Cov->SetParameters(params);
	} else {
		TVector<double> p(Cov->GetParametersSize(), TModel::ParametersDefault);
		L_DEBUG << "Setting default hyperparameters " << p;
		Cov->SetParameters(p);
	}
}

void TCovWrap::ListEntities() {
	for(const auto &e: Factory.GetCovNames()) {
		std::cout << e << "\n";
	}
}

TMatWrap TCovWrap::CalculateKernel(const TMatWrap &left, const TMatWrap &right) const {
	return TMatWrap::FromMatrix(Cov->Calc(left.ToMatrix(), right.ToMatrix()).Value());
}

TMatWrap TCovWrap::CalculateKernel(const TMatWrap &m) const {
	return CalculateKernel(m, m);
}


TMeanWrap::TMeanWrap(const char* meanName, size_t dim_size, vector<double> params) {
	Mean = Factory.CreateMean(meanName, dim_size);
	L_DEBUG << "Creating mean function: " << meanName;

	if(params.size()>0) {
		L_DEBUG << "Setting hyperparameters " << NLa::Trans(NLa::StdToVec(params));
		Mean->SetParameters(params);
	} else {
		TVector<double> p(Mean->GetParametersSize(), TModel::ParametersDefault);
		L_DEBUG << "Setting default hyperparameters " << p;
		Mean->SetParameters(p);
	}
}

void TMeanWrap::ListEntities() {
	for(const auto &e: Factory.GetMeanNames()) {
		std::cout << e << "\n";
	}
}

TLikWrap::TLikWrap(const char* likName, size_t dim_size, vector<double> params) {
	Lik = Factory.CreateLik(likName, dim_size);
	L_DEBUG << "Creating likelihood function: " << likName;

	if(params.size()>0) {
		L_DEBUG << "Setting hyperparameters " << NLa::Trans(NLa::StdToVec(params));
		Lik->SetParameters(params);
	} else {
		L_DEBUG << "Using default hyperparameters " << Lik->GetParameters();
	}

}

void TLikWrap::ListEntities() {
	for(const auto &e: Factory.GetLikNames()) {
		std::cout << e << "\n";
	}
}

TAcqWrap::TAcqWrap(const char* acqName, size_t dim_size, vector<double> params) {
	Acq = Factory.CreateAcq(acqName, dim_size);
	L_DEBUG << "Creating acquisition function: " << acqName;

	if(params.size()>0) {
		L_DEBUG << "Setting hyperparameters " << NLa::Trans(NLa::StdToVec(params));
		Acq->SetParameters(params);
	} else {
		L_DEBUG << "Using default parameters for Acq: " << Acq->GetParameters();
	}
}

TPair<TMatWrap, TMatWrap> TAcqWrap::EvaluateCriteria(const TMatWrap& m) const {
	std::vector<double> resacc;
	std::vector<double> deriv;
	TMatrixD mIn = m.ToMatrix();
	for(size_t rowId = 0; rowId < mIn.n_rows; ++rowId) {
		auto res = Acq->Calc(NLa::Trans(mIn.row(rowId)));
		resacc.push_back(res.Value());
		deriv.push_back(res.ArgDeriv());
	}
	return MakePair(TMatWrap::FromMatrix(resacc), TMatWrap::FromMatrix(deriv));
}

void TAcqWrap::SetParameters(std::vector<double> params) {
	Acq->SetParameters(params);
}

void TAcqWrap::ListEntities() {
	for(const auto &e: Factory.GetAcqNames()) {
		std::cout << e << "\n";
	}
}


TInfWrap::TInfWrap(const char* infName) {
	ENSURE(Factory.CheckInfName(infName), "Can't find inference method with name: " << infName);
	L_DEBUG << "Precreating instance of inference method: " << infName;
	InfName = infName;
}

void TInfWrap::ListEntities() {
	for(const auto &e: Factory.GetInfNames()) {
		std::cout << e << "\n";
	}
}


double TDistrWrap::GetMean() const {
	ENSURE(Distr, "Distribution is not set");
	return Distr->GetMean();
}

double TDistrWrap::GetSd() const {
	ENSURE(Distr, "Distribution is not set");
	return Distr->GetSd();
}


TModelWrap::TModelWrap(TMeanWrap* mean, TCovWrap* cov, TLikWrap* lik, TInfWrap* infWrap, TAcqWrap* acq)
	: Model(
		mean->Mean,
		cov->Cov,
		lik->Lik,
		Factory.CreateInf(infWrap->InfName, mean->Mean, cov->Cov, lik->Lik),
		acq->Acq
	)
{
	L_DEBUG << "Creating model";
}

TModel& TModelWrap::GetModel() {
	return Model;
}

void TModelWrap::SetData(const TMatWrap &x, const TMatWrap &y) {
	Model.SetData(x.ToMatrix(), y.ToMatrix());
}

void TModelWrap::SetConfig(TModelConfig config) {
	Model.SetConfig(config);
}

TVector<double> TModelWrap::GetParameters() const {
	return Model.GetParameters();
}

TVector<TDistrWrap> TModelWrap::GetPrediction(const TMatWrap &x) {
	TDistrVec d = Model.GetPrediction(x.ToMatrix());
	TVector<TDistrWrap> dw(d.size());
	for(size_t di=0; di<d.size(); ++di) {
		dw[di] = TDistrWrap(d[di]);
	}
	return dw;
}

void TModelWrap::Optimize(FOptimCallback cb, void* userData) {
	Model.Optimize([&userData, &cb](const TVectorD &x) -> double {
		return cb(NLa::VecToStd(x), userData);
	});
}

TPair<TMatWrap, TMatWrap> TModelWrap::GetData() const {
	auto data = Model.GetData();
	return MakePair(TMatWrap::FromMatrix(data.first), TMatWrap::FromMatrix(data.second));
}

void TModelWrap::OptimizeHyp() {
	Model.OptimizeHyp();
}
