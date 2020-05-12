

/** 
 *  Copyright (c) 2020 Iason Batzianoulis
 *  
 *  emgAcquire source file
 *  
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    MIT
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