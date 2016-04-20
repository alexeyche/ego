#pragma once

#include "base_model.h"

#include <ego/acq/acq.h>

namespace NEgo {

    struct TSplitPoint {
        TSplitPoint() {}

        TSplitPoint(ui32 dimId, double value)
            : DimId(dimId)
            , Value(value)
        {}

        ui32 DimId;
        double Value;
    };

    struct TSplitMatrix {
        TSplitMatrix(const TMatrixD& m, const TVector<ui32>& ids)
            : M(m)
            , Ids(ids)
        {}
        
        operator bool () const  {
            return M.n_rows > 0;
        }

        TMatrixD M;
        TVector<ui32> Ids;
    };

    class TTreeModel: public IModel
    {
    public:

        static const ui32 MinLeafSize;
        static const ui32 SplitSizeCriteria;
        static const double UncertaintyThreshold;
        static const ui32 MaxDepth;

        using TBase = IModel;

        TTreeModel(const TModelConfig& config, ui32 D);

        TTreeModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y, int id = 0);

        TTreeModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq);

        TTreeModel(const TTreeModel& model);

        // Functor methods

        SPtr<IModel> Copy() const override;

        void SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) override;

        size_t GetParametersSize() const override;

        TVector<double> GetParameters() const override;

        void SetParameters(const TVector<double> &v) override;

        TTreeModel::Result UserCalc(const TMatrixD& Xnew) const override;

        IAcq::Result CalcCriterion(const TVectorD& x) const override;

        TInfResult GetNegativeLogLik() const override final;

        void AddPoint(const TVectorD& x, double y) override;

        void Update() override;

        TMatrixD GetX() const override;

        TVectorD GetY() const override;

        ui32 GetDimSize() const override;
       
        ui32 GetSize() const override;
        
        const double& GetMinimumY() const override;

        TVectorD GetMinimumX() const override;

        void SetData(const TMatrixD &x, const TVectorD &y) override;

        void SerialProcess(TProtoSerial& serial) override;

        SPtr<IDistr> GetPointPrediction(const TVectorD& Xnew) override;  

        SPtr<IDistr> GetPointPredictionWithDerivative(const TVectorD& Xnew) override;

        TDistrVec GetPrediction(const TMatrixD &Xnew) override;

        bool Split();

        TMatrixD GetAcqParameters();

        void SplitRecursively();

        void OptimizeHypers(const TOptConfig& config) override;

        SPtr<ICov> GetCovariance() const override;

        SPtr<IAcq> GetAcqusitionFunction() const override;
        
        void EnhanceGlobalSearch() override;

        template <typename Ret>
        Ret Call(const TVectorD& splitVal, std::function<Ret(SPtr<IModel>)> callback) const {
            if (Model) {
                return callback(Model);
            }
            if (splitVal(SplitPoint.DimId) <= SplitPoint.Value) {
                return callback(LeftLeaf);
            } else {
                return callback(RightLeaf);
            }
        }

        template <typename Ret>
        Ret Call(const TVectorD& splitVal, std::function<Ret(SPtr<IModel>)> modelCallback, std::function<Ret(SPtr<IModel>)> callback) const {
            if (Model) {
                return modelCallback(Model);
            }
            if (splitVal(SplitPoint.DimId) <= SplitPoint.Value) {
                return callback(LeftLeaf);
            } else {
                return callback(RightLeaf);
            }
        }
        
        TPair<TSplitMatrix, TSplitMatrix> SplitMatrix(const TMatrixD& m) const {
            TMatrixD mLeft;
            TMatrixD mRight;
            TVector<ui32> leftIds;
            TVector<ui32> rightIds;
            for (ui32 rId=0; rId < m.n_rows; ++rId) {
                if (m(rId, SplitPoint.DimId) <= SplitPoint.Value) {
                    mLeft = NLa::RowBind(mLeft, m.row(rId));
                    leftIds.push_back(rId);
                } else {
                    mRight = NLa::RowBind(mRight, m.row(rId));
                    rightIds.push_back(rId);
                }
            }
            ENSURE(mLeft.size() + mRight.size() == m.size(), "Split is not successfull");
            return MakePair(TSplitMatrix(mLeft, leftIds), TSplitMatrix(mRight, rightIds));
        }

        template <typename Ret>
        Ret Dispatch(
            const TMatrixD& m, 
            std::function<Ret(SPtr<IModel>, const TMatrixD& v)> callback, 
            std::function<void(const Ret&, const Ret&, Ret&)> alloc,
            std::function<void(const Ret&, ui32, Ret&, ui32)> fill
        ) {
            if (Model) {
                return callback(Model, m);
            }
            auto res = SplitMatrix(m);
            Ret leftRet;
            if (res.first) {
                leftRet = callback(LeftLeaf, res.first.M);
            }
            Ret rightRet;
            if (res.second) {
                rightRet = callback(RightLeaf, res.second.M);
            }
            
            Ret dst;
            alloc(leftRet, rightRet, dst);
            for (ui32 rId = 0; rId < res.first.M.n_rows; ++rId) {
                fill(leftRet, rId, dst, res.first.Ids[rId]);
            }
            for (ui32 rId = 0; rId < res.second.M.n_rows; ++rId) {
                fill(rightRet, rId, dst, res.second.Ids[rId]);
            }
            return dst;
        }

        template <typename Ret>
        Ret Combine(
            std::function<Ret(SPtr<IModel>)> callback, 
            std::function<Ret(const Ret&, const Ret&)> combine
        ) const {
            if (Model) {
                return callback(Model);
            }
            Ret leftRet = callback(LeftLeaf);
            Ret rightRet = callback(RightLeaf);
            return combine(leftRet, rightRet);
        }
        void Reset();
        
    private:
        int Id = 0;

        SPtr<TTreeModel> LeftLeaf;
        SPtr<TTreeModel> RightLeaf;
        
        TSplitPoint SplitPoint;

        SPtr<IModel> Model;
    };

    REGISTER_MODEL(TTreeModel);

} // namespace NEgo


