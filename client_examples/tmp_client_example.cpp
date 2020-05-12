

#include "emgAcquire.h"

#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

int main(){

    float freq = 20;
    float nb_ch = 2;

    emgAcquire::Client emgListener(freq, nb_ch);

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

        // std::cout << "data shape: " << mydata.size() << ", " << mydata[0].size() << std::endl;

        if(kbhit()){
            if(getch()=='q')
                break;
        }
        end = std::chrono::high_resolution_clock::now();
        timeEllapsed = std::chrono::duration<double, std::micro>(end-start).count();
        timings.push_back(timeEllapsed);
    }

    emgListener.shutdown();

    double sumUpdate = 0;
    // find the average computational time and print it in the terminal
    for(auto& n: timings){
        sumUpdate +=n;
    }
    double aveUpdate= sumUpdate/(double)timings.size();

    // std::cout << std::endl;

    std::cout << "in main averave time ellapsed: " << aveUpdate << " microseconds" << std::endl;

    return 0;
}