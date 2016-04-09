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

    class TTreeModel: public IModel
    {
    public:

        static const ui32 MeanLeafSize;
        static const ui32 SplitSizeCriteria;

        using TBase = IModel;

        TTreeModel(const TModelConfig& config, ui32 D);

        TTreeModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y);

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

        void Split();

    private:
        bool Root = true;

        SPtr<IModel> LeftLeaf;
        SPtr<IModel> RightLeaf;
        
        TSplitPoint SplitPoint;

        SPtr<IModel> Model;
    };

    REGISTER_MODEL(TTreeModel);

} // namespace NEgo


