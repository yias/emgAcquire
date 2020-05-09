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

#ifndef EMGACQUIRE_H
#define EMGACQUIRE_H

#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>

#include "socketStream.h"
#include "jsonWrapper.hpp"
#include "Acquisition.h"

namespace emgacquire{

    class Client{

        // comunication related variables
        std::string hostIP;                                 // the IP of the machine that the server is running. It can be localhost too, if the server runs localy (default)
        unsigned int portIP;                                // the IP port of the server machine (default is 10352)
        std::string clName;
        bool isNewMsgReceived;                              // a boolean variable for checking if the message is new or not
        socketStream svrHdlr;                               // socketStream object for handling the communication with the server
        jsonWrapper json_msg;                               // the data of the received message
        std::thread listenerThread;                         // a thread for listening to the server in parallel

        // acquisition-related variables
        std::string svr_acq_device;                         // the name of the acquisition device from the server
        std::string signals_type;                           // the type of signals the server streams (it will be EMG)
        float svr_acq_frequency;                     // the real frequency of the acquisition device
        unsigned int nb_channels_received;                  // the number of EMG channels the server streams

        // functionality varaibles
        float frequency;                                    // the frequency at which the client will acquire the data (default value 20Hz)
        std::string msg;                                    // a variable to hold the received

        unsigned int bufferSize;                            // the number of samples to hold as a buffer for each channel (default value 300)
        std::vector< std::vector<double> > buffer;          // the buffer for holding the received data

        unsigned int nb_channels_required;                  // the number of channels required by the client (default value 2)

        std::chrono::high_resolution_clock::time_point give_msg_time;   // a time-point object for stabilizing the frequency of the client

        bool isRunning;

        int listening_to_server();

    public:
        Client(){};
        Client(float freq, unsigned int nb_ch);
        Client(std::string hostIP, unsigned int port, float freq, unsigned int nb_ch);
        ~Client();

        int setNBofChannels(unsigned int nb_ch);
        int setFrequency(float freq);
        int setServerIP(std::string svrIP);
        int setServerIP(std::string svrIP, unsigned int port);
        int setBufferSize(unsigned int bfrSize);
        
        std::string getDeviceName();
        std::string getSignalsType();
        float getSvrFrequency();
        unsigned int getNBofChannelsReceived();

        std::vector< std::vector<double> > getSignals();

        int initialize();
        int start();
        int shutdown();

    };

}


#endif