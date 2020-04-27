
#include "Acquisition.h"


int main(int argc, char **argv){

    Acquisition emgAcq;

    float freq = 100.0;
    int nb_channels = 2;

    if(emgAcq.initialize(freq, nb_channels)<0){
        std::cout << "Unable to initialize devices" << std::endl;
        return -1;
    }

    emgAcq.activate();

    return 0;
}