/** 
 *  Copyright (c) 2020 Iason Batzianoulis
 *  
 *  emgAcquire source file
 *  
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    MIT
 * 
**/

#include "emgAcquire_dllexport.h"
// #include "emgAcquire.h"

#ifdef _WIN32

    int emgAcquire::getComputerName(std::string *pc_name) {
        /**
         * 
         * a function to get the computer name (windows version)
         * 
         */


        char buffer[MAX_COMPUTERNAME_LENGTH + 1] = {0};
        DWORD cchBufferSize = sizeof(buffer) / sizeof(buffer[0]);
        if (!GetComputerNameA(buffer, &cchBufferSize)){
            return -1;
        }

        *pc_name = std::string(buffer);
            
        
        return 0;
    }
#else

    int emgAcquire::dirExists(std::string tpath){
        /*	This function checks if the directory 'tpath' exists
            if it exists the function returns 0
        */

        struct stat info;

        if (stat(tpath.c_str(), &info) != 0){
            // could not have access in the folder
            return 0;
        }else if (info.st_mode & S_IFDIR){
            // the folder exists
            return 1;
        }
        else{
            // the folder doesn't exist
            return 1;
        }
    }

    int emgAcquire::getComputerName(std::string *pc_name) {
        /**
         * 
         * a function to get the computer name (linux version)
         * 
         */

        char hostname[HOST_NAME_MAX];
        gethostname(hostname, HOST_NAME_MAX);

        *pc_name = std::string(hostname);
        return 0;
    }

#endif





emgAcquire::Client::Client(unsigned int nb_ch){

    // comunication related variables
    hostIP = std::string(emgAcquire::DEFAULT_SVR_IP);
    svrPort = emgAcquire::DEFAULT_SVR_PORT;
    std::string pc_name;
    if (emgAcquire::getComputerName(&pc_name)<0){
        std::cout << "[emgAcquireClient] Unable to get computer's name. Continue with a standard string" << std::endl;
        pc_name = "unknownPC";
    }
    clName = "emgAcquireClient_on_" + pc_name;

    isFirstMsgReceived = false;

    isConnected = false;

    // functionality varaibles
    frequency = emgAcquire::DEFAULT_FREQUENCY;

    nb_channels_required = nb_ch;

    hasRemainder = false;
    giveRemainder = false;

    bufferSize = emgAcquire::DEFAULT_BUFFER_SIZE;

    buffer = std::vector< std::vector<double> > (nb_channels_required, std::vector<double>(bufferSize, 0));

    bufferIndexes = std::vector<int>(nb_channels_required,-1);

    is_buffer_ok = false;

    initialize_ok = false;

    updateIsRunning = false;

    resample = true;

    giveDigitalSignal = false;

    isRunning = false;

    keeplog = false;

    logFileName = "emgAcquireClient_logs";
    
    logFolderName = "logs";

}


emgAcquire::Client::Client(float freq){
    
    // comunication related variables
    hostIP = std::string(emgAcquire::DEFAULT_SVR_IP);
    svrPort = emgAcquire::DEFAULT_SVR_PORT;
    std::string pc_name;
    if (emgAcquire::getComputerName(&pc_name)<0){
        std::cout << "[emgAcquireClient] Unable to get computer's name. Continue with a standard string" << std::endl;
        pc_name = "unknownPC";
    }
    clName = "emgAcquireClient_on_" + pc_name;

    isFirstMsgReceived = false;

    isConnected = false;

    // functionality varaibles
    frequency = freq;

    cycle_time_ms = 1000 / frequency;
    nb_samples_to_return = 0;
    remainder = 0;

    hasRemainder = false;
    giveRemainder = false;

    nb_channels_required = emgAcquire::DEFAULT_NB_CHANNELS;

    bufferSize = emgAcquire::DEFAULT_BUFFER_SIZE;

    buffer = std::vector< std::vector<double> > (nb_channels_required, std::vector<double>(bufferSize, 0));

    bufferIndexes = std::vector<int>(nb_channels_required,-1);

    is_buffer_ok = false;

    initialize_ok = false;

    updateIsRunning = false;

    resample = true;

    giveDigitalSignal = false;

    isRunning = false;

    keeplog = false;

    logFileName = "emgAcquireClient_logs";

    logFolderName = "logs";
    
}


