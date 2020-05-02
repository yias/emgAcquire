
#include "socketStream.h"
#include "jsonWrapper.hpp"
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

    // define the variable that holds the server IP. In this case, the server would be a local server.
    const char *srvIP = "localhost";

    int svrPort = 10352;

    // if no new server IP is defined from the user, continue with the pre-defined server IP (localhost)
    if(argc!=2){
        std::cout << "No server IP provided. Continue with localhost" << std::endl;
    }else{
        srvIP=argv[1];
    }

    // create an sockectStream object with the selected server IP address and set it up as a server
    socketStream svrHdlr(srvIP, svrPort, SOCKETSTREAM::SOCKETSTREAM_SERVER);

    // decalre a variable as a vector of strings for containing the fields of the message
    std::vector <std::string> fields;

    // define the name of the fields
    fields.push_back("type");
    fields.push_back("data");
    fields.push_back("desired_freq");
    fields.push_back("real_freq");
    fields.push_back("nb_channels");

    // initialize the message by setting the fields' names
    if(svrHdlr.initialize_msgStruct(fields)<0){
        std::cerr << "Unable to inizialize message structure" << std::endl;
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