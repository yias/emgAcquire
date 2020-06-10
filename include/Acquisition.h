/** 
 *  Copyright (c) 2020 Iason Batzianoulis
 *  
 *  Acquisition header file
 *  
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    GNU GLP v3
 * 
**/

#ifndef ACQUISITION_H
#define ACQUISITION_H

#include "stdafx.h"
#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>


class Acquisition{
    
    float sampling_frequency;
    std::string device_name;

    Easy2AcquireCom::IDeviceManagerPtr dm;                                              // device manager
    Easy2AcquireCom::IDevicePtr device;                                                 // the device from which to acquire the data

    unsigned int nb_analog_devices;                                                     // number of analog-input devices
    std::vector<Easy2AcquireCom::IAnalogInputPtr> analogInputDevices;                   // analog input devices (for emg)
    std::vector< std::vector<double> > analogData;                                      // data from the analog inputs
    std::vector< _variant_t > analogBuffers;                                            // buffers for the analog inputs
    std::vector<float> real_freq_AI;                                                    // the real frequencies of the analog inputs

    unsigned int nb_digital_devices;                                                    // number of digital-input devices
    std::vector<Easy2AcquireCom::IDigitalInputPtr> digitalInputDevice;                  // degital input devices (for switches)
    std::vector< std::vector<double> > digitalData;                                     // data from the digital inputs
    std::vector< _variant_t > digitalBuffers;                                           // buffers for the digital inputs
    std::vector<float> real_freq_DI;                                                    // the real frequencies of the digital inputs

    std::thread continuousAcquirer;
    std::mutex threadMutex;
    bool isContinuous;
    bool isSetNew;

    std::chrono::high_resolution_clock::time_point gotNewTime;
    float acquisition_time_interval; 

    int printComponents(std::string t_prefix);
    int initializeInput(LONG index, _bstr_t type);
    int continuousUpdate();

public:

    Acquisition();

    int initialize(float freq, unsigned int selected_nb_signals=0);
    int activate();
    
    int update();
    std::vector< std::vector<double> > getlatest();
    std::vector< std::vector<double> > getlatest(bool* isNew);

    std::vector< std::vector<double> > getlatestAnalog();
    std::vector< std::vector<double> > getlatestAnalog(bool* isNew);
    
    std::vector< std::vector<double> > getlatestDigital();
    std::vector< std::vector<double> > getlatestDigital(bool* isNew);

    float getTimeInterval();

    std::string getDeviceName();
    float getSamplingFrequency();
    float getRealSamplingFrequency();
    std::vector<int> getNumberOfChannels();
    
    int runContinuously();
    int stop();

    ~Acquisition();
};


#endif