emgAcquire::Client::Client(float freq, unsigned int nb_ch){

    // comunication related variables
    hostIP = std::string(emgAcquire::DEFAULT_SVR_IP);
    svrPort = emgAcquire::DEFAULT_SVR_PORT;
    std::string pc_name;
    if (emgAcquire::getComputerName(&pc_name)<0){
        std::cout << "[emgAcquireClient] Unable to get computer's name. Continue with a standard string" << std::endl;
        pc_name = "unknownPC";
    }
    clName = "emgAcquireClient_on_" + pc_name;

    isFirstMsgReceived = false;

    isConnected = false;

    // functionality varaibles
    frequency = freq;

    cycle_time_ms = 1000 / frequency;
    nb_samples_to_return = 0;
    remainder = 0;

    hasRemainder = false;
    giveRemainder = false;

    nb_channels_required = nb_ch;

    bufferSize = emgAcquire::DEFAULT_BUFFER_SIZE;

    buffer = std::vector< std::vector<double> > (nb_channels_required, std::vector<double>(bufferSize, 0));

    bufferIndexes = std::vector<int>(nb_channels_required,-1);

    is_buffer_ok = false;

    initialize_ok = false;

    updateIsRunning = false;

    resample = true;

    giveDigitalSignal = false;

    isRunning = false;

    keeplog = false;

    logFileName = "emgAcquireClient_logs";

    logFolderName = "logs";

}


emgAcquire::Client::Client(std::string srvIP){
    
    // comunication related variables
    hostIP = srvIP;
    svrPort = emgAcquire::DEFAULT_SVR_PORT;
    std::string pc_name;
    if (emgAcquire::getComputerName(&pc_name)<0){
        std::cout << "[emgAcquireClient] Unable to get computer's name. Continue with a standard string" << std::endl;
        pc_name = "unknownPC";
    }
    clName = "emgAcquireClient_on_" + pc_name;

    isFirstMsgReceived = false;

    isConnected = false;

    // functionality varaibles
    frequency = emgAcquire::DEFAULT_FREQUENCY;

    cycle_time_ms = 1000 / frequency;
    nb_samples_to_return = 0;
    remainder = 0;

    hasRemainder = false;
    giveRemainder = false;

    nb_channels_required = emgAcquire::DEFAULT_NB_CHANNELS;

    bufferSize = emgAcquire::DEFAULT_BUFFER_SIZE;

    buffer = std::vector< std::vector<double> > (nb_channels_required, std::vector<double>(bufferSize, 0));

    bufferIndexes = std::vector<int>(nb_channels_required,-1);

    is_buffer_ok = false;

    initialize_ok = false;

    updateIsRunning = false;

    resample = true;

    giveDigitalSignal = false;

    isRunning = false;

    keeplog = false;

    logFileName = "emgAcquireClient_logs";

    logFolderName = "logs";

}

emgAcquire::Client::Client(std::string srvIP, unsigned int port, float freq, unsigned int nb_ch){
    
    // comunication related variables
    hostIP = srvIP;
    svrPort = port;
    std::string pc_name;
    if (emgAcquire::getComputerName(&pc_name)<0){
        std::cout << "[emgAcquireClient] Unable to get computer's name. Continue with a standard string" << std::endl;
        pc_name = "unknownPC";
    }
    clName = "emgAcquireClient_on_" + pc_name;

    isFirstMsgReceived = false;

    isConnected = false;

    // functionality varaibles
    frequency = freq;

    cycle_time_ms = 1000 / frequency;
    nb_samples_to_return = 0;
    remainder = 0;

    hasRemainder = false;
    giveRemainder = false;

    nb_channels_required = nb_ch;

    bufferSize = emgAcquire::DEFAULT_BUFFER_SIZE;

    buffer = std::vector< std::vector<double> > (nb_channels_required, std::vector<double>(bufferSize, 0));

    bufferIndexes = std::vector<int>(nb_channels_required,-1);

    is_buffer_ok = false;

    initialize_ok = false;

    updateIsRunning = false;

    resample = true;

    giveDigitalSignal = false;

    isRunning = false;

    keeplog = false;

    logFileName = "emgAcquireClient_logs";

    logFolderName = "logs";

}


int emgAcquire::Client::setNBofChannels(unsigned int nb_ch){

    /**
     *  setting a new number of channels for returning to the client. 
     *  
     *  nb_ch:  the desired number of channels to returned
     *
     *  Returns:
     *      0: if the new number of channels has been set correctly
     *     -1: otherwise
     */
    

    if (nb_ch <= emgAcquire::MAXIMUM_NB_CHANNELS){
        nb_channels_required = nb_ch;
        return 0;
    }else{
        std::cout << "[emgAcquireClient] The number of channels cannot be greater than " << emgAcquire::MAXIMUM_NB_CHANNELS << std::endl;
        std::cout << "[emgAcquireClient] The number of channels has not been changed. The current number of channels size is: " << nb_channels_required << std::endl;
        return -1;
    }

    return 0;
}


