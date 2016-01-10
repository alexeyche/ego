#pragma once

#include "distr.h"

namespace NEgo {


	class TDistrGauss : public IDistr {
	public:
	
		TDistrGauss(double mean, double sd, ui32 seed)
			: IDistr(mean, sd, seed)
			, Distr(Mean, Sd)
		{
		}
		
		
		double StandardPdf(double x) const override final;
	    
	    double StandardCdf(double x) const override final;
    	
	    double Sample() override final;

	protected:
		std::normal_distribution<double> Distr;
	};

} // namespace NEgo