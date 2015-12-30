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

	    virtual double Pdf(double x) = 0;
    	
    	virtual double NegativeExpectedImprovement(double min, size_t g) = 0;
	    
	    virtual double LowerConfidenceBound(double beta = 1) = 0;
	    
	    virtual double NegativeProbabilityOfImprovement(double yMin, double epsilon) = 0;
	    
	    virtual double Sample() = 0;

	    const double& GetMean() const {
	    	return Mean;
	    }

	    const double& GetSd() const {
	    	return Sd;
	    }
	
	private:
		double Mean;
		double Sd;

		std::mt19937 Generator;
	};
 	

} // namespace NEgo