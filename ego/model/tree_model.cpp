#include "tree_model.h"

#include <ego/base/factory.h>

#include <ego/util/maybe.h>
#include <ego/base/la.h>

#include <ego/model/model.h>

namespace NEgo {

    const ui32 TTreeModel::MinLeafSize = 20;
    const ui32 TTreeModel::SplitSizeCriteria = 60;
    const double TTreeModel::UncertaintyThreshold = 0.0;
    const ui32 TTreeModel::MaxDepth = 10;

    TTreeModel::TTreeModel(const TModelConfig& config, ui32 D) {
        Config = config;
        Model = MakeShared(new TModel(Config, D));
    }

    TTreeModel::TTreeModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y, int id)
        : Id(id) 
    {
        Config = config;
        Model = MakeShared(new TModel(Config, x, y));
    }

    TTreeModel::TTreeModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) {
        Model = MakeShared(new TModel(mean, cov, lik, inf, acq));
        Model->SetModel(mean, cov, lik, inf, acq);
    }

    TTreeModel::TTreeModel(const TTreeModel& model) {
        if (model.Model) {
            Model = model.Model->Copy();
        } else {
            LeftLeaf = std::static_pointer_cast<TTreeModel, IModel>(model.LeftLeaf->Copy());
            RightLeaf = std::static_pointer_cast<TTreeModel, IModel>(model.RightLeaf->Copy());
            SplitPoint = model.SplitPoint;
        }
        Id = model.Id;
        Config = model.Config;
    }

    SPtr<IModel> TTreeModel::Copy() const {
        return MakeShared(new TTreeModel(*this));
    }

    // Setters

    void TTreeModel::SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) {
        if (Model) {
            Model->SetModel(mean, cov, lik, inf, acq);
            return;    
        }
        LeftLeaf->SetModel(mean, cov, lik, inf, acq);
        RightLeaf->SetModel(mean, cov, lik, inf, acq);
    }

    
    TMatrixD TTreeModel::GetX() const {
        if (Model) {
            return Model->GetX();    
        }
        return NLa::RowBind(LeftLeaf->GetX(), RightLeaf->GetX());
    }
    
    TVectorD TTreeModel::GetY() const {
        if (Model) {
            return Model->GetY();    
        }
        return NLa::RowBind(LeftLeaf->GetY(), RightLeaf->GetY());   
    }

    ui32 TTreeModel::GetSize() const {
        if (Model) {
            return Model->GetSize();    
        }
        return LeftLeaf->GetSize() + RightLeaf->GetSize();
    }

    ui32 TTreeModel::GetDimSize() const {
        if (Model) {
            return Model->GetDimSize();    
        }
        return LeftLeaf->GetDimSize();
    }


    const double& TTreeModel::GetMinimumY() const {
        if (Model) {
            return Model->GetMinimumY();    
        }
        return LeftLeaf->GetMinimumY() < RightLeaf->GetMinimumY() ? LeftLeaf->GetMinimumY() : RightLeaf->GetMinimumY();
    }

    TVectorD TTreeModel::GetMinimumX() const {
        if (Model) {
            return Model->GetMinimumX();    
        }
        return LeftLeaf->GetMinimumY() < RightLeaf->GetMinimumY() ? LeftLeaf->GetMinimumX() : RightLeaf->GetMinimumX();
    }

    SPtr<IDistr> TTreeModel::GetPointPrediction(const TVectorD& Xnew) {
        return Call<SPtr<IDistr>>(Xnew, [&](SPtr<IModel> model) {
            return model->GetPointPrediction(Xnew);
        });
    }

    SPtr<IDistr> TTreeModel::GetPointPredictionWithDerivative(const TVectorD& Xnew) {
        return Call<SPtr<IDistr>>(Xnew, [&](SPtr<IModel> model) {
            return model->GetPointPredictionWithDerivative(Xnew);
        });
    }

    TDistrVec TTreeModel::GetPrediction(const TMatrixD &Xnew) {
        return Dispatch<TDistrVec>(
            Xnew, 
            [&](SPtr<IModel> model, const TMatrixD& v) {
                return model->GetPrediction(v);
            },
            [&](const TDistrVec& left, const TDistrVec& right, TDistrVec& d) {
                d.resize(left.size() + right.size());
            },
            [&](const TDistrVec& src, ui32 srcId, TDistrVec& dst, ui32 dstId) {
                dst[dstId] = src[srcId];
            }
        );
    }


    // Functor methods

    size_t TTreeModel::GetParametersSize() const {
        if (Model) {
            return Model->GetParametersSize();    
        }
        return LeftLeaf->GetParametersSize() + RightLeaf->GetParametersSize();
    }

    TVector<double> TTreeModel::GetParameters() const {
        return Combine<TVector<double>>(
            [&](SPtr<IModel> model) {
                return model->GetParameters();
            },
            [&](const TVector<double>& left, const TVector<double>& right) {
                TVector<double> res;
                res.reserve(left.size() + right.size());
                res.insert(res.end(), left.begin(), left.end()); 
                res.insert(res.end(), right.begin(), right.end());
                return res;
            }
        );
    }

    void TTreeModel::SetParameters(const TVector<double> &v) {
        if (Model) {
            Model->SetParameters(v);
            return;    
        }
        TVector<double> leftPars(LeftLeaf->GetParametersSize());
        std::copy(v.begin(), v.begin() + leftPars.size(), leftPars.begin());
        LeftLeaf->SetParameters(leftPars);

        TVector<double> rightPars(RightLeaf->GetParametersSize());
        std::copy(v.begin() + leftPars.size(), v.end(), rightPars.begin());
        RightLeaf->SetParameters(rightPars);
    }

    TTreeModel::Result TTreeModel::UserCalc(const TMatrixD& Xnew) const {
        if (Model) {
            return Model->UserCalc(Xnew);  
        }
        
        auto spl = SplitMatrix(Xnew);
        TTreeModel::Result leftRes;
        if (spl.first) {
            leftRes = LeftLeaf->UserCalc(spl.first.M);
        }
        TTreeModel::Result rightRes;
        if (spl.second) {
            rightRes = RightLeaf->UserCalc(spl.second.M);
        }
        
        return TTreeModel::Result()
            .SetValue(
                [=]() -> TPair<TVectorD, TVectorD> {
                    TVectorD mean(Xnew.n_rows);
                    TVectorD sd(Xnew.n_rows);
                    auto leftVal = leftRes.Value();
                    for (ui32 rId = 0; rId < spl.first.M.n_rows; ++rId) {
                        mean(spl.first.Ids[rId]) = leftVal.first(rId); 
                        sd(spl.first.Ids[rId]) = leftVal.second(rId);
                    }
                    auto rightVal = rightRes.Value();
                    for (ui32 rId = 0; rId < spl.second.M.n_rows; ++rId) {
                        mean(spl.second.Ids[rId]) = rightVal.first(rId); 
                        sd(spl.second.Ids[rId]) = rightVal.second(rId);
                    }
                    return MakePair(mean, sd);
                }
            )
            .SetArgDeriv(
                [=]() -> TPair<TVectorD, TVectorD> {
                    TVectorD mean(Xnew.n_rows);
                    TVectorD sd(Xnew.n_rows);
                    auto leftVal = leftRes.ArgDeriv();
                    for (ui32 rId = 0; rId < spl.first.M.n_rows; ++rId) {
                        mean(spl.first.Ids[rId]) = leftVal.first(rId); 
                        sd(spl.first.Ids[rId]) = leftVal.second(rId);
                    }
                    auto rightVal = rightRes.ArgDeriv();
                    for (ui32 rId = 0; rId < spl.second.M.n_rows; ++rId) {
                        mean(spl.second.Ids[rId]) = rightVal.first(rId); 
                        sd(spl.second.Ids[rId]) = rightVal.second(rId);
                    }
                    return MakePair(mean, sd);
                }
            )
            .SetArgPartialDeriv(
                [=](ui32 indexRow, ui32 indexCol) -> TPair<TVectorD, TVectorD> {
                    TVectorD mean = NLa::Zeros(Xnew.n_rows);
                    TVectorD sd = NLa::Zeros(Xnew.n_rows);
                    auto leftFindRes = std::find(spl.first.Ids.begin(), spl.first.Ids.end(), indexRow);
                    if (leftFindRes != spl.first.Ids.end()) {
                        auto leftVal = leftRes.ArgPartialDeriv(leftFindRes - spl.first.Ids.begin(), indexCol);
                        for (ui32 rId = 0; rId < spl.first.M.n_rows; ++rId) {
                            mean(spl.first.Ids[rId]) = leftVal.first(rId); 
                            sd(spl.first.Ids[rId]) = leftVal.second(rId);
                        }
                    } else {
                        auto rightFindRes = std::find(spl.second.Ids.begin(), spl.second.Ids.end(), indexRow);
                        ENSURE(rightFindRes != spl.second.Ids.end(), "Can't find id: " << indexRow);
                        auto rightVal = rightRes.ArgPartialDeriv(rightFindRes - spl.second.Ids.begin(), indexCol);
                        for (ui32 rId = 0; rId < spl.second.M.n_rows; ++rId) {
                            mean(spl.second.Ids[rId]) = rightVal.first(rId); 
                            sd(spl.second.Ids[rId]) = rightVal.second(rId);
                        }    
                    }
                    return MakePair(mean, sd);
                }
            );

    }

    IAcq::Result TTreeModel::CalcCriterion(const TVectorD& x) const {
        return Call<IAcq::Result>(
            x,
            [&](SPtr<IModel> model) {
                return model->CalcCriterion(x);
            }
        );
    }

    TInfResult TTreeModel::GetNegativeLogLik() const {
        if (Model) {
            return Model->GetNegativeLogLik();
        }
        auto leftRes = LeftLeaf->GetNegativeLogLik(); 
        auto rightRes = RightLeaf->GetNegativeLogLik(); 
        return TInfResult()
            .SetValue(
                [=]() {
                    return leftRes.Value() + rightRes.Value();
                }
            )
            .SetParamDeriv(
                [=]() -> TVector<double> {
                    TVector<double> pars = leftRes.ParamDeriv();
                    const TVector<double> rightPars = rightRes.ParamDeriv();
                    std::copy(rightPars.begin(), rightPars.end(), std::back_inserter(pars));
                    return pars;
                }
            )
            .SetPosterior(
                [=]() -> TPosterior {
                    throw TErrLogicError() << "Not implemented here";    
                }
            );
    }

    void TTreeModel::AddPoint(const TVectorD& x, double y) {
        return Call<void>(
            x,
            [&](SPtr<IModel> model) {
                model->AddPoint(x, y);

                if (model->GetSize() > SplitSizeCriteria) {
                    model->Update();
                    Split();
                }
            },
            [&](SPtr<IModel> model) {
                model->AddPoint(x, y);
            }
        );
    }

    SPtr<ICov> TTreeModel::GetCovariance() const {
        if (Model) {
            return Model->GetCovariance();    
        }
        return LeftLeaf->GetCovariance();
    }


    bool TTreeModel::Split() {
        ENSURE(Model->GetSize() > MinLeafSize*2, "Need more rows for split");
        if (std::abs(Id) >= MaxDepth) {
            L_DEBUG << "Max tree depth reached. No splitting";
            return false;
        }

        const TMatrixD& x = Model->GetX();
        const TVectorD& y = Model->GetY();

        // TDistrVec preds = GetPrediction(x);

        // TVectorD squaredError(preds.size());
        // TVectorD means(preds.size());
        // ui32 idx = 0;
        // double pmeanSum = 0.0;
        // for (const auto& p: preds) {
        //     pmeanSum += p->GetMean();
        //     means(idx) = p->GetMean();
            
        //     // squaredError(idx) = (p->GetMean() - y(idx)) * (p->GetMean() - y(idx));
        //     ++idx;
        // }
        // double wholeUnc = NLa::Sum(NLa::Pow(means - pmeanSum/means.size(), 2.0))/means.size();
        // double wholeUnc = NLa::Sum(squaredError) / squaredError.size();

        // double minUnc = std::numeric_limits<double>::max();
        // TMaybe<TPair<ui32, ui32>> splitPoint;
        
        TVector<TVectorUW> sortIds(x.n_cols);

        double entropyConstant = std::log(std::pow(2 * M_PI * exp(1.0), x.n_cols));
        
        TMatrixD K = Model->GetCovariance()->CrossCovariance(x).Value();
        
        double wholeEnt = 0.5 * (entropyConstant + std::log(NLa::Det(K)));
        L_DEBUG << "Whole entropy is " << wholeEnt;
        double minEnt = std::numeric_limits<double>::max();
        TMaybe<TPair<ui32, ui32>> splitPoint;

        for (ui32 dimId = 0; dimId < x.n_cols; ++dimId) {
            sortIds[dimId] = NLa::SortIndex(x.col(dimId));
            for (auto id = MinLeafSize; id < (x.n_rows-MinLeafSize); ++id) {
                const TVectorUW leftLeafIds(sortIds[dimId].subvec(0, id));
                const TVectorUW rightLeafIds(sortIds[dimId].subvec(id+1, x.n_rows-1));
                
                TMatrixD leftK = Model->GetCovariance()->CrossCovariance(x.rows(leftLeafIds)).Value();
                TMatrixD rightK = Model->GetCovariance()->CrossCovariance(x.rows(rightLeafIds)).Value();
                double leftEntropy = 0.5 * (entropyConstant + std::log(NLa::Det(leftK)));
                double rightEntropy = 0.5 * (entropyConstant + std::log(NLa::Det(rightK)));
                double currentNodeEnt = wholeEnt - leftEntropy - rightEntropy;
                L_DEBUG << "Entropy rule: " << currentNodeEnt;

                // double currentNodeUnc = wholeUnc;
                // double yLeftMean = NLa::Sum(means(leftLeafIds))/leftLeafIds.size();
                // currentNodeUnc -= NLa::Sum(NLa::Pow(yLeftMean - means(leftLeafIds), 2.0))/leftLeafIds.size();
                
                // double yRightMean = NLa::Sum(means(rightLeafIds))/rightLeafIds.size();
                // currentNodeUnc -= NLa::Sum(NLa::Pow(yRightMean - means(rightLeafIds), 2.0))/rightLeafIds.size();
                
                // currentNodeUnc -= NLa::Sum(squaredError(leftLeafIds))/leftLeafIds.size();
                // currentNodeUnc -= NLa::Sum(squaredError(rightLeafIds))/rightLeafIds.size();
                // L_DEBUG << x(sortIds[dimId](id), dimId) << " " << id << " " << squaredError(id) << " " << currentNodeUnc;
                // L_DEBUG << x(sortIds[dimId](id), dimId) << " " << id << " " << currentNodeUnc;
                L_DEBUG << x(sortIds[dimId](id), dimId) << " " << id << " " << currentNodeEnt;
                // if (currentNodeUnc < minUnc) {
                //     L_DEBUG << "best";
                //     splitPoint = MakePair(dimId, id);
                //     minUnc = currentNodeUnc;
                // }
                if (currentNodeEnt < minEnt) {
                    L_DEBUG << "best";
                    splitPoint = MakePair(dimId, id);
                    minEnt = currentNodeEnt;   
                }
            }
        }
        L_DEBUG << "Uncertainity: " << minEnt << " " << wholeEnt << " " << minEnt/wholeEnt;

        ENSURE(splitPoint, "Split point is not choosen");
        
        if (minEnt > UncertaintyThreshold) {
            L_DEBUG << minEnt << " is bigger than threshold " << UncertaintyThreshold << ", no split today";
            return false;
        }

        ui32 dimId = splitPoint.GetRef().first;
        ui32 splitId = splitPoint.GetRef().second;
        const TVectorUW& ids = sortIds[dimId];

        L_DEBUG << Id << ": split point is " << splitPoint.GetRef().first << ":" << splitPoint.GetRef().second << " at " << x(ids(splitId), dimId);

        TMatrixD xLeft = x.rows(ids.subvec(0, splitId));
        TVectorD yLeft = y(ids.subvec(0, splitId));
        TMatrixD xRight = x.rows(ids.subvec(splitId+1, ids.size()-1));
        TVectorD yRight = y(ids.subvec(splitId+1, ids.size()-1));

        LeftLeaf = MakeShared(new TTreeModel(Model->GetConfig(), xLeft, yLeft, Id-1));
        RightLeaf = MakeShared(new TTreeModel(Model->GetConfig(), xRight, yRight, Id+1));
        SplitPoint = TSplitPoint(dimId, x(ids(splitId), dimId));
        Model.reset();
        return true;
    }
    
    void TTreeModel::SplitRecursively() {
        L_DEBUG << Id << ": " <<  Model->GetSize() << " > " << SplitSizeCriteria << "?";
        if (Model->GetSize() > SplitSizeCriteria) {
            Model->Update();
            if (Split()) {
                LeftLeaf->SplitRecursively();
                RightLeaf->SplitRecursively();
            }
        }
    }

    void TTreeModel::SetData(const TMatrixD &x, const TVectorD &y) {
        if (Model) {
            Model->SetData(x, y);
            SplitRecursively();
            return;
        }
        auto xspl = SplitMatrix(x);
        auto yspl = SplitMatrix(y);
        ENSURE(xspl.first.M.size() + xspl.second.M.size() == x.n_rows, "Bad split of " << x);
        ENSURE(yspl.first.M.size() + yspl.second.M.size() == y.size(), "Bad split of " << y);
        L_DEBUG << "Setting to left leaf " << yspl.first.M << " right leaf " << yspl.second.M << " from " << y; 
        LeftLeaf->SetData(xspl.first.M, yspl.first.M);
        RightLeaf->SetData(xspl.second.M, yspl.second.M);
    }

    void TTreeModel::Update() {
        if (Model) {
            Model->Update();
            return;
        }
        LeftLeaf->Update();
        RightLeaf->Update();
    }

    void TTreeModel::SerialProcess(TProtoSerial& serial) {
        if (Model) {
            Model->SerialProcess(serial);
            if (serial.IsInput()) {
                SplitRecursively();    
            }
            return;    
        }
        LeftLeaf->SerialProcess(serial);
        RightLeaf->SerialProcess(serial);
    }
    
    void TTreeModel::Reset() {
        if (Model) {
            Model.reset();
        } else {
            LeftLeaf->Reset();
            RightLeaf->Reset();
        }
    }
    
    TMatrixD TTreeModel::GetAcqParameters() {
        if (Model) {
            return NLa::Trans(NLa::StdToVec(Model->GetAcqusitionFunction()->GetParameters()));
        }
        return NLa::RowBind(LeftLeaf->GetAcqParameters(), RightLeaf->GetAcqParameters());
    }
    
    SPtr<IAcq> TTreeModel::GetAcqusitionFunction() const {
        if (Model) {
            return Model->GetAcqusitionFunction();
        }
        throw TErrLogicError() << "Can't get acqusition function for non leaf models";
    }

    void TTreeModel::OptimizeHypers(const TOptConfig& config) {
        if (Model) {
            Model->OptimizeHypers(config);
            return;
        }

        const TMatrixD x = GetX();
        const TVectorD y = GetY();
        
        TMatrixD acqParam = GetAcqParameters();

        L_DEBUG << "Going to rebuild tree model with data " << x.n_rows << ":" << x.n_cols;

        LeftLeaf->Reset();
        RightLeaf->Reset();
        
        Config.AcqParameters = NLa::VecToStd(NLa::Trans(NLa::ColMean(acqParam)));
        
        (*this) = TTreeModel(Config, x, y, 0);
        Model->OptimizeHypers(config);

        SplitRecursively();
        
        if (!Model) {
            LeftLeaf->OptimizeHypers(config);
            RightLeaf->OptimizeHypers(config);    
        }
    }

    void TTreeModel::EnhanceGlobalSearch() {
        if (Model) {
            Model->EnhanceGlobalSearch();
            return;
        }
        LeftLeaf->EnhanceGlobalSearch();
        RightLeaf->EnhanceGlobalSearch();
    }
    
} // namespace NEgo
