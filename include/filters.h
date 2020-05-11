

#include <algorithm>
#include <exception>
#include <vector>
//#include <Dense>
//#include <Eigen>
#include <math.h>
#include <complex>
#include <iostream>




using namespace std;


#define PI 3.14159

double *ComputeLP(int FilterOrder){
	/**
	 * 
	 * 	compute the coefficients for a low-pass filter
	 *
	 */

	double *NumCoeffs;
	int m;
	int i;

	NumCoeffs = (double *)calloc(FilterOrder + 1, sizeof(double));
	if (NumCoeffs == NULL) 
		return(NULL);

	NumCoeffs[0] = 1;
	NumCoeffs[1] = FilterOrder;
	m = FilterOrder / 2;
	for (i = 2; i <= m; ++i)
	{
		NumCoeffs[i] = (double)(FilterOrder - i + 1)*NumCoeffs[i - 1] / i;
		NumCoeffs[FilterOrder - i] = NumCoeffs[i];
	}
	NumCoeffs[FilterOrder - 1] = FilterOrder;
	NumCoeffs[FilterOrder] = 1;

	return NumCoeffs;
}

double *ComputeHP(int FilterOrder){
	/**
	 * 
	 * 	compute the coefficients for a high-pass filter
	 *
	 */

	double *NumCoeffs;
	int i;

	NumCoeffs = ComputeLP(FilterOrder);
	if (NumCoeffs == NULL) return(NULL);

	for (i = 0; i <= FilterOrder; ++i)
		if (i % 2) NumCoeffs[i] = -NumCoeffs[i];

	return NumCoeffs;
}

double *TrinomialMultiply(int FilterOrder, double *b, double *c)
{
	int i, j;
	double *RetVal;

	RetVal = (double *)calloc(4 * FilterOrder, sizeof(double));
	if (RetVal == NULL) return(NULL);

	RetVal[2] = c[0];
	RetVal[3] = c[1];
	RetVal[0] = b[0];
	RetVal[1] = b[1];

	for (i = 1; i < FilterOrder; ++i)
	{
		RetVal[2 * (2 * i + 1)] += c[2 * i] * RetVal[2 * (2 * i - 1)] - c[2 * i + 1] * RetVal[2 * (2 * i - 1) + 1];
		RetVal[2 * (2 * i + 1) + 1] += c[2 * i] * RetVal[2 * (2 * i - 1) + 1] + c[2 * i + 1] * RetVal[2 * (2 * i - 1)];

		for (j = 2 * i; j > 1; --j)
		{
			RetVal[2 * j] += b[2 * i] * RetVal[2 * (j - 1)] - b[2 * i + 1] * RetVal[2 * (j - 1) + 1] +
				c[2 * i] * RetVal[2 * (j - 2)] - c[2 * i + 1] * RetVal[2 * (j - 2) + 1];
			RetVal[2 * j + 1] += b[2 * i] * RetVal[2 * (j - 1) + 1] + b[2 * i + 1] * RetVal[2 * (j - 1)] +
				c[2 * i] * RetVal[2 * (j - 2) + 1] + c[2 * i + 1] * RetVal[2 * (j - 2)];
		}

		RetVal[2] += b[2 * i] * RetVal[0] - b[2 * i + 1] * RetVal[1] + c[2 * i];
		RetVal[3] += b[2 * i] * RetVal[1] + b[2 * i + 1] * RetVal[0] + c[2 * i + 1];
		RetVal[0] += b[2 * i];
		RetVal[1] += b[2 * i + 1];
	}

	return RetVal;
}


