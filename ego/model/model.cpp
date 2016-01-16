#include "model.h"

#include <ego/base/factory.h>


namespace NEgo {

    const double TModel::ParametersDefault = 1.0;

    TModel::TModel() : MinF(std::numeric_limits<double>::max()) {
    }

    TModel::TModel(TModelConfig config)
        : Config(config), MinF(std::numeric_limits<double>::max())
    {
        TMatrixD inputData = NLa::ReadCsv(Config.Input);
        X = NLa::HeadCols(inputData, inputData.n_cols-1);
        Y = NLa::TailCols(inputData, 1);
        
        SetData(X, Y);
        
        size_t D = X.n_cols;

        Mean = Factory.CreateMean(Config.Mean, D);
        Cov = Factory.CreateCov(Config.Cov, D);
        Lik = Factory.CreateLik(Config.Lik, D);
        Inf = Factory.CreateInf(Config.Inf, Mean, Cov, Lik);
        Acq = Factory.CreateAcq(Config.Acq, D);
        Acq->SetModel(*this);

        TVector<double> v(GetHyperParametersSize(), TModel::ParametersDefault);
        SetHyperParameters(v);
    }


    TInfResult TModel::GetNegativeLogLik(const TVector<double>& v) {
        SetHyperParameters(v);
        return Inf->Calc(X, Y);
    }

    TInfResult TModel::GetNegativeLogLik() const {
        return Inf->Calc(X,Y);
    }

    size_t TModel::GetDimSize() const {
        return X.n_cols;
    }

    size_t TModel::GetHyperParametersSize() const {
        return Inf->GetParametersSize();
    }
    
    void TModel::SetHyperParameters(const TVector<double> &v) {
        Inf->SetParameters(v);
    }

    TVector<double> TModel::GetHyperParameters() const {
        return Inf->GetParameters();
    }

    void TModel::SetData(const TMatrixD &x, const TVectorD &y) {
        X = x;
        Y = y;
        MinF = NLa::Min(Y);
        L_DEBUG << "Got input values with size [" << X.n_rows << "x" << X.n_cols << "] and " << " target values with size [" << Y.n_rows << "x" << Y.n_cols << "] with minimum target " << MinF;
        NOpt::OptimizeModelLogLik(*this, NOpt::MethodFromString(Config.HypOptMethod), NOpt::TOptimizeConfig(Config.HypOptMaxEval));
    }

    void TModel::SetConfig(TModelConfig config) {
        Config = config;
    }

    const double& TModel::GetMinimum() const {
        return MinF;
    }
    
    void TModel::SetMinimum(double v) {
        MinF = v;
    }
    
    void TModel::SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) {
        Mean = mean;
        Cov = cov;
        Lik = lik;
        Inf = inf;
        Acq = acq;
        Acq->SetModel(*this);
    }


    TDistrVec TModel::GetPrediction(const TMatrixD &Xnew) {
  //       ENSURE(X.n_rows>0, "Data is not set");

  //       TVectorD kss = NLa::Diag(Cov->CrossCovariance(Xnew).Value());
  //       TMatrixD Ks = Cov->Calc(X, Xnew).Value();
  //       TVectorD ms = Mean->CalculateMean(Xnew).GetValue();
		// if(!Posterior || (Posterior->L.n_rows != X.n_rows)) {
  //           L_DEBUG << "Initializing posterior";
  //           Posterior.emplace(Inf->CalculateNegativeLogLik(X, Y).GetPosterior());    
  //       }
        
  //       bool isCholesky = false;
  //       if(NLa::Sum(NLa::TriangLow(Posterior->L, true)) < std::numeric_limits<double>::epsilon()) {
  //           // L_DEBUG << "Is Cholesky";
  //           isCholesky = true;
  //       }
  //       // L_DEBUG << "Sum triang: " << NLa::Sum(NLa::TriangLow(Posterior->L, true));

  //       TVectorD Fmu = ms + NLa::Trans(Ks) * Posterior->Alpha;
  //       TVectorD Fs2;
  //       if(isCholesky) {
  //           TMatrixD V = NLa::Solve(NLa::Trans(Posterior->L), NLa::RepMat(Posterior->DiagW, 1, Xnew.n_rows) % Ks);
  //           Fs2 = kss - NLa::Trans(NLa::ColSum(V % V));
  //       } else {
  //           TMatrixD LKs = Posterior->L * Ks;
  //           Fs2 = kss + NLa::Trans(NLa::ColSum(Ks % LKs));
  //           NLa::ForEach(Fs2, [](double &v) { if(v < 0.0) v = 0; });
  //       }
        
  //       auto predDistrParams = Lik->CalculatePredictiveDistribution(Fmu, Fs2);
  //       return Lik->GetPredictiveDistributions(predDistrParams, Config.Seed);
        return TDistrVec();
    }

    SPtr<IDistr> TModel::GetPointPrediction(const TVectorD& Xnew) {
        TDistrVec v = GetPrediction(NLa::Trans(Xnew));
        ENSURE(v.size() == 1, "UB");
        return v[0];
    }

    TPair<TMatrixD, TVectorD> TModel::GetData() const {
        return MakePair(X, Y);
    }

    void TModel::Optimize(TOptimCallback cb) {
        // for(size_t iterNum=0; iterNum < Config.MaxEval; ++iterNum) {
        //     L_DEBUG << "Iteration number " << iterNum << ", best " << GetMinimum();
        //     L_DEBUG << "Optimizing acquisition function ...";

        //     TVectorD x;
        //     double crit;
        //     Tie(x, crit) = NOpt::OptimizeAcquisitionFunction(Acq, NOpt::MethodFromString(Config.AcqOptMethod));
            
        //     L_DEBUG << "Found criteria value: " << crit;
        //     double res = cb(x);

        //     L_DEBUG << "Got result: " << res;

        //     if(res < GetMinimum()) {
        //         L_DEBUG << "Got new minimum (" << res << "<" << GetMinimum() << ")";
        //         SetMinimum(res);
        //     }
            
        //     L_DEBUG << "Updating posterior";
            
        //     X = NLa::RowBind(X, NLa::Trans(x));
        //     Y = NLa::RowBind(Y, NLa::VectorFromConstant(1, res));

        //     Posterior.emplace(Inf->CalculateNegativeLogLik(X, Y).GetPosterior());
        //     if((iterNum+1) % Config.HypOptFreq == 0) {
        //         NOpt::OptimizeModelLogLik(
        //             *this, 
        //             NOpt::MethodFromString(Config.HypOptMethod), 
        //             NOpt::TOptimizeConfig(Config.HypOptMaxEval)
        //         );
        //     }
        // }
    }

} // namespace NEgo
