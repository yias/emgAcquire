/** 
 *  Copyright (c) 2020 Iason Batzianoulis
 *  
 *  emgAcquire client header file for exporting functions to lib
 *  
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    GNU GPL v3
 * 
**/

#ifndef EMGACQUIRE_DLLEXPORT_H
#define EMGACQUIRE_DLLEXPORT_H

#ifdef __linux__
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <limits.h>
#endif

// #define MAKEDLL 1

#ifdef MAKEDLL
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif


#ifdef _WIN32
    // #include <windows.h>
    #include <codecvt>
#endif
    
#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <fstream>
#include <ctime>

#include "socketStream.h"
#include "jsonWrapper.hpp"
#include "acquireFilters.hpp"
// #include "Acquisition.h"

namespace emgAcquire{

    const char DEFAULT_SVR_IP[] = "localhost";
    const unsigned int DEFAULT_SVR_PORT = 10352;
    const unsigned int DEFAULT_BUFFER_SIZE = 1000;
    const unsigned int SMALL_BUFFER_SIZE = 2;
    const unsigned int MAXIMUM_BUFFER_SIZE = 5000;
    const float DEFAULT_FREQUENCY = 20.0;
    const float MAXIMUM_FREQUENCY = 1500.0;
    const float DEFAULT_RESAMPLE_FREQUENCY = 1000;
    const unsigned int RESAMPLE_WINDOW_LENGTH = 3;
    const unsigned int DEFAULT_NB_CHANNELS = 2;
    const unsigned int MAXIMUM_NB_CHANNELS = 16;

    class EXPORT Client{

        // comunication related variables
        std::string hostIP;                                 // the IP of the machine that the server is running. It can be localhost too, if the server runs localy (default)
        unsigned int svrPort;                               // the IP port of the server machine (default is 10352)
        std::string clName;
        bool isFirstMsgReceived;                            // a boolean variable for checking if the message is new or not
        socketStream comHdlr;                               // socketStream object for handling the communication with the server
        jsonWrapper json_msg;                               // the data of the received message
        bool isConnected;                                   // a flag indicating if a connection with the server exists
        
        std::thread listenerThread;                         // a thread for listening to the server in parallel
        std::mutex threadMutex;                             // a mutex object for handling share memory between threads
        std::condition_variable cv;                         // a condition variable to wait for the buffer
        

        // acquisition-related variables
        std::string svr_acq_device;                         // the name of the acquisition device from the server
        std::string signals_type;                           // the type of signals the server streams (it will be EMG)
        float svr_acq_frequency;                            // the real frequency of the acquisition device
        unsigned int nb_channels_received;                  // the number of signals channels the server streams

        // functionality varaibles
        float frequency;                                    // the frequency at which the client will acquire the data (default value 20Hz)
        unsigned int nb_samples_to_return;                  // the number of samples to return to client
        unsigned int nb_samples_to_get;                     // the number of sampples to get from the buffer
        float cycle_time_ms;                                // the cycle time in milli-seconds
        unsigned int remainder;                             // the remainder of the samples to be sent
        bool hasRemainder;                                  // a flag to signal if the number of samples to return is not even
        bool giveRemainder;                                 // a flag to signal if the remainder is been given to the client or not
        unsigned int nb_channels_required;                  // the number of channels required by the client (default value 2)
        std::string msg;                                    // a variable to hold the received

        unsigned int bufferSize;                            // the number of samples to hold as a buffer for each channel (default value 300)
        std::vector< std::vector<double> > buffer;          // the buffer for holding the received data
        std::vector< std::vector<double> > small_buffer;    // a small buffer to acquire the last samples of the message
        std::vector<int> bufferIndexes;                     // the indexes indicating where the last sample is
        bool is_buffer_ok;                                  // a flag to signal if the buffer has the required amount of samples
        bool resample;                                      // a bool flag for signalling the use of interpolation or not
        bool giveDigitalSignal;                             // a bool flag for returning or not the digital signal of the device

        std::chrono::high_resolution_clock::time_point give_msg_time;   // a time-point object for stabilizing the frequency of the client

        bool isRunning;                                     // a boolian variable for signalling is the node is running (start filling the buffer)
        bool initialize_ok;                                 // a flag indicating that initialization is done

        bool keeplog;                                       // a flag indivating if logfiles will be kept
        std::string logFileName;                            // the name of the logfile
        std::string logFolderName;                          // the name of the folder to keep the logfile
        std::ofstream wfile;
        std::ofstream csvfile;
        std::chrono::high_resolution_clock::time_point startingTime; // the time the program started
        int openLogFile();                                  // function to create a folder id need and open the logfile
        int closeLogfile();                                 // function to close the logfile
        

        int listening_to_server();                          // a function for listening to the server and acquire the signals asynchronously to the main thread
        int updateBuffer(std::vector< std::vector<double> > mdata);                                 // updating the buffer (check if size of the buffer is reached, if yes rearrange, and append the signals to the buffer)
        bool updateIsRunning;                               // a flag indicating if the unpdate function is running

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
        int setResampling(bool doResample);                                                 // setting if the signal interpolation is desired or not
        int setKeepLog(bool choice);                                                        // function to set the keeplog flag
        int setDigitalSignalReturn(bool digSingalReturn);                                   // setting if the client wants the digital signal to be returned or not
        
        std::string getDeviceName();                                                        // returning the acquisition device name
        std::string getSignalsType();                                                       // getting the type of the signals
        float getSvrFrequency();                                                            // getting the device acquisition frequency
        unsigned int getNBofChannelsReceived();                                             // get the overall number of channels received from the server

        std::vector< std::vector<double> > getSignals();                                    // get the signals (EMG and the digital signal if giveDigitalSignal is true)

        int initialize();                                                                   // initalization of the node
        int start();                                                                        // start the acquisition (start filling the buffer)
        int stop();                                                                         // stop the acquisition (stop filling the buffer)
        int shutdown();                                                                     // shutdown the node (close all communication, shutdown all the threads and clear the buffer)
    };

    int getComputerName(std::string *pc_name);                                              // get the name of the pc to introduce it to the client name

    #ifdef __linux__
        int dirExists(std::string tpath);
    #endif

    extern "C"{
        EXPORT Client* new_Client(const char* hostIP, int hostPort, float freq, int nb_ch){
            std::string hIP (hostIP);
            std::cout << hIP << std::endl;
            return new Client(hIP, (unsigned int)hostPort, freq, (unsigned int)nb_ch);
        }
        EXPORT int client_initialize(Client *clnt){
            return clnt->initialize();
        }
        void EXPORT client_start(Client* clnt){
            clnt->start();
        }
        void EXPORT client_stop(Client* clnt){
            clnt->stop();
        }
        void EXPORT client_shutdown(Client* clnt){
            clnt->shutdown();
            delete clnt;
        }
        EXPORT double* client_getSignals(Client* clnt){
            std::vector< std::vector<double> > tmp_data = clnt->getSignals();
            int nb_rows = tmp_data.size();
            int nb_cols = tmp_data[0].size();

            std::vector<double> returnedVector;
            returnedVector.reserve(nb_rows * nb_cols);

            for (int i=0; i < nb_rows; i++){
                returnedVector.insert(returnedVector.end(), tmp_data[i].begin(), tmp_data[i].end());
            }

            
            return returnedVector.data();
        }
    }
}


#endif