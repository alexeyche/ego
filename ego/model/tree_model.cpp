#include "tree_model.h"

#include <ego/base/factory.h>

#include <ego/util/maybe.h>
#include <ego/util/pretty_print.h>

#include <ego/model/model.h>

namespace NEgo {

    const ui32 TTreeModel::MinLeafSize = 10;
    const ui32 TTreeModel::SplitSizeCriteria = 30;



    TTreeModel::TTreeModel(const TModelConfig& config, ui32 D) {
        Model = MakeShared(new TModel(config, D));
    }

    TTreeModel::TTreeModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y) {
        Model = MakeShared(new TModel(config, x, y));
        Root = false;
    }

    TTreeModel::TTreeModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) {
        Model = MakeShared(new TModel(mean, cov, lik, inf, acq));
        Model->SetModel(mean, cov, lik, inf, acq);
    }

    TTreeModel::TTreeModel(const TTreeModel& model) {
        if (model.Model) {
            Model = model.Model->Copy();
        } else {
            LeftLeaf = std::dynamic_pointer_cast<TTreeModel, IModel>(model.LeftLeaf->Copy());
            RightLeaf = std::dynamic_pointer_cast<TTreeModel, IModel>(model.RightLeaf->Copy());
            SplitPoint = model.SplitPoint;
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

    void TTreeModel::Split() {
        ENSURE(Model->GetSize() > MinLeafSize*2, "Need more rows for split");

        const TMatrixD& x = Model->GetX();
        const TVectorD& y = Model->GetY();

        TDistrVec preds = GetPrediction(x);

        // TVectorD squaredError(preds.size());
        TVectorD means(preds.size());
        ui32 idx = 0;
        double pmeanSum = 0.0;
        for (const auto& p: preds) {
            pmeanSum += p->GetMean();
            means(idx) = p->GetMean();
            
            // squaredError(idx) = (p->GetMean() - y(idx)) * (p->GetMean() - y(idx));
            ++idx;
        }
        double wholeUnc = NLa::Sum(NLa::Pow(means - pmeanSum/means.size(), 2.0))/means.size();
        // double wholeUnc = NLa::Sum(squaredError) / squaredError.size();

        double minUnc = std::numeric_limits<double>::max();
        TMaybe<TPair<ui32, ui32>> splitPoint;
        
        TVector<TVectorUW> sortIds(x.n_cols);

        for (ui32 dimId = 0; dimId < x.n_cols; ++dimId) {
            sortIds[dimId] = NLa::SortIndex(x.col(dimId));
            for (auto id = MinLeafSize; id < (x.n_rows-MinLeafSize); ++id) {
                const TVectorUW leftLeafIds(sortIds[dimId].subvec(0, id));
                const TVectorUW rightLeafIds(sortIds[dimId].subvec(id+1, x.n_rows-1));
                double currentNodeUnc = wholeUnc;
                double yLeftMean = NLa::Sum(means(leftLeafIds))/leftLeafIds.size();
                currentNodeUnc -= NLa::Sum(NLa::Pow(yLeftMean - means(leftLeafIds), 2.0))/leftLeafIds.size();
                
                double yRightMean = NLa::Sum(means(rightLeafIds))/rightLeafIds.size();
                currentNodeUnc -= NLa::Sum(NLa::Pow(yRightMean - means(rightLeafIds), 2.0))/rightLeafIds.size();
                
                // currentNodeUnc -= NLa::Sum(squaredError(leftLeafIds))/leftLeafIds.size();
                // currentNodeUnc -= NLa::Sum(squaredError(rightLeafIds))/rightLeafIds.size();
                // L_DEBUG << x(sortIds[dimId](id), dimId) << " " << id << " " << squaredError(id) << " " << currentNodeUnc;
                L_DEBUG << x(sortIds[dimId](id), dimId) << " " << id << " " << currentNodeUnc;
                if (currentNodeUnc < minUnc) {
                    L_DEBUG << "best";
                    splitPoint = MakePair(dimId, id);
                    minUnc = currentNodeUnc;
                }
            }
        }
        L_DEBUG << "Uncertainity: " << minUnc << " " << wholeUnc << " " << minUnc/wholeUnc;
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
    
    void TTreeModel::SplitRecursively() {
        L_DEBUG << Model->GetSize() << " > " << SplitSizeCriteria << "?";
        if (Model->GetSize() > SplitSizeCriteria) {
            Model->Update();
            Split();
            // LeftLeaf->SplitRecursively();
            // RightLeaf->SplitRecursively();
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

    void TTreeModel::OptimizeHypers(const TOptConfig& config) {
        if (Model) {
            Model->OptimizeHypers(config);
            return;
        }
        LeftLeaf->OptimizeHypers(config);
        RightLeaf->OptimizeHypers(config);
    }

} // namespace NEgo
