#include "model.h"


namespace NEgo {

    TModel::TModel(TModelConfig config)
        : Config(config)
    {
        TMatrixD inputData = NLa::ReadCsv(Config.Input);
        X = NLa::HeadCols(inputData, inputData.n_cols-1);
        Y = NLa::TailCols(inputData, 1);
        L_DEBUG << "Got input values with size [" << X.n_rows << "x" << X.n_cols << "] and " << " target values with size [" << Y.n_rows << "x" << Y.n_cols << "]";

        size_t D = X.n_cols;

        Mean = ParseParenthesis<SPtr<IMean>>(
            Config.Mean,
            [&](TString childName) -> SPtr<IMean> {
                L_DEBUG << "Creating mean function \"" << childName << "\"";
                return Factory.CreateMean(childName, D);
            },
            [&](TString parentName, TVector<SPtr<IMean>> childs) -> SPtr<IMean> {
                L_DEBUG << "Creating composite mean function \"" << parentName << "\", with childs size " << childs.size();
                return Factory.CreateCompMean(parentName, childs);
            }
        );

        Cov = Factory.CreateCov(Config.Cov, D);
        Lik = Factory.CreateLik(Config.Lik, D);
        Inf = Factory.CreateInf(Config.Inf, Mean, Cov, Lik);
        
    }

    TInfValue TModel::GetNegativeLogLik(const TVectorD& v) const {
        size_t hypSizeIdx = 0;

        Mean->SetHyperParameters(NLa::SubVec(v, hypSizeIdx, hypSizeIdx + Mean->GetHyperParametersSize()));
        hypSizeIdx += Mean->GetHyperParametersSize();

        Cov->SetHyperParameters(NLa::SubVec(v, hypSizeIdx, hypSizeIdx + Cov->GetHyperParametersSize()));
        hypSizeIdx += Cov->GetHyperParametersSize();

        Lik->SetHyperParameters(NLa::SubVec(v, hypSizeIdx, hypSizeIdx + Lik->GetHyperParametersSize()));
        hypSizeIdx += Lik->GetHyperParametersSize();

        return Inf->CalculateNegativeLogLik(X, Y);
    }

    size_t TModel::GetDimSize() const {
        return X.n_cols;
    }

    size_t TModel::GetHyperParametersSize() const {
        return
            Mean->GetHyperParametersSize() +
            Cov->GetHyperParametersSize() +
            Lik->GetHyperParametersSize();
    }


    TDistrVec TModel::GetPrediction(const TMatrixD &Xnew) {
    	TVectorD kss = NLa::Diag(Cov->CalculateKernel(Xnew).GetValue());
    	TMatrixD Ks = Cov->CalculateKernel(X, Xnew).GetValue();
    	TVectorD ms = Mean->CalculateMean(Xnew).GetValue();
		
		TPosterior post;
		{
			auto infRes = Inf->CalculateNegativeLogLik(X, Y);
        	post = infRes.GetPosterior();
        }

        TVectorD Fmu = ms + NLa::Trans(Ks) * post.Alpha;
        TVectorD V = NLa::Solve(NLa::Trans(post.L), NLa::RepMat(post.DiagW, 1, Xnew.n_rows));
        TVectorD Fs2 = kss - NLa::Trans(NLa::ColSum(V % V));

        auto predDistrParams = Lik->CalculatePredictiveDistribution(Fmu, Fs2);
        
        return Lik->GetPredictiveDistributions(predDistrParams, Config.Seed);
    }

} // namespace NEgo
