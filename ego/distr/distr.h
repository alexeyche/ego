#pragma once

#include <ego/base/base.h>

#include <random>

namespace NEgo {

	class IDistr {
	public:
	
		IDistr(double mean, double sd, ui32 seed)
			: Mean(mean)
			, Sd(sd)
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
	
	protected:
		double Mean;
		double Sd;

		std::mt19937 Generator;
	};
 	

} // namespace NEgo