double *ComputeNumCoeffs(int FilterOrder, double Lcutoff, double Ucutoff, double *DenC)
{
	double *TCoeffs;
	double *NumCoeffs;
	std::complex<double> *NormalizedKernel;
	double Numbers[15] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
	int i;

	NumCoeffs = (double *)calloc(2 * FilterOrder + 1, sizeof(double));
	if (NumCoeffs == NULL) return(NULL);

	NormalizedKernel = (std::complex<double> *)calloc(2 * FilterOrder + 1, sizeof(std::complex<double>));
	if (NormalizedKernel == NULL) return(NULL);

	TCoeffs = ComputeHP(FilterOrder);
	if (TCoeffs == NULL) return(NULL);

	for (i = 0; i < FilterOrder; ++i)
	{
		NumCoeffs[2 * i] = TCoeffs[i];
		NumCoeffs[2 * i + 1] = 0.0;
	}
	NumCoeffs[2 * FilterOrder] = TCoeffs[FilterOrder];
	double cp[2];
	double Bw, Wn;
	cp[0] = 2 * 2.0*tan(PI * Lcutoff / 2.0);
	cp[1] = 2 * 2.0*tan(PI * Ucutoff / 2.0);

	Bw = cp[1] - cp[0];
	//center frequency
	Wn = sqrt(cp[0] * cp[1]);
	Wn = 2 * atan2(Wn, 4);
	//	double kern;
	const std::complex<double> result = std::complex<double>(-1, 0);

	for (int k = 0; k<2 * FilterOrder + 1; k++)
	{
		NormalizedKernel[k] = std::exp(-sqrt(result)*Wn*Numbers[k]);
	}
	double b = 0;
	double den = 0;
	for (int d = 0; d<2 * FilterOrder + 1; d++)
	{
		b += real(NormalizedKernel[d] * NumCoeffs[d]);
		den += real(NormalizedKernel[d] * DenC[d]);
	}
	for (int c = 0; c<2 * FilterOrder + 1; c++)
	{
		NumCoeffs[c] = (NumCoeffs[c] * den) / b;
	}

	free(TCoeffs);
	return NumCoeffs;
}





double *ComputeNumCoeffs(int FilterOrder)
{
	double *TCoeffs;
	double *NumCoeffs;
	int i;

	NumCoeffs = (double *)calloc(2 * FilterOrder + 1, sizeof(double));
	if (NumCoeffs == NULL) return(NULL);

	TCoeffs = ComputeHP(FilterOrder);
	if (TCoeffs == NULL) return(NULL);

	for (i = 0; i < FilterOrder; ++i)
	{
		NumCoeffs[2 * i] = TCoeffs[i];
		NumCoeffs[2 * i + 1] = 0.0;
	}
	NumCoeffs[2 * FilterOrder] = TCoeffs[FilterOrder];

	free(TCoeffs);

	return NumCoeffs;
}




double *ComputeDenCoeffs(int FilterOrder, double Lcutoff, double Ucutoff)
{
	int k;            // loop variables
	double theta;     // PI * (Ucutoff - Lcutoff) / 2.0
	double cp;        // cosine of phi
	double st;        // sine of theta
	double ct;        // cosine of theta
	double s2t;       // sine of 2*theta
	double c2t;       // cosine 0f 2*theta
	double *RCoeffs;     // z^-2 coefficients
	double *TCoeffs;     // z^-1 coefficients
	double *DenomCoeffs;     // dk coefficients
	double PoleAngle;      // pole angle
	double SinPoleAngle;     // sine of pole angle
	double CosPoleAngle;     // cosine of pole angle
	double a;         // workspace variables

	cp = cos(PI * (Ucutoff + Lcutoff) / 2.0);
	theta = PI * (Ucutoff - Lcutoff) / 2.0;
	st = sin(theta);
	ct = cos(theta);
	s2t = 2.0*st*ct;        // sine of 2*theta
	c2t = 2.0*ct*ct - 1.0;  // cosine of 2*theta

	RCoeffs = (double *)calloc(2 * FilterOrder, sizeof(double));
	TCoeffs = (double *)calloc(2 * FilterOrder, sizeof(double));

	for (k = 0; k < FilterOrder; ++k)
	{
		PoleAngle = PI * (double)(2 * k + 1) / (double)(2 * FilterOrder);
		SinPoleAngle = sin(PoleAngle);
		CosPoleAngle = cos(PoleAngle);
		a = 1.0 + s2t*SinPoleAngle;
		RCoeffs[2 * k] = c2t / a;
		RCoeffs[2 * k + 1] = s2t*CosPoleAngle / a;
		TCoeffs[2 * k] = -2.0*cp*(ct + st*SinPoleAngle) / a;
		TCoeffs[2 * k + 1] = -2.0*cp*st*CosPoleAngle / a;
	}

	DenomCoeffs = TrinomialMultiply(FilterOrder, TCoeffs, RCoeffs);
	free(TCoeffs);
	free(RCoeffs);

	DenomCoeffs[1] = DenomCoeffs[0];
	DenomCoeffs[0] = 1.0;
	for (k = 3; k <= 2 * FilterOrder; ++k)
		DenomCoeffs[k] = DenomCoeffs[2 * k - 2];


	return DenomCoeffs;
}








