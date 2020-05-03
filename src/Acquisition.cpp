/** 
 *  Copyright (c) 2020 Iason Batzianoulis
 *  
 *  socketStream source file
 *  
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    MIT
 * 
**/


#include "Acquisition.h"

Acquisition::Acquisition(){
    // initialize COM
    HRESULT tmp = CoInitialize(0);
    if(tmp<0){
        std::cerr << "[Acquisition] Unable inialize COM" << std::endl;
    }
    else{
        std::cout << "[Acquisition] Successfully initialize COM lib" << std::endl;
    }
    

    // obtain the device manager
    _com_util::CheckError(dm.CreateInstance(__uuidof(Easy2AcquireCom::DeviceManager)));
    dm->Initialize(L"");

    // invoke the setup (optional)
    LONG ok = dm->Setup(0);
    if (!ok){
        std::cerr<< "[Acquisition] The device is not properly set-up" << std::endl;
    }

    nb_analog_devices = 0;
    nb_digital_devices = 0;
    isContinuous = false;
    isSetNew = false;
        
}

Acquisition::~Acquisition(){

    if(isContinuous){
        std::cout << "[Acquisition] Breaking continuous acquisition" << std::endl;
        threadMutex.lock();
        isContinuous = false;
        threadMutex.unlock();
        continuousAcquirer.join();
    }
    
    // close the COM library gracefully
    CoUninitialize();
    std::cout << "[Acquisition] COM closed" << std::endl;
    device->Deactivate();
    std::cout << "[Acquisition] Device deactivated" << std::endl;

}

int Acquisition::printComponents(std::string t_prefix){

    SAFEARRAY* tags  = device->GetTagsByPrefix(t_prefix.c_str());
    int count = tags->rgsabound[0].cElements;

    for(int i = 0; i < count; ++i)
    {
        BSTR tag = ((BSTR*)tags->pvData) [i];
        std::cout << "[Acquisition] #" << i+1 << " " << tag << " (" << device->GetTagName(tag) << ")" << std::endl;
    }

    SafeArrayDestroy(tags);

    return 0;
}

int Acquisition::initialize(float freq, unsigned int selected_nb_signals){


    // setting the sampling rate
    sampling_frequency = freq;
    
    // obtain an instance of the currently-selected device
    device = dm->GetCurrentDevice();

    // clear the enumeration filter (initial step for the new device)
    device->ClearComponentFilter();

    // print the device information
    std::cout << "[Acquisition] Devices:" << std::endl;
    if(printComponents("device")<0){
        std::cout << "[Acquisition] Unable to print devices" << std::endl;
    }
    std::cout << std::endl;

    // print information of the component types
    std::cout << "[Acquisition] Component types:" << std::endl;
    if(printComponents("type")<0){
        std::cout << "[Acquisition] Unable to print devices" << std::endl;
    }
    std::cout << std::endl;

    // get the overall number of components
    LONG count = device->GetComponentCount();
    std::cout << "[Acquisition] Overall number of components: " << count << std::endl;
    std::cout << "[Acquisition] Components:" << std::endl;

    // get the type of each component
    for(int i = 0; i < count; i++){
        Easy2AcquireCom::IComponentPtr component = device->GetComponent(i);

        _bstr_t component_type = component->GetType();

        std::string type_str;

        if (!std::strcmp(component_type, "type.input.analog")){
            nb_analog_devices++;
            type_str = "analog";
        }
        if (!std::strcmp(component_type, "type.input.digital")){
            nb_digital_devices++;
            type_str = "digital";
        }

        std::cout << "[Acquisition] #" << (i+1) << " input type: " << type_str << std::endl;
    }

    std::cout << std::endl;
    std::cout << "[Acquisition] Number of digital inputs: " << nb_digital_devices << std::endl;
    std::cout << "[Acquisition] Number of analog inputs: " << nb_analog_devices << std::endl;
    std::cout << std::endl;

    if(selected_nb_signals>0){
        nb_analog_devices = selected_nb_signals;
    }
    std::cout << "[Acquisition] Initializing " << nb_analog_devices << " analog inputs and " << nb_digital_devices << " digital inputs" << std::endl;
    for(int i=0; i<nb_analog_devices; i++){

        if(initializeInput(i, L"type.input.analog")<0){
            std::cerr << "[Acquisition] Unable to initialize analog input " << i << std::endl;
            return -i-1;
        }
    }
    analogData = std::vector< std::vector<double> >(nb_analog_devices, std::vector<double>());

    for(int i=0; i<nb_digital_devices; i++){

        if(initializeInput(i, L"type.input.digital")<0){
            std::cerr << "[Acquisition] Unable to initialize digital input " << i << std::endl;
            return -i-1;
        }
    }
    digitalData = std::vector< std::vector<double> >(nb_digital_devices, std::vector<double>());

    // std::cout << "Number of initialized analog inputs: " << analogInputDevices.size() << std::endl;
    // std::cout << "Number of initialized digital inputs: " << digitalInputDevice.size() << std::endl;
    // std::cout << "analog inputs: " << std::endl;
    // for(int i=0; i<analogInputDevices.size(); i++){
    //     std::cout << analogInputDevices[i]->GetId();
    // }
    // std::cout << "digital inputs: " << std::endl;
    // for(int i=0; i<digitalInputDevice.size(); i++){
    //     std::cout << digitalInputDevice[i]->GetId();
    // }
    std::cout << "[Acquisition] Input devices initialized" << std::endl;

    return 0;
}