int emgAcquire::Client::setFrequency(float freq){

    /**
     *  setting a new frequency for acquiring the signals. 
     *  
     *  freq:  the desired frequency in Hz
     *
     *  Returns:
     *      0: if the new frequency has been set correctly
     *     -1: otherwise
     */
    

    if (freq <= emgAcquire::MAXIMUM_FREQUENCY){
        frequency = freq;
        cycle_time_ms = 1000 / frequency; 
        return 0;
    }else{
        std::cout << "[emgAcquireClient] The acquisition frequency cannot be greater than " << emgAcquire::MAXIMUM_FREQUENCY << " Hz" << std::endl;
        std::cout << "[emgAcquireClient] The frequency has not been changed. The current frequency size is: " << frequency << std::endl;
        return -1;
    }
   
}


int emgAcquire::Client::setServerIP(std::string svrIP){

    /**
     *  setting a new IP address for connecting to the server. 
     *  
     *  svrIP:  the IP adress of the server
     *
     *  Returns:
     *      0: if the new IP address has been set correctly
     *     -1: otherwise
     */
    
    hostIP = svrIP;

    if (hostIP.compare(svrIP)==0){
        return 0;
    }else{
        return -1;
    }
}


int emgAcquire::Client::setServerIP(std::string svrIP, unsigned int port){

    /**
     *  setting a new IP address and port for connecting to the server. 
     *  
     *  svrIP:  the IP adress of the server
     *  port: the communication port
     *
     *  Returns:
     *      0: if the new IP address and port have been set correctly
     *     -1: otherwise
     */
    
    hostIP = svrIP;
    svrPort = port;

    if ((hostIP.compare(svrIP)==0) && (svrPort==port)){
        return 0;
    }else{
        return -1;
    }

}


int emgAcquire::Client::setBufferSize(unsigned int bfrSize){

    /**
     *  setting a new size of the buffer. It modifies how deep the buffer 
     *  of the signals would be. 
     *  
     *  bfrSize:  the number of samples of each channel to keep in the buffer
     *
     *  Returns:
     *      0: the size of the buffer has been correctly modified
     *     -1: the size of the buffer was not modified because the desired size is greater than the maximum allowed buffer-size
     *     -2: the size of the buffer was not modified due to an undefined reason
     *     -3: the size of the buffer was not modified because the desired size is less than the current buffer and the application is running (avoiding data loss)
     *     -4: the size of the buffer was not modified due to an undefined reason (while the application was running)
     */

    if (bfrSize<=emgAcquire::MAXIMUM_BUFFER_SIZE){
        
        if(!isRunning){

            buffer = std::vector< std::vector<double> > (nb_channels_required, std::vector<double>(bfrSize, 0));

            if ((unsigned int)buffer.size()==bfrSize){
                bufferSize = bfrSize;
                return 0;
            }else{
                return -2;
            }
        }else{
            if(bfrSize < bufferSize){
                std::cout << "[emgAcquireClient] The new buffer-size cannot be less than the current buffer-size while the application is running" << std::endl;
                std::cout << "[emgAcquireClient] The buffer has not been changed. The current buffer size is: " << bufferSize << std::endl;
                return -3;
            }else{
                threadMutex.lock();
                for (int i = 0; i < (int)buffer.size(); i++){
                    buffer[i].resize(bfrSize, 0);
                }
                threadMutex.unlock();
                bool test = true;
                for (int i = 0; i < (int)buffer.size(); i++){
                    if ((unsigned int)buffer[i].size()==bfrSize){
                        test =  test && true;
                    }else{
                        test =  test && false;
                    }
                }
                
                if (test){
                    bufferSize = bfrSize;
                    return 0;
                }else{
                    return -4;
                }
                
            }
        }
        

    }else{
        std::cout << "[emgAcquireClient] The new size cannot be greater than the maximum buffer-size(" << emgAcquire::MAXIMUM_BUFFER_SIZE << " samples)" << std::endl;
        std::cout << "[emgAcquireClient] The buffer has not been changed. The current buffer size is: " << bufferSize << std::endl;
        return -1;
    }
    
}


int emgAcquire::Client::setResampling(bool doResamle){

    /**
     *  changing the interpolation flag.
     *  
     *  interpol:  the desired interpolation flag.
     *              ture:   if signal interpolation is desired
     *              false:  if signal interpolation is not desired
     *
     *  Returns:
     *      0: always
     */
    
    resample = doResamle;

    return 0;
}


