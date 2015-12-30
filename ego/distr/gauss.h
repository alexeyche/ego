#pragma once

#include "distr.h"

namespace NEgo {


	class TDistrGauss : public IDistr {
	public:
	
		TDistrGauss(double mean, double sd, ui32 seed)
			: IDistr(mean, sd, seed)
		{
		}
		
		double Pdf(double x) override final;

    	double NegativeExpectedImprovement(double min, size_t g) override final;
	    
	    double LowerConfidenceBound(double beta = 1) override final;
	    
	    double NegativeProbabilityOfImprovement(double yMin, double epsilon) override final;
	    
	    double Sample() override final;

	private:
		std::normal_distribution<double> Distr;
	};

} // namespace NEgo