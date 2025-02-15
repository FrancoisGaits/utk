/*
 * Hélène Perrier helene.perrier@liris.cnrs.fr
 * and David Coeurjolly david.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the UTK project.
 */
#ifndef _UTK_INTEGRATION_DISK_ND_
#define _UTK_INTEGRATION_DISK_ND_

#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"
#include <cmath>
#include <array>

namespace utk
{
  
class IntegrationGaussian2D
{
public:
  IntegrationGaussian2D() { m_mean_x=0.5; m_stddev_x=0.25; m_mean_y=0.5; m_stddev_y=0.25; }
  
  void setMeanX(double arg_mean) { m_mean_x = arg_mean; }
  void setStddevX(double arg_stddev) { m_stddev_x = arg_stddev; }
  void setMeanY(double arg_mean) { m_mean_y = arg_mean; }
  void setStddevY(double arg_stddev) { m_stddev_y = arg_stddev; }
  
  template<unsigned int D, typename T, typename P>
  bool compute(const Pointset<D, T, P>& arg_pointset, double& integration_value, double& analytic_value)
  {
    std::array<double, D> nd_domain_extent;
    for(unsigned int d=0; d<D; d++)
    {
      nd_domain_extent[d] = (arg_pointset.domain.pMax.pos() - arg_pointset.domain.pMin.pos())[d];
    
      if(nd_domain_extent[d] == 0)
      {
			WARNING("IntegrationGaussian::compute domain extent is 0 on at least one dimension, scaling might fail");
      }
      if(nd_domain_extent[d] < 0)
      {
			ERROR("IntegrationGaussian::compute domain extent is negative on at least one dimension");
			return false;
	  }
    }
    
    analytic_value = analyticIntegrand();

    integration_value = 0;
    for(uint i=0; i<arg_pointset.size(); i++)
    {
      Point<D, double> pt;
      for(unsigned int d=0; d<D; d++)
		pt.pos()[d] = (arg_pointset[i].pos() - arg_pointset.domain.pMin.pos())[d] / nd_domain_extent[d];
      
	  integration_value += sampleIntegrand(pt);
    }
    integration_value /= (double)arg_pointset.size();
    
    return true;
  }

  double analyticIntegrand()
  {
	double integrand = 0;
	Point<2, double> pt;
	double res = 4000.0;
    for(int i=0; i<res; i++)
	{
		double integrand2 = 0;
		for(int j=0; j<res; j++)
		{
			pt.pos()[0] = (double)i/res;
			pt.pos()[1] = (double)j/res;
			integrand2 += sampleIntegrand(pt);
		}
		integrand2 /= res;
		integrand += integrand2;
	}
	integrand /= res;
	return integrand;
  }
  
  double sampleIntegrand(Point<2, double>& pt)
  {
	 double deltax = pt.pos()[0] - m_mean_x;
	 double deltay = pt.pos()[1] - m_mean_y;
	 double a = (deltax*deltax)/(2.0*m_stddev_x*m_stddev_x);
	 double b = (deltay*deltay)/(2.0*m_stddev_y*m_stddev_y);
	 return 1.0/(2.0*M_PI*m_stddev_x*m_stddev_y) * exp(-a-b);
  }
  
  double m_stddev_x;
  double m_mean_x;
  double m_stddev_y;
  double m_mean_y;
  
};

}//end namespace

#endif