int emgAcquire::Client::setKeepLog(bool choice){

    /**
     *  changing the keeplog flag.
     *  
     *  choice:     the desired flag.
     *              ture:   keep logfile
     *              false:  don't keep logfile
     *
     *  Returns:
     *      0: always
     */
    
    keeplog = choice;

    return 0;
}


int emgAcquire::Client::setDigitalSignalReturn(bool digSingalReturn){

    /**
     *  setting the digital signal to return or not.
     *  
     *  digSignalRetrun:  the desired flag.
     *              ture:   to retrieve the digital signal
     *              false:  not to retrieve the digital signal
     *
     *  Returns:
     *      0: always
     */

    giveDigitalSignal = digSingalReturn;

    return 0;
}


std::string emgAcquire::Client::getDeviceName(){
    /**
     *   returning the acquisition device name
     */

     return svr_acq_device;
}


std::string emgAcquire::Client::getSignalsType(){
    /**
     *   returning the type of the signals
     */
    
    return signals_type;
}


float emgAcquire::Client::getSvrFrequency(){
    /**
     *   returning the real acquisition frequency of the device
     */
    
    return svr_acq_frequency;
}


unsigned int emgAcquire::Client::getNBofChannelsReceived(){
    /**
     *   returning the number of channels received from the server
     */
    
    return nb_channels_received;
}


std::vector< std::vector<double> > emgAcquire::Client::getSignals(){
    /**
     *   returning the signals (EMG and the digital signal if giveDigitalSignal is true)
     */


    std::unique_lock<std::mutex> lk(threadMutex);
    
    unsigned int nb_r_channells = nb_channels_required;
    if(giveDigitalSignal){
        nb_r_channells ++;
    }

    unsigned int s2return = nb_samples_to_return;

    if (hasRemainder && giveRemainder){
        s2return += 2*remainder;
    }

    std::vector< std::vector<double> > returnedMatrix(nb_r_channells, std::vector<double>(s2return) );

    if(!is_buffer_ok){
        while(!is_buffer_ok){ 
            cv.wait(lk);
        }
    }
    lk.unlock();
    

    bool tmp_buffer_ok = true;

    threadMutex.lock();
    for (unsigned int i = 0; i < nb_channels_required; i++ ){

        std::vector<double> tmp_vec(nb_samples_to_get);

        // copy from buffer
        std::copy_n(buffer[i].begin(), nb_samples_to_get, tmp_vec.begin());


        // resample the signals if needed
        if(resample){
            std::vector<double> to_resample;
            to_resample.reserve(nb_samples_to_get + emgAcquire::SMALL_BUFFER_SIZE);
            to_resample.insert(to_resample.end(), small_buffer[i].begin(), small_buffer[i].end());
            to_resample.insert(to_resample.end(), tmp_vec.begin(), tmp_vec.end());

            tmp_vec = acquireFilters::resample_data(to_resample, emgAcquire::DEFAULT_RESAMPLE_FREQUENCY, svr_acq_frequency, emgAcquire::SMALL_BUFFER_SIZE);
            tmp_vec.erase(tmp_vec.end()-1);
            std::copy_n(buffer[i].begin() + nb_samples_to_get - emgAcquire::SMALL_BUFFER_SIZE, emgAcquire::SMALL_BUFFER_SIZE, small_buffer[i].begin());
        }
        
        returnedMatrix[i] =  tmp_vec;
        
        // remove these elements from the buffer
        buffer[i] = std::vector<double> (buffer[i].begin() + nb_samples_to_get, buffer[i].end());
        buffer[i].resize(bufferSize, 0);
        
        // update the indexes and check if the new ones are less than the number of samples to get next time
        bufferIndexes[i] = bufferIndexes[i] - nb_samples_to_get;
        if (bufferIndexes[i] < (int)nb_samples_to_get){
            tmp_buffer_ok = tmp_buffer_ok && false;
        }
    }
    if(giveDigitalSignal){
        // if the digital signal is required, inlude that into the returned matrix

        // copy from buffer
        std::vector<double> tmp_vec(nb_samples_to_get);
        std::copy_n(buffer.back().begin(), nb_samples_to_get, tmp_vec.begin());

        if(resample){
            tmp_vec = acquireFilters::resample_data(tmp_vec, emgAcquire::DEFAULT_RESAMPLE_FREQUENCY, svr_acq_frequency, 0);
        }
        returnedMatrix.back() =  tmp_vec;
    }

    // update the last index, corresponds to the digital channel, and check if its value is ok
    bufferIndexes.back() = bufferIndexes.back() - nb_samples_to_get;
    if (bufferIndexes.back() < (int)nb_samples_to_get){
        tmp_buffer_ok = tmp_buffer_ok && false;
    }

    // remove the same number of ellements from the rest of the channels
    for (int i = nb_r_channells; i<(int)buffer.size(); i++){
        buffer[i] = std::vector<double> (buffer[i].begin() + nb_samples_to_get, buffer[i].end());
        buffer[i].resize(bufferSize, 0);
    }
    
    threadMutex.unlock();

    // update the flag
    is_buffer_ok = tmp_buffer_ok;

    if (hasRemainder) {
        if (giveRemainder){
            giveRemainder = false;
        }else{
            giveRemainder = true;
        }
    }

    // wait until the cycle time has passed
    // auto cTime = std::chrono::high_resolution_clock::now();

    // double timeElapsed = std::chrono::duration<double, std::milli>(cTime - give_msg_time).count();
    // if (timeElapsed < cycle_time_ms-4){
    //     while(timeElapsed < cycle_time_ms-4){
    //         cTime = std::chrono::high_resolution_clock::now();
    //         timeElapsed = std::chrono::duration<double, std::milli>(cTime - give_msg_time).count();
    //     }
    // }

    // get the time the client retrieved the signals
    give_msg_time = std::chrono::high_resolution_clock::now();

    // if logfiles are kept, throw this info to the file
    if(keeplog){
        wfile << std::chrono::duration<float, std::micro>(give_msg_time - startingTime).count()/1000.0 << " | data sent to client" << std::endl; 
    }


    return returnedMatrix;
}


