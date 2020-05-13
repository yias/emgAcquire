/** 
 *  Copyright (c) 2020 Iason Batzianoulis
 *  
 *  acquireFilters header file
 *  
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    MIT
 * 
**/

#ifndef ACQUIREFILTERS_HPP
#define ACQUIREFILTERS_HPP


#include <iostream>
#include <algorithm>
// #include <exception>
#include <vector>

#include <math.h>
#include <complex>



// #pragma once



namespace acquireFilters{

	const double PI = 3.14159265359;


	int _gcd(int a, int b);			// a function to compute the greatest common divisor of two integer


	template<typename T>
	std::vector< std::vector<T> > resample_data(std::vector< std::vector<T> > data, float desired_freq, float original_freq, unsigned int nb_of_forget_points=0){

		/**
		 *  resample function for downsampling. It implements a moving average on a sliding window 
		 * 
		 *  Inputs:
		 *      data:                   the data to resample (vector of vectors)
		 *      desired_frequency:      the desired frequency after downsampling
		 *      original_frequency:     the original frequency of the data
		 *      nb_of_forget_points:    (optional) the number of points in the beginning of the original data to forget when returning the resampled data
		 *                              These forget-points will be used in downsampling but the resulted samples that correspond to these points 
		 *                              will be removed before returning the resampled data.
		 * 
		 *  Output:
		 *      a vector of vectors with the resampled data. The returned data have one-to-one correspondence with the original data 
		 *      (i.e., the first vector of the returned corresponds to the first vector of the original data and so on and so forth)
		 * 
		 * 
		 */

		if (desired_freq == original_freq){
			// if the desired frequency is the same as the original, 
			// then remove any forget-points and return the data
			if (nb_of_forget_points != 0){
				for (int i = 0; i< (int)data.size(); ++i){
					data[i].erase(data[i].begin(), data[i].begin() + nb_of_forget_points);
				}
			}
			return data;
		}else{

			// allocate memory for the returned matrix (vector of vectors)
			std::vector< std::vector<T> > resampled_Mat (data.size(), std::vector<T>());

			// find the Greatest Common Divisor of the original and desired frequency
			unsigned int gCd = acquireFilters::_gcd((int)desired_freq, (int)original_freq);

			// std::cout << "gcd: " << gCd << std::endl;

			// find the window length
			unsigned int window_length = original_freq / gCd;

			// std::cout << "window: " << window_length << std::endl;

			// find the overlap
			unsigned int overlap = desired_freq / gCd;

			// std::cout << "overlap: " << overlap << std::endl;

			if (desired_freq == gCd){
				overlap = 0;
			}

			// // check if there would be a floating overlap (not constant overlap)
			
			bool floating_overlap = false;
			if (overlap != 0){
				if ((window_length/overlap)!=2){
					floating_overlap = true;
				}
			}
			unsigned int samples_to_forget = 0;
			
			// check if there are samples to be removed
			// and if yes find how many
			if(nb_of_forget_points != 0){
				if (nb_of_forget_points >= window_length){
					samples_to_forget = nb_of_forget_points / window_length;
				}
			}

			// find the ratio between the orignal and the desired frequency
			float ratio = desired_freq / original_freq;

			// resample each of the signals
			for (int i = 0; i < (int)data.size(); ++i){
				
				unsigned int new_length = std::round((float)data[i].size() * ratio);
				
				// create a temporary vector and reserve a capacity for the retruned vector
				std::vector<T> tmp_vec(new_length, 0);
				resampled_Mat[i].reserve(new_length);

				int j = 0;
				int counter = 0;
				bool ch_flag = true;
				unsigned int t_overlap = overlap;

				// for each sliding time window, campute its average
				for (j=0; j<(int)data[i].size(); j += window_length-t_overlap){

					tmp_vec[counter] = 0;

					// check if the reaming samples are enough for covering a full time window
					// if there are not, then compute the average of the remaining samples
					if (j+window_length < (int)data[i].size()){
						for (int k=0; k<window_length; k++){
							tmp_vec[counter] += data[i][j+k];
						}
						tmp_vec[counter] = tmp_vec[counter]/window_length;
					}else{

						unsigned int f_window_length = (int)data[i].size() - j;
						
						for (int k=0; k<f_window_length; k++){

							tmp_vec[counter] += data[i][j+k];
						}
						tmp_vec[counter] = tmp_vec[counter]/f_window_length;
						break;
					}
					
					counter ++;

					// in the case of floating overlap, compute the next overlap
					if(floating_overlap){
						if(ch_flag){
							t_overlap = t_overlap - 1;
							ch_flag = false;
						}else{
							t_overlap = overlap;
							ch_flag = true;
						}
					}
				}
				
				// forget the initial samples if needed
				if (samples_to_forget>0){
					resampled_Mat[i].insert(resampled_Mat[i].end(), tmp_vec.begin() + samples_to_forget, tmp_vec.end());
				}else{
					resampled_Mat[i] = tmp_vec;
				}
				
			}
			
			return resampled_Mat;
		}
	}
}

#endif