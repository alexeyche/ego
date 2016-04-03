#include "solver.h"

#include <ego/util/json.h>

namespace NEgo {

	template <>
    TJsonDocument TSolver::GetVariableSlice(const TString& varName, ui32 gridSize) {
		if (Model->Empty()) {
            return TJsonDocument::Array();
        }
        const auto& vars = Problem.GetVariables();
        auto res = vars.find(varName);
        if (res == vars.end()) {
            throw TErrElementNotFound() << "Variable " << varName << " is not found in problem " << GetProblemName();
        }
        const TVariable& var = res->second;

        TVectorD Xbest = Model->GetMinimumX();

        TMatrixD X;
        TVectorD grid = NLa::Linspace(0.0, 1.0, gridSize);
        for (ui32 dim = 0; dim < Xbest.size(); ++dim) {
            if (dim == var.Id) {
                X = NLa::ColBind(X, grid);
            } else {
                X = NLa::ColBind(X, NLa::VectorFromConstant(gridSize, Xbest(dim)));
            }
        }

        TDistrVec vec = Model->GetPrediction(X);

        TJsonDocument mean = TJsonDocument::Array();
        TJsonDocument leftSd = TJsonDocument::Array();
        TJsonDocument rightSd = TJsonDocument::Array();
        TJsonDocument acqValues = TJsonDocument::Array();
        ui32 idx = 0;
        for (const auto& distr: vec) {
            mean.PushBack(TJsonDocument::Array({grid(idx), distr->GetMean()}));
            leftSd.PushBack(TJsonDocument::Array({grid(idx), distr->GetMean() - distr->GetSd()}));
            rightSd.PushBack(TJsonDocument::Array({grid(idx), distr->GetMean() + distr->GetSd()}));
            acqValues.PushBack(
                TJsonDocument::Array(
                    {
                        grid(idx), 
                        Model->CalcCriterion(NLa::Trans(X.row(idx))).Value()
                    }
                )
            );
            ++idx;
        }
        TJsonDocument points = TJsonDocument::Array();

        const TMatrixD& Xfull = Model->GetData().first;
        const TVectorD& Yfull = Model->GetData().second;

        for (size_t pi=0; pi < Yfull.size(); ++pi) {
            points.PushBack(TJsonDocument::Array({Xfull(pi, var.Id), Yfull(pi)}));
        }

        TJsonDocument minimum = TJsonDocument::Array();
        minimum.PushBack(TJsonDocument::Array({Xbest(var.Id), Model->GetMinimumY()}));

        TJsonDocument ret = TJsonDocument::Object();
        ret["left_band"] = leftSd;
        ret["mean"] = mean;
        ret["right_band"] = rightSd;
        ret["points"] = points;
        ret["acq_values"] = acqValues;
        ret["minimum"] = minimum;
        return ret;
    }


    template <>
    TJsonDocument TSolver::GetNextPoint() {
        TRawPoint nextP = GetNextPoint<TRawPoint>();
        TJsonDocument nextPjson;
        
        nextPjson["Id"] = nextP.GetId();

        for (auto v: nextP.GetVariables()) {
            nextPjson["Point"][v.first] = TJsonDocument::FromAny(v.second);
        }
        return nextPjson;
    }

    template <>
    void TSolver::AddPoint(const TJsonDocument& rawPoint) {
        TRawPoint p;

        const TJsonDocument pointJson = rawPoint["Point"];
        for(const auto& k: pointJson.GetKeys()) {
            p.GetVariables().insert(MakePair(k, pointJson[k].ToAny()));
        }
        p.SetId(rawPoint["Id"].GetValue<TString>());
        AddPoint<TRawPoint>(p);
    }

} // namespace NEgo