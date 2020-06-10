

/** 
 *  Copyright (c) 2020 Iason Batzianoulis
 *  
 *  acquireFilters source file
 *  
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    GNU GPL v3
 * 
**/

#include "acquireFilters.hpp"


int acquireFilters::_gcd(int a, int b){
    /**
	 *  a function to find the greatest common divisor of two integers 
	 * 
	 */

	if (b==0){
		return a;
	}
	return _gcd(b, a % b);
}

double acquireFilters::average(std::vector<double> data){
		
	double sumUpdate = 0;
    
	// find the average computational time and print it in the terminal
    for(auto& n: data){
        sumUpdate +=n;
    }

    return sumUpdate/(double)data.size();
}


double acquireFilters::standDev(std::vector<double> data){
		
	double average = acquireFilters::average(data);

	double m_sum= 0;

	for(auto& n: data){
        m_sum += std::sqrt(std::abs(n - average));
    }

    return m_sum/(double)(data.size()-1);
}