int emgAcquire::Client::initialize(){

    /**
     *  initialize the socketStream and attempt a connection to the server 
     *  
     *
     *  Returns:
     *      0: successfully initialized the socket stream and connected to the server
     *     -1: unable to initialize socketStream
     *     -2: unable to connect to the server
     *      
     *      
     */

    initialize_ok = false;

    // initialize the socket
    if(comHdlr.initialize_socketStream(hostIP.c_str(), svrPort)<0){
        std::cerr << "[emgAcquireClient] Unable to initialize socketStream" << std::endl;
        return -1;
    }
    
    std::cout << "[emgAcquireClient] socketStream initialized successfully" << std::endl;

    comHdlr.set_clientName(clName);

    // attemp a connection with the server
    if(comHdlr.make_connection()<0){
        std::cerr << "[emgAcquireClient] Unable to connect to " << hostIP << " in the port " << svrPort << std::endl;
        return -2;
    }

    isConnected = true;

    std::cout << "[emgAcquireClient] Successful connection to " << hostIP << " in the port " << svrPort << std::endl;

    // starting listener
    listenerThread = std::thread(&emgAcquire::Client::listening_to_server, this);

    std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(100));

    std::cout << "[emgAcquireClient] Device on server: " << svr_acq_device << std::endl;
    std::cout << "[emgAcquireClient] Type of signals: " << signals_type << std::endl;
    std::cout << "[emgAcquireClient] Device acquisition frequency: " << svr_acq_frequency << std::endl;
    std::cout << "[emgAcquireClient] Device number of channels: " << nb_channels_received << std::endl;


    // check if the number of signals received is enough
    if (nb_channels_received < nb_channels_required){
        nb_channels_required = nb_channels_received;
        buffer = std::vector< std::vector<double> > (nb_channels_required + 1, std::vector<double>(bufferSize, 0));
        bufferIndexes = std::vector<int>(nb_channels_required + 1,-1);
        threadMutex.lock();
        std::cout << "[emgAcquireClient] Warning!!!!! The number of required signals is greater than the received signals" << std::endl;
        std::cout << "[emgAcquireClient] Continuing with the number of received signals" << std::endl;
        std::cout << "[emgAcquireClient] The number of signals is " << nb_channels_required << std::endl;
        threadMutex.unlock();
    }

    cycle_time_ms = 1000.0 / frequency;

    nb_samples_to_return = cycle_time_ms * frequency / 1000.0;

    nb_samples_to_get = cycle_time_ms * svr_acq_frequency / 1000.0;

    if(!resample){
        nb_samples_to_return = nb_samples_to_get;
    }

    giveRemainder = false;

    buffer = std::vector< std::vector<double> > (nb_channels_required +1, std::vector<double>(bufferSize, 0));

    small_buffer = std::vector< std::vector<double> > (nb_channels_required +1, std::vector<double>(emgAcquire::SMALL_BUFFER_SIZE, 0));

    bufferIndexes = std::vector<int>(nb_channels_required + 1, 0);

    if(keeplog){
        if (openLogFile() < 0){
            std::cout << "[emgAcquireClient] No logs will be kept" << std::endl;
            keeplog = false;
        }else{
            std::time_t rawtime;
            char time_buffer[80];
            struct tm *timeinfo;
            std::time(&rawtime);
            timeinfo = localtime(&rawtime);

            std::strftime(time_buffer, sizeof(time_buffer), "%Y%m%d_%H_%M_%S", timeinfo);
            std::string covString(time_buffer);
            startingTime = std::chrono::high_resolution_clock::now();
            wfile << covString << " | Program started" << std::endl;
            csvfile << "time;buffer 1;";
            if (nb_channels_required>9){
                csvfile << "buffer 2;";
            }  
            csvfile << "buffer digital" << std::endl;
            
        }
    }

    

    initialize_ok = true;
    std::cout << "[emgAcquireClient] Ready to start acquiring" << std::endl;

    return 0;
}

