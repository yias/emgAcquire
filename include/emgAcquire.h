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




#ifdef _WIN32
    // #include <windows.h>
    #include <codecvt>
#endif
    
#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>

#include "socketStream.h"
#include "jsonWrapper.hpp"
// #include "Acquisition.h"

namespace emgAcquire{

    const char DEFAULT_SVR_IP[] = "localhost";
    const unsigned int DEFAULT_SVR_PORT = 10352;
    const unsigned int DEFAULT_BUFFER_SIZE = 300;
    const unsigned int MAXIMUM_BUFFER_SIZE = 5000;
    const float DEFAULT_FREQUENCY = 20.0;
    const float MAXIMUM_FREQUENCY = 1500.0;
    const unsigned int DEFAULT_NB_CHANNELS = 2;
    const unsigned int MAXIMUM_NB_CHANNELS = 16;

    class Client{

        // comunication related variables
        std::string hostIP;                                 // the IP of the machine that the server is running. It can be localhost too, if the server runs localy (default)
        unsigned int svrPort;                               // the IP port of the server machine (default is 10352)
        std::string clName;
        bool isNewMsgReceived;                              // a boolean variable for checking if the message is new or not
        socketStream svrHdlr;                               // socketStream object for handling the communication with the server
        jsonWrapper json_msg;                               // the data of the received message
        std::thread listenerThread;                         // a thread for listening to the server in parallel
        std::mutex threadMutex;                             // a mutex object for handling share memory between threads

        // acquisition-related variables
        std::string svr_acq_device;                         // the name of the acquisition device from the server
        std::string signals_type;                           // the type of signals the server streams (it will be EMG)
        float svr_acq_frequency;                            // the real frequency of the acquisition device
        unsigned int nb_channels_received;                  // the number of signals channels the server streams

        // functionality varaibles
        float frequency;                                    // the frequency at which the client will acquire the data (default value 20Hz)
        unsigned int nb_channels_required;                  // the number of channels required by the client (default value 2)
        std::string msg;                                    // a variable to hold the received

        unsigned int bufferSize;                            // the number of samples to hold as a buffer for each channel (default value 300)
        std::vector< std::vector<double> > buffer;          // the buffer for holding the received data
        bool interpolate;                                   // a bool flag for signalling the use of interpolation or not
        bool giveDigitalSignal;                             // a bool flag for returning or not the digital signal of the device

        std::chrono::high_resolution_clock::time_point give_msg_time;   // a time-point object for stabilizing the frequency of the client

        bool isRunning;                                     // a boolian variable for signalling is the node is running

        int listening_to_server();                          // a function for listening to the server and acquire the signals asynchronously to the main thread

    public:
        Client(){};                                                                         // empty contructor
        Client(unsigned int nb_ch);                                                         // constructor with setting only the number of required signal channels
        Client(float freq);                                                                 // constructor with setting only the required frequency
        Client(float freq, unsigned int nb_ch);                                             // constructor with setting the desired frequency and the numer of channels required from the client
        Client(std::string hostIP);                                                         // constructor with setting the IP of the server
        Client(std::string hostIP, unsigned int port, float freq, unsigned int nb_ch);      // constructor with setting all the above including the communication port
        ~Client();

        int setNBofChannels(unsigned int nb_ch);                                            // setting the number of channels to be retrieved (returned to the client)
        int setFrequency(float freq);                                                       // setting the desired acquisition frequency
        int setServerIP(std::string svrIP);                                                 // setting the IP address of the server
        int setServerIP(std::string svrIP, unsigned int port);                              // setting the IP address and the port of the server
        int setBufferSize(unsigned int bfrSize);                                            // setting the buffer-size
        int setInterpolation(bool interpol);                                                // setting if the signal interpolation is desired or not
        int setDigitalSignalReturn(bool digSingalReturn);                                   // setting if the client wants the digital signal to be returned or not
        
        std::string getDeviceName();                                                        // returning the acquisition device name
        std::string getSignalsType();                                                       // getting the type of the signals
        float getSvrFrequency();                                                            // getting the device acquisition frequency
        unsigned int getNBofChannelsReceived();                                             // get the overall number of channels received from the server

        std::vector< std::vector<double> > getSignals();                                    // get the signals (EMG and the digital signal if giveDigitalSignal is true)

        int initialize();                                                                   // initalization of the node
        int start();                                                                        // start the acquisition (start filling the buffer)
        int shutdown();                                                                     // shutdown the node (close all communication, shutdown all the threads and clear the buffer)
    };

    int getComputerName(std::string *pc_name);                                              // get the name of the pc to introduce it to the client name
}


#endif