int Acquisition::initializeInput(LONG index, _bstr_t type){

    
    // check if the components <type> have enough indexes
    device->SetComponentFilterTags(type);
    LONG count = device->GetComponentCount();

    if (ULONG(index) >= ULONG(count)){
        return -1; // no, just return -1
    }

    if (!std::strcmp(type, "type.input.analog")){

        // retrieve the component and append the vector which contain the input devices
        analogInputDevices.push_back(device->GetComponent(index));

        // enable the device 
        analogInputDevices.back()->Enable();

        // ask frequency (we must read back the actual frequency after IDevice::Activate)
        analogInputDevices.back()->SetDesiredFrequency(sampling_frequency);

        // initialize buffers
        analogBuffers.push_back(_variant_t());

    }else{

        // retrieve the component and append the vector which contain the input devices
        digitalInputDevice.push_back(device->GetComponent(index));

        // enable the device 
        digitalInputDevice.back()->Enable();

        // ask frequency (we must read back the actual frequency after IDevice::Activate)
        digitalInputDevice.back()->SetDesiredFrequency(sampling_frequency);

        // initialize buffers
        digitalBuffers.push_back(_variant_t());

    }

    // clear tag filters
    device->SetComponentFilterTags("");


    return 0;
}

int Acquisition::activate(){

    /**
     * Activate the device and all its components
     * 
     * Return 0 if everything is ok, or the error otherwise 
     * 
     */

    // activate the device
    device->Activate();

    std::cout << std::endl;
    std::cout << "[Acquisition] Activating analog input devices" << std::endl;
    // activate all analog input devices
    for (int i=0; i < nb_analog_devices; i++){
        _bstr_t name = analogInputDevices[i]->GetId();

        // read back the granted frequency
        double frequency = analogInputDevices[i]->GetFrequency();
 
        real_freq_AI.push_back((float)frequency);

        std::cout << name << std::endl << " Frequency: " << frequency << std::endl;

        // let the component create our transmission buffer (convenience function);
        // actually, it allocates SAFEARRAY according to the nature of the component (e.g. double values for the analog input),
        // with a maximum transmission size (GetMaxQuantCount())
        analogInputDevices[i]->CreateCompatibleBuffer(&analogBuffers[i]);
        LONG maxQuants = analogInputDevices[i]->GetMaxQuantCount();
        std::cout << "maximum number of quants: " << maxQuants << std::endl;
    }

    std::cout << "[Acquisition] Activating digital input devices" << std::endl;
     // activate all digital input devices
    for (int i=0; i < nb_digital_devices; i++){
        _bstr_t name = digitalInputDevice[i]->GetId();

        // read back the granted frequency
        double frequency = digitalInputDevice[i]->GetFrequency();

        real_freq_DI.push_back((float)frequency);

        std::cout << name << std::endl << " Frequency: " << frequency << std::endl;

        // let the component create our transmission buffer (convenience function);
        // actually, it allocates SAFEARRAY according to the nature of the component (e.g. double values for the analog input),
        // with a maximum transmission size (GetMaxQuantCount())
        digitalInputDevice[i]->CreateCompatibleBuffer(&digitalBuffers[i]);
    }

    std::cout << "[Acquisition] Activation complete" << std::endl;
    std::cout << std::endl;
    
    return 0;
}


