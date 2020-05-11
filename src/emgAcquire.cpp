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

#include "emgAcquire.h"

#ifdef _WIN32
        std::wstring utf8ToUtf16(const std::string& utf8Str){
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(utf8Str);
    }

    std::string utf16ToUtf8(const std::wstring& utf16Str){
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(utf16Str);
    }

    int emgAcquire::getComputerName(std::string *pc_name) {
        wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1] = {0};
        DWORD cchBufferSize = sizeof(buffer) / sizeof(buffer[0]);
        if (!GetComputerNameW(buffer, &cchBufferSize)){
            return -1;
        }
            
        const std::wstring compName = std::wstring(&buffer[0]);
        *pc_name = utf16ToUtf8(compName);
        return 0;
    }
#endif


std::vector< std::vector<double> > resampe_data(std::vector< std::vector<double> > data, float desired_freq, float original_freq, unsigned int nb_of_forget_points){

    std::vector< std::vector<double> > resampled_Mat;


    return resampled_Mat;
}


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


int emgAcquire::Client::resampling(bool doResamle){

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
    unsigned int nb_r_channells = (nb_channels_required < nb_channels_received) ? nb_channels_required : nb_channels_received;

    if(giveDigitalSignal){
        nb_r_channells ++;
    }

    unsigned int s2return = nb_samples_to_return;

    if (hasRemainder && giveRemainder){
        s2return += 2*remainder;
    }

    std::vector< std::vector<double> > returnedMatrix(nb_r_channells, std::vector<double>(s2return) );

    while(!is_buffer_ok || updateIsRunning){
        cv.wait(lk);
    }


    // threadMutex.lock();
    for (int i = 0; i < ((giveDigitalSignal) ? nb_r_channells-1 : nb_r_channells); i++ ){

        // copy from buffer
        std::copy_n(buffer[i].begin(), s2return, returnedMatrix[i].begin());

        // remove these elements from the buffer
        buffer[i] = std::vector<double> (buffer[i].begin() + s2return, buffer[i].end());
        buffer[i].resize(bufferSize, 0);
        bufferIndexes[i] = bufferIndexes[i] - s2return;
        if (bufferIndexes[i] < nb_samples_to_return){
            is_buffer_ok = is_buffer_ok && false;
        }
    }
    if(giveDigitalSignal){
        // if the digital signal is required, inlude that into the returned matrix

        // copy from buffer
        std::copy_n(buffer.back().begin(), s2return, returnedMatrix.back().begin());
    }

    // remove the same number of ellements from the rest of the channels
    for (int i = nb_r_channells; i<(int)buffer.size(); i++){
        buffer[i] = std::vector<double> (buffer[i].begin() + s2return, buffer[i].end());
        buffer[i].resize(bufferSize, 0);
    }
    // threadMutex.unlock();

    if (hasRemainder) {
        if (giveRemainder){
            giveRemainder = false;
        }else{
            giveRemainder = true;
        }
    }

    // wait until the cycle time has passed
    auto cTime = std::chrono::high_resolution_clock::now();

    double timeElapsed = std::chrono::duration<double, std::milli>(cTime - give_msg_time).count();
    if (timeElapsed < cycle_time_ms){
        while(timeElapsed < cycle_time_ms){
            cTime = std::chrono::high_resolution_clock::now();
            timeElapsed = std::chrono::duration<double, std::milli>(cTime - give_msg_time).count();
        }
    }

    give_msg_time = std::chrono::high_resolution_clock::now();


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
     *     -3: 
     *     -4: 
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

    std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(30));

    std::cout << "[emgAcquireClient] Device on server: " << svr_acq_device << std::endl;
    std::cout << "[emgAcquireClient] Type of signals: " << signals_type << std::endl;
    std::cout << "[emgAcquireClient] Device acquisition frequency: " << svr_acq_frequency << std::endl;
    std::cout << "[emgAcquireClient] Device number of channels" << nb_channels_received << std::endl;


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

    giveRemainder = false;

    if(!resample){
        nb_samples_to_return = cycle_time_ms * (svr_acq_frequency / 1000.0);
        float tmp = cycle_time_ms * (svr_acq_frequency/1000.0);
         if (nb_samples_to_return == tmp){
            hasRemainder = false;
            remainder = 0;
        }else{
            hasRemainder = true;
            remainder = 1;
        }
    }else{
        nb_samples_to_return = cycle_time_ms;
        if (cycle_time_ms == nb_samples_to_return){
            hasRemainder = false;
            remainder = 0;
        }else{
            hasRemainder = true;
            remainder = 1;
        }
    }

    buffer = std::vector< std::vector<double> > (nb_channels_required, std::vector<double>(bufferSize, 0));

    small_buffer = std::vector< std::vector<double> > (nb_channels_required, std::vector<double>(emgAcquire::SMALL_BUFFER_SIZE, 0));

    bufferIndexes = std::vector<int>(nb_channels_required, 0);

    initialize_ok = true;
    std::cout << "[emgAcquireClient] Ready to start acquiring" << std::endl;

    return 0;
}