int emgAcquire::Client::listening_to_server(){

    /**
     *  receiving the signals from the server in a continuous manner and updates the buffer 
     *  
     *  
     *  Returns:
     *      0: if terminated properly
     *     -1: otherwise
     */

    bool isNew = false;
    std::string msg;
    std::vector< std::vector<double> > dataMat;

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    double timeEllapsed;
    std::vector<double> timings;

    // run continuously until the connection is lost
    while(isConnected){

        if(comHdlr.socketStream_ok()){
            
            // if the socketStream client is ok, retreive the message
            
            msg = comHdlr.get_latest(&isNew);
            
            if(isNew){
                // if the message is new:
                // parse the json string in a json document
                
                jsonWrapper testObj(msg);
                

                if(!isFirstMsgReceived){
                    
                    svr_acq_device = testObj.getField<rapidJson_types::String>("device_name");
                    
                    svr_acq_frequency = testObj.getField<rapidJson_types::Float>("device_freq");

                    signals_type = testObj.getField<rapidJson_types::String>("type");
                    
                    std::vector<int> nb_ch_vec = testObj.getField<rapidJson_types::VecInt>("nb_channels");
                    
                    nb_channels_received = nb_ch_vec[0] + nb_ch_vec[1];
                    
                    isFirstMsgReceived = true;
                }

                // get the contents of the field "data"
                dataMat = testObj.getField<rapidJson_types::Mat2DD>(std::string("data"));

                if(isRunning){
                    // if the acquisition of the signals from the client has started, update the buffer

                    start = std::chrono::high_resolution_clock::now();

                    // if logs are kept, throw the time stamp and the number of signals received in the logfile
                    if(keeplog){

                        wfile << std::chrono::duration<float, std::micro>(start - startingTime).count()/1000.0 << " | samples received: " << dataMat[0].size();
                        if (nb_channels_required>9){
                            wfile << ", " << dataMat[8].size();
                        }
                        wfile << ", " << dataMat.back().size() << std::endl;
                    }

                    // update the buffer
                    updateBuffer(dataMat);

                    // keep diagnostics on the loop time
                    end = std::chrono::high_resolution_clock::now();
                    timeEllapsed = std::chrono::duration<double, std::micro>(end-start).count();
                    timings.push_back(timeEllapsed);

                    // std::cout << "size of the matrix: " << dataMat.size() << ", " << dataMat[0].size() << std::endl;
                }

                

            }       
        }else{
            std::cout << "[emgAcquireClient] Server is shut-down" << std::endl;
            isConnected = false;
        }
    }

    if (timings.size()>0){

        #ifdef _WIN32
            SetConsoleCP(437);
            SetConsoleOutputCP(437);
        #endif
        
        std::cout << "averave updating time: " << acquireFilters::average(timings) << " "; 
       
        #ifdef _WIN32
            std::cout << (char)241 << " ";
        #else
            std::cout << "\u00b1 ";
        #endif
        
        std::cout << acquireFilters::standDev(timings) << " microseconds" << std::endl;
    }

    std::cout << "[emgAcquireClient] Stopped listening to server" << std::endl;

    return 0;
}