std::vector<double> filter(std::vector<double> B, std::vector<double> A, std::vector<double> Xs){
	/*
	 * 
	 *		(C++ implementation of matlab function y=filter(b,a,x))
	 *
	 *		It filters the data in vector X with the filter described by vectors A and B to create the filtered data Y.
	 *
	 *		The filter is a "Direct Form II Transposed" implementation of the standard difference equation:
	 *
	 *	    a(1)*y(n) = b(1)*x(n) + b(2)*x(n-1) + ... + b(nb+1)*x(n-nb)
	 *						      - a(2)*y(n-1) - ... - a(na+1)*y(n-na)
	 *	
	 */
	

	std::vector<double> Ys(Xs.size(), 0); //initialize filtered signal(output)

	// if necessary, normalize the coefficients by A(1)
	if (A[0] != 1){
		for (int i = 1; i < (int)A.size(); i++){
			A[i] = A[i] / A[0];
		}
		A[0] = 1;
	}

	Ys[0] = B[0] * Xs[0];

	
	for (int i = 1; i < (int)A.size(); i++){
		for (int j = 0; j <= i; j++){
			Ys[i] += B[j] * Xs[i - j];
		}
		for (int j = 1; j <= i; j++){
			Ys[i] -= A[j] * Ys[i - j];
		}
		
	}

	for (int i = (int)A.size(); i < (int)Xs.size(); i++){
		for (int j = 0; j < (int)B.size(); j++){
			Ys[i] += B[j] * Xs[i - j];
		}
		for (int j = 1; j < (int)A.size(); j++){
			Ys[i] -= A[j] * Ys[i - j];
		}

	}


	return Ys;

}


std::vector<double> preProcessSignal(std::vector<double> B, std::vector<double> A, std::vector<double> Xsignal, std::vector<double> B_low, std::vector<double> A_low, double mvc){

	/*
	 *		The preprocess an 1-D signal and returns the filtered signal. More particularly, the function process 
	 *		the input signal 'Xsignal' by the following order:
	 *
	 *		1. inserts the signal through a band-pass filter with coefficients B,A
	 *
	 *		2. rectifies the output of the band-pass filter
	 *
	 *		3. inserts the rectified signal to a low-pass filter with coefficients B_low, A_low
	 *
	 *		4. normalizes the output of the low-pass filter by the value of 'mvc'
	 *		
	 */


	
	// bandpass filtering
	std::vector<double> yTmp = filter(B, A, Xsignal);


	// rectification
	for (int i = 0; i < (int)Xsignal.size(); i++){
		yTmp[i] = abs(yTmp[i]);
	}


	// low-pass filtering
	yTmp = filter(B_low, A_low, yTmp);


	// normalization and store the outcome a vector
	for (int i = 0; i < (int)Xsignal.size(); i++){
		yTmp[i] = yTmp[i] / mvc;
	}


	return yTmp;

}


std::vector< std::vector<double> > preProcessTW(std::vector<double> Bbdp, std::vector<double> Abdp, std::vector<double> Blp, std::vector<double> Alp, std::vector<double> MVC, std::vector< std::vector<double> > timeWindow){

	/*
	*		The preprocess all the channels of a timeWindow and returns the filtered signal. More particularly, the function process
	*		the input signal 'Xsignal' by the following order:
	*
	*		Inputs:
	*
	*			- Bbdp,Abdp: the coefficients of a band-pass filter
	*
	*			- Blp,Alp: the coefficients of a low-pass filter
	*
	*			- MVC:  a vector containing the MVC values of all the channels
	*
	*			- timeWindow: a vector of vector containing the signals to be preprocessed. Each vector should correspond to a channel
	*
	*/

	std::vector< std::vector<double> > filteredTW;

	for (int i = 0; i < (int)timeWindow.size(); i++){
		filteredTW.push_back(preProcessSignal(Bbdp, Abdp, timeWindow[i], Blp, Alp,MVC[i]));
	}

	return filteredTW;
}

std::vector<complex<double>> prototypeAnalogLowPass(double filterOrder){

	//******************************************************************************
	// Lowpass analogue prototype. Places Butterworth poles evenly around
	// the S-plane unit circle.
	//
	//******************************************************************************

	vector <complex<double>> poles;

	for (int k = 0; k < (filterOrder + 1) / 2; k++){
		double theta = (double)(2 * k + 1) * PI / (2 * filterOrder);
		double real = -sin(theta);
		double imag = cos(theta);
		poles.push_back(complex<double>(real, imag));
		poles.push_back(complex<double>(real, -imag)); // conjugate
	}

	return poles;


}




double blt(complex<double> & sz){

	//******************************************************************************
	//
	// Z = (2 + S) / (2 - S) is the S-plane to Z-plane bilinear transform
	//
	//******************************************************************************



	complex<double> two(2.0, 0);
	complex<double> s = sz;      // sz is an input(S-plane) & output(Z-plane) arg
	sz = (two + s) / (two - s);

	// return the gain
	return abs((two - s));
}



