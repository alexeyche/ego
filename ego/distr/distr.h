#pragma once

#include <ego/base/la.h>
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

	    virtual double StandardPdfDeriv(double x) const = 0;

	    double StandardCdfDeriv(double x) const {
	    	return StandardPdf(x);
	    }


	    virtual double Sample() = 0;

	    const double& GetMean() const {
	    	return Mean;
	    }

	    const double& GetSd() const {
	    	return Sd;
	    }

	    // ----------------
		const TVectorD& GetMeanDeriv() const {
	    	return MeanDeriv;
	    }

	    const TVectorD& GetSdDeriv() const {
	    	return SdDeriv;
	    }

		void SetMeanDeriv(const TVectorD& md) {
	    	MeanDeriv = md;
	    }

	    void SetSdDeriv(const TVectorD& sdd) {
	    	SdDeriv = sdd;
	    }

	    // ----------------
		// double GetMeanTotalDeriv() const {
	 //    	return MeanTotalDeriv;
	 //    }

	 //    double GetSdTotalDeriv() const {
	 //    	return SdTotalDeriv;
	 //    }

		// void SetMeanTotalDeriv(double md) {
	 //    	MeanTotalDeriv = md;
		// }

	 //    void SetSdTotalDeriv(double sdd) {
	 //    	SdTotalDeriv = sdd;
	 //    }

	protected:
		double Mean;
		double Sd;

		TVectorD MeanDeriv;
		TVectorD SdDeriv;

		// double MeanTotalDeriv;
		// double SdTotalDeriv;

		std::mt19937 Generator;
	};


} // namespace NEgo