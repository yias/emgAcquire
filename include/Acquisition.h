/** 
 *  Copyright (c) 2020 Iason Batzianoulis
 *  
 *  Acquisition header file
 *  
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    MIT
 * 
**/

#ifndef ACQUISITION_H
#define ACQUISITION_H

#include "stdafx.h"
#include <vector>
#include <string>
#include <iostream>


class Acquisition{
    
    float sampling_frequency;

    Easy2AcquireCom::IDeviceManagerPtr dm;                                              // device manager
    Easy2AcquireCom::IDevicePtr device;                                                 // the device from which to acquire the data

    unsigned int nb_analog_devices;                                                     // number of analog-input devices
    std::vector<Easy2AcquireCom::IAnalogInputPtr> analogInputDevices;                   // analog input devices (for emg)
    std::vector< std::vector<double> > analogData;                                      // data from the analog inputs
    std::vector< _variant_t > analogBuffers;                                            // buffers for the analog inputs
    // double **analogBuffers;
    std::vector<float> real_freq_AI;                                                    // the real frequencies of the analog inputs

    unsigned int nb_digital_devices;                                                    // number of digital-input devices
    std::vector<Easy2AcquireCom::IDigitalInputPtr> digitalInputDevice;                  // degital input devices (for switches)
    std::vector< std::vector<double> > digitalData;                                     // data from the digital inputs
    std::vector< _variant_t > digitalBuffers;                                           // buffers for the digital inputs
    std::vector<float> real_freq_DI;                                                    // the real frequencies of the digital inputs

    int printComponents(std::string t_prefix);
    int initializeInput(LONG index, _bstr_t type);

public:
    int initialize(float freq, unsigned int selected_nb_signals=0);
    int activate();
    int update();
    int getlatest();
    Acquisition();
    ~Acquisition();
};


#endif