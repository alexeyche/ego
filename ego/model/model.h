#pragma once

#include <ego/base/factory.h>
#include <ego/base/entities.h>
#include <ego/base/la.h>
#include <ego/protos/config.pb.h>

#include <ego/util/log/log.h>

namespace NEgo {

    class TModel {
    public:
        TModel(const NEgoProto::TModelConfig &config)
            : Config(config)
        {
            ENSURE(Config.has_inputfile() && Config.has_cov(), "Need kernel and input data");

            TMatrixD inputData = NLa::ReadCsv(Config.inputfile());
            X = NLa::HeadCols(inputData, inputData.n_cols-1);
            Y = NLa::TailCols(inputData, 1);
            L_DEBUG << "Got input values with size [" << X.n_rows << "x" << X.n_cols << "] and " << " target values with size [" << Y.n_rows << "x" << Y.n_cols << "]";

            Covariance = Factory.CreateCov(Config.cov(), X.n_cols);
        }

    private:
        TMatrixD X;
        TVectorD Y;

        UPtr<ICov> Covariance;

        NEgoProto::TModelConfig Config;
    };

} // namespace NEgo


