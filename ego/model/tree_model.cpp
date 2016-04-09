#include "tree_model.h"

#include <ego/base/factory.h>

#include <ego/util/maybe.h>

namespace NEgo {

    const ui32 TTreeModel::MeanLeafSize = 5;
    const ui32 TTreeModel::SplitSizeCriteria = 50;



    TTreeModel::TTreeModel(const TModelConfig& config, ui32 D) {
        Model = Factory.CreateModel("Model", config, D);
        Model->InitWithConfig(config, D);
    }

    TTreeModel::TTreeModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y) {
        Model = Factory.CreateModel("Model", config, x.n_cols);
        Model->InitWithConfig(config, x.n_cols);
        Model->SetData(x, y);
        Root = false;
    }

    TTreeModel::TTreeModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) {
        Model = Factory.CreateModel("Model", TModelConfig(), mean->GetDimSize());
        Model->SetModel(mean, cov, lik, inf, acq);
    }

    TTreeModel::TTreeModel(const TTreeModel& model) {
        if (model.Model) {
            Model = model.Model->Copy();
        } else {
            LeftLeaf = model.LeftLeaf->Copy();
            RightLeaf = model.RightLeaf->Copy();
        }
        Root = model.Root;
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
        if (Model) {
            return Model->GetPointPrediction(Xnew);
        }
        if (Xnew(SplitPoint.DimId) <= SplitPoint.Value) {
            return LeftLeaf->GetPointPrediction(Xnew);
        } else {
            return RightLeaf->GetPointPrediction(Xnew);
        }
    }

    SPtr<IDistr> TTreeModel::GetPointPredictionWithDerivative(const TVectorD& Xnew) {
        if (Model) {
            return Model->GetPointPredictionWithDerivative(Xnew);
        }
        if (Xnew(SplitPoint.DimId) <= SplitPoint.Value) {
            return LeftLeaf->GetPointPredictionWithDerivative(Xnew);
        } else {
            return RightLeaf->GetPointPredictionWithDerivative(Xnew);
        }
    }

    TDistrVec TTreeModel::GetPrediction(const TMatrixD &Xnew) {
        if (Model) {
            return Model->GetPrediction(Xnew);
        }
        TMatrixD xNewLeft;
        TMatrixD xNewRight;
        TVector<ui32> leftIds;
        TVector<ui32> rightIds;
        for (ui32 rId=0; rId < Xnew.n_rows; ++rId) {
            if (Xnew(rId, SplitPoint.DimId) <= SplitPoint.Value) {
                xNewLeft = NLa::RowBind(xNewLeft, Xnew.row(rId));
                leftIds.push_back(rId);
            } else {
                xNewRight = NLa::RowBind(xNewRight, Xnew.row(rId));
                rightIds.push_back(rId);
            }
        }
        auto leftRes = LeftLeaf->GetPrediction(xNewLeft);
        auto rightRes = RightLeaf->GetPrediction(xNewRight);
        TDistrVec res(leftRes.size() + rightRes.size());
        for (ui32 rId = 0; rId < xNewLeft.n_rows; ++rId) {
            res[leftIds[rId]] = leftRes[rId];
        }
        for (ui32 rId = 0; rId < xNewRight.n_rows; ++rId) {
            res[rightIds[rId]] = rightRes[rId];
        }
        return res;
    }


    // Functor methods

    size_t TTreeModel::GetParametersSize() const {
        if (Model) {
            return Model->GetParametersSize();    
        }
        return LeftLeaf->GetParametersSize() + RightLeaf->GetParametersSize();
    }

    TVector<double> TTreeModel::GetParameters() const {
        if (Model) {
            return Model->GetParameters();    
        }
        
        TVector<double> pars = LeftLeaf->GetParameters();
        const TVector<double> rightPars = RightLeaf->GetParameters();
        std::copy(rightPars.begin(), rightPars.end(), std::back_inserter(pars));
        return pars;
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
        std::copy(v.begin() + leftPars.size(), v.begin() + leftPars.size() + rightPars.size(), rightPars.begin());
        RightLeaf->SetParameters(rightPars);
    }

    TTreeModel::Result TTreeModel::UserCalc(const TMatrixD& Xnew) const {
        if (Model) {
            return Model->UserCalc(Xnew);    
        }
        TMatrixD xNewLeft;
        TMatrixD xNewRight;
        TVector<ui32> leftIds;
        TVector<ui32> rightIds;
        for (ui32 rId=0; rId < Xnew.n_rows; ++rId) {
            if (Xnew(rId, SplitPoint.DimId) <= SplitPoint.Value) {
                xNewLeft = NLa::RowBind(xNewLeft, Xnew.row(rId));
                leftIds.push_back(rId);
            } else {
                xNewRight = NLa::RowBind(xNewRight, Xnew.row(rId));
                rightIds.push_back(rId);
            }
        }
        auto leftRes = LeftLeaf->UserCalc(xNewLeft);
        auto rightRes = RightLeaf->UserCalc(xNewRight);
        return TTreeModel::Result()
            .SetValue(
                [=]() -> TPair<TVectorD, TVectorD> {
                    TVectorD mean(Xnew.n_rows);
                    TVectorD sd(Xnew.n_rows);
                    auto leftVal = leftRes.Value();
                    for (ui32 rId = 0; rId < xNewLeft.n_rows; ++rId) {
                        mean(leftIds[rId]) = leftVal.first(rId); 
                        sd(leftIds[rId]) = leftVal.second(rId);
                    }
                    auto rightVal = rightRes.Value();
                    for (ui32 rId = 0; rId < xNewRight.n_rows; ++rId) {
                        mean(rightIds[rId]) = rightVal.first(rId); 
                        sd(rightIds[rId]) = rightVal.second(rId);
                    }
                    return MakePair(mean, sd);
                }
            )
            .SetArgDeriv(
                [=]() -> TPair<TVectorD, TVectorD> {
                    TVectorD mean(Xnew.n_rows);
                    TVectorD sd(Xnew.n_rows);
                    auto leftVal = leftRes.ArgDeriv();
                    for (ui32 rId = 0; rId < xNewLeft.n_rows; ++rId) {
                        mean(leftIds[rId]) = leftVal.first(rId); 
                        sd(leftIds[rId]) = leftVal.second(rId);
                    }
                    auto rightVal = rightRes.ArgDeriv();
                    for (ui32 rId = 0; rId < xNewRight.n_rows; ++rId) {
                        mean(rightIds[rId]) = rightVal.first(rId); 
                        sd(rightIds[rId]) = rightVal.second(rId);
                    }
                    return MakePair(mean, sd);
                }
            )
            .SetArgPartialDeriv(
                [=](ui32 indexRow, ui32 indexCol) -> TPair<TVectorD, TVectorD> {
                    TVectorD mean(Xnew.n_rows);
                    TVectorD sd(Xnew.n_rows);
                    auto leftVal = leftRes.ArgPartialDeriv(indexRow, indexCol);
                    for (ui32 rId = 0; rId < xNewLeft.n_rows; ++rId) {
                        mean(leftIds[rId]) = leftVal.first(rId); 
                        sd(leftIds[rId]) = leftVal.second(rId);
                    }
                    auto rightVal = rightRes.ArgPartialDeriv(indexRow, indexCol);
                    for (ui32 rId = 0; rId < xNewRight.n_rows; ++rId) {
                        mean(rightIds[rId]) = rightVal.first(rId); 
                        sd(rightIds[rId]) = rightVal.second(rId);
                    }
                    return MakePair(mean, sd);
                }
            );

    }

    IAcq::Result TTreeModel::CalcCriterion(const TVectorD& x) const {
        if (Model) {
            return Model->CalcCriterion(x);    
        }
        if (x(SplitPoint.DimId) <= SplitPoint.Value) {
            return LeftLeaf->CalcCriterion(x);
        } else {
            return RightLeaf->CalcCriterion(x);
        }
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
        if (Model) {
            Model->AddPoint(x, y);

            if (Model->GetSize() > SplitSizeCriteria) {
                Model->Update();
                Split();
            }
            return;    
        }
        if (x(SplitPoint.DimId) <= SplitPoint.Value) {
            LeftLeaf->AddPoint(x, y);
        } else {
            RightLeaf->AddPoint(x, y);
        }
    }

    void TTreeModel::Split() {
        ENSURE(Model->GetSize() > MeanLeafSize*2, "Need more rows for split");

        const TMatrixD& x = Model->GetX();
        const TVectorD& y = Model->GetY();

        TDistrVec preds = GetPrediction(x);

        TVectorD squaredError(preds.size());
        ui32 idx = 0;
        for (const auto& p: preds) {
            squaredError(idx) = (p->GetMean() - y(idx)) * (p->GetMean() - y(idx));
            ++idx;
        }

        double wholeUnc = NLa::Sum(squaredError) / squaredError.size();

        double minUnc = std::numeric_limits<double>::max();
        TMaybe<TPair<ui32, ui32>> splitPoint;
        
        TVector<TVectorUW> sortIds(x.n_cols);

        for (ui32 dimId = 0; dimId < x.n_cols; ++dimId) {
            sortIds[dimId] = NLa::SortIndex(x.col(dimId));
            for (auto id = MeanLeafSize; id < (x.n_rows-MeanLeafSize); ++id) {
                const TVectorUW leftLeafIds(sortIds[dimId].subvec(0, id));
                const TVectorUW rightLeafIds(sortIds[dimId].subvec(id+1, x.n_rows-1));
                double currentNodeUnc = wholeUnc;
                currentNodeUnc -= NLa::Sum(squaredError(leftLeafIds))/leftLeafIds.size();
                currentNodeUnc -= NLa::Sum(squaredError(rightLeafIds))/rightLeafIds.size();
                L_DEBUG << x(sortIds[dimId](id), dimId) << " " << id << " " << squaredError(id) << " " << currentNodeUnc;
                if (currentNodeUnc < minUnc) {
                    L_DEBUG << "best";
                    splitPoint = MakePair(dimId, id);
                    minUnc = currentNodeUnc;
                }
            }
        }

        ENSURE(splitPoint, "Split point is not choosen");
        
        ui32 dimId = splitPoint.GetRef().first;
        ui32 splitId = splitPoint.GetRef().second;
        const TVectorUW& ids = sortIds[dimId];

        L_DEBUG << "Split point is " << splitPoint.GetRef().first << ":" << splitPoint.GetRef().second << " at " << x(ids(splitId), dimId);

        TMatrixD xLeft = x.rows(ids.subvec(0, splitId));
        TVectorD yLeft = y(ids.subvec(0, splitId));
        TMatrixD xRight = x.rows(ids.subvec(splitId+1, ids.size()-1));
        TVectorD yRight = y(ids.subvec(splitId+1, ids.size()-1));

        LeftLeaf = MakeShared(new TTreeModel(Model->GetConfig(), xLeft, yLeft));
        RightLeaf = MakeShared(new TTreeModel(Model->GetConfig(), xRight, yRight));
        SplitPoint = TSplitPoint(dimId, x(ids(splitId), dimId));
        Model.reset();
    }

    void TTreeModel::SetData(const TMatrixD &x, const TVectorD &y) {
        if (Model) {
            Model->SetData(x, y);
            return;
        }
        TMatrixD xLeft;
        TMatrixD xRight;
        TVectorD yLeft;
        TVectorD yRight;
        for (ui32 rId=0; rId < x.n_rows; ++rId) {
            if (x(rId, SplitPoint.DimId) <= SplitPoint.Value) {
                xLeft = NLa::RowBind(xLeft, x.row(rId));
                yLeft = NLa::RowBind(yLeft, NLa::VectorFromConstant(1, y(rId)));
            } else {
                xRight = NLa::RowBind(xRight, x.row(rId));
                yRight = NLa::RowBind(yRight, NLa::VectorFromConstant(1, y(rId)));
            }
        }
        LeftLeaf->SetData(xLeft, yLeft);
        RightLeaf->SetData(xRight, yRight);
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
            return;    
        }
    }

} // namespace NEgo
