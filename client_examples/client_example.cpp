/**
 * 
 *  Examples on how to use the socketStream class for implementing 
 *  a TCP client and listen to server
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  
 *  
*/


#include "acquireFilters.hpp"
#include "emgAcquire.h"

#include <vector>
#include <iostream>
#include <chrono>
#include <thread>


int main(){

    // define the acquisition frequency of the packet signals
    float freq = 20;

    // define the number of signals to acquire
    float nb_ch = 16;

    emgAcquire::Client emgListener(freq, nb_ch);

    // emgListener.setServerIP("localhost");

    // emgListener.setResampling(false);
    
    emgListener.setKeepLog(true);
    emgListener.setDigitalSignalReturn(false);

    if(emgListener.initialize()<0){
        std::cout << "unable to initialize" << std::endl;
        return -1;
    }

    std::vector< std::vector<double>> mydata;

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    double timeEllapsed;// = std::chrono::duration<double, std::micro>(end-start).count();

    std::vector<double> timings;

    emgListener.start();

    while(true){
        start = std::chrono::high_resolution_clock::now();
        mydata = emgListener.getSignals();

        if(kbhit()){
            if(getch()=='q')
                break;
        }
        end = std::chrono::high_resolution_clock::now();
        timeEllapsed = std::chrono::duration<double, std::micro>(end-start).count();
        timings.push_back(timeEllapsed);
    }

    emgListener.shutdown();

    std::cout << "in main averave updating time: " << acquireFilters::average(timings) << " "; 
       
    #ifdef _WIN32
        std::cout << (char)241 << " ";
    #else
        std::cout << "\u00b1 ";
    #endif
        
    std::cout << acquireFilters::standDev(timings) << " microseconds" << std::endl;

    return 0;
}