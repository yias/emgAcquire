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

    isNewMsgReceived = false;

    // functionality varaibles
    frequency = emgAcquire::DEFAULT_FREQUENCY;

    nb_channels_received = nb_ch;

    bufferSize = emgAcquire::DEFAULT_BUFFER_SIZE;

    buffer = std::vector< std::vector<double> > (bufferSize, std::vector<double>());

    interpolate = false;

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

    isNewMsgReceived = false;

    // functionality varaibles
    frequency = freq;

    nb_channels_received = emgAcquire::DEFAULT_NB_CHANNELS;

    bufferSize = emgAcquire::DEFAULT_BUFFER_SIZE;

    buffer = std::vector< std::vector<double> > (bufferSize, std::vector<double>());

    interpolate = false;

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

    isNewMsgReceived = false;

    // functionality varaibles
    frequency = freq;

    nb_channels_received = nb_ch;

    bufferSize = emgAcquire::DEFAULT_BUFFER_SIZE;

    buffer = std::vector< std::vector<double> > (bufferSize, std::vector<double>());

    interpolate = false;

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

    isNewMsgReceived = false;

    // functionality varaibles
    frequency = emgAcquire::DEFAULT_FREQUENCY;

    nb_channels_received = emgAcquire::DEFAULT_NB_CHANNELS;

    bufferSize = emgAcquire::DEFAULT_BUFFER_SIZE;

    buffer = std::vector< std::vector<double> > (bufferSize, std::vector<double>());

    interpolate = false;

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

    isNewMsgReceived = false;

    // functionality varaibles
    frequency = freq;

    nb_channels_received = nb_ch;

    bufferSize = emgAcquire::DEFAULT_BUFFER_SIZE;

    buffer = std::vector< std::vector<double> > (bufferSize, std::vector<double>());

    interpolate = false;

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
        nb_channels_received = nb_ch;
        return 0;
    }else{
        std::cout << "[emgAcquireClient] The number of channels cannot be greater than " << emgAcquire::MAXIMUM_NB_CHANNELS << std::endl;
        std::cout << "[emgAcquireClient] The number of channels has not been changed. The current number of channels size is: " << nb_channels_received << std::endl;
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
     *      0: if the size of the buffer has been correctly modified
     *     -1: if the size of the buffer is not modified
     */

    if (bfrSize<=emgAcquire::MAXIMUM_BUFFER_SIZE){
        
        bufferSize = bfrSize;

        buffer = std::vector< std::vector<double> > (bufferSize, std::vector<double>());

        if ((unsigned int)buffer.size()==bfrSize){
            return 0;
        }else{
            return -2;
        }

    }else{
        std::cout << "[emgAcquireClient] The new size cannot be greater than the maximum buffer-size(" << emgAcquire::MAXIMUM_BUFFER_SIZE << " samples)" << std::endl;
        std::cout << "[emgAcquireClient] The buffer has not been changed. The current buffer size is: " << bufferSize << std::endl;
        return -1;
    }
    
}


int emgAcquire::Client::setInterpolation(bool interpol){

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
    
    interpolate = interpol;

    return 0;
}


int emgAcquire::Client::setDigitalSignalReturn(bool digSingalReturn){

    /**
     *  changing the interpolation flag.
     *  
     *  interpol:  the desired interpolation flag.
     *              ture:   to retrieve the digital signal
     *              false:  not to retrieve the digital signal
     *
     *  Returns:
     *      0: always
     */

    giveDigitalSignal = digSingalReturn;

    return 0;
}