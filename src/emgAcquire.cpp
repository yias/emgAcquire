
#include "Acquisition.h"
#include <chrono>
#include <thread>


int main(int argc, char **argv){

    Acquisition emgAcq;

    float freq = 10.0;
    int nb_channels = 2;

    if(emgAcq.initialize(freq, nb_channels)<0){
        std::cout << "Unable to initialize devices" << std::endl;
        return -1;
    }

    // declare a variable to hold the receiving times from the device
    std::vector<double> updateTimings;

    // define an object to hold the current time
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();

    // define sleep time for keepring the frequency
    std::chrono::milliseconds timespan((int)(1000.0/freq));

    emgAcq.activate();
    start = std::chrono::steady_clock::now();
    while(true){
        
        if(emgAcq.update()==0){

            end = std::chrono::steady_clock::now();
            updateTimings.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()/1000.0);

            start = std::chrono::steady_clock::now();

            std::this_thread::sleep_for(timespan - (start - end));
        }else{
            // std::cout<<"yes\n";
        }
        
        if(kbhit()){
            if(getch()=='q')
                break;
        }
    }

    double sumUpdate = 0;
    // find the average computational time and print it in the terminal
    for(auto& n: updateTimings){
        sumUpdate +=n;
    }
    double aveUpdate= sumUpdate/(double)updateTimings.size();
    std::cout << "average update time: " << aveUpdate << " ms" << std::endl;
    std::cout << "average frequency: " << 1/(aveUpdate/1000.0) << " Hz" << std::endl;

    return 0;
}