int Acquisition::update(){
    /**
     * Read the new values of the signals and update the data
     * 
     * Return 0 if everything is ok, or the error otherwise 
     * 
     */

    LONG state = device->Transfer();

    if (state & Easy2AcquireCom::TransferDataReady){

        // std::vector<double> tmp_data((int)analogInputDevices.size()+(int)digitalInputDevice.size(), 0);

        // get the updated data for all the analog input devices
        for (int i=0; i < nb_analog_devices; i++){

            // get the number of received quants from the component
            LONG quant_count = analogInputDevices[i]->GetQuantCount();

            // get the received quants into the pre-allocated buffer
            analogInputDevices[i]->GetQuants(0, quant_count, &analogBuffers[i], 0);

            const double* quants = (const double*) analogBuffers[i].GetVARIANT().parray->pvData;

            threadMutex.lock();
            analogData[i] = std::vector<double>(quants, quants + quant_count); 
            threadMutex.unlock();

            // double sumquants = 0;
            // for (int j=0; j<quant_count; j++){
            //     sumquants += quants[j];
            // }

            // tmp_data[i]=sumquants/quant_count;

            // print how many quants received
            // if(i==0)
            //     std::cout << "quants " << quant_count << "size: " << analogData[i].size() << std::endl;
           
            // std::cout << "data: ";
            // for(int j=0; j<quant_count; j++ ){
            //     std::cout << quants[j] << ", ";
            // }
            // std::cout << std::endl;

            

        }

        // get the updated data for all the digital input devices
        for (int i=0; i < nb_digital_devices; i++){

            // get the number of received quants from the component
            LONG quant_count = digitalInputDevice[i]->GetQuantCount();

            // get the received quants into the pre-allocated buffer
            digitalInputDevice[i]->GetQuants(0, quant_count, &digitalBuffers[i], 0);

            const double* quants = (const double*) digitalBuffers[i].GetVARIANT().parray->pvData;

            threadMutex.lock();
            digitalData[i] = std::vector<double>(quants, quants + quant_count);
            threadMutex.unlock();

            // double sumquants = 0;
            // for (int j=0; j<quant_count; j++){
            //     sumquants += quants[j];
            // }

            // tmp_data[nb_analog_devices+i]=sumquants/quant_count;

            // print how many quants received
            // if(i==0)
            //     std::cout << "quants "<<  quant_count  << std::endl;
            
            // std::cout << "data: ";
            // for(int j=0; j<quant_count; j++ ){
            //     std::cout << quants[j] << ", ";
            // }
            // std::cout << std::endl;

            

        }

        // for(int j=0; j<tmp_data.size(); j++){
        //     std::cout << tmp_data[j] << "  ";
        // }
        // std::cout << std::endl;
        
        
        return 0;
    }else{
        return -1;
    }
    

    // return 0;
}

std::vector< std::vector<double> > Acquisition::getlatestAnalog(){

    /*
     *  return the latest set of alalog values 
     * 
     */

    return analogData;
}

std::vector< std::vector<double> > Acquisition::getlatestAnalog(bool* isNew){

    /*
     *  return the latest set of alalog values 
     * 
     */

    *isNew = isSetNew;
    if(isSetNew){
        threadMutex.lock();
        isSetNew = false;
        threadMutex.unlock();
    }

    return analogData;
}

std::vector< std::vector<double> > Acquisition::getlatestDigital(){

    /*
     *  return the latest set of digital values 
     * 
     */

    return digitalData;
}

std::vector< std::vector<double> > Acquisition::getlatestDigital(bool* isNew){

    /*
     *  return the latest set of digital values 
     * 
     */

    *isNew = isSetNew;
    if(isSetNew){
        threadMutex.lock();
        isSetNew = false;
        threadMutex.unlock();
    }

    return digitalData;
}

std::vector< std::vector<double> > Acquisition::getlatest(){

    /*
     *  return the latest set of values 
     * 
     */
    
    std::lock_guard<std::mutex> lck (threadMutex);
    
    std::vector< std::vector<double> > returnVector = analogData;
    returnVector.reserve(analogData.size() + digitalData.size());
    returnVector.insert(returnVector.end(), digitalData.begin(), digitalData.end());


    return returnVector;
}

std::vector< std::vector<double> > Acquisition::getlatest(bool* isNew){

    /*
     *  return the latest set of values 
     * 
     */
    
    std::lock_guard<std::mutex> lck (threadMutex);

    *isNew = isSetNew;
    if(isSetNew){
        isSetNew = false;
    }
    
    std::vector< std::vector<double> > returnVector = analogData;
    returnVector.reserve(analogData.size() + digitalData.size());
    returnVector.insert(returnVector.end(), digitalData.begin(), digitalData.end());


    return returnVector;
}

int Acquisition::continuousUpdate(){

    while(isContinuous){
        update();
        if(!isSetNew){
            isSetNew = true;
        }
        std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(int(1000/sampling_frequency)-2));
    }

    std::cout << "[Acquisition] Stopped acquiring signals continuously" << std::endl;

    return 0;
}

int Acquisition::runContinuously(){

    isContinuous = true;
    isSetNew = false;
    continuousAcquirer = std::thread(&Acquisition::continuousUpdate, this);
    std::cout << "[Acquisition] Continuous acquisition of the signals has started" << std::endl;

    return 0;
}

int Acquisition::stop(){

    if(isContinuous){
        std::cout << "[Acquisition] Breaking continuous acquisition" << std::endl;
        threadMutex.lock();
        isContinuous = false;
        threadMutex.unlock();
        continuousAcquirer.join();
    }else{
        std::cout << "[Acquisition] Continuous acquisition was not set on. Nothing to stop." << std::endl;
        return -1;
    }

    return 0;
}