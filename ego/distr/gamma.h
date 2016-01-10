#pragma once

#include "distr.h"

namespace NEgo {


	class TDistrGamma : public IDistr {
	public:
	
		TDistrGamma(double mean, double sd, ui32 seed)
			: IDistr(mean, sd, seed)
		{
		}
		
		double StandardPdf(double x) const override final;
	    
	    double StandardCdf(double x) const override final;
    	
	    double Sample() override final;

	private:
		std::normal_distribution<double> Distr;
	};

} // namespace NEgo