bool ButterLP(double n, double Wn){

	std::vector< complex<double> > zeros(2 * (int)n);
	std::vector< complex<double> > poles(2 * (int)n);


	// get analog pre-wraped frequencies
	double fs = 2;
	double u = 2 * fs*tan(PI*Wn / fs);

	// convert to low-pass prototype estimate
	Wn = u;

	// **************************************************************
	//
	//    Design basic S-plane poles-only analogue LP prototype
	//
	//***************************************************************

	// get nth-order Butterworth analog low-pass prototype

	zeros = prototypeAnalogLowPass(n);

	int numPoles = (int)zeros.size();
	std::cout << "\n";
	for (int i = 0; i < numPoles; i++){
		std::cout << zeros[i] << "\n";
	}
	std::cout << "\n";

	int numZeros = 0;			// butterworth LP prototype has no zeros.
	double gain = 1.0;			// always 1 for the butterworth prototype lowpass.

	//
	//------------------------------------------------------------------------------


	//******************************************************************************
	//      
	//      Convert prototype to target filter type  - S-plane
	// 

	gain *= pow(Wn, numPoles);

	std::cout << "\npoles of s-state\n";
	for (int i = 0; i < numPoles; i++){    // scale poles by the cutoff Wc
		poles[i] = Wn * poles[i];
		std::cout << poles[i] << "\n";
	}


	//
	//--------------------------------------------------------------------------------


	//********************************************************************************
	//
	//        SANITY CHECK: Ensure poles are in the left half of the S-plane

	for (int i = 0; i < numPoles; i++){
		if (poles[i].real() > 0){
			std::cout << "[Butterworth Filter Design] Error: poles must be in the left half plane" << endl;
			return false;
		}
	}

	//
	//-------------------------------------------------------------------------------


	//******************************************************************************
	//
	//		Map zeros & poles from S-plane to Z-plane


	int nba = 0;
	double *ba = new double[2 * max(numPoles, numZeros) + 5];
	double preBLTgain = gain;

	// Convert poles & zeros from S-plane to Z-plane via Bilinear Tranform (BLT)

	// blt zeros
	for (int i = 0; i < numZeros; i++){
		gain /= blt(zeros[i]);
	}

	// blt poles
	for (int i = 0; i < numPoles; i++){
		gain *= blt(poles[i]);
	}


	// Split up Z-plane poles and zeros into SOS (second-order sections)


	int filterOrder = max(numZeros, numPoles);
	complex<double> * zerosTempVec = new complex<double>[filterOrder];
	complex<double> * polesTempVec = new complex<double>[filterOrder];

	// Copy
	for (int i = 0; i < numZeros; i++){
		zerosTempVec[i] = zeros[i];
	}

	// Add zeros at -1, so if S-plane degenerate case where
	// numZeros = 0 will map to -1 in Z-plane.
	for (int i = numZeros; i < filterOrder; i++){
		zerosTempVec[i] = complex<double>(-1, 0);
	}

	// Copy
	for (int i = 0; i < numPoles; i++){
		polesTempVec[i] = poles[i];
	}

	ba[0] = gain; // store gain

	int numSOS = 0;
	for (int i = 0; i + 1 < filterOrder; i += 2, numSOS++){
		ba[4 * numSOS + 1] = -(zerosTempVec[i] + zerosTempVec[i + 1]).real();
		ba[4 * numSOS + 2] = (zerosTempVec[i] * zerosTempVec[i + 1]).real();
		ba[4 * numSOS + 3] = -(polesTempVec[i] + polesTempVec[i + 1]).real();
		ba[4 * numSOS + 4] = (polesTempVec[i] * polesTempVec[i + 1]).real();
	}

	// Odd filter order thus one pair of poles/zeros remains
	if (filterOrder % 2 == 1){
		ba[4 * numSOS + 1] = -zerosTempVec[filterOrder - 1].real();
		ba[4 * numSOS + 2] = ba[4 * numSOS + 4] = 0;
		ba[4 * numSOS + 3] = -polesTempVec[filterOrder - 1].real();
		numSOS++;
	}

	// Set output param
	nba = 1 + 4 * numSOS;

	delete[] zerosTempVec;
	delete[] polesTempVec;

	// correct the overall gain
	ba[0] = preBLTgain * (preBLTgain / gain);


	std::cout << "\n\n";
	for (int i = 0; i < filterOrder; i++){
		std::cout << ba[i] << "\n";
	}

	std::cout << "\n";

	//******************************************************************************
	// Init biquad chain with coefficients from SOS





}