
#include "socketStream.h"
#include "jsonWrapper.hpp"
#include "Acquisition.h"
#include <chrono>
#include <thread>


int main(int argc, char **argv){

    Acquisition emgAcq;

    float freq = 20.0;
    int nb_channels = 16;

    if(emgAcq.initialize(freq, nb_channels)<0){
        std::cout << "Unable to initialize devices" << std::endl;
        return -1;
    }

    // define the variable that holds the server IP. In this case, the server would be a local server.
    const char *srvIP = "128.179.140.26";

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
    fields.push_back("device_name");    // 0
    fields.push_back("type");           // 1
    fields.push_back("desired_freq");   // 2
    fields.push_back("device_freq");    // 3
    fields.push_back("nb_channels");    // 4
    fields.push_back("data");           // 5
    fields.push_back("time_interval");  // 6

    
    // initialize the message by setting the fields' names
    if(svrHdlr.initialize_msgStruct(fields)<0){
        std::cerr << "Unable to inizialize message structure" << std::endl;
        return -1;
    }


    // update the message with acquisition information
    const char *dev_name = emgAcq.getDeviceName().c_str();
    if(svrHdlr.updateMSG(fields[0], "Noraxon Desk Receiver")){
        std::cerr << "Unable to update the message" << std::endl;
        return -2;
    }


    if(svrHdlr.updateMSG(fields[1], "EMG") ){
        std::cerr << "Unable to update the message" << std::endl;
        return -2;
    }


    if(svrHdlr.updateMSG(fields[2], (double)emgAcq.getSamplingFrequency()) ){
        std::cerr << "Unable to update the message" << std::endl;
        return -2;
    }

    
    if(svrHdlr.updateMSG(fields[3], 1500.0) ){ // (double)emgAcq.getRealSamplingFrequency()
        std::cerr << "Unable to update the message" << std::endl;
        return -2;
    }

    
    if(svrHdlr.updateMSG(fields[4], emgAcq.getNumberOfChannels()) ){
        std::cerr << "Unable to update the message" << std::endl;
        return -2;
    }

    

    
    // svrHdlr.printMSGcontentsTypes();

    // svrHdlr.printMSGcontents();


    std::vector< std::vector<double> > emgData;

    // declare a variable to hold the receiving times from the device
    std::vector<double> updateTimings;
    std::vector<double> sleepingTimings;

    // define objects to hold the current and elapsed time
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    double timeElapsed;

    // define sleep time for keepring the frequency
    std::chrono::milliseconds timespan((int)(1000.0/freq));

    // initialize socketStream
    svrHdlr.initialize_socketStream();

    // activate the server
    svrHdlr.runServer();

    // activate acquisition
    emgAcq.activate();
    
    // set the acquisition to run continuously in the background
    emgAcq.runContinuously();
    bool isNewMsg = false;

    //debug
    std::string msg;

    start = std::chrono::high_resolution_clock::now();
    while(true){
        
        
        emgData = emgAcq.getlatest(&isNewMsg);
        if(isNewMsg){
            // std::cout << "size: " << emgData.size() << ", " << emgData[0].size() << std::endl;
            if(svrHdlr.updateMSG(fields[6], emgAcq.getTimeInterval()) ){
                std::cerr << "Unable to update the message" << std::endl;
                return -2;
            }
            if(svrHdlr.updateMSG(fields[5], emgData) ){
                std::cerr << "[emgAcquire] Unable to update the message" << std::endl;
            }

            // msg=svrHdlr.getFullmsg();
            // jsonWrapper testObj(msg);
            
            if(svrHdlr.socketStream_ok()){
                svrHdlr.sendMSg2All();
                // svrHdlr.sendMSg2Client("listener");
            }

            end = std::chrono::high_resolution_clock::now();

            timeElapsed = std::chrono::duration<double, std::micro>(end-start).count()/1000.0;
            
            start = std::chrono::high_resolution_clock::now();
            updateTimings.push_back(timeElapsed);
        }


        // if(emgAcq.update()==0){

        //     emgData = emgAcq.getlatest();

        //     // std::cout << "size: " << emgData.size() << ", " << emgData[0].size() << std::endl;

        //     end = std::chrono::high_resolution_clock::now();
        //     timeEllapsed = std::chrono::duration<double, std::micro>(end-start).count()/1000.0;
        //     // std::cout << "te: " << timeEllapsed << std::endl;
        //     // std::cout << "te int: " << (int)timeEllapsed << std::endl;
        //     start = std::chrono::high_resolution_clock::now();
        //     updateTimings.push_back(timeEllapsed);
            

        //     // sleepingTimings.push_back((double) (timespan - (start-end)).count() );

        //     std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(int(1000/freq)-2)); // 48
        // }else{
        //     // std::cout<<"yes\n";
        // }
        
        if(kbhit()){
            if(getch()=='q')
                break;
        }
    }

    emgAcq.stop();
    double sumUpdate = 0;
    // find the average computational time and print it in the terminal
    for(auto& n: updateTimings){
        sumUpdate +=n;
    }
    double aveUpdate= sumUpdate/(double)updateTimings.size();

    std::cout << "average update time: " << aveUpdate << " ms" << std::endl;
    std::cout << "average frequency: " << 1/(aveUpdate/1000.0) << " Hz" << std::endl;


    // kill all the connections and the socket
    svrHdlr.closeCommunication();

    return 0;

}