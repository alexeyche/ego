#pragma once

#include <ego/base/base.h>

#include <random>

namespace NEgo {

	class IDistr {
	public:

		IDistr(double mean, double sd, ui32 seed)
			: Mean(mean)
			, Sd(sd)
			, MeanDeriv(0.0)
			, SdDeriv(0.0)
			, Generator(seed)
		{
		}

		IDistr(double mean, double sd, double meanDeriv, double sdDeriv, ui32 seed)
			: Mean(mean)
			, Sd(sd)
			, MeanDeriv(meanDeriv)
			, SdDeriv(sdDeriv)
			, Generator(seed)
		{
		}

		virtual ~IDistr() {
		}


	    virtual double StandardPdf(double x) const = 0;

	    virtual double StandardCdf(double x) const = 0;

	    virtual double Sample() = 0;

	    const double& GetMean() const {
	    	return Mean;
	    }

	    const double& GetSd() const {
	    	return Sd;
	    }

		const double& GetMeanDeriv() const {
	    	return MeanDeriv;
	    }

	    const double& GetSdDeriv() const {
	    	return SdDeriv;
	    }

	protected:
		double Mean;
		double Sd;

		double MeanDeriv;
		double SdDeriv;

		std::mt19937 Generator;
	};


} // namespace NEgo