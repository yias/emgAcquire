
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
    fields.push_back("device_name");    // 0
    fields.push_back("type");           // 1
    fields.push_back("desired_freq");   // 2
    fields.push_back("device_freq");    // 3
    fields.push_back("nb_channels");    // 4
    fields.push_back("data");           // 5

    
    // initialize the message by setting the fields' names
    if(svrHdlr.initialize_msgStruct(fields)<0){
        std::cerr << "Unable to inizialize message structure" << std::endl;
        return -1;
    }


    // update the message with acquisition information
    const char *dev_name = emgAcq.getDeviceName().c_str();
    if(svrHdlr.updateMSG(fields[0], dev_name)){
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

    
    // // define a 2D matrix and update the field "data" of the message
    // double t_value1[] = {1.5, 4.67, 50.095, 14.99, 12, 45.11, 7.986, 134.67, 99.324, 0.452, 1.5, 4.67, 50.095, 14.99, 12, 45.11, 7.986, 134.67, 99.324, 0.452,1.5, 4.67, 50.095, 14.99, 12, 45.11, 7.986, 134.67, 99.324, 0.452, 3.2, 15.4, 1502.898, 12, 5.4, 0.569, 12.33, 5311.1, 234.22, 14.45, 3.2, 15.4, 1502.898, 12, 5.4, 0.569, 12.33, 5311.1, 234.22, 14.45};
    // double t_value2[] = {3.2, 15.4, 1502.898, 12, 5.4, 0.569, 12.33, 5311.1, 234.22, 14.45, 1.5, 4.67, 50.095, 14.99, 12, 45.11, 7.986, 134.67, 99.324, 0.452,1.5, 4.67, 50.095, 14.99, 12, 45.11, 7.986, 134.67, 99.324, 0.452, 3.2, 15.4, 1502.898, 12, 5.4, 0.569, 12.33, 5311.1, 234.22, 14.45, 3.2, 15.4, 1502.898, 12, 5.4, 0.569, 12.33, 5311.1, 234.22, 14.45};
    // std::vector< std::vector<double> > t_value(2);
    // t_value[0]=std::vector<double>(t_value1, t_value1 +(sizeof(t_value1)/sizeof(t_value1[0])));
    // t_value[1]=std::vector<double>(t_value2, t_value2 +(sizeof(t_value2)/sizeof(t_value2[0])));

    
    // if(svrHdlr.updateMSG(fields[5], t_value) ){
    //     std::cerr << "Unable to update the message" << std::endl;
    //     return -2;
    // }



    // svrHdlr.printMSGcontentsTypes();

    // svrHdlr.printMSGcontents();

//***********************************************************************************************************************

    std::vector< std::vector<double> > emgData;

    // declare a variable to hold the receiving times from the device
    std::vector<double> updateTimings;
    std::vector<double> sleepingTimings;

    // define an object to hold the current time
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    double timeEllapsed;

    // define sleep time for keepring the frequency
    std::chrono::milliseconds timespan((int)(1000.0/freq));

    emgAcq.activate();
    
    emgAcq.runContinuously();
    bool isNewMsg = false;

    start = std::chrono::high_resolution_clock::now();
    while(true){
        
        
        emgData = emgAcq.getlatest(&isNewMsg);
        if(isNewMsg){
            // std::cout << "size: " << emgData.size() << ", " << emgData[0].size() << std::endl;
            if(svrHdlr.updateMSG(fields[5], emgData) ){
                std::cerr << "Unable to update the message" << std::endl;
                return -2;
            }
            end = std::chrono::high_resolution_clock::now();
            timeEllapsed = std::chrono::duration<double, std::micro>(end-start).count()/1000.0;
            // std::cout << "te: " << timeEllapsed << std::endl;
            // std::cout << "te int: " << (int)timeEllapsed << std::endl;
            start = std::chrono::high_resolution_clock::now();
            updateTimings.push_back(timeEllapsed);
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

//***********************************************************************************************************************

    // sumUpdate = 0;
    // // find the average computational time and print it in the terminal
    // for(auto& n: sleepingTimings){
    //     sumUpdate +=n;
    // }
    // aveUpdate= sumUpdate/(double)updateTimings.size();
    // std::cout << "average sleeping time: " << aveUpdate << " ms" << std::endl;

    return 0;
}