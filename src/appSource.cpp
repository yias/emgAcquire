
#include "socketStream.h"
#include "jsonWrapper.hpp"
#include "Acquisition.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <conio.h>
#include <mutex>

#include <atlbase.h>

extern CComModule _Module;

#include <atlcom.h>

#include <sapi.h>


int speech_handler(std::vector<std::wstring> sentences, int nbRepetitions);
int write2csv(std::ofstream& csvFile, int nbLabels, int label, double timeStamp, double timeElapsed, std::vector< std::vector<double> > emgData);

int label;

bool isRunning;

std::mutex threadMutex;


int main(int argc, char **argv){

    Acquisition emgAcq;

    float freq = 20.0;
    int nb_channels = 6;
    bool audioCue = true;
    bool isRecording = true;
    std::vector< std::wstring> sentences;
    std::thread spThread;
    std::ofstream csvFile;
    int nbRepetitions;

    ///////////////////////////////////////////////////////////////////////////////////
    // initialization if isRecording is true

    if (isRecording){
        
        nbRepetitions = 30;
        std::string data_fname = "data\\emg_data_s2.csv";

        if (audioCue){
            
            sentences.push_back(L"Flex Wrist");
            sentences.push_back(L"Extend Wrist");
            sentences.push_back(L"Extend Thumb");
            sentences.push_back(L"Flex Biceps");
            sentences.push_back(L"Flex Triceps");
            sentences.push_back(L"Flex Shoulder");
        }

        

        csvFile.open(data_fname);
        if(!csvFile.is_open()){
            std::cerr << "[emgAcquireClient] Unable to open csv logfile" <<std::endl;
            return -2;
        }

        std::string header("time;");

        for (int i=0; i<nb_channels; i++){
            header += ("ch" + std::to_string(i+1) + ";");
        }
         

        if (audioCue){
            for(int i=0; i<(int)sentences.size();i++){
                header += ("label" + std::to_string(i+1) + ";");
            }
        }

        header += "\n";

        csvFile << header;

        label = 0;
        
    }


    ////////////////////////////////////////////////////////////////////////////////////

    if(emgAcq.initialize(freq, nb_channels)<0){
        std::cout << "Unable to initialize devices" << std::endl;
        return -1;
    }

    // define the variable that holds the server IP. In this case, the server would be a local server.
    const char *srvIP = "localhost"; // 128.179.140.26

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
    auto acquisitionSTime = std::chrono::high_resolution_clock::now();
    auto acquiredTime = std::chrono::high_resolution_clock::now();
    double timeElapsed;
    double acquiredTime_d;

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

    if(audioCue){
        spThread = std::thread(&speech_handler, sentences, nbRepetitions);
    }

    //debug
    std::string msg;
    acquisitionSTime = std::chrono::high_resolution_clock::now();
    start = std::chrono::high_resolution_clock::now();
    isRunning = true;
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

            // std::cout << emgData[0][0] << ", " << emgData[1][0] << ", " << emgData[2][0] << ", " << emgData[3][0] << std::endl;
            // msg=svrHdlr.getFullmsg();
            // jsonWrapper testObj(msg);
            
            if(svrHdlr.socketStream_ok()){
                svrHdlr.sendMSg2All();
                // svrHdlr.sendMSg2Client("listener");
            }

            

            end = std::chrono::high_resolution_clock::now();

            timeElapsed = std::chrono::duration<double, std::milli>(end-start).count();
            start = std::chrono::high_resolution_clock::now();
            updateTimings.push_back(timeElapsed);
            if(isRecording){
                acquiredTime = std::chrono::high_resolution_clock::now();
                acquiredTime_d = std::chrono::duration<double, std::milli>(acquiredTime-acquisitionSTime).count();
                write2csv(csvFile, (int)sentences.size(), label, acquiredTime_d, timeElapsed, emgData);
                if(audioCue){
                    if(!isRunning){
                        break;
                    }
                }
            }
            
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

    if(isRecording){
        csvFile.close();
        isRunning = false;
        if(spThread.joinable()){
            spThread.join();
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


    // kill all the connections and the socket
    svrHdlr.closeCommunication();

    return 0;

}


int speech_handler(std::vector<std::wstring> sentences, int nbRepetitions ){

    ISpVoice * pVoice = NULL;

    if (FAILED(::CoInitialize(NULL))){
        std::cout <<"COM not initialized\n";
        return -1;
    }
        
    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
    if( !SUCCEEDED( hr ) )
    {
        std::cout << "Unable to create speech instance\n";
        return -2;
        
    }

    
    int repCounter = 0;
    int waitingTime = 0;

    do{    
        for (int i = 0; i<(int)sentences.size(); i++){
            
            hr = pVoice->Speak(sentences[i].c_str(), 0, NULL);
            threadMutex.lock();
            label = i+1;
            threadMutex.unlock();
            if(!isRunning){
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            threadMutex.lock();
            label = 0;  
            threadMutex.unlock(); 
            waitingTime = std::rand() % 2000 + 2000;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
        }
         if(!isRunning){
            break;
        }
        repCounter++;
        std::cout << "trial: " << repCounter << std::endl;
    }while(repCounter < nbRepetitions);

    std::cout << "repetitions = " << repCounter << std::endl;

    hr = pVoice->Speak(L"End of the recording", 0, NULL);

    pVoice->Release();
    pVoice = NULL;

    ::CoUninitialize();

    isRunning = false;

    std::cout << "end program\n";

    return 0;

}


int write2csv(std::ofstream& csvFile, int nbLabels, int label, double timeStamp, double timeElapsed, std::vector< std::vector<double> > emgData){

    double startingTime = timeStamp - timeElapsed;
    double timeStep = timeElapsed / (double)emgData[0].size();
    // std::cout << "size data: " << emgData.size();

    for(int j=0; j< (int)emgData[0].size(); j++){

        csvFile << startingTime + ( (j+1) * timeStep) << ";";
        
        for (int i=0; i< (int)emgData.size() -1 ; i++){
            
            csvFile << emgData[i][j] << ";";
        }
        
        std::vector<double> toCSV(nbLabels, 0);
        if(label != 0){
            toCSV[label-1] = 1;
        }
        

        for(int i=0; i< nbLabels ; i++){
            csvFile << toCSV[i] << "; ";
        }
        csvFile << "\n";

    }


    return 0;
}