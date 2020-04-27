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
        
}

Acquisition::~Acquisition(){
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

    std::cout << "[Acquisition] Number of digital inputs: " << nb_digital_devices << std::endl;
    std::cout << "[Acquisition] Number of analog inputs: " << nb_analog_devices << std::endl;
    std::cout << std::endl;

    if(selected_nb_signals>0){
        nb_analog_devices = selected_nb_signals;
    }
    std::cout << "[Acquisition] Initializing " << nb_analog_devices << " analog inputs and " << nb_digital_devices << " digital inputs" << std::endl;
    for(int i=0; i<nb_analog_devices+nb_digital_devices; i++){

        if(initializeInput(i, device->GetComponent(i)->GetType())<0){
            std::cerr << "[Acquisition] Unable to initialize input " << i << std::endl;
            return -i-1;
        }
    }

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

    device->Activate();

    for (int i=0; i < nb_analog_devices; i++){
        _bstr_t name = analogInputDevices[i]->GetId();

        // read back the granted frequency
        double frequency = analogInputDevices[i]->GetFrequency();

        std::cout << name << std::endl << " Frequency: " << frequency << std::endl;

        // let the component create our transmission buffer (convenience function);
        // actually, it allocates SAFEARRAY according to the nature of the component (e.g. double values for the analog input),
        // with a maximum transmission size (GetMaxQuantCount())
        analogInputDevices[i]->CreateCompatibleBuffer(&analogBuffers[i]);
    }
    
    
    return 0;
}