int emgAcquire::Client::start(){
    /**
     *  setting the running flag to true, so that the acquisition of the signals can start
     *  
     *  Returns:
     *      0: always
     *     
     */
    threadMutex.lock();
    if(keeplog){
        auto ct_time = std::chrono::high_resolution_clock::now();
        wfile << std::chrono::duration<float, std::micro>(ct_time - startingTime).count()/1000.0 << " | acquisition started" << std::endl;
    }
    isRunning = true;
    threadMutex.unlock();
    std::cout << "[emgAcquireClient] Acquisition started" << std::endl;
    return 0;
}


int emgAcquire::Client::stop(){
    /**
     *  setting the running flag to false, stopping the updte of the buffer
     *  
     *  Returns:
     *      0: always
     *     
     */

    threadMutex.lock();
    if(keeplog){
        auto ct_time = std::chrono::high_resolution_clock::now();
        wfile << std::chrono::duration<float, std::micro>(ct_time - startingTime).count()/1000.0 << " | acquisition sropped" << std::endl;
    }
    isRunning = false;
    threadMutex.unlock();
    std::cout << "[emgAcquireClient] Acquisition stopped" << std::endl;
    return 0;
}


int emgAcquire::Client::updateBuffer(std::vector< std::vector<double> > mdata){
    /**
     *  This function updates the buffer to the end and the corresponding indexes
     *  If the buffer is full, removes the oldest samples.
     *  
     *  Returns:
     *      0: always
     *     
     */
    
    

    updateIsRunning = true;

    bool is_buffer_full = false;

    bool tmp_is_buffer_ok = true;
    
    
    std::lock_guard<std::mutex> lk(threadMutex);
    
    // put the channels in the buffer, one by one, only for the channels required from the user
    for (unsigned int i=0; i<nb_channels_required; i++){
        
        // get the received number of samples for this channel
        int nb_new_samples = (int)mdata[i].size();
        
        // check if the indexes are greater than the number the client requires and update coresponding indexes and flad
        if (bufferIndexes[i] + nb_new_samples > (int)bufferSize){
            buffer[i].erase(buffer[i].begin(), buffer[i].begin() + nb_new_samples);
            std::copy(mdata[i].begin(), mdata[i].end(), buffer[i].end() - nb_new_samples);

            is_buffer_full = is_buffer_full || true;
            bufferIndexes[i] = bufferSize;
        }else{
            std::copy(mdata[i].begin(), mdata[i].end(), buffer[i].begin() + bufferIndexes[i]);
            
            bufferIndexes[i] += nb_new_samples - 1;
        }

        if (bufferIndexes[i] < (int)nb_samples_to_get){
            tmp_is_buffer_ok = tmp_is_buffer_ok && false;
        }   
        
        
    }

    // adding the digital channel
    int nb_new_samples = (int)mdata.back().size();

    if (bufferIndexes.back() + nb_new_samples > (int)bufferSize){

        buffer.back().erase(buffer.back().begin(), buffer.back().begin() + nb_new_samples);
        std::copy(mdata.back().begin(), mdata.back().end(), buffer.back().end() - nb_new_samples);

        is_buffer_full = is_buffer_full || true;
        bufferIndexes.back() = bufferSize;
    }else{
        std::copy(mdata.back().begin(), mdata.back().end(), buffer.back().begin() + bufferIndexes.back());
        bufferIndexes.back() += nb_new_samples - 1;
    }
        
    if (bufferIndexes.back() < (int)nb_samples_to_get){
        tmp_is_buffer_ok = tmp_is_buffer_ok && false;
    }

    // update the buffer flag
    is_buffer_ok = tmp_is_buffer_ok;

    // if logs are kept, throw the timestamp and the buffer indexes
    if(keeplog){
        auto ct_time = std::chrono::high_resolution_clock::now();
        wfile << std::chrono::duration<float, std::micro>(ct_time - startingTime).count()/1000.0 << " | buffer_ok " << is_buffer_ok << " | index: " << bufferIndexes[0];
        if (nb_channels_required>9){
            wfile << ", " << bufferIndexes[8];
        }
        wfile << std::endl;

        csvfile << std::chrono::duration<float, std::micro>(ct_time - startingTime).count()/1000.0 << ";" << bufferIndexes[0] << ";";
        if (nb_channels_required>9){
            csvfile << bufferIndexes[8] << ";";
        }
        csvfile << bufferIndexes.back() << "\n";
    }

    // if the buffer is full, throw a warning in the console
    if (is_buffer_full){
        std::cout << "[emgAcquireClient] WARNING!! Buffer is full! Possible data loss." << std::endl;
    }

    updateIsRunning = false;
    
    cv.notify_all();
    
    return 0;
}