int emgAcquire::Client::listening_to_server(){

    bool isNew = false;
    std::string msg;
    std::vector< std::vector<double> > dataMat;

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    double timeEllapsed;
    std::vector<double> timings;

    while(isConnected){
        if(comHdlr.socketStream_ok()){
            msg = comHdlr.get_latest(&isNew);
            if(isNew){
                // if the message is new:
                // parse the json string in a json document
                json_msg.updateDoc(msg);

                if(!isFirstMsgReceived){
                    svr_acq_device = json_msg.getField<rapidJson_types::String>("device_name");
                    svr_acq_frequency = json_msg.getField<rapidJson_types::Float>("device_freq");
                    nb_channels_received = json_msg.getField<rapidJson_types::Int>("nb_channels");
                    isFirstMsgReceived = true;
                }

                // get the contents of the field "data"
                dataMat = json_msg.getField<rapidJson_types::Mat2DD>(std::string("data"));

                if(isRunning){
                    start = std::chrono::high_resolution_clock::now();

                    // if acquisition has started from the client, undate the buffer                    
                    
                    if(resample){
                        // if resample is true, resample the signals to 1000 Hz
                        
                        // concatenate the small buffer with the new data
                        std::vector< std::vector<double> > tmp_vec (nb_channels_required, std::vector<double>());

                        for (int i=0; i<nb_channels_required; i++){
                            tmp_vec[i].reserve(dataMat[i].size() + small_buffer[i].size());
                    
                            tmp_vec[i].insert(tmp_vec[i].end(), small_buffer[i].begin(), small_buffer[i].end());
                            tmp_vec[i].insert(tmp_vec[i].end(), dataMat[i].begin(), dataMat[i].end());
                        } 

                        // resample the data
                        tmp_vec = resampe_data(tmp_vec, 1000.0, svr_acq_frequency, emgAcquire::SMALL_BUFFER_SIZE);
                        
                        // update buffer
                        updateBuffer(dataMat);

                    }else{

                        // update buffer
                        updateBuffer(dataMat);
                    }

                    end = std::chrono::high_resolution_clock::now();
                    timeEllapsed = std::chrono::duration<double, std::micro>(end-start).count();
                    timings.push_back(timeEllapsed);

                    // std::cout << "size of the matrix: " << dataMat.size() << ", " << dataMat[0].size() << std::endl;
                }

                // get the latest raw samples as a small buffer
                if(initialize_ok){
                    for(int i=0; i< nb_channels_required; i++){
                        std::copy_n(dataMat[i].begin(), emgAcquire::SMALL_BUFFER_SIZE, small_buffer[i].begin());
                    }
                }

            }          
        }
    }

    double sumUpdate = 0;
    
    // find the average computational time and print it in the terminal
    for(auto& n: timings){
        sumUpdate +=n;
    }
    double aveUpdate= sumUpdate/(double)timings.size();

    std::cout << "averave updating time: " << aveUpdate << " microseconds" << std::endl;

    std::cout << "[emgAcquireClient] Stopped listening to server" << std::endl;

    return 0;
}

int emgAcquire::Client::start(){
    threadMutex.lock();
    isRunning = true;
    threadMutex.unlock();
    std::cout << "[emgAcquireClient] Acquisition started" << std::endl;
    return 0;
}

int emgAcquire::Client::updateBuffer(std::vector< std::vector<double> > mdata){

    std::lock_guard<std::mutex> lk(threadMutex);

    updateIsRunning = true;

    bool is_buffer_full = false;

    bool tmp_is_buffer_ok = true;
    
    std::vector< std::vector<double> > tmp_vec (nb_channels_required, std::vector<double>());

    for (int i=0; i<nb_channels_required; i++){
        int nb_new_samples = (int) mdata[i].size();

        tmp_vec[i].reserve(bufferSize);
                    
        tmp_vec[i].insert(tmp_vec[i].end(), mdata[i].begin(), mdata[i].end());
        tmp_vec[i].insert(tmp_vec[i].end(), buffer[i].begin(), buffer[i].end() - nb_new_samples);

        if (bufferIndexes[i] + nb_new_samples > bufferSize){
            is_buffer_full = is_buffer_full || true;
            bufferIndexes[i] = bufferSize;
        }else{
            bufferIndexes[i] += nb_new_samples;
        }

        if (bufferIndexes[i] < nb_samples_to_return){
            tmp_is_buffer_ok = tmp_is_buffer_ok && false;
        }
        
    }

    is_buffer_ok = tmp_is_buffer_ok;

    if (is_buffer_full){
        std::cout << "[emgAcquireClient] Buffer is full. Possible data loss." << std::endl;
    }

    updateIsRunning = false;

    cv.notify_all();
    return 0;
}

int emgAcquire::Client::shutdown(){

    // close communication with the server
    comHdlr.closeCommunication();
    return 0;
}

emgAcquire::Client::~Client(){


}