int emgAcquire::Client::shutdown(){

    /**
     *  The function shuts-down the communication witht the server,
     *  stops the threads running in the background, closes the logfiles
     *  and clears the buffer
     *  
     *  
     *  Returns:
     *      0: always
     *     
     */

    std::cout << "[emgAcquireClient] Shutting-down client..." << std::endl;
    threadMutex.lock();
    
    isRunning = false;
    isConnected = false;
    is_buffer_ok = false;
    isFirstMsgReceived = false;
    threadMutex.unlock();
    listenerThread.join();
    
    std::cout << "[emgAcquireClient] Closing communication with the server..." << std::endl;
    // close communication with the server
    comHdlr.closeCommunication();

    std::cout << "[emgAcquireClient] Clearing the buffer..." << std::endl;
    for(int i=0; i<(int)buffer.size(); i++){
        buffer[i].clear();
        small_buffer[i].clear();
    }
    buffer.clear();
    bufferIndexes.clear();
    small_buffer.clear();
    std::cout << "[emgAcquireClient] Shut-down complete" << std::endl;

    if(keeplog){
        auto ct_time = std::chrono::high_resolution_clock::now();
        wfile << std::chrono::duration<float, std::micro>(ct_time - startingTime).count()/1000.0 << " | Program ended" << std::endl;
        closeLogfile();
        keeplog = false;
    }

    return 0;
}


int emgAcquire::Client::openLogFile(){
    
    /**
     *  The function creates a folder to store the logfiles (if need)
     *  and opens the logfiles
     *  
     *  
     *  Returns:
     *      0: always
     *     
     */
    
    #ifdef _WIN32
        if(CreateDirectory(logFolderName.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()){
            std::cout << "[emgAcquireClient] A folder "<< logFolderName << " is created" << std::endl;
        }
    #else
        std::string curent_path=get_current_dir_name();
        std::string dir_path = curent_path + '/' + logFolderName;
        if (!dirExists(dir_path)){
            if(mkdir(dir_path.c_str(),0777)==0){
                std::cout << "[emgAcquireClient] A folder "<< logFolderName << " is created" << std::endl;
            }else{
                std::cout<<"[emgAcquireClient] Unable to create the folder:\n"<<dir_path<<"\n";
            }

            dir_path.clear();
    }

    #endif

    
    #ifdef _WIN32
        std::string t_logfileName = logFolderName + "\\" + logFileName + ".txt";
        std::string c_logfileName = logFolderName + "\\" + logFileName + ".csv";
    #else
        std::string t_logfileName = logFolderName + '/' + logFileName + ".txt";
        std::string c_logfileName = logFolderName + '/' + logFileName + ".csv";
    #endif

    wfile.open(t_logfileName);
    if(!wfile.is_open()){
        std::cerr << "[emgAcquireClient] Unable to open logfile" <<std::endl;
        return -1;
    }

    csvfile.open(c_logfileName);
    if(!csvfile.is_open()){
        std::cerr << "[emgAcquireClient] Unable to open csv logfile" <<std::endl;
        wfile.close();
        return -2;
    }

    return 0;
}


int emgAcquire::Client::closeLogfile(){

    /**
     *  The function closes the logfiles
     *
     *  Returns:
     *      0: always
     *     
     */

    // close logfiles
    wfile.close();

    csvfile.close();

    return 0;
}

emgAcquire::Client::~Client(){

    /**
     *  Class destructor
     * 
     *  Checks if the application is still running or
     *  if a connection with the server is still on
     *  and if any of the above is true, repeats all the
     *  steps of the shutdown process
     *     
     */

    if(isRunning || isConnected){
        std::cout << "[emgAcquireClient] Shutting-down client..." << std::endl;
        threadMutex.lock();
        isRunning = false;
        isRunning = false;
        isConnected = false;
        is_buffer_ok = false;
        isFirstMsgReceived = false;
        threadMutex.lock();
        if(listenerThread.joinable()){
            listenerThread.join();
        }
        std::cout << "[emgAcquireClient] Closing communication with the server..." << std::endl;
        // close communication with the server
        comHdlr.closeCommunication();
    }    
    
    
    if (buffer.size() > 0){
        std::cout << "[emgAcquireClient] Clearing the buffer..." << std::endl;
        for(int i=0; i<(int)buffer.size(); i++){
            buffer[i].clear();
            small_buffer[i].clear();
        }
        buffer.clear();
        bufferIndexes.clear();
        small_buffer.clear();
        std::cout << "[emgAcquireClient] Shut-down complete" << std::endl;

    }

    if(keeplog){
        closeLogfile();
    }
    
}

